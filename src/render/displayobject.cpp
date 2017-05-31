/*
	*** Display Object
	*** src/render/displayobject.cpp
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

#include "render/displayobject.h"
#include <QtOpenGL/QGLWidget>

// Static members
QColor DisplayObject::defaultColour_(1,1,1,1);
	
// Constructor
DisplayObject::DisplayObject() : ListItem<DisplayObject>()
{
	// Dynamic Properties
	colour_ = defaultColour_;
	transformCoordinate_ = 0.0;
	transformContinueAt_ = -1.0;
	isTransforming_ = false;
	deleteAfterTransform_ = false;
}

// Destructor
DisplayObject::~DisplayObject()
{
}

/*
 * Definition
 */

// Set position
void DisplayObject::setPosition(double x, double y, double z)
{
	position_.set(x, y, z);
}

// Set position
void DisplayObject::setPosition(Vec3<double> position)
{
	position_ = position;
}

// Adjust position
void DisplayObject::adjustPosition(double dx, double dy, double dz)
{
	position_.add(dx, dy, dz);
}

// Return position
Vec3<double> DisplayObject::position()
{
	return position_;
}

// Set text size
void DisplayObject::setTextSize(double textSize)
{
	textPrimitive_.setTextSize(textSize);
}

// Return text size
double DisplayObject::textSize()
{
	return textPrimitive_.textSize();
}

/*
 * Primitive Information
 */

// Return primitive for object
PrimitiveList& DisplayObject::primitive()
{
	return primitive_;
}

// Return primitive for object
TextPrimitive& DisplayObject::textPrimitive()
{
	return textPrimitive_;
}

// Set colour
void DisplayObject::setColour(QColor colour)
{
	colour_ = colour;
}

// Set colour
void DisplayObject::setColour(double r, double g, double b, double a)
{
	colour_.setRgbF(r, g, b, a);
}

// Return colour
QColor DisplayObject::colour()
{
	return colour_;
}

// Set primitive data used to generate it (size, radius etc.)
void DisplayObject::setData(Vec3<double> v)
{
	data_ = v;
}

// Return primitive data used to generate it (size, radius etc.)
Vec3<double> DisplayObject::data()
{
	return data_;
}

// Send primitive to GL
void DisplayObject::sendToGL(double aspectRatio) const
{
	// Reset matrix
	glLoadIdentity();

	// Set colour
	GLfloat colour[4] =  { colour_.redF(), colour_.greenF(), colour_.blueF(), colour_.alphaF() };
	glColor4fv(colour);

	// Translate to perspective 'zero' plane
	glTranslated(0.0, 0.0, -1.0);

	// Apply transformation
	Matrix matrix = transformationMatrix_;
	matrix[12] *= aspectRatio;
	glMultMatrixd(matrix.matrix());

	// Send text to GL
	textPrimitive_.sendToGL();

	// Send primitive to GL
	primitive_.sendToGL();
}

/*
 * Dynamic Properties
 */

// Add new transform
Transform* DisplayObject::addTransform()
{
	isTransforming_ = true;
	transformCoordinate_ = 0.0;
	return transforms_.add();
}

// Initialise transforms
void DisplayObject::initialiseTransforms()
{
	// Loop over defined transforms and set initial colour and transform
	transformationMatrix_.setIdentity();

	// Apply position to transformation matrix
	transformationMatrix_.setTranslation(position_);
	
	bool continueTransform = true;
	for (Transform* transform = transforms_.first(); transform != NULL; transform = transform->next)
	{
		if (continueTransform) continueTransform = transform->applyTransform(transformationMatrix_, colour_, false);
	}
}

// Clear all existing transforms
void DisplayObject::clearTransforms()
{
	transforms_.clear();

	transformCoordinate_ = 0.0;
	transformContinueAt_ = -1.0;
	isTransforming_ = false;
}

// Set transform coordinate at which next object should start/continue transforming
void DisplayObject::setTransformContinueAt(double continueAt)
{
	transformContinueAt_ = continueAt;
}

// Return whether object is transforming
bool DisplayObject::isTransforming()
{
	return isTransforming_;
}

// Return whether update of the next object should be stalled
bool DisplayObject::stall()
{
	return (transformCoordinate_ < transformContinueAt_);
}

// Set object to be deleted after it has finished transforming
void DisplayObject::setDeleteAfterTransform()
{
	deleteAfterTransform_ = true;
}

// Return whether this object should be deleted
bool DisplayObject::toBeDeleted()
{
	return (deleteAfterTransform_ && (transformCoordinate_ > 0.9999));
}

// Update dynamic properties
bool DisplayObject::updateDynamic()
{
	// If we have finished transforming, no need to do anything else
	if (!isTransforming_) return false;

	// Reset transformation matrix, and apply initial position
	transformationMatrix_.setIdentity();
	transformationMatrix_.addTranslation(position_);

	// Loop over transforms
	bool continueTransform = true;
	transformCoordinate_ = 0.0;
	for (Transform* transform = transforms_.first(); transform != NULL; transform = transform->next)
	{
		if (continueTransform) continueTransform = transform->applyTransform(transformationMatrix_, colour_);
		transformCoordinate_ += transform->coordinate();
	}
	transformCoordinate_ /= transforms_.nItems();

	// Have we finished transforming?
	if (transformCoordinate_ > 0.9999) isTransforming_ = false;

	return (transformCoordinate_ > transformContinueAt_);
}
