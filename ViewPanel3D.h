#ifndef VIEWPANEL3D_H
#define VIEWPANEL3D_H


#include <QThread>
#include <QMutex>
#include <QWaitCondition>
#include <QDebug>
#include <QTimer>
#include <QFutureWatcher>
#include <QWidget>
#include <QVTKOpenGLWidget.h>
#include <QToolButton>


namespace Ui {
  class ViewPanel3D;
}


class QMenu;


class ViewPanel3D : public QWidget
{
  Q_OBJECT

public:
  explicit ViewPanel3D(QWidget *parent = 0);
  ~ViewPanel3D();

  QVTKOpenGLStereoWidget* getView3d();

  // Register with the global model
  //void Initialize(GlobalUIModel *model);

  //GenericView3D *Get3DView();

signals:

  void renderProgress(int progress);
  void btnExpandClicked(bool);

private slots:
  //virtual void onModelUpdate(const EventBucket &bucket);

  void on_btnUpdateMesh_clicked();

  void on_btnScreenshot_clicked();

  void on_btnAccept_clicked();

  void on_btnCancel_clicked();

  void on_btnExpand_clicked();

  void onTimer();


  void on_actionReset_Viewpoint_triggered();

  void on_actionSave_Viewpoint_triggered();

  void on_actionRestore_Viewpoint_triggered();

  void on_actionContinuous_Update_triggered();

  void on_btnMenu_pressed();

  void on_btnFlip_clicked();

  void on_actionClear_Rendering_triggered();

private:
  Ui::ViewPanel3D *ui;

  //GlobalUIModel *m_GlobalUI;

  //Generic3DModel *m_Model;

  QMenu *m_DropMenu;

  QTimer *m_RenderTimer;

  // A future used to track background rendering
  QFuture<void> m_RenderFuture;

  // A mutex on the progress value, which is accesed by multiple threads
  mutable QMutex m_RenderProgressMutex;

  // Progress of the rendering operation
  double m_RenderProgressValue;

  // Elapsed time since begin of render operation
  int m_RenderElapsedTicks;

  // 是否已经扩展
  bool expanded;

  //typedef itk::MemberCommand<ViewPanel3D> CommandType;
  //SmartPtr<CommandType> m_RenderProgressCommand;

  void UpdateExpandViewButton();

  void UpdateMeshesInBackground();

  void UpdateActionButtons();

  //void ProgressCallback(itk::Object *source, const itk::EventObject &event);
};

#endif // VIEWPANEL3D_H
