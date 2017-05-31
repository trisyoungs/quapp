/*
	*** Rendering Primitive
	*** src/render/primitive.cpp
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

#include <QtOpenGL/QGLWidget>
#include "render/primitive.h"
#include "base/messenger.h"
#include "render/glextensions.h"
#include <string.h>
#ifdef __APPLE__
#include <OpenGL/gl.h>
#endif

// Constructor
Primitive::Primitive() : ListItem<Primitive>()
{
	vertexIndex_ = 0;
	vertexData_ = NULL;
	indexData_ = NULL;
	centroids_ = NULL;
	verticesPerType_ = 0;
	dataPerVertex_ = 0;
	nDefinedVertices_ = 0;
	nDefinedIndices_ = 0;
	maxVertices_ = 0;
	maxIndices_ = 0;
	hasIndices_ = false;
	hasColourData_ = false;
	hasTextureCoordinateData_ = false;
	maxTypes_ = 0;
	nDefinedTypes_ = 0;
	calcCentroids_ = false;
	type_ = GL_TRIANGLES;
	interleaveType_ = GL_N3F_V3F;
	useInstances_ = true;
}

// Destructor
Primitive::~Primitive()
{
}

/*
 * Data
 */

// Initialise primitive
void Primitive::initialise(int newMaxVertices, int newMaxIndices, GLenum type, bool colourData, bool textureData)
{
	hasColourData_ = colourData;
	hasTextureCoordinateData_ = textureData;
	type_ = type;

	// Set data per vertex and array type
	if (hasColourData_)
	{
		dataPerVertex_ = (hasTextureCoordinateData_ ? 12 : 10);
		interleaveType_ = (hasTextureCoordinateData_ ? GL_T2F_C4F_N3F_V3F : GL_C4F_N3F_V3F);
	}
	else
	{
		dataPerVertex_ = (hasTextureCoordinateData_ ? 8 : 6);
		interleaveType_ = (hasTextureCoordinateData_ ? GL_T2F_N3F_V3F : GL_N3F_V3F);
	}
	if (type_ == GL_TRIANGLES) verticesPerType_ = 3;
	else if ((type_ == GL_LINES) || (type_ == GL_LINE_LOOP) || (type_ == GL_LINE_STRIP)) verticesPerType_ = 2;
	else if (type_ == GL_POINTS) verticesPerType_ = 1;
	else printf("Warning - Invalid GLenum type given to VertexChunk::initialise (%i)\n", type_);

	// (Re)create vertexData_ array if it doesn't currently exist or is too small
	if ((!vertexData_) || (newMaxVertices > maxVertices_))
	{
		if (vertexData_) delete[] vertexData_;
		maxVertices_ = newMaxVertices;
		vertexData_ = new GLfloat[maxVertices_*dataPerVertex_];
	}

	// (Re)create indexData_ array if it doesn't currently exist or is too small
	if ((!indexData_) || (newMaxIndices > maxIndices_))
	{
		if (indexData_) delete[] indexData_;
		maxIndices_ = newMaxIndices;
		indexData_ = new GLuint[maxIndices_];
	}
	hasIndices_ = (newMaxIndices > 0);

	// (Re)create centroids_ array if it doesn't currently exist or is too small
	if (newMaxIndices == 0)
	{
		calcCentroids_ = true;
		if ((!centroids_) || ((maxVertices_/verticesPerType_) > maxTypes_))
		{
			if (centroids_) delete[] centroids_;
			maxTypes_ = maxVertices_/verticesPerType_;
			centroids_ = new GLfloat[maxTypes_*3];
		}
		for (int n=0; n<maxTypes_*3; ++n) centroids_[n] = 0.0f;
	}
	else calcCentroids_ = false;

	nDefinedVertices_ = 0;
	nDefinedIndices_ = 0;
	nDefinedTypes_ = 0;
	vertexIndex_ = 0;
}

// Forget all data, leaving arrays intact
void Primitive::forgetAll()
{
	nDefinedTypes_ = 0;
	nDefinedVertices_ = 0;
	nDefinedIndices_ = 0;
	vertexIndex_ = 0;
	
	// Clear centroid array if we were using it
	if (calcCentroids_ && centroids_) for (int n=0; n<maxTypes_; ++n) centroids_[n] = 0.0f;
}

// Return number of vertices currently defined in primitive
int Primitive::nDefinedVertices() const
{
	return nDefinedVertices_;
}

// Return number of indices currently defined in primitive
int Primitive::nDefinedIndices() const
{
	return nDefinedIndices_;
}

