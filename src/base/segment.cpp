/*
	*** Segment
	*** src/base/segment.cpp
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

#include "base/segment.h"
#include "base/team.h"
#include "base/messenger.h"
#include "render/displayobject.h"
#include "render/fontinstance.h"
#include "gui/quapp.h"
#include "templates/reflist.h"
#include <QtCore/QString>

// SegmentType Keywords
const char* SegmentTypeKeywords[] = { "Answer", "Question", "Score", "Title" };

// Convert text string to SegmentType
Segment::SegmentType Segment::segmentType(QString s)
{
	for (int n=0; n<Segment::nSegmentTypes; ++n) if (s == SegmentTypeKeywords[n]) return (Segment::SegmentType) n;
	return Segment::nSegmentTypes;
}

// Convert SegmentType to text string
const char* Segment::segmentType(Segment::SegmentType rt)
{
	return SegmentTypeKeywords[rt];
}

// Constructor
Segment::Segment(Quiz& parent) : parent_(parent)
{
	name_ = "New Segment";
	titleText_ = "New Segment";
	imageOK_ = false;
	type_ = Segment::TitleSegment;
	currentQuestion_ = NULL;
	questionSource_ = NULL;
	nonVisual_ = false;

	// Display Options
	bodyTextSizeModifier_ = 1.0;
	textQuestionDisplayText_ = false;
	imageQuestionDisplayIndex_ = true;
}

// Destructor
Segment::~Segment()
{
}

/*
 * Definition
 */

// Set name of segment
void Segment::setName(QString name)
{
	name_ = name;
}

// Return name of segment
QString Segment::name()
{
	return name_;
}

// Set title text of segment
void Segment::setTitleText(QString text)
{
	titleText_ = text;
}

// Return title text of segment
QString Segment::titleText()
{
	return titleText_;
}

// Set sub text of segment
void Segment::setSubText(QString subtext)
{
	subText_ = subtext;
}

// Return sub text of segment
QString Segment::subText()
{
	return subText_;
}

// Set image (from filename)
void Segment::setImage(QString fileName)
{
	imageFileName_ = fileName;
	imageOK_ = image_.load(fileName);
}

// Return image filename
QString Segment::imageFileName()
{
	return imageFileName_;
}

// Return whether image is valid
bool Segment::imageOK()
{
	return imageOK_;
}

// Return image
QImage& Segment::image()
{
	return image_;
}

// Set segment type
void Segment::setType(Segment::SegmentType type)
{
	type_ = type;
}

// Return segment type
Segment::SegmentType Segment::type()
{
	return type_;
}

// Set source QuestionSet
void Segment::setQuestionSource(QuestionSet* set)
{
	questionSource_ = set;
}

// Return source QuestionSet
QuestionSet* Segment::questionSource()
{
	return questionSource_;
}

// Set whether Segment is hidden (contributes to score, but has no role in running order)
void Segment::setNonVisual(bool nonVisual)
{
	nonVisual_ = nonVisual;
}

// Return whether Segment is hidden (contributes to score, but has no role in running order)
bool Segment::nonVisual()
{
	return nonVisual_;
}

/*
 * Options
 */

// Set whether to display question texts in pure text question rounds
void Segment::setTextQuestionDisplayText(bool b)
{
	textQuestionDisplayText_ = b;
}

// Return whether to display question texts in pure text question rounds
bool Segment::textQuestionDisplayText()
{
	return textQuestionDisplayText_;
}

// Set whether to display question index or question text when displaying image questions
void Segment::setImageQuestionDisplayIndex(bool b)
{
	imageQuestionDisplayIndex_ = b;
}

// Return whether to display question index or question text when displaying image questions
bool Segment::imageQuestionDisplayIndex()
{
	return imageQuestionDisplayIndex_;
}

// Set size modifier for body text in segment
void Segment::setBodyTextSizeModifier(double modifier)
{
	bodyTextSizeModifier_ = modifier;
}

// Return size modifier for body text in segment
double Segment::bodyTextSizeModifier()
{
	return bodyTextSizeModifier_;
}

/*
 * Scores
 */

// Update scores list with current team list
void Segment::updateScores(const List<Team>& currentTeams)
{
	// Copy existing reflist of teams/scores, and clear original list (we will copy back valid entries)
	RefList<Team,double> extra = scores_;
	scores_.clear();
	RefListItem<Team,double>* item;

	// Loop over list of teams supplied, adding those which don't currently exist to our list
	for (Team* team = currentTeams.first(); team != NULL; team = team->next)
	{
		item = extra.contains(team);
		if (item)
		{
			extra.cut(item);
			scores_.own(item);
		}
		else scores_.add(team, 0.0);
	}
}

