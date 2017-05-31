/*
	*** Viewer Functions
	*** src/gui/viewer_funcs.cpp
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

// On OSX must include glextensions.h before any other file since (or OSX doesn't see PROC typedefs) since gl.h and gl3.h clash
// However, on normal unix systems it must be included *after* viewer.uih, otherwise Qt throws lots of errors.
#ifdef __APPLE__
#include "render/glextensions.h"
#endif
#include "gui/viewer.uih"
#include "gui/quapp.h"
#include "base/messenger.h"
#include "render/fontinstance.h"
#ifndef __APPLE__
#include "render/glextensions.h"
#endif

// Constructor
Viewer::Viewer(QWidget *parent) : QGLWidget(parent)
{
	// Setup context format
	QGLFormat format;
	format.setSampleBuffers(true);
	format.setDirectRendering(true);
	setFormat(format);

	quapp_ = NULL;

	// Character / Setup
	contextWidth_ = 0;
	contextHeight_ = 0;
	valid_ = false;
	drawing_ = false;
	renderingOffScreen_ = false;

	// Prevent QPainter from autofilling widget background
	setAutoFillBackground(false);
}

// Destructor
Viewer::~Viewer()
{
}

// Set QuappWindow pointer
void Viewer::setQuapp(QuappWindow* ptr)
{
	quapp_ = ptr;
}

/*
// Protected Qt Virtuals
*/

// Initialise context widget (when created by Qt)
void Viewer::initializeGL()
{
	// Initialize GL
	msg.enter("Viewer::initializeGL");
	valid_ = true;

	// Create a GLExtensions object to probe features and give when pushing instances etc.
	GLExtensions* extensions = extensionsStack_.add();

	// Check for vertex buffer extensions
	if ((!extensions->hasVBO()) && (PrimitiveInstance::globalInstanceType() == PrimitiveInstance::VBOInstance))
	{
		printf("VBO extension is requested but not available, so reverting to display lists instead.\n");
		PrimitiveInstance::setGlobalInstanceType(PrimitiveInstance::ListInstance);
	}

	msg.exit("Viewer::initializeGL");
}

void Viewer::paintGL()
{
	msg.enter("Viewer::paintGL");

	// Do nothing if the canvas is not valid, or we are still drawing from last time, or the Quapp pointer has not been set
	if ((!valid_) || drawing_ || (!quapp_))
	{
		msg.exit("Viewer::paintGL");
		return;
	}

	// Set the drawing flag so we don't have any rendering clashes
	drawing_ = true;

	// Grab topmost GLExtensions pointer
	GLExtensions* extensions = extensionsStack_.last();

	// Setup basic GL stuff
	setupGL();

	// Set viewport to be entire window
	glViewport(0, 0, width(), height());

	// Set colour mode
	glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
	glEnable(GL_COLOR_MATERIAL);

	// Clear view
	glClear(GL_COLOR_BUFFER_BIT);
	glClear(GL_DEPTH_BUFFER_BIT);

	// Set some colours
	GLfloat colourBlack[4] = { 0.0, 0.0, 0.0, 1.0 };
	GLfloat colourGray[4] = { 0.8, 0.8, 0.8, 1.0 };
	GLfloat colourBlue[4] = { 0.88, 0.95, 1.0, 1.0 };
	GLfloat colourWhite[4] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat colourTransparentGray[4] = { 0.0, 0.0, 0.0, 0.3 };
	glEnable(GL_MULTISAMPLE);
	glEnable(GL_BLEND);
	glEnable(GL_LIGHTING);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	/*
	 * Draw Background Objects
	 */

	// -- Set projection matrix
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0.0, aspectRatio_, 0.0, 1.0, -1.0, 2000.0);

	// -- Switch to modelview matrix and draw background objects
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	for (DisplayObject* object = quapp_->backgroundObjects().first(); object != NULL; object = object->next)
	{
		if (object->primitive().nInstances() == 0) object->primitive().pushInstance(context(), extensions, this);
		object->sendToGL(aspectRatio_);
	}

	/*
	 * Draw Foreground
	 */

	// -- Set projection matrix
	glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
	glFrustum(-0.5*aspectRatio_, 0.5*aspectRatio_, -0.5, 0.5, 1.0, 100.0);

	// -- Switch to modelview matrix and draw background objects
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
	glClear(GL_DEPTH_BUFFER_BIT);

	// -- Persistent foreground objects
	for (DisplayObject* object = quapp_->persistentForegroundObjects().first(); object != NULL; object = object->next)
	{
		if (object->primitive().nInstances() == 0) object->primitive().pushInstance(context(), extensions, this);
		object->sendToGL(aspectRatio_);
	}

	// -- Other foreground objects
	for (DisplayObject* object = quapp_->foregroundObjects().first(); object != NULL; object = object->next)
	{
		if (object->primitive().nInstances() == 0) object->primitive().pushInstance(context(), extensions, this);
		object->sendToGL(aspectRatio_);
	}

	// Set the rendering flag to false
	drawing_ = false;

	// If we were rendering offscreen, delete the topmost GLExtensions object here (primitives will have already been popped)
	if (renderingOffScreen_) extensionsStack_.removeLast();
	
	msg.exit("Viewer::paintGL");
}

// Resize function
void Viewer::resizeGL(int newwidth, int newheight)
{
	// Store the new width and height of the widget
	contextWidth_ = (GLsizei) newwidth;
	contextHeight_ = (GLsizei) newheight;
	aspectRatio_ = (double (contextWidth_) / double (contextHeight_));

	// Adjust aspect ratio by modified
	if (quapp_) aspectRatio_ *= quapp_->aspectModifier();
}