// Return whether vertex data contains colour information
bool Primitive::hasColourData() const
{
	return hasColourData_;
}

// Return whether vertex data contains texture coordinate information
bool Primitive::hasTextureCoordinateData() const
{
	return hasTextureCoordinateData_;
}

// Flag that this primitive should not use instances (rendering will use vertex arrays)
void Primitive::setNoInstances()
{
	useInstances_ = false;
}

// Push instance of primitive
void Primitive::pushInstance(const QGLContext* context, GLExtensions* extensions, QGLWidget* glWidget)
{
	// Does this primitive use instances?
	if (!useInstances_) return;

	// Clear the error flag
	glGetError();

	// Create new instance
	PrimitiveInstance *pi = instances_.add();
	pi->setExtensions(extensions);

	// Vertex buffer object or plain old display list?
	if (PrimitiveInstance::globalInstanceType() == PrimitiveInstance::VBOInstance)
	{
		// Prepare local array of data to pass to VBO
		int offset;
		GLuint vertexVBO = 0, indexVBO = 0;
		if (nDefinedVertices_ <= 0)
		{
			// Store instance data
			pi->setVBO(context, 0, 0);
			msg.exit("Primitive::pushInstance");
			return;
		}

		// Determine total size of array (in bytes) for VBO
		int vboSize = nDefinedVertices_ * dataPerVertex_ * sizeof(GLfloat);
		
		// Generate vertex array object
		extensions->glGenBuffers(1, &vertexVBO);

		// Bind VBO
		extensions->glBindBuffer(GL_ARRAY_BUFFER, vertexVBO);
		
		// Initialise vertex array data
		extensions->glBufferData(GL_ARRAY_BUFFER, vboSize, vertexData_, GL_STATIC_DRAW);
		if (glGetError() != GL_NO_ERROR)
		{
			extensions->glBindBuffer(GL_ARRAY_BUFFER, 0);
			printf("Error occurred while generating vertex buffer object for Primitive.\n");
			extensions->glDeleteBuffers(1, &vertexVBO);
			vertexVBO = 0;
			return;
		}
		extensions->glBindBuffer(GL_ARRAY_BUFFER, 0);

		// Generate index array object (if using indices)
		if (hasIndices_)
		{
			// Generate index array object
			extensions->glGenBuffers(1, &indexVBO);

			// Bind VBO
			extensions->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexVBO);
			
			// Initialise index array data
			extensions->glBufferData(GL_ELEMENT_ARRAY_BUFFER, nDefinedIndices_*sizeof(GLuint), indexData_, GL_STATIC_DRAW);
			if (glGetError() != GL_NO_ERROR)
			{
				extensions->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
				printf("Error occurred while generating index buffer object for Primitive.\n");
				extensions->glDeleteBuffers(1, &indexVBO);
				indexVBO = 0;
				return;
			}
			extensions->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		}

		// Store instance data
		pi->setVBO(context, vertexVBO, hasIndices_ ? indexVBO : 0);
	}
	else
	{
		// Generate display list
		int listId = glGenLists(1);
		if (listId == 0) msg.print("Primitive::pushInstance - glGenLists(1) returned 0!\n!");
		else
		{
			glNewList(listId, GL_COMPILE);
			sendToGL(true);
			glEndList();
		}

		// Store data
		pi->setDisplayList(context, listId);
	}

	// Generate texture object (if relevant)
	if (hasTextureCoordinateData_)
	{
		GLuint texture = glWidget->bindTexture(textureImage_);
		pi->setTexture(texture);
	}
}

// Pop topmost instance on primitive's stack
void Primitive::popInstance(const QGLContext *context)
{
	// Does this primitive use instances?
	if (!useInstances_) return;
	PrimitiveInstance *pi = instances_.last();
	if (pi != NULL)
	{
		if (pi->context() == context)
		{
			// Vertex buffer object or plain old display list?
			if (pi->type() == PrimitiveInstance::VBOInstance)
			{
				const GLExtensions* extensions = pi->extensions();
				GLuint bufid  = pi->vboVertexObject();
				if (bufid != 0) extensions->glDeleteBuffers(1, &bufid);
				if (hasIndices_)
				{
					bufid = pi->vboIndexObject();
					if (bufid != 0) extensions->glDeleteBuffers(1, &bufid);
				}
			}
			else if (pi->listObject() != 0) glDeleteLists(pi->listObject(),1);
		}
	}
	instances_.removeLast();
}

// Return number of instances available
int Primitive::nInstances()
{
	return instances_.nItems();
}

