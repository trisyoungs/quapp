/*
	*** Text Primitive
	*** src/render/textprimitive.h
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

#ifndef QUAPP_TEXTPRIMITIVE_H
#define QUAPP_TEXTPRIMITIVE_H

#include "render/textfragment.h"
#include "render/textformat.h"
#include "math/matrix.h"
#include "math/cuboid.h"
#include "templates/vector3.h"
#include "templates/list.h"
#include <QtCore/QString>

// External Declarations
extern int TextPrimitiveParser_parse();

// Forward Declarations
/* none */

// Text Primitive
class TextPrimitive : public ListItem<TextPrimitive>
{
	public:
	// Constructor / Destructor
	TextPrimitive();
	~TextPrimitive();
	// Text Anchors enum
	enum TextAnchor { TopLeftAnchor, TopMiddleAnchor, TopRightAnchor, MiddleLeftAnchor, CentralAnchor, MiddleRightAnchor, BottomLeftAnchor, BottomMiddleAnchor, BottomRightAnchor, nTextAnchors };
	// Convert text string to TextAnchor
	static TextAnchor textAnchor(QString s);
	// Convert TextAnchor to text string
	static const char* textAnchor(TextAnchor anchor);
	// Escape Sequence enum
	enum EscapeSequence { BoldEscape, ItalicEscape, NewLineEscape, SubScriptEscape, SuperScriptEscape, nEscapeSequences };
	// Convert text string to EscapeSequence
	static EscapeSequence escapeSequence(QString s);


	/*
	 * Definition
	 */
	private:
	// Location of anchorpoint on text bounding box
	TextAnchor anchorPosition_;
	// Text size
	double textSize_;
	// Text fragments to render
	List<TextFragment> fragments_;

	public:
	// Clear fragments
	void clear();
	// Set text size
	void setTextSize(double textSize);
	// Return text size
	double textSize();
	// Set text
	void set(QString text, TextAnchor anchorPosition);
	// Return transformation matrix to use when rendering (including fragment scale/translation if one is specified)
	Matrix transformationMatrix(TextFragment* fragment = 0) const;
	// Calculate bounding box of primitive
	void boundingBox(Vec3<double>& lowerLeft, Vec3<double>& upperRight) const;
	// Return total width of text primitive
	double textWidth();
	// Sent to GL
	void sendToGL() const;


	/*
	 * Generation
	 */
	private:
	// Character string source
	static QString stringSource_;
	// Integer position in stringSource, total length of string, and starting position of current token/function
	static int stringPos_, stringLength_;
	// Get next character from current input stream
	static QChar getChar();
	// Peek next character from current input stream
	static QChar peekChar();
	// 'Replace' last character read from current input stream
	static void unGetChar();
	// Current target for generation
	static TextPrimitive* target_;
	// Format stack, used when generating primitive
	static List<TextFormat> formatStack_;
	// Current horizontal position, used when generating primitive
	static double horizontalPosition_;

	public:
	// Parser lexer, called by yylex()
	static int lex();
	// Generate TextFragment data for specified TextPrimitive from supplied string
	static bool generateFragments(TextPrimitive* target, QString inputString);
	// Return current target
	static TextPrimitive* target();
	// Add text fragment
	bool addFragment(QString text);
	// Add escape marker
	static bool addEscape(TextPrimitive::EscapeSequence escSeq);
	// Remove escape marker
	static void removeEscape();
};

#endif
