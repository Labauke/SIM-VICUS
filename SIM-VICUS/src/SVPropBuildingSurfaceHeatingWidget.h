#ifndef SVPropBuildingSurfaceHeatingWidgetH
#define SVPropBuildingSurfaceHeatingWidgetH

#include <QWidget>

#include <set>

class QTableWidgetItem;

namespace Ui {
	class SVPropBuildingSurfaceHeatingWidget;
}

namespace VICUS {
	class ComponentInstance;
	class SupplySystem;
}

/*! A property widget to assign and edit surface heatings. */
class SVPropBuildingSurfaceHeatingWidget : public QWidget {
	Q_OBJECT

public:
	explicit SVPropBuildingSurfaceHeatingWidget(QWidget *parent = nullptr);
	~SVPropBuildingSurfaceHeatingWidget();

	/*! Updates user interface. Use the onlySelectionModified flag if only selection has been changed to avoid costly updating the table widget. */
	void updateUi(bool onlySelectionModified);

private slots:
	void on_comboBoxSurfaceHeatingComponentFilter_currentIndexChanged(int index);
	void on_tableWidgetSurfaceHeating_itemChanged(QTableWidgetItem *item);
	void on_pushButtonRemoveSurfaceHeating_clicked();
	void on_pushButtonAssignSurfaceHeating_clicked();
	void on_tableWidgetSurfaceHeating_currentCellChanged(int currentRow, int currentColumn, int previousRow, int previousColumn);
	void on_tableWidgetSurfaceHeating_itemSelectionChanged();
	void on_pushButtonRemoveSelectedSurfaceHeating_clicked();
	void on_pushButtonAssignSurfaceHeatingControlZone_clicked();

	void on_pushButtonSwitchControlZone_clicked();

private:
	Ui::SVPropBuildingSurfaceHeatingWidget *m_ui;

	std::set<const VICUS::ComponentInstance*>				m_selectedComponentInstances;
};

#endif // SVPropBuildingSurfaceHeatingWidgetH
