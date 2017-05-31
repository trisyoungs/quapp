/*
	*** Question
	*** src/base/question.h
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

#ifndef QUAPP_QUESTION_H
#define QUAPP_QUESTION_H

#include "templates/list.h"
#include <QtCore/QString>
#include <QtGui/QImage>
#include <QtGui/QSound>

// Forward Declarations
/* none */

// Question
class Question : public ListItem<Question>
{
	public:
	// Constructor / Destructor
	Question();
	~Question();


	/*
	 * Definition
	 */
	private:
	// Question text
	QString textQuestion_;
	// Answer text
	QString textAnswer_;
	// Question image filename
	QString imageQuestionFileName_;
	// Whether question image was loaded successfully
	bool imageQuestionOK_;
	// Question image
	QImage imageQuestion_;
	// Answer image filename
	QString imageAnswerFileName_;
	// Whether answer image was loaded successfully
	bool imageAnswerOK_;
	// Answer image
	QImage imageAnswer_;
	// Question sound filename
	QString audioQuestionFileName_;
	// Whether question audio is OK (exists)
	bool audioQuestionOK_;
	// Answer sound filename
	QString audioAnswerFileName_;
	// Whether answer audio is OK (exists)
	bool audioAnswerOK_;

	public:
	// Set text question
	void setTextQuestion(QString text);
	// Return text question
	QString textQuestion();
	// Set text answer
	void setTextAnswer(QString text);
	// Return text answer
	QString textAnswer();
	// Set image question (from filename)
	void setImageQuestion(QString fileName);
	// Return image question filename
	QString imageQuestionFileName();
	// Return whether image question is valid
	bool imageQuestionOK();
	// Return image question
	QImage& imageQuestion();
	// Set image answer (from filename)
	void setImageAnswer(QString fileName);
	// Return image answer filename
	QString imageAnswerFileName();
	// Return whether image answer is valid
	bool imageAnswerOK();
	// Return image answer
	QImage& imageAnswer();
	// Set audio question (from filename)
	void setAudioQuestion(QString fileName);
	// Return audio question filename
	QString audioQuestionFileName();
	// Return whether audio question is OK (exists)
	bool audioQuestionOK();
	// Set audio answer (from filename)
	void setAudioAnswer(QString fileName);
	// Return audio answer filename
	QString audioAnswerFileName();
	// Return whether audio answer is OK (exists)
	bool audioAnswerOK();
};

#endif
