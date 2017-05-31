/*
	*** QuestionSet
	*** src/base/questionset.h
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

#ifndef QUAPP_QUESTIONSET_H
#define QUAPP_QUESTIONSET_H

#include "base/question.h"
#include "templates/list.h"
#include <QtCore/QString>

// Forward Declarations
class Quiz;

// QuestionSet
class QuestionSet : public ListItem<QuestionSet>
{
	public:
	// Constructor / Destructor
	QuestionSet();
	~QuestionSet();


	/*
	 * Definition
	 */
	private:
	// Name of question set
	QString name_;
	// QuestionSet Data
	List<Question> questions_;

	public:
	// Set name of question set
	void setName(QString name);
	// Return name of question set
	QString name();
	// Add question
	Question* addQuestion();
	// Remove question
	void removeQuestion(Question* item);
	// Return number of question set items
	int nQuestions();
	// Return questions
	Question* questions();
	// Return specified question set item
	Question* question(int index);
	// Return index of specified question
	int indexOf(Question* item);
	// Move specified question set item up in list (towards head)
	void moveQuestionUp(Question* item);
	// Move specified question set item down in list (towards tail)
	void moveQuestionDown(Question* item);
};

#endif
