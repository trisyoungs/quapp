/*
	*** Segment
	*** src/base/segment.h
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

#ifndef QUAPP_SEGMENT_H
#define QUAPP_SEGMENT_H

#include "base/questionset.h"
#include "templates/list.h"
#include "templates/reflist.h"
#include "render/displayobject.h"
#include <QtCore/QString>

// Forward Declarations
class QuappWindow;
class Quiz;
class Team;

// Segment
class Segment : public ListItem<Segment>
{
	public:
	// Constructor / Destructor
	Segment(Quiz& parent);
	~Segment();
	// Segment Type
	enum SegmentType { AnswerSegment, QuestionSegment, ScoreSegment, TitleSegment, nSegmentTypes };
	// Convert text string to SegmentType
	static SegmentType segmentType(QString s);
	// Convert SegmentType to text string
	static const char* segmentType(SegmentType id);


	/*
	 * Parent
	 */
	private:
	// Quiz parent
	Quiz& parent_;


	/*
	 * Definition
	 */
	private:
	// Name of segment
	QString name_;
	// Title text for the segment (if relevant)
	QString titleText_;
	// Subtext for the segment (if relevant)
	QString subText_;
	// Image filename  for the segment (if relevant)
	QString imageFileName_;
	// Image for segment (if relevant)
	QImage image_;
	// Whether image was loaded successfully
	bool imageOK_;
	// Segment type
	SegmentType type_;
	// Source QuestionSet
	QuestionSet* questionSource_;
	// Whether Segment is hidden (contributes to score, but has no role in running order)
	bool nonVisual_;

	public:
	// Set name of segment
	void setName(QString name);
	// Return name of segment
	QString name();
	// Set title text for the segment
	void setTitleText(QString text);
	// Return title text for the segment
	QString titleText();
	// Set subtext for the segment
	void setSubText(QString subtext);
	// Return subtext for the segment
	QString subText();
	// Set image (from filename)
	void setImage(QString fileName);
	// Return image filename
	QString imageFileName();
	// Return whether image is valid
	bool imageOK();
	// Return image 
	QImage& image();
	// Set segment type
	void setType(Segment::SegmentType type);
	// Return segment type
	Segment::SegmentType type();
	// Set source QuestionSet
	void setQuestionSource(QuestionSet* set);
	// Return source QuestionSet
	QuestionSet* questionSource();
	// Set whether Segment is hidden (contributes to score, but has no role in running order)
	void setNonVisual(bool nonVisual);
	// Return whether Segment is hidden (contributes to score, but has no role in running order)
	bool nonVisual();


	/*
	 * Segment Options
	 */
	private:
	// Whether to display question texts in pure text question rounds
	bool textQuestionDisplayText_;
	// Whether to display question index or question text when displaying image questions
	bool imageQuestionDisplayIndex_;
	// Size modifier for body text in segment
	double bodyTextSizeModifier_;

	public:
	// Set whether to display question texts in pure text question rounds
	void setTextQuestionDisplayText(bool b);
	// Return whether to display question texts in pure text question rounds
	bool textQuestionDisplayText();
	// Set whether to display question index or question text when displaying image questions
	void setImageQuestionDisplayIndex(bool b);
	// Return whether to display question index or question text when displaying image questions
	bool imageQuestionDisplayIndex();
	// Set size modifier for body text in segment
	void setBodyTextSizeModifier(double modifier);
	// Return size modifier for body text in segment
	double bodyTextSizeModifier();


	/*
	 * Scores
	 */
	private:
	// List of teams/scores
	RefList<Team,double> scores_;

	public:
	// Update scores list with current team list
	void updateScores(const List<Team>& currentTeams);
	// Set score for specified team
	bool setScore(Team* team, double score);
	// Return list of scores
	RefListItem<Team,double>* scores();


	/*
	 * Segment Run
	 */
	private:
	// Current question to display (if any)
	Question* currentQuestion_;
	// Current rank/score to display (if any)
	RefListItem<Team,int>* currentTeamScore_;
	// List of lines to use when displaying data text for segment
	RefList<DisplayObject,int> displayLines_;
	// Next display line to use
	RefListItem<DisplayObject,int>* displayLine_;

	public:
	// Begin segment, creating title object
	void begin(QuappWindow& quapp);
	// Create objects to show part of segment
	bool showNextPart(QuappWindow& quapp);
	// Return whether there is another part after this one
	bool hasNextPart();
};

#endif
