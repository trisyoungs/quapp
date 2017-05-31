/*
	*** File I/O - Load
	*** src/gui/io_load.cpp
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
#include "base/lineparser.h"
#include "base/messenger.h"
#include "base/session.h"

/*
 * I/O
 */

// Segment Block Keywords
const char* SegmentBlockKeywords[] = { "End", "Image", "Name", "NonVisual", "QuestionSource", "Score", "SubText", "TitleText", "Type" };

// Segment Block NArguments
int SegmentBlockKeywordNArguments[] = { 0, 1, 1, 0, 1, 2, 1, 1, 1 };

// Convert text string to SegmentBlockKeyword
Quiz::SegmentBlockKeyword Quiz::segmentBlockKeyword(QString s)
{
	for (int n=0; n<nSegmentKeywords; ++n) if (s == SegmentBlockKeywords[n]) return (Quiz::SegmentBlockKeyword) n;
	return nSegmentKeywords;
}

// Convert SegmentBlockKeyword to text string
const char* Quiz::segmentBlockKeyword(Quiz::SegmentBlockKeyword kwd)
{
	return SegmentBlockKeywords[kwd];
}

// Return minimum number of expected arguments
int Quiz::segmentBlockKeywordNArguments(Quiz::SegmentBlockKeyword kwd)
{
	return SegmentBlockKeywordNArguments[kwd];
}

// Question Block Keywords
const char* QuestionBlockKeywords[] = { "AudioAnswer", "AudioQuestion", "End", "ImageAnswer", "ImageQuestion", "TextQuestion", "TextAnswer" };

// Question Block NArguments
int QuestionBlockKeywordNArguments[] = { 1, 1, 0, 1, 1, 1, 1 };

// Convert text string to QuestionBlockKeyword
Quiz::QuestionBlockKeyword Quiz::questionBlockKeyword(QString s)
{
	for (int n=0; n<nQuestionKeywords; ++n) if (s == QuestionBlockKeywords[n]) return (Quiz::QuestionBlockKeyword) n;
	return nQuestionKeywords;
}

// Convert QuestionBlockKeyword to text string
const char* Quiz::questionBlockKeyword(Quiz::QuestionBlockKeyword kwd)
{
	return QuestionBlockKeywords[kwd];
}

// Return minimum number of expected arguments
int Quiz::questionBlockKeywordNArguments(Quiz::QuestionBlockKeyword kwd)
{
	return QuestionBlockKeywordNArguments[kwd];
}

// QuestionSet Block Keywords
const char* QuestionSetBlockKeywords[] = { "End", "Question" };

// QuestionSet Block NArguments
int QuestionSetBlockKeywordNArguments[] = { 0, 0 };

// Convert text string to QuestionSetBlockKeyword
Quiz::QuestionSetBlockKeyword Quiz::questionSetBlockKeyword(QString s)
{
	for (int n=0; n<nQuestionSetKeywords; ++n) if (s == QuestionSetBlockKeywords[n]) return (Quiz::QuestionSetBlockKeyword) n;
	return nQuestionSetKeywords;
}

// Convert QuestionSetBlockKeyword to text string
const char* Quiz::questionSetBlockKeyword(Quiz::QuestionSetBlockKeyword kwd)
{
	return QuestionSetBlockKeywords[kwd];
}

// Return minimum number of expected arguments
int Quiz::questionSetBlockKeywordNArguments(Quiz::QuestionSetBlockKeyword kwd)
{
	return QuestionSetBlockKeywordNArguments[kwd];
}

// Main Block Keywords
const char* MainBlockKeywords[] = { "QuestionSet", "Segment", "Team", "Title" };

// Main Block NArguments
int MainKeywordNArguments[] = { 1, 0, 1, 1 };

// Convert text string to MainKeyword
Quiz::MainKeyword Quiz::mainKeyword(QString s)
{
	for (int n=0; n<nMainKeywords; ++n) if (s == MainBlockKeywords[n]) return (Quiz::MainKeyword) n;
	return nMainKeywords;
}

// Convert MainKeyword to text string
const char* Quiz::mainKeyword(Quiz::MainKeyword kwd)
{
	return MainBlockKeywords[kwd];
}

