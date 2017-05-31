/*
	*** Transform
	*** src/render/transform.h
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

#ifndef QUAPP_TRANSFORM_H
#define QUAPP_TRANSFORM_H

#include "math/matrix.h"
#include <templates/list.h>
#include <QtGui/QColor>

// Forward Declarations
/* none */

// Transform
class Transform : public ListItem<Transform>
{
	public:
	// Constructor / Destructor
	Transform();
	~Transform();
	// Transform type enum
	enum TransformType { ColourTransform, RotateXTransform, RotateYTransform, RotateZTransform, ScaleTransform, TranslateTransform, nTransformTypes };


	/*
	 * Definition
	 */
	private:
	// Transform type
	TransformType type_;
	// Transform origin
	Vec3<double> origin_;
 	// Transform destination
	Vec3<double> destination_;
	// Origin colour
	Vec4<double> colourOrigin_;
	// Destination colour
	Vec4<double> colourDestination_;
	// Colour delta
	Vec4<double> colourDelta_;
	// Speed of transformation
	double speed_;
	// Coordinate for transformation
	double coordinate_;
	// Whether we are transforming, or have finished
	bool transforming_;
	// Coordinate at which the subsequent transform starts/continues
	double continueAt_;

	public:
	// Set translation transform
	void setTranslationTransform(Vec3<double> origin, Vec3<double> destination, double speed, double continueAt = 0.0);
	// Set translation transform (no dynamics)
	void setTranslationTransform(Vec3<double> coordinates);
	// Set colour transform
	void setColourTransform(QColor origin, QColor dest, double speed, double continueAt = 0.0);
	// Add scale transform
	void setScaleTransform(Vec3<double> origin, Vec3<double> destination, double speed, double continueAt = 0.0);
	// Add rotation transform
	void setRotationTransform(int axis, double origin, double dest, double speed, double continueAt = 0.0);
	// Apply transform to supplied QColor and Matrix, returning whether to continue with subsequent transforms
	bool applyTransform(Matrix& matrix, QColor& colour, bool progress = true);
	// Return current coordinate
	double coordinate();
};

#endif