// Set score for specified team
bool Segment::setScore(Team* team, double score)
{
	// Find entry for specified team...
	RefListItem<Team,double>* ri = scores_.contains(team);
	if (!ri)
	{
		msg.print("Internal Error: Couldn't find team %p in scores list...\n", team);
		return false;
	}
	else ri->data = score;

	return true;
}

// Return list of scores
RefListItem<Team,double>* Segment::scores()
{
	return scores_.first();
}

/*
 * Segment Run
 */

// Begin segment
void Segment::begin(QuappWindow& quapp)
{
	DisplayObject* object;
	Primitive* primitive;
	QColor colourA, colourB;
	double aspectRatio = quapp.displayAspectRatio(), titleLineHeight = quapp.titleLineHeight(), fgMargin = quapp.foregroundMargin();
	double maxTextWidth = -1.0, delta;

	// Setup necessary objects
	switch (type_)
	{
		case (Segment::TitleSegment):
			// Do we have an image to display?
			if (imageOK_)
			{
				// Create object for image
				object = quapp.addForeGroundObject();
				primitive = object->primitive().addPrimitive(6, 0, GL_TRIANGLES, false, true);
				primitive->setTextureImage(image_);
				double imageAspect = double(image_.width()) / double(image_.height());
				primitive->rectangle(0.0, 0.0, 0.0, 0.8*imageAspect, 0.8, true);
				colourA.setRgbF(1.0, 1.0, 1.0, 0.0);
				colourB.setRgbF(1.0, 1.0, 1.0, 1.0);
				object->setPosition(0.0, 0.0, -0.5);
				object->addTransform()->setRotationTransform(1, 90.0, 0.0, 0.05);
				object->addTransform()->setColourTransform(colourA, colourB, 0.05);
				object->initialiseTransforms();
			}

			// Add text
			object = quapp.addTitleObject();
			object->setPosition(0.0, 0.6*quapp.titleLineHeight(), 0.0);
			object->textPrimitive().set(titleText_, TextPrimitive::CentralAnchor);
			colourA = Qt::black;
			colourA.setAlphaF(0.0);
			colourB = Qt::black;
			object->addTransform()->setColourTransform(colourA, colourB, 0.01);
			object->initialiseTransforms();
			if (!subText_.isEmpty())
			{
				object = quapp.addTitleObject();
				object->setPosition(0.0, -0.6*quapp.titleLineHeight(), 0.0);
				object->textPrimitive().set(subText_, TextPrimitive::CentralAnchor);
				object->addTransform()->setColourTransform(colourA, colourB, 0.01);
				object->initialiseTransforms();
			}
			break;
		case (Segment::ScoreSegment):
			// Add background fade for title area
			object = quapp.addPersistentForeGroundObject();
			object->setPosition(-0.5, 0.5, -0.0001);
			primitive = object->primitive().addPrimitive(6, 0, GL_TRIANGLES, true, false);
			primitive->defineVertexNC(0.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.8, 0.1);
			primitive->defineVertexNC(aspectRatio, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.8, 0.1);
			primitive->defineVertexNC(aspectRatio, -(titleLineHeight+fgMargin), 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.8, 0.1);
			primitive->defineVertexNC(aspectRatio, -(titleLineHeight+fgMargin), 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.8, 0.1);
			primitive->defineVertexNC(0.0, -(titleLineHeight+fgMargin), 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.8, 0.1);
			primitive->defineVertexNC(0.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.8, 0.1);
			object->initialiseTransforms();
			// Add title object
			object = quapp.addTitleObject();
			object->textPrimitive().set(titleText_, TextPrimitive::TopLeftAnchor);
			colourA = Qt::black;
			colourA.setAlphaF(0.0);
			colourB = Qt::black;
			object->addTransform()->setColourTransform(colourA, colourB, 0.01);

			// Get some lines for text display, and add on the text to display (all objects will be hidden to start with)
			displayLines_ = quapp.addForeGroundTextLines();
			// -- We will start on the n'th line in the list where n is the number of teams
			displayLine_ = displayLines_[quapp.quiz().nTeams()];
			if (!displayLine_) displayLine_ = displayLines_.last();
			
			// Get some body text primitives
			for (RefListItem<Team,int>* ri = quapp.quiz().lastRankedTeam(); ri != NULL; ri = ri->prev)
			{
				// Check object
				object = displayLine_->item;
				if (!object)
				{
					printf("Internal Error: NULL DisplayObject found in reflist.\n");
					return;
				}
				
				// Set text
				QString text = QString::number(ri->item->score(), 'f', 1) + "  " + ri->item->name();
				object->textPrimitive().set(text, TextPrimitive::TopLeftAnchor);
				double textWidth = object->textPrimitive().textWidth() * object->textSize();
				if (textWidth > maxTextWidth) maxTextWidth = textWidth;

				// Move on to previous display line and team score
				displayLine_ = displayLine_->prev;
			}

			// Adjust left edge of lines to centre text, and apply dynamic transforms
			// -- Get pixel width of longest text line by multiplying by display height (against which font size is calculated)
			maxTextWidth *= quapp.displayHeight();
			// -- Calculate normalised position delta (i.e. between 0.0 and 1.0) to apply
			delta = (quapp.displayWidth() - maxTextWidth - 2.0*quapp.foregroundMargin()*quapp.displayWidth()) * 0.5 / quapp.displayWidth();
			colourA.setRgbF(0.0, 0.0, 0.0, 0.0);
			colourB.setRgbF(0.0, 0.0, 0.0, 1.0);
			for (RefListItem<DisplayObject,int>* ri = displayLines_.last(); ri != NULL; ri = ri->prev)
			{
				if (delta > 0) ri->item->adjustPosition(delta, 0.0, 0.0);
				ri->item->adjustPosition(1.0,0.0,0.0);
			}

			// Reset displayLine_
			displayLine_ = displayLines_[quapp.quiz().nTeams()];
			if (!displayLine_) displayLine_ = displayLines_.last();
			break;
		case (Segment::QuestionSegment):
		case (Segment::AnswerSegment):
			// Add background fade for title area
			object = quapp.addPersistentForeGroundObject();
			object->setPosition(-0.5, 0.5, -0.0001);
			primitive = object->primitive().addPrimitive(6, 0, GL_TRIANGLES, true, false);
			primitive->defineVertexNC(0.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.8, 0.1);
			primitive->defineVertexNC(aspectRatio, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.8, 0.1);
			primitive->defineVertexNC(aspectRatio, -(titleLineHeight+fgMargin), 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.8, 0.1);
			primitive->defineVertexNC(aspectRatio, -(titleLineHeight+fgMargin), 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.8, 0.1);
			primitive->defineVertexNC(0.0, -(titleLineHeight+fgMargin), 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.8, 0.1);
			primitive->defineVertexNC(0.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.8, 0.1);
			object->initialiseTransforms();
			// Add title object
			object = quapp.addTitleObject();
			object->textPrimitive().set(titleText_, TextPrimitive::TopLeftAnchor);
			colourA = Qt::black;
			colourA.setAlphaF(0.0);
			colourB = Qt::black;
			object->addTransform()->setColourTransform(colourA, colourB, 0.01);

			// Do we have an image to display?
			if (imageOK_)
			{
				// Create object for image
				object = quapp.addForeGroundObject();
				primitive = object->primitive().addPrimitive(6, 0, GL_TRIANGLES, false, true);
				primitive->setTextureImage(image_);
				double imageAspect = double(image_.width()) / double(image_.height());
				primitive->rectangle(0.0, 0.0, 0.0, 0.8*imageAspect, 0.8, true);
				colourA.setRgbF(1.0, 1.0, 1.0, 0.0);
				colourB.setRgbF(1.0, 1.0, 1.0, 1.0);
				object->setPosition(0.0, 0.0, -0.01);
				object->addTransform()->setRotationTransform(1, 90.0, 0.0, 0.05);
				object->addTransform()->setColourTransform(colourA, colourB, 0.05);
				object->initialiseTransforms();
			}
			break;
	}

	currentQuestion_ = questionSource_ ? questionSource_->questions() : NULL;
	currentTeamScore_ = quapp.quiz().lastRankedTeam();
}

