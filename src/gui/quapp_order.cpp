/*
	*** Quapp - Order 
	*** src/gui/quapp_order.cpp
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
#include "base/session.h"
#include <templates/variantpointer.h>
#include <QtGui/QInputDialog>
#include <QtGui/QFileDialog>

// Return current segment
Segment* QuappWindow::currentSegment()
{
	if (ui.SegmentsList->currentRow() == -1) return NULL;
	return quiz_.segment(ui.SegmentsList->currentRow());
}

/*
 * Segment List
 */

void QuappWindow::on_SegmentsList_currentRowChanged(int row)
{
	ui.SegmentTypeGroup->setEnabled(row != -1);
	ui.SegmentDataGroup->setEnabled(row != -1);
	ui.SegmentRemoveButton->setEnabled(row != -1);
	if (row == -1) return;

	// Get segment
	Segment* segment = quiz_.segment(row);
	if (!segment) return;

	updateSegments(segment, false);
	updateRunControls();
}

void QuappWindow::on_SegmentsList_itemDoubleClicked(QListWidgetItem* item)
{
	if (!item) return;

	// Get segment from item
	Segment* segment = VariantPointer<Segment>(item->data(Qt::UserRole));
	if (!segment) return;

	QString startText = segment->name();
	bool ok;
	QString name = QInputDialog::getText(this, "Rename Segment", "Enter new name:", QLineEdit::Normal, startText, &ok);
	if (!ok) return;

	segment->setName(name);
	item->setText(name);

	updateRunData();
}	

void QuappWindow::on_SegmentAddButton_clicked(bool checked)
{
	QString startText = "New Segment " + QString::number(quiz_.nSegments()+1);
	bool ok;
	QString name = QInputDialog::getText(this, "New Segment", "Enter segment name:", QLineEdit::Normal, startText, &ok);
	if (!ok) return;

	Segment* newSegment = quiz_.addSegment(name);

	updateSegments(newSegment);
	updateRunData();
	updateRunControls();
}

void QuappWindow::on_SegmentRemoveButton_clicked(bool checked)
{
	Segment* segment = currentSegment();
	if (!segment) return;

	Segment* newCurrentSegment = segment->next ? segment->next : segment->prev;
	quiz_.removeSegment(segment);

	updateSegments(newCurrentSegment);
	updateRunData();
}

void QuappWindow::on_SegmentMoveUpButton_clicked(bool checked)
{
	Segment* segment = currentSegment();
	if (!segment) return;

	if (segment->prev) quiz_.moveSegmentUp(segment);

	updateSegments(segment);
}

void QuappWindow::on_SegmentMoveDownButton_clicked(bool checked)
{
	Segment* segment = currentSegment();
	if (!segment) return;

	if (segment->next) quiz_.moveSegmentDown(segment);

	updateSegments(segment);
}

/*
 * Segment Type
 */

void QuappWindow::on_SegmentTypeTitleRadio_clicked(bool checked)
{
	Segment* segment = quiz_.segment(ui.SegmentsList->currentRow());
	if (refreshing_ || (!segment)) return;

	segment->setType(Segment::TitleSegment);

	updateSegments(segment, false);
}

void QuappWindow::on_SegmentTypeQuestionRadio_clicked(bool checked)
{
	Segment* segment = quiz_.segment(ui.SegmentsList->currentRow());
	if (refreshing_ || (!segment)) return;

	segment->setType(Segment::QuestionSegment);

	updateSegments(segment, false);
}

void QuappWindow::on_SegmentTypeAnswerRadio_clicked(bool checked)
{
	Segment* segment = quiz_.segment(ui.SegmentsList->currentRow());
	if (refreshing_ || (!segment)) return;

	segment->setType(Segment::AnswerSegment);

	updateSegments(segment, false);
}

void QuappWindow::on_SegmentTypeScoreRadio_clicked(bool checked)
{
	Segment* segment = quiz_.segment(ui.SegmentsList->currentRow());
	if (refreshing_ || (!segment)) return;

	segment->setType(Segment::ScoreSegment);

	updateSegments(segment, false);
}

/*
 * Options
 */

void QuappWindow::on_SegmentNonVisualCheck_clicked(bool checked)
{
	Segment* segment = quiz_.segment(ui.SegmentsList->currentRow());
	if (refreshing_ || (!segment)) return;

	segment->setNonVisual(checked);

	updateSegments(segment, false);

	updateRunData();
}

/*
 * Data
 */

void QuappWindow::on_SegmentQuestionSourceCombo_currentIndexChanged(int index)
{
	Segment* segment = quiz_.segment(ui.SegmentsList->currentRow());
	if (refreshing_ || (!segment)) return;

	if (index == quiz_.nQuestionSets()) segment->setQuestionSource(NULL);
	else segment->setQuestionSource(quiz_.questionSet(index));
}

/*
 * Segment data
 */

void QuappWindow::on_SegmentTitleTextEdit_textChanged(QString text)
{
	Segment* segment = quiz_.segment(ui.SegmentsList->currentRow());
	if (refreshing_ || (!segment)) return;

	segment->setTitleText(text);

	if (segment->type() == Segment::QuestionSegment) updateRunData();
}

void QuappWindow::on_SegmentSubTextEdit_textChanged(QString text)
{
	Segment* segment = quiz_.segment(ui.SegmentsList->currentRow());
	if (refreshing_ || (!segment)) return;

	segment->setSubText(text);
}