// Send to OpenGL (i.e. render)
void Primitive::sendToGL(bool drawSimple) const
{
	// If no vertices are defined, nothing to do...
	if (nDefinedVertices_ == 0) return;

	// Check if using instances...
	if (useInstances_)
	{
		// Grab topmost instance
		PrimitiveInstance *pi = instances_.last();
		if (pi == NULL)
		{
			printf("Internal Error: No instance on stack in primitive %p.\n", this);
			return;
		}

		// Bind texture object if relevant
		if (hasTextureCoordinateData_ && pi->textureObject())
		{
			glBindTexture(GL_TEXTURE_2D, pi->textureObject());
			glEnable(GL_TEXTURE_2D);
		}

		if (pi->type() == PrimitiveInstance::VBOInstance)
		{
			const GLExtensions* extensions = pi->extensions();
			glEnableClientState(GL_VERTEX_ARRAY);
			glEnableClientState(GL_NORMAL_ARRAY);
			glEnableClientState(GL_COLOR_ARRAY);
			if (hasTextureCoordinateData_) glEnableClientState(GL_TEXTURE_COORD_ARRAY);
			else glDisableClientState(GL_TEXTURE_COORD_ARRAY);
			if (hasIndices_) glEnableClientState(GL_INDEX_ARRAY);
			else glDisableClientState(GL_INDEX_ARRAY);

			// Bind VBO and index buffer (if using it)
			extensions->glBindBuffer(GL_ARRAY_BUFFER, pi->vboVertexObject());
			if (hasIndices_) extensions->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, pi->vboIndexObject());

			// Set interleave type for arrays
			glInterleavedArrays(interleaveType_, 0, NULL);

			if (hasIndices_) glDrawElements(type_, nDefinedIndices_, GL_UNSIGNED_INT, 0);
			else glDrawArrays(type_, 0, nDefinedVertices_);

			// Revert to normal operation - pass 0 as VBO index
			extensions->glBindBuffer(GL_ARRAY_BUFFER, 0);
			if (hasIndices_) extensions->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
			glDisableClientState(GL_VERTEX_ARRAY);
			glDisableClientState(GL_NORMAL_ARRAY);
			glDisableClientState(GL_COLOR_ARRAY);
			if (hasIndices_) glDisableClientState(GL_INDEX_ARRAY);
			if (hasTextureCoordinateData_) glDisableClientState(GL_TEXTURE_COORD_ARRAY);
		}
		else if (pi->listObject() != 0) glCallList(pi->listObject());
	}
	else
	{
		// Set interleave type for array
		glInterleavedArrays(interleaveType_, 0, vertexData_);

		// Check if we are using indices
		if (hasIndices_) glDrawElements(type_, nDefinedIndices_, GL_UNSIGNED_INT, indexData_);
		else glDrawArrays(type_, 0, nDefinedVertices_);
	}

	glDisable(GL_TEXTURE_2D);
}

/*
 * Vertex / Index Generation
 */

// Store data (2)
void Primitive::storeVertexData(GLfloat a, GLfloat b)
{
	vertexData_[vertexIndex_++] = a;
	vertexData_[vertexIndex_++] = b;
}

// Store data (3)
void Primitive::storeVertexData(GLfloat a, GLfloat b, GLfloat c)
{
	vertexData_[vertexIndex_++] = a;
	vertexData_[vertexIndex_++] = b;
	vertexData_[vertexIndex_++] = c;
}

// Store data (4)
void Primitive::storeVertexData(GLfloat a, GLfloat b, GLfloat c, GLfloat d)
{
	vertexData_[vertexIndex_++] = a;
	vertexData_[vertexIndex_++] = b;
	vertexData_[vertexIndex_++] = c;
	vertexData_[vertexIndex_++] = d;
}

// Update (or finalise) centroid for current primitive type
void Primitive::updateCentroid(GLfloat x, GLfloat y, GLfloat z)
{
	if (!calcCentroids_) return;

	// Accumulate centroid
	int coff = nDefinedTypes_*3;
	centroids_[coff] += x;
	centroids_[coff+1] += y;
	centroids_[coff+2] += z;

	// Finalise centroid?
	if ((nDefinedVertices_%verticesPerType_) == 0)
	{
		centroids_[coff] /= verticesPerType_;
		centroids_[coff+1] /= verticesPerType_;
		centroids_[coff+2] /= verticesPerType_;	
		++nDefinedTypes_;
	}
}

