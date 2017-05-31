/*
	*** Transform
	*** src/render/transform.cpp
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

#include "render/transform.h"

// Constructor
Transform::Transform() : ListItem<Transform>()
{
	type_ = Transform::nTransformTypes;
	colourOrigin_.set(1.0, 1.0, 1.0, 0.0);
	colourDestination_.set(1.0, 1.0, 1.0, 0.0);
	speed_ = 0.01;
	coordinate_ = -speed_;
	transforming_ = true;
	continueAt_ = 0.9999;
}

// Destructor
Transform::~Transform()
{
}

/*
 * Definition
 */

// Set translation transform
void Transform::setTranslationTransform(Vec3<double> origin, Vec3<double> destination, double speed, double continueAt)
{
	type_ = Transform::TranslateTransform;
	origin_ = origin;
	destination_ = destination;
	speed_ = speed;
	coordinate_ = -speed_;
	continueAt_ = continueAt;
}

// Set translation transform (no dynamics)
void Transform::setTranslationTransform(Vec3<double> coordinates)
{
	type_ = TranslateTransform;
	origin_ = coordinates;
	destination_ = coordinates;
	speed_ = 1.0;
	coordinate_ = 0.0;
	continueAt_ = -1.0;
}

// Set colour transform
void Transform::setColourTransform(QColor origin, QColor dest, double speed, double continueAt)
{
	type_ = Transform::ColourTransform;
	colourOrigin_.set(origin.redF(), origin.greenF(), origin.blueF(), origin.alphaF());
	colourDestination_.set(dest.redF(), dest.greenF(), dest.blueF(), dest.alphaF());
	colourDelta_ = colourDestination_ - colourOrigin_;
	speed_ = speed;
	coordinate_ = -speed_;
	continueAt_ = continueAt;
}

// Set scale transform
void Transform::setScaleTransform(Vec3<double> origin, Vec3<double> destination, double speed, double continueAt)
{
	type_ = Transform::ScaleTransform;
	origin_ = origin;
	destination_ = destination;
	speed_ = speed;
	coordinate_ = -speed_;
	continueAt_ = continueAt;
}

// Set rotation transform
void Transform::setRotationTransform(int axis, double origin, double destination, double speed, double continueAt)
{
	if (axis == 0) type_ = Transform::RotateXTransform;
	else if (axis == 1) type_ = Transform::RotateYTransform;
	else type_ = Transform::RotateZTransform;
	origin_.x = origin;
	destination_.x = destination;
	speed_ = speed;
	coordinate_ = -speed_;
	continueAt_ = continueAt;
}

// Apply transform to supplied QColor and Matrix, returning whether to continue with subsequent transforms
bool Transform::applyTransform(Matrix& matrix, QColor& colour, bool progress)
{
	if (transforming_ && progress) coordinate_ += speed_;

	Vec3<double> v;
	Vec4<double> newColour;
	switch (type_)
	{
		case (Transform::ColourTransform):
			newColour = colourOrigin_ + colourDelta_*coordinate_;
			colour.setRedF( newColour.x < 0 ? 0.0 : (newColour.x > 1.0 ? 1.0 : newColour.x) );
			colour.setGreenF( newColour.y < 0 ? 0.0 : (newColour.y > 1.0 ? 1.0 : newColour.y) );
			colour.setBlueF( newColour.z < 0 ? 0.0 : (newColour.z > 1.0 ? 1.0 : newColour.z) );
			colour.setAlphaF( newColour.w < 0 ? 0.0 : (newColour.w > 1.0 ? 1.0 : newColour.w) );
			break;
		case (Transform::RotateXTransform):
			matrix.applyRotationX(origin_.x + (destination_.x-origin_.x)*coordinate_);
			break;
		case (Transform::RotateYTransform):
			matrix.applyRotationY(origin_.x + (destination_.x-origin_.x)*coordinate_);
			break;
		case (Transform::RotateZTransform):
			matrix.applyRotationZ(origin_.x + (destination_.x-origin_.x)*coordinate_);
			break;
		case (Transform::ScaleTransform):
			v = origin_ + (destination_-origin_)*coordinate_;
			matrix.applyScaling(v);
			break;
		case (Transform::TranslateTransform):
			v = origin_ + (destination_-origin_)*coordinate_;
			v.z = -v.z;
			matrix.applyTranslation(v);
			break;
	}

	// Transform finished?
	if (coordinate_ > 1.0)
	{
		coordinate_ = 1.0;
		transforming_ = false;
	}

	return (coordinate_ > continueAt_);
}

// Return current coordinate
double Transform::coordinate()
{
	return coordinate_;
}
