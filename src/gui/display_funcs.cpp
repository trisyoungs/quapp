/*
	*** Display Window Functions
	*** src/gui/display_funcs.cpp
	Copyright T. Youngs 2013-2014

	This file is part of Quapp.

	Quapp is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	Quapp is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with Quapp.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "gui/display.h"
#include "gui/quapp.h"
#include <QtGui/QMessageBox>

// Constructor
DisplayWindow::DisplayWindow(QuappWindow& parent) : QMainWindow(&parent), quapp_(parent)
{
	// Call the main creation function
	ui.setupUi(this);

// 	setWindowFlags(Qt::Tool);

	refreshing_ = false;

	show();
}

// Destructor
DisplayWindow::~DisplayWindow()
{
}

/*
 * Window Functions
 */

// Window close event
void DisplayWindow::closeEvent(QCloseEvent *event)
{
}

