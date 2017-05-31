/*
	*** Quapp - Questions 
	*** src/gui/quapp_questions.cpp
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
#include <QtGui/QInputDialog>
#include <QtGui/QFileDialog>
#include <QtGui/QMessageBox>
#include <QtMultimedia/QAudioOutput>

// Return current questionset
QuestionSet* QuappWindow::currentQuestionSet()
{
	if (ui.QuestionSetList->currentRow() == -1) return NULL;
	return quiz_.questionSet(ui.QuestionSetList->currentRow());
}

// Return current segment item
Question* QuappWindow::currentQuestion()
{
	QuestionSet* questionSet= currentQuestionSet();
	if (!questionSet) return NULL;
	if (ui.QuestionList->currentRow() == -1) return NULL;
	Question* question = questionSet->question(ui.QuestionList->currentRow());
	return question;
}

/*
 * Question Sets
 */

void QuappWindow::on_QuestionSetAddButton_clicked(bool checked)
{
	bool ok;
	QString newName = QInputDialog::getText(this, "New Question Set Name", "New Name", QLineEdit::Normal, "New Question Set", &ok);
	if (!ok) return;

	QuestionSet* questionSet = quiz_.addQuestionSet();
	questionSet->setName(newName);

	updateQuestionSets(questionSet);
	updateQuestions();
	updateSegments();
	updateRunData();
}

void QuappWindow::on_QuestionSetRemoveButton_clicked(bool checked)
{
	QuestionSet* questionSet = currentQuestionSet();
	if (!questionSet) return;

	QuestionSet* newCurrentQuestionSet = questionSet->next == NULL ? questionSet->prev : questionSet->next;
	quiz_.removeQuestionSet(questionSet);

	updateQuestionSets(newCurrentQuestionSet);
	updateQuestions();
}

void QuappWindow::on_QuestionSetMoveUpButton_clicked(bool checked)
{
	QuestionSet* questionSet = currentQuestionSet();
	if (!questionSet) return;

	if (questionSet->prev) quiz_.moveQuestionSetUp(questionSet);

	updateQuestionSets(questionSet);
}

void QuappWindow::on_QuestionSetMoveDownButton_clicked(bool checked)
{
	QuestionSet* questionSet = currentQuestionSet();
	if (!questionSet) return;

	if (questionSet->next) quiz_.moveQuestionSetDown(questionSet);

	updateQuestionSets(questionSet);
}

void QuappWindow::on_QuestionSetList_currentRowChanged(int row)
{
	QuestionSet* questionSet = currentQuestionSet();
	if (refreshing_ || (!questionSet)) return;

	updateQuestionSets(questionSet);
	updateQuestions();
}

void QuappWindow::on_QuestionSetList_itemDoubleClicked(QListWidgetItem* item)
{
	QuestionSet* questionSet = currentQuestionSet();
	if (refreshing_ || (!questionSet)) return;

	bool ok;
	QString newName = QInputDialog::getText(this, "Change Question Set Name", "New Name", QLineEdit::Normal, questionSet->name(), &ok);
	if (!ok) return;

	questionSet->setName(newName);
	updateQuestionSets(questionSet);
}

/*
 * Question Data
 */

void QuappWindow::on_QuestionAddButton_clicked(bool checked)
{
	QuestionSet* questionSet = currentQuestionSet();
	if (refreshing_ || (!questionSet)) return;

	Question* newQuestion = questionSet->addQuestion();
	updateQuestions(newQuestion); 
}

void QuappWindow::on_QuestionRemoveButton_clicked(bool checked)
{
	QuestionSet* questionSet = currentQuestionSet();
	if (!questionSet) return;
	Question* question = currentQuestion();	
	if (!question) return;

	Question* newCurrentQuestion = question->next == NULL ? question->prev : question->next;
	questionSet->removeQuestion(question);

	updateQuestions(newCurrentQuestion);
}

void QuappWindow::on_QuestionMoveUpButton_clicked(bool checked)
{
	QuestionSet* questionSet = currentQuestionSet();
	if (!questionSet) return;
	Question* question = currentQuestion();	
	if (!question) return;

	if (question->prev) questionSet->moveQuestionUp(question);

	updateQuestions(question);
}

void QuappWindow::on_QuestionMoveDownButton_clicked(bool checked)
{
	QuestionSet* questionSet = currentQuestionSet();
	if (!questionSet) return;
	Question* question = currentQuestion();	
	if (!question) return;

	if (question->next) questionSet->moveQuestionDown(question);

	updateQuestions(question);
}