// Return minimum number of expected arguments
int Quiz::mainKeywordNArguments(Quiz::MainKeyword kwd)
{
	return MainKeywordNArguments[kwd];
}

// Read QuestionSet keywords
bool Quiz::readQuestionSetBlock(LineParser& parser, QuestionSet* questionSet)
{
	Quiz::QuestionSetBlockKeyword setKwd;
	Question* question;
	while (!parser.atEnd())
	{
		// Get line from file
		parser.getArgs(LineParser::UseQuotes + LineParser::SkipBlanks);

		// Get keyword and check number of arguments provided
		setKwd = Quiz::questionSetBlockKeyword(parser.argString(0));
		if ((setKwd != Quiz::nQuestionSetKeywords) && (Quiz::questionSetBlockKeywordNArguments(setKwd) > (parser.nArgs()-1)))
		{
			msg.print("Error : QuestionSet keyword '%s' requires %i arguments, but only %i have been provided.\n", Quiz::questionSetBlockKeyword(setKwd), Quiz::questionSetBlockKeywordNArguments(setKwd), parser.nArgs()-1);
			return false;
		}
		switch (setKwd)
		{
			// End block 
			case (Quiz::EndQuestionSetKeyword):
				return true;
				break;
			// Question
			case (Quiz::QuestionKeyword):
				question = questionSet->addQuestion();
				if (!readQuestionBlock(parser, question)) return false;
				break;
			// Unrecognised Keyword
			default:
				msg.print("Warning: Unrecognised QuestionSet keyword: %s\n", parser.argChar(0));
				break;
		}
	}
	msg.print("Error : Unterminated 'QuestionSet' block.\n");
	return false;
}

// Read QuestionBlock keywords
bool Quiz::readQuestionBlock(LineParser& parser, Question* question)
{
	Quiz::QuestionBlockKeyword itemKwd;
	while (!parser.atEnd())
	{
		// Get line from file
		parser.getArgs(LineParser::UseQuotes + LineParser::SkipBlanks);

		// Get keyword and check number of arguments provided
		itemKwd = Quiz::questionBlockKeyword(parser.argString(0));
		if ((itemKwd != Quiz::nQuestionKeywords) && (Quiz::questionBlockKeywordNArguments(itemKwd) > (parser.nArgs()-1)))
		{
			msg.print("Error : Question keyword '%s' requires %i arguments, but only %i have been provided.\n", Quiz::questionBlockKeyword(itemKwd), Quiz::questionBlockKeywordNArguments(itemKwd), parser.nArgs()-1);
			return false;
		}
		switch (itemKwd)
		{
			// Audio answer keyword
			case (Quiz::AudioAnswerKeyword):
				question->setAudioAnswer(parser.argString(1));
				break;
			// Audio question keyword
			case (Quiz::AudioQuestionKeyword):
				question->setAudioQuestion(parser.argString(1));
				break;
			// End block
			case (Quiz::EndQuestionKeyword):
				return true;
				break;
			// Image answer keyword
			case (Quiz::ImageAnswerKeyword):
				question->setImageAnswer(parser.argString(1));
				break;
			// Image question keyword
			case (Quiz::ImageQuestionKeyword):
				question->setImageQuestion(parser.argString(1));
				break;
			// Text answer keyword
			case (Quiz::TextAnswerKeyword):
				question->setTextAnswer(parser.argString(1));
				break;
			// Text 2 keyword
			case (Quiz::TextQuestionKeyword):
				question->setTextQuestion(parser.argString(1));
				break;
			// Unrecognised Keyword
			default:
				msg.print("Warning: Unrecognised Question keyword: %s\n", parser.argChar(0));
				break;
		}
	}
	msg.print("Error : Unterminated 'Question' block.\n");
	return false;
}

