/*
	*** Quiz
	*** src/base/quiz.h
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

#ifndef QUAPP_QUIZ_H
#define QUAPP_QUIZ_H

#include "base/questionset.h"
#include "base/segment.h"
#include "base/team.h"
#include "templates/list.h"
#include <QtCore/QString>

// Forward Declarations
class LineParser;

// Quiz
class Quiz
{
	public:
	// Constructor / Destructor
	Quiz();
	~Quiz();
	// Clear all data
	void clear();


	/*
	 * Definition
	 */
	private:
	// Quiz title
	QString title_;

	public:
	// Set quiz title
	void setTitle(QString title);
	// Return quiz title
	QString title();


	/*
	 * Question Sets
	 */
	private:
	// List of defined question sets
	List<QuestionSet> questionSets_;

	public:
	// Add question set
	QuestionSet* addQuestionSet();
	// Remove question set
	void removeQuestionSet(QuestionSet* questionSet);
	// Return first QuestionSet in list
	QuestionSet* questionSets();
	// Return number of QuestionSets in list
	int nQuestionSets();
	// Return question set specified
	QuestionSet* questionSet(int index);
	// Return named question set specified
	QuestionSet* questionSet(QString name);
	// Move specified question set up in list (towards head)
	void moveQuestionSetUp(QuestionSet* item);
	// Move specified question set down in list (towards tail)
	void moveQuestionSetDown(QuestionSet* item);


	/*
	 * Segments
	 */
	private:
	// List of segments
	ParentList<Segment,Quiz> segments_;
	// Current segment (for display)
	Segment* currentSegment_;

	public:
	// Add segment
	Segment* addSegment(QString name);
	// Remove segment
	void removeSegment(Segment* segment);
	// Return segments
	Segment* segments();
	// Return number of defined segments
	int nSegments();
	// Return segment specified
	Segment* segment(int index);
	// Return segment specified (by name)
	Segment* segment(QString name);
	// Move specified segment up in list (towards head)
	void moveSegmentUp(Segment* segment);
	// Move specified segment down in list (towards tail)
	void moveSegmentDown(Segment* segment);
	// Update segments
	void updateSegmentScores();
	// Set current segment for display
	void setCurrentSegment(Segment* segment);
	// Return current segment
	Segment* currentSegment();


	/*
	 * Teams
	 */
	private:
	// List of teams
	List<Team> teams_;
	// Ranked list of teams
	RefList<Team,int> rankedTeams_;

	public:
	// Return unique team name based on string provided
	QString uniqueTeamName(QString baseName);
	// Add team
	Team* addTeam(QString name = QString());
	// Remove team
	void removeTeam(Team* team);
	// Return number of defined teams
	int nTeams();
	// Return list of teams
	Team* teams();
	// Return nth team in list
	Team* team(int n);
	// Return team with name specified
	Team* team(QString name);
	// Update scores / ranks of teams
	void updateTeamScoresAndRanks();
	// Return team first in current rank / score order
	RefListItem<Team,int>* firstRankedTeam();
	// Return team last in current rank / score order
	RefListItem<Team,int>* lastRankedTeam();


	/*
	 * I/O
	 */
	public:
	// SegmentBlockKeyword Enum
	enum SegmentBlockKeyword
	{
		EndSegmentKeyword,
		ImageKeyword,
		NameKeyword,
		NonVisualKeyword,
		QuestionSourceKeyword,
		ScoreKeyword,
		SubTextKeyword,
		TitleTextKeyword,
		TypeKeyword,
		nSegmentKeywords
	};
	// Convert text string to SegmentBlockKeyword
	SegmentBlockKeyword segmentBlockKeyword(QString s);
	// Convert SegmentBlockKeyword to text string
	const char* segmentBlockKeyword(SegmentBlockKeyword kwd);
	// Return minimum number of expected arguments
	int segmentBlockKeywordNArguments(SegmentBlockKeyword kwd);

	// QuestionBlockKeyword Enum
	enum QuestionBlockKeyword
	{
		AudioAnswerKeyword,
		AudioQuestionKeyword,
		EndQuestionKeyword,
		ImageAnswerKeyword,
		ImageQuestionKeyword,
		TextAnswerKeyword,
		TextQuestionKeyword,
		nQuestionKeywords
	};
	// Convert text string to SegmentBlockKeyword
	QuestionBlockKeyword questionBlockKeyword(QString s);
	// Convert QuestionBlockKeyword to text string
	const char* questionBlockKeyword(QuestionBlockKeyword kwd);
	// Return minimum number of expected arguments
	int questionBlockKeywordNArguments(QuestionBlockKeyword kwd);

	// QuestionSetBlockKeyword Enum
	enum QuestionSetBlockKeyword
	{
		EndQuestionSetKeyword,
		QuestionKeyword,
		nQuestionSetKeywords
	};
	// Convert text string to QuestionSetBlockKeyword
	QuestionSetBlockKeyword questionSetBlockKeyword(QString s);
	// Convert QuestionSetBlockKeyword to text string
	const char* questionSetBlockKeyword(QuestionSetBlockKeyword kwd);
	// Return minimum number of expected arguments
	int questionSetBlockKeywordNArguments(QuestionSetBlockKeyword kwd);

	// Main Keywords Enum
	enum MainKeyword
	{
		QuestionSetKeyword,
		SegmentKeyword,
		TeamKeyword,
		TitleKeyword,
		nMainKeywords
	};
	// Convert text string to MainKeyword
	MainKeyword mainKeyword(QString s);
	// Convert MainKeyword to text string
	const char* mainKeyword(MainKeyword kwd);
	// Return minimum number of expected arguments
	int mainKeywordNArguments(MainKeyword kwd);

	private:
	// Read QuestionSetBlock keywords
	bool readQuestionSetBlock(LineParser& parser, QuestionSet* questionSetItem);
	// Read QuestionBlock keywords
	bool readQuestionBlock(LineParser& parser, Question* questionItem);
	// Read SegmentBlock keywords
	bool readSegmentBlock(LineParser& parser, Segment* segment);

	public:
	// Load from file
	bool load(QString fileName);
	// Save to file
	bool save(QString fileName);
};

#endif
