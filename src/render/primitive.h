/*
	*** Rendering Primitive
	*** src/render/primitive.h
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

#ifndef QUAPP_PRIMITIVE_H
#define QUAPP_PRIMITIVE_H

#ifdef _WIN32
#include <windows.h>
#include <GL/gl.h>
#include "glext.h"
#endif
#include "render/primitiveinstance.h"
#include "math/matrix.h"
#include "templates/list.h"
#include <QtGui/QImage>

// Forward Declarations
class QGLWidget;

// Rendering Primitive
class Primitive : public ListItem<Primitive>
{
	public:
	// Constructor / Destructor
	Primitive();
	~Primitive();


	/*
	 * Data
	 */
	private:
	// Vertex data array (containing normals and, possibly, colour and texture coordinate data)
	GLfloat* vertexData_;
	// Index data array
	GLuint* indexData_;
	// Centroid array
	GLfloat* centroids_;
	// Number of data points per vertex (NR=6, TNR=8, CNR=10)
	int dataPerVertex_;
	// Number of defined vertices in chunk
	int nDefinedVertices_;
	// Number of defined indices in chunk
	int nDefinedIndices_;
	// Nomber of primitive types (nDefinedVertices/verticesPerType) currently defined (only for non-indexed data)
	int nDefinedTypes_;
	// Maximum number of vertices
	int maxVertices_;
	// Maximum number of indices
	int maxIndices_;
	// Whether indices are being used when rendering
	bool hasIndices_;
	// Whether vertex data also contains colour information
	bool hasColourData_;
	// Whether vertex data also contains texture coordinate information
	bool hasTextureCoordinateData_;
	// Maximum number of types (for non-indexed data)
	int maxTypes_;
	// Primitive type (GL)
	GLenum type_;
	// Interleaved array type (GL)
	GLenum interleaveType_;
	// Number of vertices per primitive type
	int verticesPerType_;
	// Whether centroid calculation (per primitive type) is on
	bool calcCentroids_;
	// Stack of OpenGL VBO or display list IDs and the contexts in which they were created
	List<PrimitiveInstance> instances_;
	// Flag stating whether or not instances should be used for this primitive
	bool useInstances_;

	public:
	// Initialise primitive storage
	void initialise(int newMaxVertices, int newMaxIndices, GLenum type, bool colourData, bool textureData);
	// Forget all data, leaving arrays intact
	void forgetAll();
	// Return number of vertices currently defined in primitive
	int nDefinedVertices() const;
	// Return number of indices currently defined in primitive
	int nDefinedIndices() const;
	// Return whether vertex data contains colour information
	bool hasColourData() const;
	// Return whether vertex data contains texture coordinate information
	bool hasTextureCoordinateData() const;
	// Flag that this primitive should not use instances (rendering will use vertex arrays)
	void setNoInstances();
	// Push instance layer from current vertex chunk list
	void pushInstance(const QGLContext* context, GLExtensions* extensions, QGLWidget* glWidget);
	// Pop topmost instance layer
	void popInstance(const QGLContext *context);
	// Return number of instances available
	int nInstances();
	// Send to OpenGL (i.e. render)
	void sendToGL(bool drawSimple = false) const;


	/*
	 * Vertex / Index Generation
	 */
	private:
	// Current index for vertex array
	int vertexIndex_;
	// Store data (2)
	void storeVertexData(GLfloat a, GLfloat b);
	// Store data (3)
	void storeVertexData(GLfloat a, GLfloat b, GLfloat c);
	// Store data (4)
	void storeVertexData(GLfloat a, GLfloat b, GLfloat c, GLfloat d);
	// Update (and finalise) centroid for current primitive type
	void updateCentroid(GLfloat x, GLfloat y, GLfloat z);

	public:
	// Define next vertex and normal
	GLuint defineVertexN(GLfloat x, GLfloat y, GLfloat z, GLfloat nx, GLfloat ny, GLfloat nz);
	// Define next vertex and normal (as Vec3<double>)
	GLuint defineVertexN(Vec3<double> vertex, Vec3<double> normal);
	// Define next vertex, normal, and colour
	GLuint defineVertexNC(GLfloat x, GLfloat y, GLfloat z, GLfloat nx, GLfloat ny, GLfloat nz, GLfloat r, GLfloat g, GLfloat b, GLfloat a);
	// Define next vertex, normal, and colour
	GLuint defineVertexNC(GLfloat x, GLfloat y, GLfloat z, Vec3<double>& normal, Vec4<GLfloat>& colour);
	// Define next vertex, normal, and colour (as Vec3<double>s and array)
	GLuint defineVertexNC(Vec3<double> &v, Vec3<double> &u, Vec4<GLfloat> &colour);
	// Define next vertex, normal, and texture coordinate
	GLuint defineVertexNT(GLfloat x, GLfloat y, GLfloat z, GLfloat nx, GLfloat ny, GLfloat nz, GLfloat tx, GLfloat ty);
	// Define next vertex, normal, and texture coordinate
	GLuint defineVertexNT(Vec3<double> vertex, Vec3<double> normal, GLfloat tx, GLfloat ty);
	// Define next vertex, normal, and colour (as array)
	GLuint defineVertexNCT(GLfloat x, GLfloat y, GLfloat z, GLfloat nx, GLfloat ny, GLfloat nz, GLfloat r, GLfloat g, GLfloat b, GLfloat a, GLfloat tx, GLfloat ty);
	// Define next vertex, normal, and colour (as Vec3<double>s and array) and texture coordinate
	GLuint defineVertexNCT(Vec3<double> &v, Vec3<double> &u, Vec4<GLfloat> &colour, GLfloat tx, GLfloat ty);
	// Define next index double
	bool defineIndices(GLuint a, GLuint b);
	// Define next index triple
	bool defineIndices(GLuint a, GLuint b, GLuint c);


	/*
	 * Texture
	 */
	private:
	// Image to use for texture
	QImage textureImage_;

	public:
	// Set texture image
	void setTextureImage(QImage& image);


	/*
	 * Geometric Primitive Generation
	 */
	public:
	// Draw line
	void line(double x1, double y1, double z1, double x2, double y2, double z2);
	// Draw line
	void line(Vec3<double> v1, Vec3<double> v2);
	// Draw disc
	void disc(double radius, int nSegments);
	// Draw rectangle
	void rectangle(double cx, double cy, double cz, double width, double height, bool withTexture = false);
};

#endif