// Define next vertex and normal
GLuint Primitive::defineVertexN(GLfloat x, GLfloat y, GLfloat z, GLfloat nx, GLfloat ny, GLfloat nz)
{
	if (nDefinedVertices_ >= maxVertices_)
	{
		printf("Internal Error: Vertex limit for VertexChunk reached.\n");
		return -1;
	}

	if (hasColourData_)
	{
		printf("Internal Error: No colour given to defineVertex(), but the primitive requires one.\n");
		return -1;
	}

	if (hasTextureCoordinateData_)
	{
		printf("Internal Error: No texture coordinate given to defineVertex(), but the primitive requires one.\n");
		return -1;
	}

	// Store normal
	storeVertexData(nx, ny, nz);

	// Store vertex
	storeVertexData(x, y, z);

	// Increase vertex counter
	++nDefinedVertices_;

	// Update centroid
	if (calcCentroids_) updateCentroid(x, y, z);

	// Return index of vertex
	return (nDefinedVertices_-1);
}

// Define next vertex and normal (as Vec3<double>)
GLuint Primitive::defineVertexN(Vec3<double> vertex, Vec3<double> normal)
{
	return defineVertexN(vertex.x, vertex.y, vertex.z, normal.x, normal.y, normal.z);
}

// Define next vertex and normal with colour
GLuint Primitive::defineVertexNC(GLfloat x, GLfloat y, GLfloat z, GLfloat nx, GLfloat ny, GLfloat nz, GLfloat r, GLfloat g, GLfloat b, GLfloat a)
{
	if (nDefinedVertices_ >= maxVertices_)
	{
		printf("Internal Error: Vertex limit for VertexChunk reached.\n");
		return -1;
	}

	// Store colour
	if (!hasColourData_)
	{
		printf("Internal Error: Colour specified in vertex creation, but it is not required for primitive.\n");
		return -1;
	}
	storeVertexData(r, g, b, a);

	// Store normal
	storeVertexData(nx, ny, nz);

	// Store vertex
	storeVertexData(x, y, z);

	// Increase vertex counter
	++nDefinedVertices_;

	// Update centroid
	updateCentroid(x, y, z);

	// Return index of vertex
	return (nDefinedVertices_-1);
}

// Define next vertex and normal with colour (as array)
GLuint Primitive::defineVertexNC(GLfloat x, GLfloat y, GLfloat z, Vec3<double>& normal, Vec4<GLfloat>& colour)
{
	return defineVertexNC(x,y,z,normal.x,normal.y,normal.z,colour.x,colour.y,colour.z,colour.w);
}

// Define next vertex, normal, and colour (as Vec3<double>s and array)
GLuint Primitive::defineVertexNC(Vec3<double>& v, Vec3<double>& u, Vec4<GLfloat>& colour)
{
	return defineVertexNC(v.x,v.y,v.z,u.x,u.y,u.z,colour.x,colour.y,colour.z,colour.w);
}

// Define next vertex, normal, and texture coordinate
GLuint Primitive::defineVertexNT(GLfloat x, GLfloat y, GLfloat z, GLfloat nx, GLfloat ny, GLfloat nz, GLfloat tx, GLfloat ty)
{
	if (nDefinedVertices_ >= maxVertices_)
	{
		printf("Internal Error: Vertex limit for VertexChunk reached.\n");
		return -1;
	}

	// Store 2D texture coordinate
	if (!hasTextureCoordinateData_)
	{
		printf("Internal Error: Texture coordinate specified in vertex creation, but it is not required for primitive.\n");
		return -1;
	}
	storeVertexData(tx, ty);

	// Check colour data
	if (hasColourData_)
	{
		printf("Internal Error: Colour not specified in vertex creation, but is required for primitive.\n");
		return -1;
	}

	// Store normal
	storeVertexData(nx, ny, nz);

	// Store vertex
	storeVertexData(x, y, z);

	// Increase vertex counter
	++nDefinedVertices_;

	// Update centroid
	if (calcCentroids_) updateCentroid(x, y, z);

	// Return index of vertex
	return (nDefinedVertices_-1);
}

// Define next vertex, normal, and texture coordinate
GLuint Primitive::defineVertexNT(Vec3<double> vertex, Vec3<double> normal, GLfloat tx, GLfloat ty)
{
	return defineVertexNT(vertex.x, vertex.y, vertex.z, normal.x, normal.y, normal.z, tx, ty);
}

