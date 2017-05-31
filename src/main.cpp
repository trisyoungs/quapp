/*
	*** uChroma Main
	*** src/main.cpp
	Copyright T. Youngs 2013-2014

	This file is part of uChroma.

	uChroma is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	uChroma is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with uChroma.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "version.h"
#include "gui/quapp.h"
#include "base/messenger.h"

int main(int argc, char *argv[])
{	
	/* Create the main QApplication */
	QApplication app(argc, argv, QApplication::GuiClient);
	QCoreApplication::setOrganizationName("uChroma");
	QCoreApplication::setOrganizationDomain("www.projectaten.net");
	QCoreApplication::setApplicationName("uChroma");

	#if QT_VERSION >= 0x040600
	QGL::setPreferredPaintEngine(QPaintEngine::OpenGL);
	#endif

	/* Tweak the default QGLFormat */
	QGLFormat::defaultFormat().setSampleBuffers(true);

	/* Create the main window */
	QuappWindow mainWindow;

	/* Do we have CLI options? */
	bool fileLoaded = false;
	if (argc > 1)
	{
		int n = 1;
		while (n < argc)
		{
			if (argv[n][0] != '-')
			{
				// Must be an input file to load....
				fileLoaded = mainWindow.quiz().load(argv[n]);
				if (!fileLoaded) return 1;
				++n;
				continue;
			}

			bool missingArg = false;

			// Command-line switch
			switch (argv[n][1])
			{
				case ('h'):
					printf("Quapp revision %s, %s\n\nAvailable CLI options are:\n\n", QUAPPREVISION, QUAPPDATE);
					printf("\t-h\t\tShow this help\n");
					return 1;
					break;
				default:
					msg.print("Unrecognised command-line switch '%s'.\n", argv[n]);
					msg.print("Run with -h to see available switches.\n");
					return 1;
					break;
			}

			// Check for missing argument flag
			if (missingArg)
			{
				msg.print("Error: Argument expected but none was given for switch '%s'\n", argv[n]);
				return 1;
			}

			++n;
		}
	}

	/* Update main window */
	mainWindow.updateQuestionSets();
	mainWindow.updateQuestions();
	mainWindow.updateSegments();
	mainWindow.updateRunData();
	mainWindow.updateRunControls();
	
	/* Show the main window */
	mainWindow.show();

	/* Enter Qt's main events loop */
	return app.exec();
}

