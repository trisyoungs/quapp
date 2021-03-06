/*
	*** GL Extensions
	*** src/render/glextensions.h
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

#ifndef QUAPP_GLEXTENSIONS_H
#define QUAPP_GLEXTENSIONS_H

#ifdef _WIN32
#include <windows.h>
#include <GL/gl.h>
#include "glext.h"
#elif __APPLE__
#include <OpenGL/gl3.h>
#else
#include <GL/glx.h>
#endif
#include "templates/list.h"
 
// GL Extensions
class GLExtensions : public ListItem<GLExtensions>
{
	public:
	// Constructor
	GLExtensions();


	/*
	 * Query Extensions
	 */
	private:
	// Whether we have the query extensions
	bool hasQueries_;

	public:
	// Return whether we have the query extension
	bool hasQueries();
	// Query function pointers
	PFNGLBEGINQUERYPROC glBeginQuery;
	PFNGLENDQUERYPROC glEndQuery;
	PFNGLGENQUERIESPROC glGenQueries;
	PFNGLGETQUERYOBJECTIVPROC glGetQueryObjectiv;
	PFNGLGETQUERYOBJECTUI64VPROC glGetQueryObjectui64v;


	/*
	 * VBO Extensions
	 */
	private:
	// Whether we have the VBO extension
	bool hasVBO_;

	public:
	// Return whether we have the VBO extension
	bool hasVBO();
	// VBO function pointers
	PFNGLGENBUFFERSPROC glGenBuffers;
	PFNGLBINDBUFFERPROC glBindBuffer;
	PFNGLBUFFERDATAPROC glBufferData;
	PFNGLBUFFERSUBDATAPROC glBufferSubData;
	PFNGLDELETEBUFFERSPROC glDeleteBuffers;
};

#endif
