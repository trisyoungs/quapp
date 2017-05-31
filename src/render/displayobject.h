/*
	*** Display Object
	*** src/render/displayobject.h
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

#ifndef QUAPP_DISPLAYOBJECT_H
#define QUAPP_DISPLAYOBJECT_H

#include "render/primitivelist.h"
#include "render/textprimitive.h"
#include "render/transform.h"
#include <QtCore/QString>
#include <QtGui/QImage>
#include <QtGui/QColor>

// Forward Declarations
class QGLWidget;

// Drawing Object
class DisplayObject : public ListItem<DisplayObject>
{
	public:
	// Constructor / Destructor
	DisplayObject();
	~DisplayObject();


	/*
	 * Definition
	 */
	private:
	// Position of object
	Vec3<double> position_;
	// Transformation matrix for object
	Matrix transformationMatrix_;

	public:
	// Set position
	void setPosition(double x, double y, double z);
	// Set position (Vec3)
	void setPosition(Vec3<double> position);
	// Adjust position
	void adjustPosition(double dx, double dy, double dz);
	// Return position
	Vec3<double> position();
	// Set text size
	void setTextSize(double textSize);
	// Return text size
	double textSize();


	/*
	 * Primitive Information
	 */
	private:
	// Primitive for object
	PrimitiveList primitive_;
	// TextPrimitive for object
	TextPrimitive textPrimitive_;
	// Default colour for primitives
	static QColor defaultColour_;
	// Colour for primitive
	QColor colour_;
	// Primitive data used to generate it (size, radius etc.)
	Vec3<double> data_;

	public:
	// Return primitive for object
	PrimitiveList& primitive();
	// Return primitive for object
	TextPrimitive& textPrimitive();
	// Set colour
	void setColour(QColor colour);
	// Set colour
	void setColour(double r, double g, double b, double a);
	// Return colour
	QColor colour();
	// Set primitive data used to generate it (size, radius etc.)
	void setData(Vec3<double> v);
	// Return primitive data used to generate it (size, radius etc.)
	Vec3<double> data();
	// Send primitive to GL
	void sendToGL(double aspectRatio) const;


	/*
	 * Dynamic Properties
	 */
	private:
	// List of transforms
	List<Transform> transforms_;
	// Overall transform coordinate
	double transformCoordinate_;
	// Overall transform coordinate at which next object should start/continue transforming
	double transformContinueAt_;
	// Whether object is currently transforming
	bool isTransforming_;
	// Whether to delete object once transform has finished
	bool deleteAfterTransform_;
	
	public:
	// Add new transform
	Transform* addTransform();
	// Initialise transforms
	void initialiseTransforms();
	// Clear all existing transforms
	void clearTransforms();
	// Set transform coordinate at which next object should start/continue transforming
	void setTransformContinueAt(double continueAt);
	// Return whether object is currently transforming
	bool isTransforming();
	// Return whether update of the next object should be stalled
	bool stall();
	// Set object to be deleted after it has finished transforming
	void setDeleteAfterTransform();
	// Return whether this object should be deleted
	bool toBeDeleted();
	// Update dynamic properties, returning if anything was changed
	bool updateDynamic();
};

#endif