void QuappWindow::on_QuestionList_currentRowChanged(int row)
{
	QuestionSet* questionSet = currentQuestionSet();
	if (refreshing_ || (!questionSet)) return;
	Question* question = currentQuestion();
	updateQuestions(question);
}

void QuappWindow::on_QuestionTextQuestionEdit_editingFinished()
{
	QuestionSet* questionSet = currentQuestionSet();
	if (refreshing_ || (!questionSet)) return;
	Question* question = currentQuestion();	
	if (!question) return;

	question->setTextQuestion(ui.QuestionTextQuestionEdit->text());
	updateQuestions(question);
}

void QuappWindow::on_QuestionTextAnswerEdit_editingFinished()
{
	QuestionSet* questionSet = currentQuestionSet();
	if (refreshing_ || (!questionSet)) return;
	Question* question = currentQuestion();	
	if (!question) return;

	question->setTextAnswer(ui.QuestionTextAnswerEdit->text());
}

void QuappWindow::on_QuestionAudioQuestionSelectButton_clicked(bool checked)
{
	QuestionSet* questionSet = currentQuestionSet();
	if (refreshing_ || (!questionSet)) return;
	Question* question = currentQuestion();	
	if (!question) return;

	QString fileName = QFileDialog::getOpenFileName(this, "Select Audio File", audioDirectory_.absolutePath());
	if (fileName.isEmpty() || fileName.isNull()) return;
	audioDirectory_.setPath(fileName);

	question->setAudioQuestion(fileName);
	updateQuestions(question);
}

void QuappWindow::on_QuestionAudioAnswerSelectButton_clicked(bool checked)
{
	QuestionSet* questionSet = currentQuestionSet();
	if (refreshing_ || (!questionSet)) return;
	Question* question = currentQuestion();	
	if (!question) return;

	QString fileName = QFileDialog::getOpenFileName(this, "Select Audio File", audioDirectory_.absolutePath());
	if (fileName.isEmpty() || fileName.isNull()) return;
	audioDirectory_.setPath(fileName);

	question->setAudioAnswer(fileName);
	updateQuestions(question);
}

void QuappWindow::on_QuestionAudioQuestionPlayButton_clicked(bool checked)
{
	QuestionSet* questionSet = currentQuestionSet();
	if (refreshing_ || (!questionSet)) return;
	Question* question = currentQuestion();	
	if (!question) return;

	if (question->audioQuestionOK()) playAudio(question->audioQuestionFileName());
	else QMessageBox::warning(this, "Warning", "Audio file not found.");
}

void QuappWindow::on_QuestionAudioAnswerPlayButton_clicked(bool checked)
{
	QuestionSet* questionSet = currentQuestionSet();
	if (refreshing_ || (!questionSet)) return;
	Question* question = currentQuestion();	
	if (!question) return;

	if (question->audioAnswerOK()) playAudio(question->audioAnswerFileName());
	else QMessageBox::warning(this, "Warning", "Audio file not found.");
}

void QuappWindow::on_QuestionImageQuestionSelectButton_clicked(bool checked)
{
	QuestionSet* questionSet = currentQuestionSet();
	if (refreshing_ || (!questionSet)) return;
	Question* question = currentQuestion();	
	if (!question) return;

	QString fileName = QFileDialog::getOpenFileName(this, "Select Image", imageDirectory_.absolutePath());
	if (fileName.isEmpty() || fileName.isNull()) return;
	imageDirectory_.setPath(fileName);

	question->setImageQuestion(fileName);
	updateQuestions(question);
}

void QuappWindow::on_QuestionImageAnswerSelectButton_clicked(bool checked)
{
	QuestionSet* questionSet = currentQuestionSet();
	if (refreshing_ || (!questionSet)) return;
	Question* question = currentQuestion();	
	if (!question) return;

	QString fileName = QFileDialog::getOpenFileName(this, "Select Image", imageDirectory_.absolutePath());
	if (fileName.isEmpty() || fileName.isNull()) return;
	imageDirectory_.setPath(fileName);

	question->setImageAnswer(fileName);
	updateQuestions(question);
}

/*
 * Update
 */

