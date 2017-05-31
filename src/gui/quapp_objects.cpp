/*
	*** Quapp - Objects 
	*** src/gui/quapp_objects.cpp
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
#include "base/messenger.h"
#include "render/fontinstance.h"

// Update objects
void QuappWindow::updateObjects()
{
	DisplayObject* object;
	int nChanged = 0;
	RefList<DisplayObject,bool> toDelete;
	RefListItem<DisplayObject,bool>* refObject;

	// Background objects
	for (object = backgroundObjects_.first(); object != NULL; object = object->next)
	{
		if (object->updateDynamic()) ++nChanged;
		if (object->toBeDeleted()) toDelete.add(object);
		if (object->stall()) break;
	}
	for (refObject = toDelete.first(); refObject != NULL; refObject = refObject->next) backgroundObjects_.remove(refObject->item);
	toDelete.clear();

	// Foreground objects
	for (object = foregroundObjects_.first(); object != NULL; object = object->next)
	{
		if (object->updateDynamic()) ++nChanged;
		if (object->toBeDeleted()) toDelete.add(object);
		if (object->stall()) break;
	}
	for (refObject = toDelete.first(); refObject != NULL; refObject = refObject->next) foregroundObjects_.remove(refObject->item);
	toDelete.clear();

	// Persistent foreground objects
	for (object = persistentForegroundObjects_.first(); object != NULL; object = object->next)
	{
		if (object->updateDynamic()) ++nChanged;
		if (object->toBeDeleted()) toDelete.add(object);
		if (object->stall()) break;
	}
	for (refObject = toDelete.first(); refObject != NULL; refObject = refObject->next) persistentForegroundObjects_.remove(refObject->item);
	toDelete.clear();

	// Update background
	updateBackground();

	// Update display if necessary
	if (nChanged > 0) displayWindow_.ui.MainView->postRedisplay();

	// Restart timer...
	objectTimer_.start();
}

// Calculate text heights
void QuappWindow::calculateTextHeights()
{
	bodyLineHeight_ = (1.0-foregroundMargin_*2-titleLineHeight_) / (nBodyTextLines_);
	titleTextHeight_ = titleLineHeight_- foregroundMargin_;
	bodyTextHeight_ = bodyLineHeight_ - foregroundMargin_;
}

// Apply transition to all objects in list
void QuappWindow::applyExitTransform(List<DisplayObject>& objects, QuappWindow::ExitTransformType transformType, double speed, double stallCoordinate)
{
	QColor colour;
	switch (transformType)
	{
		case (QuappWindow::FadeOutTransform):
			for (DisplayObject* object = objects.first(); object != NULL; object = object->next)
			{
				colour = object->colour();
				colour.setAlphaF(0.0);
				object->addTransform()->setColourTransform(object->colour(), colour, speed);
				object->setTransformContinueAt(stallCoordinate);
				object->setDeleteAfterTransform();
			}
			break;
		default:
			msg.print("Transition type '%s' not handled.\n", transformType);
			break;
	}
}

// Apply entrance transform to specified object
void QuappWindow::applyEntranceTransform(DisplayObject* object, QuappWindow::EntranceTransformType transformType, double speed, double stallCoordinate)
{
	QColor colourA, colourB;
	switch (transformType)
	{
		case (QuappWindow::FadeInTransform):
			colourB = object->colour();
			colourA = colourB;
			colourA.setAlphaF(0.0);
			object->addTransform()->setColourTransform(colourA, colourB, speed);
			object->setTransformContinueAt(stallCoordinate);
			break;
		default:
			msg.print("Transition type '%s' not handled.\n", transformType);
			break;
	}
}

// Background objects
List<DisplayObject>& QuappWindow::backgroundObjects()
{
	return backgroundObjects_;
}

// Foreground objects
List<DisplayObject>& QuappWindow::foregroundObjects()
{
	return foregroundObjects_;
}

// Persistent foreground objects
List<DisplayObject>& QuappWindow::persistentForegroundObjects()
{
	return persistentForegroundObjects_;
}

// Add an empty title primitive
DisplayObject* QuappWindow::addTitleObject()
{
	DisplayObject* object = persistentForegroundObjects_.add();
	object->setPosition(-0.5+foregroundMargin_, 0.5-foregroundMargin_, 0.0);
	object->setTextSize(titleTextHeight_);

	object->initialiseTransforms();
	return object;
}

// Add a persistent foreground object
DisplayObject* QuappWindow::addPersistentForeGroundObject()
{
	DisplayObject* object = persistentForegroundObjects_.add();
	object->setTextSize(titleTextHeight_);
	return object;
}

// Add a foreground object
DisplayObject* QuappWindow::addForeGroundObject()
{
	DisplayObject* object = foregroundObjects_.add();
	object->setTextSize(bodyTextHeight_);
	return object;
}

// Add a list of empty text objects at the defined line positions on the screen, returning in a RefList
RefList<DisplayObject,int> QuappWindow::addForeGroundTextLines()
{
	Vec3<double> position(-0.5+foregroundMargin_, 0.5-foregroundMargin_*2-titleLineHeight_, .0);
	RefList<DisplayObject,int> objects;
	DisplayObject* object;
	for (int n=0; n<nBodyTextLines_; ++n)
	{
		object = foregroundObjects_.add();
		object->setPosition(position);
		object->setTextSize(bodyTextHeight_);
		object->initialiseTransforms();
		objects.add(object);
		position.y -= bodyLineHeight_;
	}
	return objects;
}

// Return aspect ratio of display
double QuappWindow::displayAspectRatio()
{
	return displayWindow_.ui.MainView->aspectRatio();
}

// Return width of display
double QuappWindow::displayWidth()
{
	return double(displayWindow_.ui.MainView->width());
}

// Return height of display
double QuappWindow::displayHeight()
{
	return double(displayWindow_.ui.MainView->height());
}


// Set fractional margin to use for foreground objects
void QuappWindow::setForegroundMargin(double margin)
{
	foregroundMargin_ = margin;

	calculateTextHeights();
}

// Return fractional margin to use for foreground objects
double QuappWindow::foregroundMargin()
{
	return foregroundMargin_;
}


// Return title line height
double QuappWindow::titleLineHeight()
{
	return titleLineHeight_;
}

// Return title text height
double QuappWindow::titleTextHeight()
{
	return titleTextHeight_;
}

// Return body line height
double QuappWindow::bodyLineHeight()
{
	return bodyLineHeight_;
}

// Return body text height
double QuappWindow::bodyTextHeight()
{
	return bodyTextHeight_;
}

// Return scale modifier for aspect ratio (to adjust odd ratios on screen)
double QuappWindow::aspectModifier()
{
	return aspectModifier_;
}
