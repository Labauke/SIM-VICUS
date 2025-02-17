/*	SIM-VICUS - Building and District Energy Simulation Tool.

	Copyright (c) 2020-today, Institut für Bauklimatik, TU Dresden, Germany

	Primary authors:
	  Andreas Nicolai  <andreas.nicolai -[at]- tu-dresden.de>
	  Dirk Weiss  <dirk.weiss -[at]- tu-dresden.de>
	  Stephan Hirth  <stephan.hirth -[at]- tu-dresden.de>
	  Hauke Hirsch  <hauke.hirsch -[at]- tu-dresden.de>

	  ... all the others from the SIM-VICUS team ... :-)

	This program is part of SIM-VICUS (https://github.com/ghorwin/SIM-VICUS)

	This program is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.
*/

#ifndef SVDBSubSurfaceComponentEditWidgetH
#define SVDBSubSurfaceComponentEditWidgetH

#include "SVAbstractDatabaseEditWidget.h"

namespace Ui {
	class SVDBSubSurfaceComponentEditWidget;
}

class SVDBSubSurfaceComponentTableModel;
class SVDatabase;
class QwtPlotCurve;

namespace VICUS {
	class SubSurfaceComponent;
}


/*! Edit widget for subsurface components (windows etc.).

	A call to updateInput() initializes the widget and fill the GUI controls with data.
	As long as the widget is visible the pointer to the data must be valid. Keep this
	in mind if you change the container that the data object belongs to! If the pointer
	is no longer valid or you want to resize the container (through adding new items)
	call updateInput() with an invalid index and/or nullptr pointer to the model.
*/
class SVDBSubSurfaceComponentEditWidget : public SVAbstractDatabaseEditWidget {
	Q_OBJECT

public:

	explicit SVDBSubSurfaceComponentEditWidget(QWidget *parent = nullptr);
	~SVDBSubSurfaceComponentEditWidget() override;

	/*! Needs to be called once, before the widget is being used. */
	void setup(SVDatabase * db, SVAbstractDatabaseTableModel * dbModel) override;

	/*! Update widget with this. */
	void updateInput(int id) override;


private slots:
	void on_lineEditName_editingFinished();
	void on_comboBoxSubSurfaceType_currentIndexChanged(int);

	void on_toolButtonSelectWindow_clicked();
	void on_toolButtonSelectBoundaryConditionSideAName_clicked();
	void on_toolButtonSelectBoundaryConditionSideBName_clicked();
	void on_toolButtonRemoveBoundaryConditionSideA_clicked();
	void on_toolButtonRemoveBoundaryConditionSideB_clicked();

	void on_pushButtonColor_clicked();

	void on_lineEditReductionFactor_editingFinishedSuccessfully();

private:
	/*! Set up the modified variable of the model to true. */
	void modelModify();

	Ui::SVDBSubSurfaceComponentEditWidget	*m_ui;

	/*! Cached pointer to database object. */
	SVDatabase								*m_db;

	/*! Pointer to the database model, to modify items when data has changed in the widget. */
	SVDBSubSurfaceComponentTableModel		*m_dbModel;

	/*! Pointer to currently edited subsurface-component.
		The pointer is updated whenever updateInput() is called.
		A nullptr pointer means that there is no subsurface to edit.
	*/
	VICUS::SubSurfaceComponent				*m_current;

	/*! Diagram curve for SHGC-plot. */
	QwtPlotCurve							*m_shgcCurve;
};

#endif // SVDBSubSurfaceComponentEditWidgetH
