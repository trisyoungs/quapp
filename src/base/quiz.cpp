/*
	*** Quiz
	*** src/base/quiz.cpp
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

#include "base/quiz.h"
#include "base/messenger.h"
#include <QtCore/QString>

// Constructor
Quiz::Quiz()
{
	title_ = "NewQuiz";
	currentSegment_ = NULL;
}

// Destructor
Quiz::~Quiz()
{
}

// Clear all data
void Quiz::clear()
{
	segments_.clear();
	teams_.clear();
}

/*
 * Definition
 */

// Set quiz title
void Quiz::setTitle(QString title)
{
	title_ = title;
}

// Return quiz title
QString Quiz::title()
{
	return title_;
}

/*
 * Question Sets
 */

// Add question set
QuestionSet* Quiz::addQuestionSet()
{
	return questionSets_.add();
}

// Remove question set
void Quiz::removeQuestionSet(QuestionSet* questionSet)
{
	questionSets_.remove(questionSet);
}

// Return first QuestionSet in list
QuestionSet* Quiz::questionSets()
{
	return questionSets_.first();
}

// Return number of QuestionSets in list
int Quiz::nQuestionSets()
{
	return questionSets_.nItems();
}

// Return question set specified
QuestionSet* Quiz::questionSet(int index)
{
	return questionSets_[index];
}

// Return named question set specified
QuestionSet* Quiz::questionSet(QString name)
{
	for (QuestionSet* set = questionSets_.first(); set != NULL; set = set->next) if (set->name() == name) return set;
	return NULL;
}

// Move specified question set up in list (towards head)
void Quiz::moveQuestionSetUp(QuestionSet* item)
{
	questionSets_.shiftUp(item);
}

// Move specified question set down in list (towards tail)
void Quiz::moveQuestionSetDown(QuestionSet* item)
{
	questionSets_.shiftDown(item);
}

/*
 * Segments
 */

// Add segment
Segment* Quiz::addSegment(QString name)
{
	Segment* segment = segments_.add(*this);
	segment->setName(name);
	segment->setTitleText(name);
	return segment;
}

// Remove segment
void Quiz::removeSegment(Segment* segment)
{
	segments_.remove(segment);
}

// Return segments
Segment* Quiz::segments()
{
	return segments_.first();
}

// Return number of defined segments
int Quiz::nSegments()
{
	return segments_.nItems();
}

// Return segment specified
Segment* Quiz::segment(int index)
{
	return segments_[index];
}

// Return segment specified (by name)
Segment* Quiz::segment(QString name)
{
	for (Segment* segment = segments_.first(); segment != NULL; segment = segment->next) if (segment->name() == name) return segment;
	return NULL;
}

// Move specified segment up in list (towards head)
void Quiz::moveSegmentUp(Segment* segment)
{
	segments_.shiftUp(segment);
}

// Move specified segment down in list (towards tail)
void Quiz::moveSegmentDown(Segment* segment)
{
	segments_.shiftDown(segment);
}

// Update segments
void Quiz::updateSegmentScores()
{
	for (Segment* segment = segments_.first(); segment != NULL; segment = segment->next) segment->updateScores(teams_);
}

// Set current segment for display
void Quiz::setCurrentSegment(Segment* segment)
{
	currentSegment_ = segment;
}

// Return current segment
Segment* Quiz::currentSegment()
{
	return currentSegment_;
}

/*
 * Teams
 */

// Update scores / ranks of teams
void Quiz::updateTeamScoresAndRanks()
{
	// Zero all team scores
	for (Team* team = teams_.first(); team != NULL; team = team->next) team->resetScore();

	// Loop over segments...
	for (Segment* segment = segments_.first(); segment != NULL; segment = segment->next)
	{
		// If not a question segment, move on...
		if (segment->type() != Segment::QuestionSegment) continue;

		// Loop over scores for each team...
		for (RefListItem<Team,double>* ri = segment->scores(); ri != NULL; ri = ri->next)
		{
			// Safety check...
			if (!teams_.contains(ri->item))
			{
				msg.print("Error: Segment '%s' contains a score for a non-existent team.\n", qPrintable(segment->name()));
				return;
			}
			ri->item->addToScore(ri->data);
		}
	}

	// Construct rank list
	rankedTeams_.clear();
	RefList<Team,int> tempList;
	int rank = 1;
	double highScore;
	for (Team* team = teams_.first(); team != NULL; team = team->next) tempList.add(team,0);
	RefListItem<Team,int>* tempItem, *ri;
	while (tempList.nItems() > 0)
	{
		highScore = -1;
		tempItem = NULL;

		// Find highest team score in tempList
		for (ri = tempList.first(); ri != NULL; ri = ri->next)
		{
			if (ri->item->score() > highScore)
			{
				highScore = ri->item->score();
				tempItem = ri;
			}
		}

		// Add the high-scorer to the rank list, updating the rank if necessary
		if (rankedTeams_.nItems() == 0) rank = 1;
		else if (fabs(highScore-rankedTeams_.last()->item->score()) > 0.1) ++rank;
		rankedTeams_.add(tempItem->item, rank);
		tempItem->item->setRank(rank);

		// Remove tempItem from list
		tempList.remove(tempItem);
	}
}

// Return unique team name based on string provided
QString Quiz::uniqueTeamName(QString baseName)
{
	QString testName = baseName, suffix;
	int index = 0;
	do
	{
		// Add on suffix (if index > 0)
		if (index > 0) testName = baseName + " "+QString::number(index);
		++index;
	} while (team(testName));

	return testName;
}

// Add team
Team* Quiz::addTeam(QString name)
{
	Team* newTeam = teams_.add();
	newTeam->setName(uniqueTeamName(name));

	updateSegmentScores();
	updateTeamScoresAndRanks();

	return newTeam;
}

// Remove team
void Quiz::removeTeam(Team* team)
{
	teams_.remove(team);

	updateSegmentScores();
	updateTeamScoresAndRanks();
}

// Return number of defined teams
int Quiz::nTeams()
{
	return teams_.nItems();
}

// Return list of teams
Team* Quiz::teams()
{
	return teams_.first();
}

// Return nth team in list
Team* Quiz::team(int n)
{
	return teams_[n];
}

// Return team with name specified
Team* Quiz::team(QString name)
{
	for (Team* team = teams_.first(); team != NULL; team = team->next) if (name == team->name()) return team;
	return NULL;
}

// Return team first in current rank / score order
RefListItem<Team,int>* Quiz::firstRankedTeam()
{
	return rankedTeams_.first();
}

// Return team last in current rank / score order
RefListItem<Team,int>* Quiz::lastRankedTeam()
{
	return rankedTeams_.last();
}