// Define next vertex, normal, and texture coordinate
GLuint Primitive::defineVertexNCT(GLfloat x, GLfloat y, GLfloat z, GLfloat nx, GLfloat ny, GLfloat nz, GLfloat r, GLfloat g, GLfloat b, GLfloat a, GLfloat tx, GLfloat ty)
{
	if (nDefinedVertices_ >= maxVertices_)
	{
		printf("Internal Error: Vertex limit for VertexChunk reached.\n");
		return -1;
	}

	// Store 2D texture coordinate
	if (!hasTextureCoordinateData_)
	{
		printf("Internal Error: Texture coordinate specified in vertex creation, but it is not required for primitive.\n");
		return -1;
	}
	storeVertexData(tx, ty);

	// Store colour
	if (!hasColourData_)
	{
		printf("Internal Error: Colour specified in vertex creation, but it is not required for primitive.\n");
		return -1;
	}
	storeVertexData(r, g, b, a);

	// Store normal
	storeVertexData(nx, ny, nz);

	// Store vertex
	storeVertexData(x, y, z);

	// Increase vertex counter
	++nDefinedVertices_;

	// Update centroid
	if (calcCentroids_) updateCentroid(x, y, z);

	// Return index of vertex
	return (nDefinedVertices_-1);
}

// Define next vertex, normal, and colour (as Vec3<double>s and array) and texture coordinate
GLuint Primitive::defineVertexNCT(Vec3<double> &v, Vec3<double> &u, Vec4<GLfloat> &colour, GLfloat tx, GLfloat ty)
{
	defineVertexNCT(v.x, v.y, v.z, u.x, u.y, u.z, colour.x, colour.y, colour.z, colour.w, tx, ty);
}

// Define next index double
bool Primitive::defineIndices(GLuint a, GLuint b)
{
	if ((maxIndices_-nDefinedIndices_) < 2)
	{
		printf("Internal Error: Index limit for IndexChunk reached.\n");
		return false;
	}
	// Store indices
	indexData_[nDefinedIndices_++] = a;
	indexData_[nDefinedIndices_++] = b;
	return true;
}

// Define next index triple
bool Primitive::defineIndices(GLuint a, GLuint b, GLuint c)
{
	if ((maxIndices_-nDefinedIndices_) < 3)
	{
		printf("Internal Error: Index limit for IndexChunk reached.\n");
		return false;
	}
	// Store indices
	indexData_[nDefinedIndices_++] = a;
	indexData_[nDefinedIndices_++] = b;
	indexData_[nDefinedIndices_++] = c;
	return true;
}

/*
 * Texture
 */

// Set texture image
void Primitive::setTextureImage(QImage& image)
{
	textureImage_ = image;
}

/*
 * Geometric Primitive Generation
 */

// Draw line
void Primitive::line(double x1, double y1, double z1, double x2, double y2, double z2)
{
	defineVertexN(x1, y1, z1, 1.0, 0.0, 0.0);
	defineVertexN(x2, y2, z2, 1.0, 0.0, 0.0);
}

// Add line to axis primitive
void Primitive::line(Vec3<double> v1, Vec3<double> v2)
{
	defineVertexN(v1.x, v1.y, v1.z, 1.0, 0.0, 0.0);
	defineVertexN(v2.x, v2.y, v2.z, 1.0, 0.0, 0.0);
}

// Draw disc
void Primitive::disc(double radius, int nSegments)
{
	double delta = TWOPI/nSegments;

	for (int n=0; n<nSegments; ++n)
	{
		defineVertexN(radius*cos(n*delta), radius*sin(n*delta), 0.0, 0.0, 0.0, 1.0);
		defineVertexN(radius*cos((n+1)*delta), radius*sin((n+1)*delta), 0.0, 0.0, 0.0, 1.0);
		defineVertexN(0.0, 0.0, 0.0, 0.0, 0.0, 1.0);
	}
}

// Draw rectangle
void Primitive::rectangle(double cx, double cy, double cz, double width, double height, bool withTexture)
{
	defineVertexNT(cx-0.5*width, cy-0.5*height, cz, 0.0, 0.0, 1.0, 0.0, 0.0);
	defineVertexNT(cx+0.5*width, cy-0.5*height, cz, 0.0, 0.0, 1.0, 1.0, 0.0);
	defineVertexNT(cx+0.5*width, cy+0.5*height, cz, 0.0, 0.0, 1.0, 1.0, 1.0);
	defineVertexNT(cx+0.5*width, cy+0.5*height, cz, 0.0, 0.0, 1.0, 1.0, 1.0);
	defineVertexNT(cx-0.5*width, cy+0.5*height, cz, 0.0, 0.0, 1.0, 0.0, 1.0);
	defineVertexNT(cx-0.5*width, cy-0.5*height, cz, 0.0, 0.0, 1.0, 0.0, 0.0);
}
