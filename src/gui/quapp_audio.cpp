/*
	*** Quapp - Audio
	*** src/gui/quapp_audio.cpp
	Copyright T. Youngs 2013-2015

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
#include <QtMultimedia/QAudioOutput>

// Play specified audio file
bool QuappWindow::playAudio(QString filename)
{
	// Make sure current audio is stopped
	audioOutput_->stop();
	if (currentAudioStream_.isOpen()) currentAudioStream_.close();

	// Set new stream
	currentAudioStream_.setFileName(filename);
	currentAudioStream_.open(QIODevice::ReadOnly);
	if (!currentAudioStream_.isOpen())
	{
		printf("Error opening audio file.\n");
		return false;
	}

	audioOutput_->start(&currentAudioStream_);

	return true;
}