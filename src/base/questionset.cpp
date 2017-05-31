/*
	*** QuestionSet
	*** src/base/questionset.cpp
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

#include "base/questionset.h"
#include "base/messenger.h"
#include <QtCore/QString>

// Constructor
QuestionSet::QuestionSet()
{
	name_ = "New QuestionSet";
}

// Destructor
QuestionSet::~QuestionSet()
{
}

/*
 * Definition
 */

// Set name of question set
void QuestionSet::setName(QString name)
{
	name_ = name;
}

// Return name of question set
QString QuestionSet::name()
{
	return name_;
}

// Add question
Question* QuestionSet::addQuestion()
{
	Question* newItem = questions_.add();

	return newItem;
}

// Remove question
void QuestionSet::removeQuestion(Question* item)
{
	questions_.remove(item);
}

// Return number of questions
int QuestionSet::nQuestions()
{
	return questions_.nItems();
}

// Return questions
Question* QuestionSet::questions()
{
	return questions_.first();
}

// Return specified question
Question* QuestionSet::question(int index)
{
	return questions_[index];
}

// Return index of specified question
int QuestionSet::indexOf(Question* item)
{
	return questions_.indexOf(item);
}

// Move specified question up in list (towards head)
void QuestionSet::moveQuestionUp(Question* item)
{
	questions_.shiftUp(item);
}

// Move specified question down in list (towards tail)
void QuestionSet::moveQuestionDown(Question* item)
{
	questions_.shiftDown(item);
}
