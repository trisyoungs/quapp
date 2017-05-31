/*
	*** Quapp - Background
	*** src/gui/quapp_background.cpp
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
#include "math/mathfunc.h"

// BackgroundType Keywords
const char* BackgroundTypeKeywords[] = { "No Background", "Circles" };

// Return string for background type
const char* QuappWindow::backgroundType(BackgroundType bt)
{
	return BackgroundTypeKeywords[bt];
}

// Initialise background
void QuappWindow::initialiseBackground(BackgroundType type)
{
	DisplayObject* object;
	double d, r, rgb;
	Vec3<double> u, v;
	QColor colourA, colourB;

	backgroundType_ = type;

	switch (backgroundType_)
	{
		// No Background
		case (QuappWindow::NoBackground):
			backgroundObjects_.clear();
			break;
		// TEST
		case (QuappWindow::TestBackground):
			// Create some discs at random positions and with random colours
			for (int n=0; n<200; ++n)
			{
				// -- Add an object
				object = backgroundObjects_.add();
				// -- Set random starting position
				u.set(QuappMath::random(), QuappMath::random(), 10-n*0.01);
				// -- Set random radius, move vector, step delta
				r = 0.1 * QuappMath::random();
				if (QuappMath::random() < 0.5) v.set(QuappMath::random() < 0.5 ? -r : 1.0+r, QuappMath::random()*(1.0+2*r)-r, 10-n*0.01);
				else v.set(QuappMath::random()*(1.0+2*r)-r, QuappMath::random() < 0.5 ? -r : 1.0+r, 10-n*0.01);
				// -- Set starting / finishing colours
				// -- 10 percent chance of a colour - otherwise gray
				if (QuappMath::random() > 0.9) colourA.setRgbF(QuappMath::random(), QuappMath::random(), QuappMath::random(), 0.0);
				else
				{
					rgb = QuappMath::random();
					colourA.setRgbF(rgb, rgb, rgb, 0.0);
				}
				colourB = colourA;
				colourB.setAlphaF(0.15);
				// -- Add dynamics
				object->addTransform()->setTranslationTransform(u, v, (v-u).magnitude()*0.001+QuappMath::random()*0.002);
				object->addTransform()->setColourTransform(colourA, colourB, 0.01+0.02*QuappMath::random());
				object->addTransform()->setScaleTransform(Vec3<double>(0.0, 0.0, 0.0), Vec3<double>(1.0, 1.0, 1.0), 0.01);
				// -- Create primitive and store radius data
				object->primitive().addPrimitive(150, 0, GL_TRIANGLES, false, false)->disc(r, 36);
				object->setData(Vec3<double>(r, 0.0, u.z));
			}
			break;
	}
}

// Update background
void QuappWindow::updateBackground()
{
	DisplayObject* object;
	double d, r, rgb;
	Vec3<double> u, v;
	QColor colourA, colourB;
	Matrix transform;

	switch (backgroundType_)
	{
		// No Background
		case (QuappWindow::NoBackground):
			break;
		// TEST
		case (QuappWindow::TestBackground):
			// Restart any old (non-moving) discs with new positions, colour etc.
			for (DisplayObject* object = backgroundObjects_.first(); object != NULL; object = object->next)
			{
				if (object->isTransforming()) continue;
				// -- Set random starting position
				u.set(QuappMath::random(), QuappMath::random(), object->data().z);
				// -- Get stored radius, and calculate finishing position
				r = object->data().x;
				if (QuappMath::random() < 0.5) v.set(QuappMath::random() < 0.5 ? -r : 1.0+r, QuappMath::random()*(1.0+2*r)-r, u.z);
				else v.set(QuappMath::random()*(1.0+2*r)-r, QuappMath::random() < 0.5 ? -r : 1.0+r, u.z);
				// -- Set colours
				// -- 10 percent chance of a colour - otherwise gray
				if (QuappMath::random() > 0.9) colourA.setRgbF(QuappMath::random(), QuappMath::random(), QuappMath::random(), 0.0);
				else
				{
					rgb = QuappMath::random();
					colourA.setRgbF(rgb, rgb, rgb, 0.0);
				}
				colourB = colourA;
				colourB.setAlphaF(0.15);
				object->setColour(colourA);
				// -- Reactivate dynamics
				object->clearTransforms();
				object->addTransform()->setTranslationTransform(u, v, (v-u).magnitude()*0.001+QuappMath::random()*0.002);
				object->addTransform()->setColourTransform(colourA, colourB, 0.01+0.02*QuappMath::random());
				object->addTransform()->setScaleTransform(Vec3<double>(0.0, 0.0, 0.0), Vec3<double>(1.0, 1.0, 1.0), 0.01);
			}
			break;
	}
}
