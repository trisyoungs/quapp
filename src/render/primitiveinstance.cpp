/*
	*** Rendering Primitive Instance
	*** src/render/primitiveinstance.cpp
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

#include "render/glextensions.h"
#include "render/primitiveinstance.h"
#include <string.h>

// Static members
PrimitiveInstance::InstanceType PrimitiveInstance::globalInstanceType_ = PrimitiveInstance::VBOInstance;

// Constructor
PrimitiveInstance::PrimitiveInstance() : ListItem<PrimitiveInstance>()
{
	// Private variables
	context_ = NULL;
	type_ = PrimitiveInstance::ListInstance;
	listObject_ = 0;
	vboVertexObject_ = 0;
	vboIndexObject_ = 0;
	textureObject_ = 0;
}

// Return global instance type to use
PrimitiveInstance::InstanceType PrimitiveInstance::globalInstanceType()
{
	return globalInstanceType_;
}

// Set global instance type to use
void PrimitiveInstance::setGlobalInstanceType(PrimitiveInstance::InstanceType instanceType)
{
	globalInstanceType_ = instanceType;
}

// Return context to which primitive instance is associated
const QGLContext *PrimitiveInstance::context()
{
	return context_;
}

// Set exensions object
void PrimitiveInstance::setExtensions(GLExtensions* extensions)
{
	extensions_ = extensions;
}

// Return GL extensions
const GLExtensions* PrimitiveInstance::extensions() const
{
	return extensions_;
}

// Set display list data
void PrimitiveInstance::setDisplayList(const QGLContext *context, GLuint listObject)
{
	context_ = context;
	type_ = PrimitiveInstance::ListInstance;
	listObject_ = listObject;
}

// Return display list object for instance
GLuint PrimitiveInstance::listObject()
{
	return listObject_;
}

// Set vbo object data
void PrimitiveInstance::setVBO(const QGLContext *context, GLuint vertexObject, GLuint indexObject)
{
	context_ = context;
	type_ = PrimitiveInstance::VBOInstance;
	vboVertexObject_ = vertexObject;
	vboIndexObject_ = indexObject;
}

// Return VBO ID of vertex array for instance
GLuint PrimitiveInstance::vboVertexObject()
{
	return vboVertexObject_;
}

// Return VBO ID of index array for instance
GLuint PrimitiveInstance::vboIndexObject() const
{
	return vboIndexObject_;
}

// Set texture object
void PrimitiveInstance::setTexture(GLuint textureObject)
{
	textureObject_ = textureObject;
}

// Return texture ID for instance
GLuint PrimitiveInstance::textureObject()
{
	return textureObject_;
}

// Return type of instance
PrimitiveInstance::InstanceType PrimitiveInstance::type()
{
	return type_;
}
