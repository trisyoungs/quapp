/*
	*** Quapp - Run 
	*** src/gui/quapp_run.cpp
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
#include "templates/variantpointer.h"
#include <QtGui/QFileDialog>
#include <QtGui/QMessageBox>
#include <QtGui/QInputDialog>

/*
 * Teams
 */

// Return current questionset
Team* QuappWindow::currentTeam()
{
	if (ui.TeamTable->currentRow() == -1) return NULL;
	return quiz_.team(ui.TeamTable->currentRow());
}

void QuappWindow::on_TeamTable_currentItemChanged(QTableWidgetItem* item, QTableWidgetItem* prevItem)
{
	Team* team = currentTeam();
	ui.TeamRemoveButton->setEnabled(team);
}

void QuappWindow::on_TeamTable_itemDoubleClicked(QTableWidgetItem* item)
{
	Team* team = currentTeam();
	if (refreshing_ || (!team)) return;

	bool ok;
	QString newName = QInputDialog::getText(this, "Change Team Name", "New Name", QLineEdit::Normal, team->name(), &ok);
	if (!ok) return;

	team->setName(newName);

	updateRunData();
}

void QuappWindow::on_TeamTable_itemChanged(QTableWidgetItem* item)
{
	if (refreshing_ || (!item)) return;

	// Get header item so we can get the Segment name
	QTableWidgetItem* headerItem = ui.TeamTable->horizontalHeaderItem(item->column());
	if (!headerItem)
	{
		printf("Couldn't find headerItem so can't get Segment.\n");
		return;
	}
	Segment* segment = quiz_.segment(headerItem->text());
	if (!segment) return;

	// Get Team from row and Segment from item
	Team* team = VariantPointer<Team>(item->data(Qt::UserRole));
	if (!team) return;

	segment->setScore(team, item->text().toDouble());
	quiz_.updateTeamScoresAndRanks();

	updateRunData();
}

void QuappWindow::on_TeamAddButton_clicked(bool checked)
{
	QString startText = "New Team " + QString::number(quiz_.nTeams()+1);
	bool ok;
	QString name = QInputDialog::getText(this, "New Team", "Enter team name:", QLineEdit::Normal, startText, &ok);
	if (!ok) return;

	quiz_.addTeam(name);

	updateRunData();
}

void QuappWindow::on_TeamRemoveButton_clicked(bool checked)
{
	Team* team = currentTeam();
	if (refreshing_ || (!team)) return;

	quiz_.removeTeam(team);
}

/*
 * Control
 */

void QuappWindow::on_RunFromStartButton_clicked(bool checked)
{
	// Clear all foreground primitives
	foregroundObjects_.clear();
	persistentForegroundObjects_.clear();

	// Get first segment
	quiz_.setCurrentSegment(quiz_.segments());

	// Run from this segment
	if (quiz_.currentSegment())
	{
		quiz_.currentSegment()->begin(*this);
	}

	// Update display
	updateRunControls();
	displayWindow_.ui.MainView->postRedisplay();
}

void QuappWindow::on_RunFromSegmentButton_clicked(bool checked)
{
	// Clear all foreground primitives
	foregroundObjects_.clear();
	persistentForegroundObjects_.clear();

	// Get current segment
	Segment* segment = quiz_.segment(ui.SegmentsList->currentRow());
	quiz_.setCurrentSegment(segment);
	if (!segment) return;

	// Run from this segment
	quiz_.currentSegment()->begin(*this);

	// Update GUI and display
	updateRunControls();
	displayWindow_.ui.MainView->postRedisplay();
}

void QuappWindow::on_NextSegmentButton_clicked(bool checked)
{
	if (quiz_.currentSegment() == NULL) return;

	// First, check if we still have remaining items in the current section
	if (quiz_.currentSegment()->hasNextPart())
	{
		if (QMessageBox::warning(this, "Warning", "Items still remain in the current segment. Do you really want to move on to the next segment?", QMessageBox::Yes | QMessageBox::No, QMessageBox::No) == QMessageBox::No) return; 
	}

	// Get next segment
	Segment* nextSegment = quiz_.currentSegment()->next;
	while (nextSegment && nextSegment->nonVisual()) nextSegment = nextSegment->next;
	quiz_.setCurrentSegment(nextSegment);

	// Set all existing foreground and title objects to transition out
	applyExitTransform(foregroundObjects_, QuappWindow::FadeOutTransform, 0.5, 0.999);
	applyExitTransform(persistentForegroundObjects_, QuappWindow::FadeOutTransform, 0.05, 0.999);

	// Begin next segment
	quiz_.currentSegment()->begin(*this);

	// Update GUI and display
	updateRunControls();
	displayWindow_.ui.MainView->postRedisplay();

	// Set state of TeamTable (to prevent accidental modification of scores/ranks during score display)
	ui.TeamTable->setEnabled(quiz_.currentSegment()->type() != Segment::ScoreSegment);
}

