/*
	*** QuestionItem
	*** src/base/question.cpp
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

#include "base/question.h"
#include <QtCore/QString>
#include <QtCore/QFile>

// Constructor
Question::Question() : ListItem<Question>()
{
	audioQuestionOK_ = false;
	audioAnswerOK_ = false;
	imageQuestionOK_ = false;
	imageAnswerOK_ = false;
}

// Destructor
Question::~Question()
{
}

/*
 * Definition
 */

// Set text question
void Question::setTextQuestion(QString text)
{
	textQuestion_ = text;
}

// Return text question
QString Question::textQuestion()
{
	return textQuestion_;
}

// Set text answer
void Question::setTextAnswer(QString text)
{
	textAnswer_ = text;
}

// Return text answer
QString Question::textAnswer()
{
	return textAnswer_;
}

// Set image question (from filename)
void Question::setImageQuestion(QString fileName)
{
	imageQuestionFileName_ = fileName;
	imageQuestionOK_ = imageQuestion_.load(fileName);
}

// Return image question filename
QString Question::imageQuestionFileName()
{
	return imageQuestionFileName_;
}

// Return whether image question is valid
bool Question::imageQuestionOK()
{
	return imageQuestionOK_;
}

// Return image question
QImage& Question::imageQuestion()
{
	return imageQuestion_;
}

// Set image answer (from filename)
void Question::setImageAnswer(QString fileName)
{
	imageAnswerFileName_ = fileName;
	imageAnswerOK_ = imageAnswer_.load(fileName);
}

// Return image answer filename
QString Question::imageAnswerFileName()
{
	return imageAnswerFileName_;
}

// Return whether image answer is valid
bool Question::imageAnswerOK()
{
	return imageAnswerOK_;
}

// Return image answer
QImage& Question::imageAnswer()
{
	return imageAnswer_;
}

// Set audio question (from filename)
void Question::setAudioQuestion(QString fileName)
{
	audioQuestionFileName_ = fileName;
	audioQuestionOK_ = QFile::exists(fileName);
}

// Return audio question filename
QString Question::audioQuestionFileName()
{
	return audioQuestionFileName_;
}

// Return whether audio question is OK (exists)
bool Question::audioQuestionOK()
{
	return audioQuestionOK_;
}

// Set audio answer (from filename)
void Question::setAudioAnswer(QString fileName)
{
	audioAnswerFileName_ = fileName;
	audioAnswerOK_ = QFile::exists(fileName);
}

// Return audio answer filename
QString Question::audioAnswerFileName()
{
	return audioAnswerFileName_;
}

// Return whether audio answer is OK (exists)
bool Question::audioAnswerOK()
{
	return audioAnswerOK_;
}