// Update question set list
void QuappWindow::updateQuestionSets(QuestionSet* newCurrentQuestionSet)
{
	refreshing_ = true;

	// Update segment list
	ui.QuestionSetList->clear();
	if (newCurrentQuestionSet == NULL) newCurrentQuestionSet = quiz_.questionSets();
	for (QuestionSet* set = quiz_.questionSets(); set != NULL; set = set->next)
	{
		ui.QuestionSetList->addItem(set->name());
		if (set == newCurrentQuestionSet) ui.QuestionSetList->setCurrentRow(ui.QuestionSetList->count()-1);
	}

	ui.QuestionSetRemoveButton->setEnabled(newCurrentQuestionSet);
	ui.QuestionSetMoveUpButton->setEnabled(newCurrentQuestionSet && newCurrentQuestionSet->prev);
	ui.QuestionSetMoveDownButton->setEnabled(newCurrentQuestionSet && newCurrentQuestionSet->next);

	refreshing_ = false;
}

// Update segment data
void QuappWindow::updateQuestions(Question* newCurrentQuestion)
{
	QuestionSet* questionSet = currentQuestionSet();
	if (questionSet && (newCurrentQuestion == NULL)) newCurrentQuestion = questionSet->questions();
	ui.QuestionRemoveButton->setEnabled(questionSet && newCurrentQuestion);
	ui.QuestionMoveUpButton->setEnabled(questionSet && newCurrentQuestion && newCurrentQuestion->prev);
	ui.QuestionMoveDownButton->setEnabled(questionSet && newCurrentQuestion && newCurrentQuestion->next);
	ui.QuestionAddButton->setEnabled(questionSet);

	refreshing_ = true;

	// Update question table
	ui.QuestionList->clear();
	if (questionSet)
	{
		int count = 0;
		for (Question* question = questionSet->questions(); question != NULL; question = question->next)
		{
			ui.QuestionList->addItem(QString::number(++count)+". "+question->textQuestion());
			if (newCurrentQuestion == question) ui.QuestionList->setCurrentRow(ui.QuestionList->count()-1);
		}
	}

	// Update controls
	if (newCurrentQuestion)
	{
		ui.QuestionTextQuestionEdit->setText(newCurrentQuestion->textQuestion());
		ui.QuestionTextAnswerEdit->setText(newCurrentQuestion->textAnswer());
		ui.QuestionAudioQuestionEdit->setText(newCurrentQuestion->audioQuestionFileName());
		ui.QuestionAudioAnswerEdit->setText(newCurrentQuestion->audioAnswerFileName());
		ui.QuestionImageQuestionFileNameEdit->setText(newCurrentQuestion->imageQuestionFileName());
		if (newCurrentQuestion->imageQuestionOK())
		{
			ui.QuestionImageQuestionFileNameEdit->setPalette(ui.QuestionTextQuestionEdit->palette());
			ui.QuestionImageQuestionLabel->setPixmap(QPixmap::fromImage(newCurrentQuestion->imageQuestion()).scaled(ui.QuestionImageQuestionLabel->size(), Qt::KeepAspectRatio));
		}
		else
		{
			QPalette redText;
			redText.setColor(QPalette::Text, Qt::red);
			ui.QuestionImageQuestionFileNameEdit->setPalette(redText);
			ui.QuestionImageQuestionLabel->setPixmap(QPixmap());
		}
		ui.QuestionImageAnswerFileNameEdit->setText(newCurrentQuestion->imageAnswerFileName());
		if (newCurrentQuestion->imageAnswerOK())
		{
			ui.QuestionImageAnswerFileNameEdit->setPalette(ui.QuestionTextQuestionEdit->palette());
			ui.QuestionImageAnswerLabel->setPixmap(QPixmap::fromImage(newCurrentQuestion->imageAnswer()).scaled(ui.QuestionImageAnswerLabel->size(), Qt::KeepAspectRatio));
		}
		else
		{
			QPalette redText;
			redText.setColor(QPalette::Text, Qt::red);
			ui.QuestionImageAnswerFileNameEdit->setPalette(redText);
			ui.QuestionImageAnswerLabel->setPixmap(QPixmap());
		}
	}
	else
	{
		ui.QuestionTextQuestionEdit->clear();
		ui.QuestionTextAnswerEdit->clear();
		ui.QuestionImageQuestionLabel->setPixmap(QPixmap());
		ui.QuestionImageQuestionFileNameEdit->setText("");
		ui.QuestionImageAnswerLabel->setPixmap(QPixmap());
		ui.QuestionImageAnswerFileNameEdit->setText("");
	}

	// Enable / disable controls
	ui.QuestionTextQuestionEdit->setEnabled(newCurrentQuestion);
	ui.QuestionTextAnswerEdit->setEnabled(newCurrentQuestion);
	ui.QuestionImageQuestionSelectButton->setEnabled(newCurrentQuestion);
	ui.QuestionImageAnswerSelectButton->setEnabled(newCurrentQuestion);

	refreshing_ = false;
}