void QuappWindow::on_NextItemButton_clicked(bool checked)
{
	Segment* currentSegment = quiz_.currentSegment();
	if (currentSegment == NULL) return;

	// First, check if we still have remaining items in the current section
	if (!currentSegment->hasNextPart()) return;

	currentSegment->showNextPart(*this);

	// Update GUI and display
	updateRunControls();
	displayWindow_.ui.MainView->postRedisplay();
}

void QuappWindow::on_EndButton_clicked(bool checked)
{
	quiz_.setCurrentSegment(NULL);

	updateRunControls();
}

void QuappWindow::on_NextAnythingButton_clicked(bool checked)
{
	// Move on to next item in segment, or next segment if there is no next item
	Segment* currentSegment = quiz_.currentSegment();
	if (currentSegment == NULL) return;

	if (currentSegment->hasNextPart()) currentSegment->showNextPart(*this);
	else if (currentSegment->next) on_NextSegmentButton_clicked(false);
	else quiz_.setCurrentSegment(NULL);

	updateRunControls();
}

/*
 * Update
 */

// Update teams / scores data
void QuappWindow::updateRunData()
{
	refreshing_ = true;

	ui.TeamTable->clear();
	ui.TeamTable->setRowCount(quiz_.nTeams());
	ui.TeamTable->setColumnCount(quiz_.nSegments());
	int row, column = 0;
	QTableWidgetItem* item;
	QBrush brush(Qt::BDiagPattern);
	QStringList horizontalLabels;
	for (Segment* segment = quiz_.segments(); segment != NULL; segment = segment->next)
	{
		// If this is not a question segment, nothing to do here...
		if (segment->type() != Segment::QuestionSegment) continue;

		row = 0;
		for (RefListItem<Team,double>* score = segment->scores(); score != NULL; score = score->next)
		{
			item = new QTableWidgetItem();
			item->setData(Qt::UserRole, VariantPointer<Team>(score->item));
			// Set flags for this segment
			if (segment->type() == Segment::QuestionSegment)
			{
				item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsEditable | Qt::ItemIsSelectable);
				item->setText(QString::number(score->data));
			}
			else
			{
				item->setFlags(Qt::ItemIsSelectable);
				item->setBackground(brush);
			}
			ui.TeamTable->setItem(row, column, item);

			++row;
		}

		// If this is column zero, set the column headers here too
		if (column == 0)
		{
			QStringList verticalItems;
			for (RefListItem<Team,double>* score = segment->scores(); score != NULL; score = score->next) verticalItems << score->item->name();
			ui.TeamTable->setVerticalHeaderLabels(verticalItems);
		}

		horizontalLabels << segment->name();
		++column;
	}

	// Set horizontal headers
	ui.TeamTable->setHorizontalHeaderLabels(horizontalLabels);
	ui.TeamTable->setColumnCount(column);

	// Update rank list
	ui.RankList->clear();
	QString text;
	for (RefListItem<Team,int>* ri = quiz_.firstRankedTeam(); ri != NULL; ri = ri->next)
	{
		text = QString::number(ri->data) + " (" + QString::number(ri->item->score(), 'f', 1) + ") " + ri->item->name();
		ui.RankList->addItem(text);
	}
	
	refreshing_ = false;
}

void QuappWindow::updateRunControls()
{
	// Run controls
	ui.RunFromSegmentButton->setEnabled((ui.SegmentsList->currentRow() != -1) && (!quiz_.currentSegment()));
	ui.RunFromStartButton->setEnabled(!quiz_.currentSegment());
	ui.EndButton->setEnabled(quiz_.currentSegment());

	// Step controls
	if (quiz_.currentSegment())
	{
		ui.NextSegmentButton->setEnabled(quiz_.currentSegment()->next);
		ui.NextItemButton->setEnabled(quiz_.currentSegment()->hasNextPart());
	}
	else
	{
		ui.NextSegmentButton->setEnabled(false);
		ui.NextItemButton->setEnabled(false);
	}
}
