/*
	*** Messaging/debug routines
	*** src/base/messenger.cpp
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

#include "base/messenger.h"
#include "base/sysfunc.h"
#include "math/constants.h"
#include <stdarg.h>
#include <stdio.h>
#include <QtGui/QTextBrowser>

// Singleton
Messenger msg;

// Message output types
const char* OutputTypeKeywords[] = { "all", "calls", "_ERROR_", "verbose" };
Messenger::OutputType Messenger::outputType(const char* s, bool reportError)
{
	Messenger::OutputType ot = (Messenger::OutputType) enumSearch("output type",Messenger::nOutputTypes,OutputTypeKeywords,s);
	if ((ot == Messenger::nOutputTypes) && reportError) enumPrintValid(Messenger::nOutputTypes,OutputTypeKeywords);
	return ot;
}
const char* Messenger::outputType(Messenger::OutputType ot)
{
	return OutputTypeKeywords[ot];
}

// Constructor
Messenger::Messenger()
{
	// Private variables
	outputTypes_= 0;
	callLevel_ = 0;
	quiet_ = false;
	textBrowser_ = NULL;
}

// Add a debug level to the debug output bitvector
void Messenger::addOutputType(Messenger::OutputType dm)
{
	// Convert output type into bit if necessary
	if (dm == Messenger::All) outputTypes_ = (1 << nOutputTypes) - 1;
	else if (!(outputTypes_&(1 << dm))) outputTypes_ += (1 << dm);
}

// Remove a debug level from the debug output bitvector
void Messenger::removeOutputType(Messenger::OutputType dm)
{
	// Convert output type into bit if necessary
	if (dm == Messenger::All) outputTypes_ = 0;
	else if (outputTypes_&(1 << dm)) outputTypes_ -= (1 << dm);
}

// Returns whether the specified debug level is set
bool Messenger::isOutputActive(Messenger::OutputType dm) const
{
	return ((outputTypes_&(1 << dm)) ? true : false);
}

// Set status of quiet mode
void Messenger::setQuiet(bool b)
{
	quiet_ = b;
}

// Return status of quiet mode
bool Messenger::isQuiet() const
{
	return quiet_;
}

/*
 * Messaging functions
 */

// Set target QTextBrowser
void Messenger::setTextBrowser(QTextBrowser* browser)
{
	textBrowser_ = browser;
}

// Standard message
void Messenger::print(const char* fmt, ...) const
{
	// Print to the text view in the main window if it has been initialised.
	// If program is in quiet mode, don't print anything to stdout
	// Otherwise, print to stdout. Also print to stdout if debuglevel >= msglevel.
	va_list arguments;
	static char msgs[8096];
	msgs[0] = '\0';
	// Parse the argument list (...) and internally write the output string into msgs[]
	va_start(arguments,fmt);
	vsprintf(msgs,fmt,arguments);
	if (!quiet_)
	{
		if (textBrowser_)
		{
			int n = strlen(msgs);
			if ((n > 0) && (msgs[n-1] == '\n')) msgs[n-1] = '\0';
			textBrowser_->append(msgs);
		}
		else printf("%s",msgs);
	}
	va_end(arguments);
}

// Standard message in specific output level
void Messenger::print(Messenger::OutputType ot, const char* fmt, ...) const
{
	// Print to the text view in the main window if it has been initialised.
	// If program is in quiet mode, don't print anything except Messenger::Force calls
	if (quiet_ && (ot != Messenger::Force)) return;
	va_list arguments;
	static char msgs[8096];
	msgs[0] = '\0';
	// Parse the argument list (...) and internally write the output string into msgs[]
	va_start(arguments,fmt);
	vsprintf(msgs,fmt,arguments);
	// Print message to stdout, but only if specified output type is active
	if (ot == Messenger::Force) printf("%s",msgs);
	else if (isOutputActive(ot) && (!quiet_))
	{
		if (textBrowser_)
		{
			int n = strlen(msgs);
			if ((n > 0) && (msgs[n-1] == '\n')) msgs[n-1] = '\0';
			textBrowser_->append(msgs);
		}
		else printf("%s",msgs);
	}
	va_end(arguments);
}

// Function enter
void Messenger::enter(const char* callname)
{
	if (!isOutputActive(Messenger::Calls)) return;
	// Debug Messaging - Enter Function
	printf("%2i ",callLevel_);
	for (int n=0; n<callLevel_; n++) printf("--");
	printf("Begin : %s...\n", callname);
	callLevel_ ++;
}

// Function leave
void Messenger::exit(const char* callname)
{
	if (!isOutputActive(Messenger::Calls)) return;
	// Debug Messaging - Leave Function
	callLevel_ --;
	printf("%2i ", callLevel_);
	for (int n=0; n<callLevel_; n++) printf("--");
	printf("End   : %s.\n", callname);
}