void QuappWindow::on_SegmentImageSelectButton_clicked(bool checked)
{
	Segment* segment = quiz_.segment(ui.SegmentsList->currentRow());
	if (refreshing_ || (!segment)) return;

	QString fileName = QFileDialog::getOpenFileName(this, "Select Image", imageDirectory_.absolutePath());
	if (fileName.isEmpty() || fileName.isNull()) return;
	imageDirectory_.setPath(fileName);

	segment->setImage(fileName);

	updateSegments();
}

/*
 * Segment Options
 */

void QuappWindow::on_SegmentBodyTextScaleSpin_valueChanged(double value)
{
	Segment* segment = quiz_.segment(ui.SegmentsList->currentRow());
	if (refreshing_ || (!segment)) return;

	segment->setBodyTextSizeModifier(value);
}

void QuappWindow::on_SegmentTextQuestionDisplayTextCheck_clicked(bool checked)
{
	Segment* segment = quiz_.segment(ui.SegmentsList->currentRow());
	if (refreshing_ || (!segment)) return;

	segment->setTextQuestionDisplayText(checked);
}

void QuappWindow::on_SegmentImageQuestionUseIndexCheck_clicked(bool checked)
{
	Segment* segment = quiz_.segment(ui.SegmentsList->currentRow());
	if (refreshing_ || (!segment)) return;

	segment->setImageQuestionDisplayIndex(checked);
}

/*
 * Update
 */

// Update segments list
void QuappWindow::updateSegments(Segment* newCurrentSegment, bool refreshList)
{
	refreshing_ = true;

	// Update segment list
	if (refreshList)
	{
		ui.SegmentsList->clear();
		if (newCurrentSegment == NULL) newCurrentSegment = quiz_.segments();
		for (Segment* segment = quiz_.segments(); segment != NULL; segment = segment->next)
		{
			QListWidgetItem* item = new QListWidgetItem(segment->name());
			item->setData(Qt::UserRole, VariantPointer<Segment>(segment));
			ui.SegmentsList->addItem(item);
			
			if (segment == newCurrentSegment) ui.SegmentsList->setCurrentRow(ui.SegmentsList->count()-1);
		}
	}

	// Set data
	if (newCurrentSegment)
	{
		// Type
		if (newCurrentSegment->type() == Segment::TitleSegment) ui.SegmentTypeTitleRadio->setChecked(true);
		else if (newCurrentSegment->type() == Segment::AnswerSegment) ui.SegmentTypeAnswerRadio->setChecked(true);
		else if (newCurrentSegment->type() == Segment::QuestionSegment) ui.SegmentTypeQuestionRadio->setChecked(true);
		else if (newCurrentSegment->type() == Segment::ScoreSegment) ui.SegmentTypeScoreRadio->setChecked(true);

		// Options
		ui.SegmentNonVisualCheck->setChecked(newCurrentSegment->nonVisual());

		// Data
		ui.SegmentTitleTextEdit->setText(newCurrentSegment->titleText());
		ui.SegmentSubTextEdit->setText(newCurrentSegment->subText());
		ui.SegmentImageFileNameEdit->setText(newCurrentSegment->imageFileName());
		if (newCurrentSegment->imageOK())
		{
			ui.SegmentImageFileNameEdit->setPalette(ui.QuestionTextQuestionEdit->palette());
			ui.SegmentImageLabel->setPixmap(QPixmap::fromImage(newCurrentSegment->image()).scaled(ui.SegmentImageLabel->size(), Qt::KeepAspectRatio));
		}
		else
		{
			QPalette redText;
			redText.setColor(QPalette::Text, Qt::red);
			ui.SegmentImageFileNameEdit->setPalette(redText);
			ui.SegmentImageLabel->setPixmap(QPixmap());
		}

		// Question source
		ui.SegmentQuestionSourceCombo->clear();
		for (QuestionSet* set = quiz_.questionSets(); set != NULL; set = set->next)
		{
			ui.SegmentQuestionSourceCombo->addItem(set->name(), VariantPointer<QuestionSet>(set));
			if (set == newCurrentSegment->questionSource()) ui.SegmentQuestionSourceCombo->setCurrentIndex(ui.SegmentQuestionSourceCombo->count()-1);
		}
		// -- Add on 'NULL' entry
		ui.SegmentQuestionSourceCombo->addItem("<None>");
		if (newCurrentSegment->questionSource() == NULL) ui.SegmentQuestionSourceCombo->setCurrentIndex(ui.SegmentQuestionSourceCombo->count()-1);

		// Options
		ui.SegmentBodyTextScaleSpin->setValue(newCurrentSegment->bodyTextSizeModifier());
		ui.SegmentImageQuestionUseIndexCheck->setChecked(newCurrentSegment->imageQuestionDisplayIndex());
		ui.SegmentTextQuestionDisplayTextCheck->setChecked(newCurrentSegment->textQuestionDisplayText());
	}

	// Enable / disable controls
	ui.SegmentRemoveButton->setEnabled(newCurrentSegment);
	ui.SegmentMoveUpButton->setEnabled(newCurrentSegment && newCurrentSegment->prev);
	ui.SegmentMoveDownButton->setEnabled(newCurrentSegment && newCurrentSegment->next);
	ui.SegmentDataGroup->setEnabled(newCurrentSegment);
	ui.SegmentQuestionSourceCombo->setEnabled(newCurrentSegment && (newCurrentSegment->type() <= Segment::QuestionSegment));

	refreshing_ = false;

	updateRunControls();
}
