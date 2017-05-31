/*
	*** Math functions
	*** src/math/mathfunc.h
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

#ifndef QUAPP_MATHFUNC_H
#define QUAPP_MATHFUNC_H

class Matrix;

// Mathematical functions
class QuappMath
{
	public:

	// Error functions
	static double erfc(double);
	static double erf(double);

	// Random number generation
	static double random();
	static int randomimax();
	static int randomi(int range);

	// Integer power function
	static int power(int i, int p);

	// Sign function
	static int sgn(int x);

	// Calculate area of triangle from supplied coordinates
	static double triangleArea(double ax, double ay, double bx, double by, double cx, double cy);
};

#endif