// Create objects to show next part
bool Segment::showNextPart(QuappWindow& quapp)
{
	// Exactly what we do here depends on the segment type...
	DisplayObject* object;
	Primitive* primitive;
	Matrix matrixA;
	QColor colourA, colourB;
	QString text;
	int row = 0;
	double aspectRatio = quapp.displayAspectRatio(), displayWidth = quapp.displayWidth(), fgMargin = quapp.foregroundMargin();
	Vec3<double> pos;
	double maxTextWidth = 0.0, textWidth;
	switch (type_)
	{
		// Reveal answers from source QuestionSet
		case (Segment::AnswerSegment):
			if (!questionSource_) break;
			if (!currentQuestion_) return false;
			if (currentQuestion_->imageAnswerOK())
			{
				// Create object for image
				object = quapp.addForeGroundObject();
				primitive = object->primitive().addPrimitive(6, 0, GL_TRIANGLES, false, true);
				primitive->setTextureImage(currentQuestion_->imageAnswer());
				double imageAspect = double(currentQuestion_->imageAnswer().width()) / double(currentQuestion_->imageAnswer().height());
				if (imageAspect > 1.0) primitive->rectangle(0.0, 0.0, 0.0, 0.8*imageAspect, 0.8, true);
				else primitive->rectangle(0.0, 0.0, 0.0, 0.8, 0.8*imageAspect, true);
				colourA.setRgbF(1.0, 1.0, 1.0, 0.0);
				colourB.setRgbF(1.0, 1.0, 1.0, 1.0);
				object->setPosition(0.0, 0.0, -0.5);
				object->addTransform()->setRotationTransform(1, 90.0, 0.0, 0.05);
				object->addTransform()->setColourTransform(colourA, colourB, 0.05);
				object->initialiseTransforms();

				// Create object for text
				object = quapp.addForeGroundObject();
				colourA.setRgbF(0.0, 0.0, 0.0, 0.0);
				colourB.setRgbF(0.0, 0.0, 0.0, 1.0);
				text = currentQuestion_->textAnswer();
				object->setTextSize(quapp.bodyTextHeight()*bodyTextSizeModifier_);
				object->textPrimitive().set(text, TextPrimitive::TopMiddleAnchor);
				object->setPosition(0.0, -0.31, -0.5);
				object->addTransform()->setRotationTransform(1, 90.0, 0.0, 0.05);
				object->addTransform()->setColourTransform(colourA, colourB, 0.05);
				object->initialiseTransforms();
			}
			else
			{
				// Transition out any previous objects
				for (object = quapp.foregroundObjects().first(); object != NULL; object = object->next)
				{
					pos = object->position();
					pos.z += 5.0;
					object->addTransform()->setTranslationTransform(object->position(), pos, 0.05, 0.5);
					object->addTransform()->setRotationTransform(1, 0.0, -90.0, 0.05);
					colourA = object->colour();
					colourB = colourA;
					colourB.setAlphaF(0.0);
					object->addTransform()->setColourTransform(colourA, colourB, 0.05);
					object->setDeleteAfterTransform();
				}

				// Get some body text primitives
				RefList<DisplayObject,int> lines = quapp.addForeGroundTextLines();
				RefListItem<DisplayObject,int>* ri = lines.first();
				bool question = true;
				while (ri != NULL)
				{
					// Check object
					object = ri->item;
					if (!object)
					{
						printf("Internal Error: NULL DisplayObject found in reflist.\n");
						return false;
					}
					// If this is a question, make sure we have at least one more object (for the answer)...
					if (question && (ri->next == NULL)) break;
					// Set text
					if (question) text = QString::number(questionSource_->indexOf(currentQuestion_) + 1) + ". " + currentQuestion_->textQuestion();
					else text = "    " + currentQuestion_->textAnswer();
					object->textPrimitive().set(text, TextPrimitive::TopLeftAnchor);
					if (!question) object->adjustPosition(0.0, fgMargin*0.5, 0.0);
					textWidth = object->textPrimitive().textWidth() * object->textSize();
					if (textWidth > maxTextWidth) maxTextWidth = textWidth;
					ri = ri->next;
					question = !question;
					if (question) currentQuestion_ = currentQuestion_->next;
					if (!currentQuestion_) break;
				}

				// Adjust left edge of lines to centre text, and apply dynamic transforms
				// -- Get pixel width of longest text line by multiplying by display height (against which font size is calculated)
				maxTextWidth *= quapp.displayHeight();
				// -- Calculate normalised position delta (i.e. between 0.0 and 1.0) to apply
				double delta = (displayWidth - maxTextWidth - 2.0*quapp.foregroundMargin()*displayWidth) * 0.5 / displayWidth;
				colourA.setRgbF(0.0, 0.0, 0.0, 0.0);
				colourB.setRgbF(0.0, 0.0, 0.0, 1.0);
				for (ri = lines.first(); ri != NULL; ri = ri->next)
				{
					if (delta > 0) ri->item->adjustPosition(delta, 0.0, 0.0);
					ri->item->addTransform()->setColourTransform(colourA, colourB, 0.01);
					ri->item->addTransform()->setTranslationTransform(Vec3<double>(aspectRatio-delta,0.0,0.0), Vec3<double>(), 0.02);
					ri->item->setTransformContinueAt(0.2);
					ri->item->initialiseTransforms();
				}
			}
			break;
		// Question segment - available media will determine what we display
		case (Segment::QuestionSegment):
			// Transition out any previous objects
			for (object = quapp.foregroundObjects().first(); object != NULL; object = object->next)
			{
				object->addTransform()->setRotationTransform(1, 0.0, -90.0, 0.05);
				colourA = object->colour();
				colourB = colourA;
				colourB.setAlphaF(0.0);
				object->addTransform()->setColourTransform(colourA, colourB, 0.05);
				object->setDeleteAfterTransform();
			}
			if (!questionSource_) break;
			if (!currentQuestion_) return false;
			if (currentQuestion_->imageQuestionOK())
			{
				// Create object for image
				object = quapp.addForeGroundObject();
				primitive = object->primitive().addPrimitive(6, 0, GL_TRIANGLES, false, true);
				primitive->setTextureImage(currentQuestion_->imageQuestion());
				double imageAspect = double(currentQuestion_->imageQuestion().width()) / double(currentQuestion_->imageQuestion().height());
				if (imageAspect > 1.0) primitive->rectangle(0.0, 0.0, 0.0, 0.8*imageAspect, 0.8, true);
				else primitive->rectangle(0.0, 0.0, 0.0, 0.8, 0.8*imageAspect, true);
				colourA.setRgbF(1.0, 1.0, 1.0, 0.0);
				colourB.setRgbF(1.0, 1.0, 1.0, 1.0);
				object->setPosition(0.0, 0.0, -0.5);
				object->addTransform()->setRotationTransform(1, 90.0, 0.0, 0.05);
				object->addTransform()->setColourTransform(colourA, colourB, 0.05);
				object->initialiseTransforms();

				// Create object for text
				object = quapp.addForeGroundObject();
				colourA.setRgbF(0.0, 0.0, 0.0, 0.0);
				colourB.setRgbF(0.0, 0.0, 0.0, 1.0);

				if (imageQuestionDisplayIndex_) text = "("+QString::number(questionSource_->indexOf(currentQuestion_)+1)+")";
				else text = currentQuestion_->textQuestion();

				object->setTextSize(quapp.bodyTextHeight()*bodyTextSizeModifier_);
				object->textPrimitive().set(text, TextPrimitive::TopMiddleAnchor);
				object->setPosition(0.4, -0.31, -0.5);
				object->addTransform()->setRotationTransform(1, 90.0, 0.0, 0.05);
				object->addTransform()->setColourTransform(colourA, colourB, 0.05);
				object->initialiseTransforms();
			}

			// Audio element?
			if (currentQuestion_->audioQuestionOK()) quapp.playAudio(currentQuestion_->audioQuestionFileName());

			currentQuestion_ = currentQuestion_->next;
			break;
		// Score segment - show current team scores
		case (Segment::ScoreSegment):
			// Get some body text primitives
			while (currentTeamScore_)
			{
				// Check object
				if (!displayLine_)
				{
					printf("Internal Error: Ran out of displayLines_ while updating items in Score segment.\n");
					return false;
				}
				object = displayLine_->item;
				if (!object)
				{
					printf("Internal Error: NULL DisplayObject found in reflist.\n");
					return false;
				}
				
				object->addTransform()->setColourTransform(colourA, colourB, 0.01);
				object->addTransform()->setTranslationTransform(Vec3<double>(0.0,0.0,0.0), Vec3<double>(-1.0,0.0,0.0), 0.02);
				object->setTransformContinueAt(0.2);
				object->initialiseTransforms();

				// Move on to previous display line and team score (since we're working in reverse order)
				displayLine_ = displayLine_->prev;
				currentTeamScore_ = currentTeamScore_->prev;

				// If the team rank is not the same as the last, we break here...
				if (currentTeamScore_ && (currentTeamScore_->next->item->rank() != currentTeamScore_->item->rank())) break;
			}
			break;
	}
	return true;
}

// Return whether there is another part after this one
bool Segment::hasNextPart()
{
	// Exactly what we return here depends on the segment type...
	switch (type_)
	{
		// Reveal answers from source QuestionSet
		case (Segment::AnswerSegment):
		case (Segment::QuestionSegment):
			if (!questionSource_) return false;
			return currentQuestion_;
			break;
		// Score segment - show current team scores.
		case (Segment::ScoreSegment):
			return currentTeamScore_;
			break;
	}
	return false;
}
