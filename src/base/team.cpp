/*
	*** Team
	*** src/base/team.cpp
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

#include "base/team.h"
#include <QtCore/QString>

// Constructor
Team::Team()
{
	name_ = "NewTeam";
}

// Destructor
Team::~Team()
{
}

/*
 * Definition
 */

// Set team name
void Team::setName(QString name)
{
	name_ = name;
}

// Return team name
QString Team::name()
{
	return name_;
}

// Reset total score for team
void Team::resetScore()
{
	score_ = 0.0;
}

// Add to total score for team
void Team::addToScore(double delta)
{
	score_ += delta;
}

// Return total score for team
double Team::score()
{
	return score_;
}

// Set rank for team
void Team::setRank(int rank)
{
	rank_ = rank;
}

// Return rank for team
int Team::rank()
{
	return rank_;
}
