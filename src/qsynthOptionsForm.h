// qsynthOptionsForm.h
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

#ifndef __qsynthOptionsForm_h
#define __qsynthOptionsForm_h

#include "ui_qsynthOptionsForm.h"

// Forward declarations.
class qsynthOptions;


//----------------------------------------------------------------------------
// qsynthOptionsForm -- UI wrapper form.

class qsynthOptionsForm : public QDialog
{
	Q_OBJECT

public:

	// Constructor.
	qsynthOptionsForm(QWidget *pParent = 0, Qt::WFlags wflags = 0);
	// Destructor.
	~qsynthOptionsForm();

    void setup(qsynthOptions *pOptions);

public slots:

    void optionsChanged();

    void chooseMessagesFont();
    void stabilizeForm();

protected slots:

    void accept();
    void reject();

private:

	// The Qt-designer UI struct...
	Ui::qsynthOptionsForm m_ui;

	// Instance variables.
    qsynthOptions *m_pOptions;

    int m_iDirtySetup;
    int m_iDirtyCount;
};


#endif	// __qsynthOptionsForm_h


// end of qsynthOptionsForm.h