// Read SegmentBlock keywords
bool Quiz::readSegmentBlock(LineParser& parser, Segment* segment)
{
	Segment::SegmentType segmentType;
	Team* t;
	QuestionSet* set;
	while (!parser.atEnd())
	{
		// Get line from file
		parser.getArgs(LineParser::UseQuotes + LineParser::SkipBlanks);

		// Get keyword and check number of arguments provided
		Quiz::SegmentBlockKeyword segmentKwd = Quiz::segmentBlockKeyword(parser.argString(0));
		if ((segmentKwd != Quiz::nSegmentKeywords) && (Quiz::segmentBlockKeywordNArguments(segmentKwd) > (parser.nArgs()-1)))
		{
			msg.print("Error : Segment keyword '%s' requires %i arguments, but only %i have been provided.\n", Quiz::segmentBlockKeyword(segmentKwd), Quiz::segmentBlockKeywordNArguments(segmentKwd), parser.nArgs()-1);
			return false;
		}
		switch (segmentKwd)
		{
			// End block
			case (Quiz::EndSegmentKeyword):
				return true;
				break;
			// Image
			case (Quiz::ImageKeyword):
				segment->setImage(parser.argString(1));
				break;
			// Segment name
			case (Quiz::NameKeyword):
				segment->setName(parser.argString(1));
				break;
			// Non-Visual flag
			case (Quiz::NonVisualKeyword):
				segment->setNonVisual(true);
				break;
			// Question source
			case (Quiz::QuestionSourceKeyword):
				set = questionSet(parser.argString(1));
				if (!set)
				{
					msg.print("Couldn't find question set '%s' specified in segment '%s'.\n", parser.argChar(1), qPrintable(segment->name()));
				}
				else segment->setQuestionSource(set);
				break;
			// Score
			case (Quiz::ScoreKeyword):
				t = team(parser.argString(1));
				if (t) segment->setScore(t, parser.argd(2));
				else msg.print("Warning: Unrecognised team name '%s' found in score.\n", parser.argChar(1));
				break;
			// Subtext
			case (Quiz::SubTextKeyword):
				segment->setSubText(parser.argString(1));
				break;
			// Title text
			case (Quiz::TitleTextKeyword):
				segment->setTitleText(parser.argString(1));
				break;
			// Segment type
			case (Quiz::TypeKeyword):
				segmentType = Segment::segmentType(parser.argString(1));
				if (segmentType == Segment::nSegmentTypes)
				{
					msg.print("Warning: Unrecognised segment type '%s'. Defaulting to '%s'.\n", parser.argChar(1), Segment::segmentType(Segment::TitleSegment));
					segmentType = Segment::TitleSegment;
				}
				segment->setType(segmentType);
				break;
			// Unrecognised Keyword
			default:
				msg.print("Warning: Unrecognised segment keyword: %s\n", parser.argChar(0));
				break;
		}
	}
	msg.print("Error : Unterminated 'Segment' block.\n");
	return false;
}

// Load from file specified
bool Quiz::load(QString fileName)
{
	LineParser parser(fileName);

	if (!parser.ready()) return false;

	clear();

	// Read line from file and decide what to do with it
	Quiz::MainKeyword kwd;
	bool success;
	Team* team;
	Segment* segment;
	QuestionSet* set;
	while (!parser.atEnd())
	{
		parser.getArgs(LineParser::UseQuotes + LineParser::SkipBlanks);

		// We expect a block keyword in this loop...
		kwd = Quiz::mainKeyword(parser.argString(0));
		switch (kwd)
		{
			// QuestionSet
			case (Quiz::QuestionSetKeyword):
				set = questionSets_.add();
				set->setName(parser.argString(1));
				success = readQuestionSetBlock(parser, set);
				break;
			// Segment
			case (Quiz::SegmentKeyword):
				segment = segments_.add(*this);
				segment->updateScores(teams_);
				success = readSegmentBlock(parser, segment);
				break;
			// Team
			case (Quiz::TeamKeyword):
				if (segments_.nItems() != 0)
				{
					msg.print("Warning: Teams must be specified before any segments are defined. Will ignore '%s'...\n", parser.argChar(1));
					break;
				}
				team = addTeam(parser.argString(1));
				break;
			// title
			case (Quiz::TitleKeyword):
				title_ = parser.argString(1);
				break;
			default:
				msg.print("Warning: Unrecognised keyword: %s\n", parser.argChar(0));
				break;
		}

		// If we have failed for any reason, exit now
		if (!success) break;
	}
	parser.closeFiles();

	// Show a message if we encountered problems...
	if (!success) return false;
	else
	{
		Session::setInputFile(fileName);
		Session::setAsNotModified();
		updateTeamScoresAndRanks();
	}

	return true;
}