// Setup basic GL properties (called each time before renderScene())
void Viewer::setupGL()
{
	msg.enter("Viewer::setupGL");

	// Define colours etc.
	GLfloat backgroundColour[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
	GLfloat spotlightAmbient[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
	GLfloat spotlightDiffuse[4] = { 0.8f, 0.8f, 0.8f, 1.0f };
	GLfloat spotlightSpecular[4] = { 0.7f, 0.7f, 0.7f, 1.0f };
	GLfloat spotlightPosition[4] = { 0.0f, 0.0f, -10.0f, 0.0f };
	GLfloat specularColour[4] = { 0.9f, 0.9f, 0.9f, 1.0f };

	// Clear (background) colour
	glClearColor(backgroundColour[0], backgroundColour[1], backgroundColour[2], backgroundColour[3]);

	// Perspective hint
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_FASTEST);

	// Enable depth buffer
	glEnable(GL_DEPTH_TEST);

	// Smooth shading
	glShadeModel(GL_SMOOTH);

	// Auto-normalise surface normals
	glEnable(GL_NORMALIZE);	

	// Set up the light model
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glEnable(GL_LIGHTING);
	glLightfv(GL_LIGHT0, GL_AMBIENT, spotlightAmbient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, spotlightDiffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, spotlightSpecular);
	glLightfv(GL_LIGHT0, GL_POSITION, spotlightPosition);
	glEnable(GL_LIGHT0);

	// Set specular reflection colour
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specularColour);
	glMateriali(GL_FRONT_AND_BACK, GL_SHININESS, 127);

	// Configure antialiasing
	glDisable(GL_MULTISAMPLE);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);
	glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);

	// Configure fog effects
//	glFogi(GL_FOG_MODE, GL_LINEAR);
//	prefs.copyColour(Prefs::BackgroundColour, col);
//	glFogfv(GL_FOG_COLOR, col);
//	glFogf(GL_FOG_DENSITY, 0.35f);
//	glHint(GL_FOG_HINT, GL_NICEST);
//	glFogi(GL_FOG_START, prefs.depthNear());
//	glFogi(GL_FOG_END, prefs.depthFar());
//	glEnable(GL_FOG);
	glDisable(GL_FOG);

	// Configure face culling
// 	glCullFace(GL_BACK);
// 	glEnable(GL_CULL_FACE);

	glEnable(GL_TEXTURE_2D);

	msg.exit("Viewer::setupGL");
}

/*
// Character / Setup
*/

// Return the current height of the drawing area
GLsizei Viewer::contextHeight() const
{
	return contextHeight_;
}

// Return the current width of the drawing area
GLsizei Viewer::contextWidth() const
{
	return contextWidth_;
}

// Return aspect ratio of the drawing area
double Viewer::aspectRatio()
{
	return aspectRatio_;
}

// Probe features
void Viewer::probeFeatures()
{
	QGLFormat fmt = context()->format();
	// Probe this format!
	printf(" QGLFormat: Alpha buffer is %s.\n", fmt.alpha() ? "enabled" : "disabled");
	printf(" QGLFormat: Accumulation buffer is %s.\n", fmt.accum() ? "enabled" : "disabled");
	printf(" QGLFormat: Depth buffer is %s.\n", fmt.depth() ? "enabled" : "disabled");
	printf(" QGLFormat: Double-buffering is %s.\n", fmt.doubleBuffer() ? "enabled" : "disabled");
	printf(" QGLFormat: Direct rendering is %s.\n", fmt.directRendering() ? "enabled" : "disabled");
	printf(" QGLFormat: RGBA colour mode is %s.\n", fmt.rgba() ? "enabled" : "disabled");
	printf(" QGLFormat: Multisample buffer is %s.\n", fmt.sampleBuffers() ? "enabled" : "disabled");
	printf(" QGLFormat: Stencil buffer is %s.\n", fmt.stencil() ? "enabled" : "disabled");
	printf(" QGLWidget: Autoswap buffers is %s.\n", autoBufferSwap() ? "enabled" : "disabled");
}

// Check for GL error
void Viewer::checkGlError()
{
	GLenum glerr = GL_NO_ERROR;
	do
	{
		switch (glGetError())
		{
			case (GL_INVALID_ENUM): msg.print(Messenger::Verbose, "GLenum argument out of range\n"); break;
			case (GL_INVALID_VALUE): msg.print(Messenger::Verbose, "Numeric argument out of range\n"); break;
			case (GL_INVALID_OPERATION): msg.print(Messenger::Verbose, "Operation illegal in current state\n"); break;
			case (GL_STACK_OVERFLOW): msg.print(Messenger::Verbose, "Command would cause a stack overflow\n"); break;
			case (GL_STACK_UNDERFLOW): msg.print(Messenger::Verbose, "Command would cause a stack underflow\n"); break;
			case (GL_OUT_OF_MEMORY): msg.print(Messenger::Verbose, "Not enough memory left to execute command\n"); break;
			case (GL_NO_ERROR): msg.print(Messenger::Verbose, "No GL error\n"); break;
			default:
				msg.print(Messenger::Verbose, "Unknown GL error?\n");
				break;
		}
	} while (glerr != GL_NO_ERROR);
}

// Refresh widget / scene
void Viewer::postRedisplay()
{
	if ((!valid_) || drawing_) return;
	update();
}
