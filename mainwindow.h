#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGroupBox>
#include <QToolBox>
#include <QListWidget>

// vtk
#include <vtkSmartPointer.h>
#include <vtkRenderer.h>
#include <QVTKOpenGLNativeWidget.h>
#include <vtkDataSet.h>
#include <vtkAlgorithm.h>
#include <vtkImageAlgorithm.h>
#include <QVTKOpenGLWidget.h>
#include <vtkImageViewer2.h>
#include <QVTKOpenGLStereoWidget.h>

#include "SliceViewPanel.h"
#include "organlabeleditor.h"
#include "ViewPanel3D.h"
#include "niiobject.h"
#include "niftiimagereader.h"

// 定义独立于实现的类名
typedef QGroupBox OrganLabelContainer ;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    QToolBox* toolBox;  // 左侧的toolbox，用于放置可折叠的面板
    OrganLabelContainer* organLable;  // 器官可视化设置面板
    QGroupBox* other;
    ViewPanel3D* tridPanel;
    QVTKOpenGLStereoWidget* view3d;
    SliceViewPanel *sliceViewPanelXY, *sliceViewPanelYZ, *sliceViewPanelXZ;
    vtkImageViewer2 *viewerXY, *viewerYZ, *viewerXZ;
    OrganLabelEditor* selectedOrgan;
    NIIObject *segmentation;
    LabelEditorDialog* labelEditor;

    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void initLeft();
    void initRight();
    void initMiddle();
    void loadSegmentation(QString filePath);
    void removeDataset();
    void addDataset(vtkImageAlgorithm* reader);
    void loadMainImage(const QString &filePath);


private slots:
    //void on_pushButton_clicked();
    void on_actionOpen_MainImage_triggered();
    void on_actionOpen_Segmentation_triggered();
    void on_actionQuit_triggered();

    void expand3DView(bool);
    void expandSliceView(bool, SliceViewPanel*);

    void segmentationRenderDone();

signals:
    void renderSegmentation(vtkRenderer* renderer);

private:
    Ui::MainWindow *ui;
    vtkSmartPointer<vtkRenderer> m_renderer;
    vtkSmartPointer<vtkRenderer> m_sliceRenderer1;
    vtkSmartPointer<vtkRenderer> m_sliceRenderer2;
    vtkSmartPointer<vtkRenderer> m_sliceRenderer3;
    SliceViewPanel* creataSliceViewPanel(std::string objectName, std::string originalIconPath);
    QThread* workerThread;
};

#endif // MAINWINDOW_H
