#include "LabelEditorDialog.h"
#include "ui_LabelEditorDialog.h"
#include <LabelEditorDialog.h>
#include <QAbstractTableModel>

#include <ColorWheel.h>
#include <QMenu>

#include <QSortFilterProxyModel>
#include <QStandardItemModel>


LabelEditorDialog::LabelEditorDialog(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::LabelEditorDialog)
{
    ui->setupUi(this);

    ui->inColorWheel->setWheelWidth(15);

    // Add some actions to the action button
    QMenu *menu = new QMenu();
    ui->btnActions->setMenu(menu);
    //menu->addAction(this, "actionLoadLabels");
    //menu->addAction(this, "actionSaveLabels");
    menu->addSeparator();
    menu->addAction(ui->actionResetLabels);
    menu->addSeparator();


    QMenu *menu_vis = new QMenu("Visibility", this);
    menu->addMenu(menu_vis);
    menu_vis->addAction(ui->actionHide_all_labels);
    menu_vis->addAction(ui->actionHide_all_labels_in_3D_window);
    menu_vis->addSeparator();
    menu_vis->addAction(ui->actionShow_all_labels);
    menu_vis->addAction(ui->actionShow_all_labels_in_3D_window);

    QStandardItemModel *simodel = new QStandardItemModel(this);
    simodel->setColumnCount(2);

    // Set up a filter model for the label list view
    m_LabelListFilterModel = new QSortFilterProxyModel(this);
    m_LabelListFilterModel->setSourceModel(simodel);
    m_LabelListFilterModel->setFilterCaseSensitivity(Qt::CaseInsensitive);
    m_LabelListFilterModel->setFilterKeyColumn(-1);
    ui->lvLabels->setModel(m_LabelListFilterModel);
}

LabelEditorDialog::~LabelEditorDialog()
{
    delete ui;
}

#include <QSortFilterProxyModel>

void LabelEditorDialog::SetModel(LabelEditorModel *model)
{
  // Store the model
  m_Model = model;

  // Couple widgets to the model
}

void LabelEditorDialog::on_btnClose_clicked()
{
    this->close();
}

#include <QMessageBox>

void LabelEditorDialog::on_btnNew_clicked()
{
  // Create a new label in the first slot after the currently selected

}

void LabelEditorDialog::on_btnDuplicate_clicked()
{
  // Create a new label in the first slot after the currently selected

}

void LabelEditorDialog::on_btnDelete_clicked()
{
  // Check if the deletion is going to affect the segmentation

}

void LabelEditorDialog::on_inLabelId_editingFinished()
{
  // Check if the new value is available

}

void LabelEditorDialog::on_actionResetLabels_triggered()
{

}

void LabelEditorDialog::on_inLabelFilter_textChanged(const QString &arg1)
{
  m_LabelListFilterModel->setFilterFixedString(arg1);
}

void LabelEditorDialog::on_actionHide_all_labels_triggered()
{

}

void LabelEditorDialog::on_actionHide_all_labels_in_3D_window_triggered()
{

}

void LabelEditorDialog::on_actionShow_all_labels_triggered()
{

}

void LabelEditorDialog::on_actionShow_all_labels_in_3D_window_triggered()
{

}
