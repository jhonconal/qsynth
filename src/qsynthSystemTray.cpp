// qsynthSystemTray.cpp
//
/****************************************************************************
   Copyright (C) 2003-2007, rncbc aka Rui Nuno Capela. All rights reserved.

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   as published by the Free Software Foundation; either version 2
   of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License along
   with this program; if not, write to the Free Software Foundation, Inc.,
   51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

*****************************************************************************/

#include "qsynthAbout.h"
#include "qsynthSystemTray.h"

#include <QPainter>
#include <QBitmap>
#include <QIcon>

#include <QToolTip>

#include <QMouseEvent>


#ifdef CONFIG_SYSTEM_TRAY

#include <QX11Info>

#include <X11/Xatom.h>
#include <X11/Xlib.h>

// System Tray Protocol Specification opcodes.
#define SYSTEM_TRAY_REQUEST_DOCK    0
#define SYSTEM_TRAY_BEGIN_MESSAGE   1
#define SYSTEM_TRAY_CANCEL_MESSAGE  2

#endif


//----------------------------------------------------------------------------
// qsynthSystemTray -- Custom system tray widget.

// Constructor.
qsynthSystemTray::qsynthSystemTray ( QWidget *pParent )
	: QWidget(pParent, Qt::Window
		| Qt::CustomizeWindowHint
		| Qt::X11BypassWindowManagerHint
		| Qt::FramelessWindowHint
		| Qt::WindowStaysOnTopHint)
{
	QWidget::setAttribute(Qt::WA_AlwaysShowToolTips);
//	QWidget::setAttribute(Qt::WA_NoSystemBackground);

//	QWidget::setBackgroundRole(QPalette::NoRole);
//	QWidget::setAutoFillBackground(true);

	QWidget::setFixedSize(22, 22);
//	QWidget::setMinimumSize(22, 22);

#ifdef CONFIG_SYSTEM_TRAY

	Display *dpy = QX11Info::display();
	WId trayWin  = winId();

	// System Tray Protocol Specification.
	Screen *screen = XDefaultScreenOfDisplay(dpy);
	int iScreen = XScreenNumberOfScreen(screen);
	char szAtom[32];
	snprintf(szAtom, sizeof(szAtom), "_NET_SYSTEM_TRAY_S%d", iScreen);
	Atom selectionAtom = XInternAtom(dpy, szAtom, false);
	XGrabServer(dpy);
	Window managerWin = XGetSelectionOwner(dpy, selectionAtom);
	if (managerWin != None)
		XSelectInput(dpy, managerWin, StructureNotifyMask);
	XUngrabServer(dpy);
	XFlush(dpy);
	if (managerWin != None) {
		XEvent ev;
		memset(&ev, 0, sizeof(ev));
		ev.xclient.type = ClientMessage;
		ev.xclient.window = managerWin;
		ev.xclient.message_type = XInternAtom(dpy, "_NET_SYSTEM_TRAY_OPCODE", false);
		ev.xclient.format = 32;
		ev.xclient.data.l[0] = CurrentTime;
		ev.xclient.data.l[1] = SYSTEM_TRAY_REQUEST_DOCK;
		ev.xclient.data.l[2] = trayWin;
		ev.xclient.data.l[3] = 0;
		ev.xclient.data.l[4] = 0;
		XSendEvent(dpy, managerWin, false, NoEventMask, &ev);
		XSync(dpy, false);
	}

	// Follwing simple KDE specs:
	Atom trayAtom;
	// For older KDE's (hopefully)...
	int data = 1;
	trayAtom = XInternAtom(dpy, "KWM_DOCKWINDOW", false);
	XChangeProperty(dpy, trayWin, trayAtom, trayAtom, 32, PropModeReplace, (unsigned char *) &data, 1);
	// For not so older KDE's...
	WId forWin = pParent ? pParent->topLevelWidget()->winId() : QX11Info::appRootWindow();
	trayAtom = XInternAtom(dpy, "_KDE_NET_WM_SYSTEM_TRAY_WINDOW_FOR", false);
	XChangeProperty(dpy, trayWin, trayAtom, XA_WINDOW, 32, PropModeReplace, (unsigned char *) &forWin, 1);

#endif

	// Set things inherited...
	if (pParent) {
		QWidget::setWindowIcon(pParent->windowIcon());
		QWidget::setToolTip(pParent->windowTitle());
	}

	setBackground(Qt::transparent);
}


// Default destructor.
qsynthSystemTray::~qsynthSystemTray (void)
{
}


// System tray icon method.
void qsynthSystemTray::updateIcon (void)
{
	// Renitialize icon as fit...
	m_pixmap = QWidget::windowIcon().pixmap(QWidget::size());

	if (!m_pixmap.mask().isNull() && m_background == Qt::transparent) {
		QBitmap mask(QWidget::size());
		mask.fill(Qt::color0);
		QBitmap maskPixmap = m_pixmap.mask();
		QPainter p(&mask);
		p.drawPixmap(
			(mask.width()  - maskPixmap.width())  / 2,
			(mask.height() - maskPixmap.height()) / 2,
			maskPixmap);
		QWidget::setMask(mask);
	} else {
		QWidget::setMask(QBitmap());
	}

	QWidget::update();
}



// Background mask methods.
void qsynthSystemTray::setBackground ( const QColor& background )
{
	// Set background color, now.
	m_background = background;

	QPalette pal(QWidget::palette());
	pal.setColor(QWidget::backgroundRole(), m_background);
	QWidget::setPalette(pal);

	updateIcon();
}

const QColor& qsynthSystemTray::background (void) const
{
	return m_background;
}


// Self-drawable methods.
void qsynthSystemTray::paintEvent ( QPaintEvent * )
{
	QPainter p(this);

	const QRect& rect = QWidget::rect();

	p.drawPixmap(
		rect.x() + (rect.width()  - m_pixmap.width())  / 2,
		rect.y() + (rect.height() - m_pixmap.height()) / 2,
		m_pixmap);
}


// Inherited mouse event.
void qsynthSystemTray::mousePressEvent ( QMouseEvent *pMouseEvent )
{
	if (!QWidget::rect().contains(pMouseEvent->pos()))
		return;

	switch (pMouseEvent->button()) {

	case Qt::LeftButton:
		// Toggle parent widget visibility.
		emit clicked();
		break;

	case Qt::RightButton:
		// Just signal we're on to context menu.
		emit contextMenuRequested(pMouseEvent->globalPos());
		break;

	default:
		break;
	}
}


// end of qsynthSystemTray.cpp