// Save quiz to file
bool Quiz::save(QString fileName)
{
	LineParser parser(fileName, true);

	if (!parser.ready()) return false;

	// Write main keywords
	parser.writeLineF("%s  '%s'\n", Quiz::mainKeyword(Quiz::TitleKeyword), qPrintable(title_));

	// Write teams
	for (Team* team = teams_.first(); team != NULL; team = team->next) parser.writeLineF("%s  '%s'\n", Quiz::mainKeyword(Quiz::TeamKeyword), qPrintable(team->name()));

	// Write question sets
	for (QuestionSet* set = questionSets_.first(); set != NULL; set = set->next)
	{
		parser.writeLineF("%s  '%s'\n", Quiz::mainKeyword(Quiz::QuestionSetKeyword), qPrintable(set->name()));
		for (Question* question = set->questions(); question != NULL; question = question->next)
		{
			parser.writeLineF("  %s\n", Quiz::questionSetBlockKeyword(Quiz::QuestionKeyword));
			parser.writeLineF("    %s  '%s'\n", Quiz::questionBlockKeyword(Quiz::TextQuestionKeyword), qPrintable(question->textQuestion()));
			parser.writeLineF("    %s  '%s'\n", Quiz::questionBlockKeyword(Quiz::TextAnswerKeyword), qPrintable(question->textAnswer()));
			parser.writeLineF("    %s  '%s'\n", Quiz::questionBlockKeyword(Quiz::AudioQuestionKeyword), qPrintable(question->audioQuestionFileName()));
			parser.writeLineF("    %s  '%s'\n", Quiz::questionBlockKeyword(Quiz::AudioAnswerKeyword), qPrintable(question->audioAnswerFileName()));
			parser.writeLineF("    %s  '%s'\n", Quiz::questionBlockKeyword(Quiz::ImageQuestionKeyword), qPrintable(question->imageQuestionFileName()));
			parser.writeLineF("    %s  '%s'\n", Quiz::questionBlockKeyword(Quiz::ImageAnswerKeyword), qPrintable(question->imageAnswerFileName()));
			parser.writeLineF("  %s\n", Quiz::questionBlockKeyword(Quiz::EndQuestionKeyword));
		}
		parser.writeLineF("%s\n", Quiz::questionSetBlockKeyword(Quiz::EndQuestionSetKeyword));
	}

	// Write segments
	for (Segment* segment = segments_.first(); segment != NULL; segment = segment->next) 
	{
		parser.writeLineF("%s\n", Quiz::mainKeyword(Quiz::SegmentKeyword));
		parser.writeLineF("  %s  '%s'\n", Quiz::segmentBlockKeyword(Quiz::ImageKeyword), qPrintable(segment->imageFileName()));
		parser.writeLineF("  %s  '%s'\n", Quiz::segmentBlockKeyword(Quiz::NameKeyword), qPrintable(segment->name()));
		if (segment->nonVisual()) parser.writeLineF("  %s\n", Quiz::segmentBlockKeyword(Quiz::NonVisualKeyword));
		parser.writeLineF("  %s  %s\n", Quiz::segmentBlockKeyword(Quiz::TypeKeyword), Segment::segmentType(segment->type()));
		if (segment->questionSource()) parser.writeLineF("  %s  '%s'\n", Quiz::segmentBlockKeyword(Quiz::QuestionSourceKeyword), qPrintable(segment->questionSource()->name()));
		for (RefListItem<Team,double>* ri = segment->scores(); ri != NULL; ri = ri->next) parser.writeLineF("  %s  '%s' %f\n", Quiz::segmentBlockKeyword(Quiz::ScoreKeyword), qPrintable(ri->item->name()), ri->data);
		parser.writeLineF("  %s  '%s'\n", Quiz::segmentBlockKeyword(Quiz::SubTextKeyword), qPrintable(segment->subText()));
		parser.writeLineF("  %s  '%s'\n", Quiz::segmentBlockKeyword(Quiz::TitleTextKeyword), qPrintable(segment->titleText()));
		parser.writeLineF("%s\n", Quiz::segmentBlockKeyword(Quiz::EndSegmentKeyword));
	}

	parser.closeFiles();

	return true;
}
