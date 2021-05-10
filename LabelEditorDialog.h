#ifndef LABELEDITORDIALOG_H
#define LABELEDITORDIALOG_H

#include <QDialog>

class LabelEditorModel;
class QStandardItemModel;
class QSortFilterProxyModel;

namespace Ui {
    class LabelEditorDialog;
}

class LabelEditorDialog : public QDialog
{
  Q_OBJECT

public:
  explicit LabelEditorDialog(QWidget *parent = nullptr);
  ~LabelEditorDialog();

  void SetModel(LabelEditorModel *model);
  void setColor(QColor* color);
  void setOpacity(int opacity);

  int selectLabelIndex;

private slots:
  void on_btnClose_clicked();

  void on_btnNew_clicked();

  void on_btnDuplicate_clicked();

  void on_btnDelete_clicked();

  void on_inLabelId_editingFinished();

  void on_actionResetLabels_triggered();

  void on_inLabelFilter_textChanged(const QString &arg1);

  void on_actionHide_all_labels_triggered();

  void on_actionHide_all_labels_in_3D_window_triggered();

  void on_actionShow_all_labels_triggered();

  void on_actionShow_all_labels_in_3D_window_triggered();

  void inColorWheelChanged(const QColor &);

  void on_inLabelOpacitySlider_valueChanged(int value);

  void on_inLabelOpacitySpinner_valueChanged(int value);


signals:
  void opacityChanged(int index, int opacity);
  void colorChanged(int index, int r, int g, int b, int a);

private:
  LabelEditorModel *m_Model;

  // The Qt model tied to the QListView
  QSortFilterProxyModel *m_LabelListFilterModel;

   Ui::LabelEditorDialog *ui;
};

#endif // LABELEDITORDIALOG_H
