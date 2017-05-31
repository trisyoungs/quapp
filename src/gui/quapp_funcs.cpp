/*
	*** Quapp - Functions 
	*** src/gui/quapp_funcs.cpp
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
#include "render/fontinstance.h"
#include "base/session.h"
#include "templates/variantpointer.h"
#include "version.h"
#include <QtGui/QMessageBox>
#include <QtMultimedia/QAudioOutput>

// Constructor
QuappWindow::QuappWindow(QMainWindow *parent) : QMainWindow(parent), displayWindow_(*this)
{
	// Initialise the icon resource
	Q_INIT_RESOURCE(icons);

	// Call the main creation function
	ui.setupUi(this);

	// Set pointer in CurrentProject
	Session::setMainWindow(this);

	refreshing_ = false;

	// Set UChroma pointers in widgets/dialogs where necessary
	displayWindow_.ui.MainView->setQuapp(this);

	// Load font for viewer
	viewerFont_ = "/usr/share/fonts/truetype/luxisb.ttf";
	if (!QFile::exists(viewerFont_)) QMessageBox::warning(this, "Font Error", "The specified font file '" + viewerFont_ + "' does not exist.");
	else if (!FontInstance::setupFont(viewerFont_)) QMessageBox::warning(this, "Font Error", "Failed to create a font from the specified font file '" + viewerFont_ +"'.");
	if (FontInstance::fontOK()) FontInstance::font()->FaceSize(1);

	// Set variable defaults
	aspectModifier_ = 1.0;

printf("Lkjlkjlkl\n");
foreach(const QAudioDeviceInfo &deviceInfo, QAudioDeviceInfo::availableDevices(QAudio::AudioOutput))
    printf("Device name: %s", qPrintable(deviceInfo.deviceName()));

	// Create audio device for playback
	// -- Set up the format
	QAudioFormat format;
	format.setSampleRate(44100);
	format.setChannelCount(2);
	format.setSampleSize(16);
	format.setCodec("audio/pcm");
	format.setByteOrder(QAudioFormat::LittleEndian);
	format.setSampleType(QAudioFormat::SignedInt);

	QAudioDeviceInfo info(QAudioDeviceInfo::defaultOutputDevice());
	if (!info.isFormatSupported(format))
	{
		printf("Requested audio format not supported - trying to use nearest.");
		format = info.nearestFormat(format);
	}
	audioOutput_ = new QAudioOutput(format, this);
// 	connect(audio, SIGNAL(stateChanged(QAudio::State)), this, SLOT(handleStateChanged(QAudio::State)));

	startNewSession();

	// Setup and start timer
	QObject::connect(&objectTimer_, SIGNAL(timeout()), this, SLOT(updateObjects()));
	objectTimer_.setInterval(25);
	objectTimer_.setSingleShot(true);
	objectTimer_.start();
}

// Destructor
QuappWindow::~QuappWindow()
{
}

/*
 * Window Functions
 */

// Return centre coordinate of main window
QPoint QuappWindow::centrePos()
{
	QPoint centre = pos();
	centre += QPoint(width()/2, height()/2);
	return centre;
}

// Window close event
void QuappWindow::closeEvent(QCloseEvent *event)
{
}

/*
 * Quiz
 */

// Return current quiz
Quiz& QuappWindow::quiz()
{
	return quiz_;
}

// Start new session
void QuappWindow::startNewSession()
{
	// Clear old data
	quiz_.clear();

	// Clear old objects
	backgroundObjects_.clear();
	foregroundObjects_.clear();
	persistentForegroundObjects_.clear();

	// Set some basic quantities affecting the view
	// Vertical layout is as follows:
	// 1.  ForeGroundMargin
	// 2.  TitleLineHeight  (BodyLineHeight * 1.2)
	// 3.  ForegroundMargin * 2
	// 4.  N * BodyLineHeight
	// 5.  ForeGroundMargin
	foregroundMargin_ = 0.02;
	titleLineHeight_ = 0.06;
	nBodyTextLines_ = 18;
	calculateTextHeights();

	// Initialise background
	initialiseBackground(QuappWindow::TestBackground);
}
