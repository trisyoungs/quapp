/*
	*** Team
	*** src/base/team.h
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

#ifndef QUAPP_TEAM_H
#define QUAPP_TEAM_H

#include "templates/list.h"
#include <QtCore/QString>

// Forward Declarations
/* none */

// Team
class Team : public ListItem<Team>
{
	public:
	// Constructor / Destructor
	Team();
	~Team();


	/*
	 * Definition
	 */
	private:
	// Team name
	QString name_;
	// Total score for team
	double score_;
	// Current rank for team
	int rank_;

	public:
	// Set team name
	void setName(QString name);
	// Return team name
	QString name();
	// Reset total score for team
	void resetScore();
	// Add to total score for team
	void addToScore(double delta);
	// Return total score for team
	double score();
	// Set rank for team
	void setRank(int rank);
	// Return rank for team
	int rank();
};

#endif
