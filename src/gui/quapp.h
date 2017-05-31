/*
	*** Quapp Main Window
	*** src/gui/quapp.h
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

#ifndef QUAPP_MAINWINDOW_H
#define QUAPP_MAINWINDOW_H

#include "gui/display.h"
#include "gui/ui_quapp.h"
#include "base/quiz.h"
#include "base/lineparser.h"
#include <QtCore/QDir>
#include <QtCore/QTimer>

// Forward Declarations
class QAudioOutput;

/*
 * Main Quapp Window
 */
class QuappWindow : public QMainWindow
{
	// All Qt declarations must include this macro
	Q_OBJECT


	/*
	 * Window Functions
	 */
	private:
	// Whether the window is refreshing / updating its controls
	bool refreshing_;
	// Font for viewer
	QString viewerFont_;
	// Display Window
	DisplayWindow displayWindow_;
	// Directory for image loading
	QDir imageDirectory_;
	// Directory for audio loading
	QDir audioDirectory_;

	public:
	// Constructor / Destructor
	QuappWindow(QMainWindow *parent = 0);
	~QuappWindow();
	// Main form declaration
	Ui::QuappWindow ui;
	// Return centre coordinate of main window
	QPoint centrePos();

	protected:
	// Window close event
	void closeEvent(QCloseEvent *event);


	/*
	 * Menus
	 */
	private:
	QDir inputFileDirectory_;

	private slots:
	// Menu
	void on_actionFileNew_triggered(bool checked);
	void on_actionFileOpen_triggered(bool checked);
	void on_actionFileSave_triggered(bool checked);
	void on_actionFileSaveAs_triggered(bool checked);
	void on_actionDisplayShow_triggered(bool checked);
	void on_actionDisplayRunFromStart_triggered(bool checked);
	void on_actionDisplayRunFromSegment_triggered(bool checked);
	void on_actionDisplayToggleFullscreen_triggered(bool checked);


	/*
	 * Questions
	 */
	private:
	// Return current QuestionSet
	QuestionSet* currentQuestionSet();
	// Return current Question
	Question* currentQuestion();

	private slots:
	// QuestionSet list
	void on_QuestionSetAddButton_clicked(bool checked);
	void on_QuestionSetRemoveButton_clicked(bool checked);
	void on_QuestionSetMoveUpButton_clicked(bool checked);
	void on_QuestionSetMoveDownButton_clicked(bool checked);
	void on_QuestionSetList_currentRowChanged(int row);
	void on_QuestionSetList_itemDoubleClicked(QListWidgetItem* item);
	// Question list
	void on_QuestionAddButton_clicked(bool checked);
	void on_QuestionRemoveButton_clicked(bool checked);
	void on_QuestionMoveUpButton_clicked(bool checked);
	void on_QuestionMoveDownButton_clicked(bool checked);
	void on_QuestionList_currentRowChanged(int row);
	// Question edit
	void on_QuestionTextQuestionEdit_editingFinished();
	void on_QuestionTextAnswerEdit_editingFinished();
	void on_QuestionAudioQuestionSelectButton_clicked(bool checked);
	void on_QuestionAudioAnswerSelectButton_clicked(bool checked);
	void on_QuestionAudioQuestionPlayButton_clicked(bool checked);
	void on_QuestionAudioAnswerPlayButton_clicked(bool checked);
	void on_QuestionImageQuestionSelectButton_clicked(bool checked);
	void on_QuestionImageAnswerSelectButton_clicked(bool checked);

	public:
	// Update question set list
	void updateQuestionSets(QuestionSet* newCurrentQuestionSet = NULL);
	// Update segment data
	void updateQuestions(Question* newCurrentQuestion = 0);


	/*
	 * Running Order
	 */
	private:
	// Return current segment
	Segment* currentSegment();
	// Return current question item
	Question* currentSegmentItem();

	private slots:
	// Segments list
	void on_SegmentsList_currentRowChanged(int row);
	void on_SegmentsList_itemDoubleClicked(QListWidgetItem* item);
	void on_SegmentAddButton_clicked(bool checked);
	void on_SegmentRemoveButton_clicked(bool checked);
	void on_SegmentMoveUpButton_clicked(bool checked);
	void on_SegmentMoveDownButton_clicked(bool checked);
	// Segment Type
	void on_SegmentTypeTitleRadio_clicked(bool checked);
	void on_SegmentTypeQuestionRadio_clicked(bool checked);
	void on_SegmentTypeAnswerRadio_clicked(bool checked);
	void on_SegmentTypeScoreRadio_clicked(bool checked);
	// Segment Options
	void on_SegmentNonVisualCheck_clicked(bool checked);
	// Segment additional data
	void on_SegmentQuestionSourceCombo_currentIndexChanged(int index);
	void on_SegmentTitleTextEdit_textChanged(QString text);
	void on_SegmentSubTextEdit_textChanged(QString text);
	void on_SegmentImageSelectButton_clicked(bool checked);
	// Segment options
	void on_SegmentBodyTextScaleSpin_valueChanged(double value);
	void on_SegmentTextQuestionDisplayTextCheck_clicked(bool checked);
	void on_SegmentImageQuestionUseIndexCheck_clicked(bool checked);

