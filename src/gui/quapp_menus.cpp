/*
	*** Quapp - Menua 
	*** src/gui/quapp_menus.cpp
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

#include "gui/quapp.h"
#include "base/session.h"
#include <QtGui/QFileDialog>
#include <QtGui/QMessageBox>
#include <QtGui/QInputDialog>

void QuappWindow::on_actionFileNew_triggered(bool checked)
{
	if (Session::isModified())
	{
		QMessageBox::StandardButton button = QMessageBox::warning(this, "Warning", "The current file has been modified.\nDo you want to save this data first?", QMessageBox::Cancel | QMessageBox::Yes | QMessageBox::No, QMessageBox::Cancel);
		if (button == QMessageBox::Cancel) return;
		else if (button == QMessageBox::Yes)
		{
			// Save file, and check Session::isModified() status to make sure it wasn't cancelled.
			on_actionFileSave_triggered(false);
			if (Session::isModified()) return;
		}
	}

	// Start new session and update GUI
	startNewSession();
	updateSegments();
	updateRunData();
}

void QuappWindow::on_actionFileOpen_triggered(bool checked)
{
	if (Session::isModified())
	{
		QMessageBox::StandardButton button = QMessageBox::warning(this, "Warning", "The current file has been modified.\nDo you want to save this data first?", QMessageBox::Cancel | QMessageBox::Yes | QMessageBox::No, QMessageBox::Cancel);
		if (button == QMessageBox::Cancel) return;
		else if (button == QMessageBox::Yes)
		{
			// Save file, and check Session::isModified() status to make sure it wasn't cancelled.
			on_actionFileSave_triggered(false);
			if (Session::isModified()) return;
		}
	}

	QString fileName = QFileDialog::getOpenFileName(this, "Choose file to load", inputFileDirectory_.absolutePath(), "Quapp files (*.qap);;All files (*.*)");
	if (fileName.isEmpty()) return;
	inputFileDirectory_ = fileName;

	// Load input file
	if (!quiz_.load(fileName))
	{
		QMessageBox::information(this, "Failed to load session", "Failed to load the session, so reverting to the default.\n");
		startNewSession();
	}

	// Update the GUI
	updateSegments();
	updateRunData();
}

void QuappWindow::on_actionFileSave_triggered(bool checked)
{
	// Has an input filename already been chosen?
	if (Session::inputFile().isEmpty())
	{
		QString fileName = QFileDialog::getSaveFileName(this, "Choose save file name", inputFileDirectory_.absolutePath(), "Quapp files (*.qap);;All files (*.*)");
		if (fileName.isEmpty()) return;

		// Make sure the file has the right extension
		QFileInfo fileInfo(fileName);
		if (fileInfo.suffix() != "qap") fileName += ".qap";
		Session::setInputFile(fileName);
	}

	if (quiz_.save(Session::inputFile())) Session::setAsNotModified();
}

void QuappWindow::on_actionFileSaveAs_triggered(bool checked)
{
	// Get a filename from the user
	QString fileName = QFileDialog::getSaveFileName(this, "Choose save file name", inputFileDirectory_.absolutePath(), "Quapp files (*.qap);;All files (*.*)");
	if (fileName.isEmpty()) return;

	// Make sure the file has the right extension
	QFileInfo fileInfo(fileName);
	if (fileInfo.suffix() != "qap") fileName += ".qap";
	Session::setInputFile(fileName);

	if (quiz_.save(Session::inputFile())) Session::setAsNotModified();
}

void QuappWindow::on_actionDisplayShow_triggered(bool checked)
{
	displayWindow_.show();
}

void QuappWindow::on_actionDisplayRunFromStart_triggered(bool checked)
{
	ui.RunFromStartButton->click();
}

void QuappWindow::on_actionDisplayRunFromSegment_triggered(bool checked)
{
	ui.RunFromSegmentButton->click();
}

void QuappWindow::on_actionDisplayToggleFullscreen_triggered(bool checked)
{
	if (displayWindow_.isFullScreen()) displayWindow_.showNormal();
	else displayWindow_.showFullScreen();
}