	public:
	// Update segments list
	void updateSegments(Segment* newCurrentSegment = NULL, bool refreshList = true);


	/*
	 * Run
	 */
	private:
	// Return current Team
	Team* currentTeam();

	private slots:
	// Teams / Scores
	void on_TeamTable_currentItemChanged(QTableWidgetItem* item, QTableWidgetItem* prevItem);
	void on_TeamTable_itemDoubleClicked(QTableWidgetItem* item);
	void on_TeamTable_itemChanged(QTableWidgetItem* item);
	void on_TeamAddButton_clicked(bool checked);
	void on_TeamRemoveButton_clicked(bool checked);
	// Control
	void on_RunFromStartButton_clicked(bool checked);
	void on_RunFromSegmentButton_clicked(bool checked);
	void on_NextItemButton_clicked(bool checked);
	void on_NextSegmentButton_clicked(bool checked);
	void on_EndButton_clicked(bool checked);
	void on_NextAnythingButton_clicked(bool checked);

	public:
	// Update scores/teams data
	void updateRunData();
	// Update run controls
	void updateRunControls();


	/*
	 * Display Objects
	 */
	public:
	// Exit Transition type enum
	enum ExitTransformType { FadeOutTransform, nExitTransformTypes };
	// Entrance transition type enum
	enum EntranceTransformType { FadeInTransform, nEntranceTransformTypes };

	private:
	// Background objects
	List<DisplayObject> backgroundObjects_;
	// Foreground objects
	List<DisplayObject> foregroundObjects_;
	// Persistent foreground objects (e.g. headers, titles etc.)
	List<DisplayObject> persistentForegroundObjects_;
	// Object timer
	QTimer objectTimer_;
	// Fractional margin to use for foreground objects
	double foregroundMargin_;
	// Height of title line
	double titleLineHeight_;
	// Full height (descender to ascender) of title text
	double titleTextHeight_;
	// Number of 'lines' to use for text display
	int nBodyTextLines_;
	// Height of single text line (incuding line spacing)
	double bodyLineHeight_;
	// Full height (descender to ascender) of body text
	double bodyTextHeight_;
	// Scale modifier for aspect ratio (to adjust odd ratios on screen)
	double aspectModifier_;
	
	protected slots:
	// Update objects
	void updateObjects();

	private:
	// Calculate text heights
	void calculateTextHeights();
	// Apply transition to all objects in list
	void applyExitTransform(List<DisplayObject>& objects, QuappWindow::ExitTransformType transformType, double speed, double stallCoordinate = 0.0);
	// Apply entrance transition to specific object (using current position as destination)
	void applyEntranceTransform(DisplayObject* object, QuappWindow::EntranceTransformType transformType, double speed, double stallCoordinate = 0.0);

	public:
	// Background objects
	List<DisplayObject>& backgroundObjects();
	// Foreground objects
	List<DisplayObject>& foregroundObjects();
	// Persistent foreground objects
	List<DisplayObject>& persistentForegroundObjects();
	// Add an empty title primitive
	DisplayObject* addTitleObject();
	// Add a persistent foreground object
	DisplayObject* addPersistentForeGroundObject();
	// Add a foreground object
	DisplayObject* addForeGroundObject();
	// Add a list of empty text objects at the defined line positions on the screen, returning in a RefList
	RefList<DisplayObject,int> addForeGroundTextLines();
	// Return aspect ratio of display
	double displayAspectRatio();
	// Return width of display
	double displayWidth();
	// Return height of display
	double displayHeight();
	// Set fractional margin to use for foreground objects
	void setForegroundMargin(double margin);
	// Return fractional margin to use for foreground objects
	double foregroundMargin();
	// Return title line height
	double titleLineHeight();
	// Return title text height
	double titleTextHeight();
	// Return body line height
	double bodyLineHeight();
	// Return body text height
	double bodyTextHeight();
	// Return scale modifier for aspect ratio (to adjust odd ratios on screen)
	double aspectModifier();


	/*
	 * Quiz
	 */
	private:
	// Current quiz
	Quiz quiz_;

	public:
	// Return current quiz
	Quiz& quiz();
	// Start new session
	void startNewSession();


	/*
	 * Backgrounds
	 */
	public:
	// Background type enum
	enum BackgroundType { NoBackground, TestBackground, nBackgroundTypes };
	// Return string for background type
	static const char* backgroundType(BackgroundType bt);

	private:
	// Current background type
	BackgroundType backgroundType_;

	public:
	// Initialise background
	void initialiseBackground(BackgroundType type);
	// Update background
	void updateBackground();


	/*
	 * Audio
	 */
	private:
	// Current audio file being played back
	QFile currentAudioStream_;
	// Current audio output instance
	QAudioOutput* audioOutput_;

	public:
	// Play specified audio file
		bool playAudio(QString filename);
};

#endif
