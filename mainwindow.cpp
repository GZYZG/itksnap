#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
#include <QFileDialog>
#include <QDebug>
#include <QGroupBox>
#include <QToolBox>
#include <QStandardPaths>
#include <QImageReader>
#include <QImageWriter>
#include <QGraphicsView>
#include <QThread>

// vtk
#include <vtkCamera.h>
#include <vtkDataSetMapper.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkProperty.h>
#include <vtkGenericOpenGLRenderWindow.h>
#include <vtkDataSetReader.h>
#include <vtkNIFTIImageReader.h>
#include <vtkImageData.h>
#include <vtkImageViewer2.h>

//ITK
#include <itkImageFileReader.h>
#include <itkNiftiImageIO.h>
#include <itkImage.h>
#include <itkJPEGImageIO.h>


// 我的头文件
#include "organlabeleditor.h"
#include "ColorWheel.h"
#include "utils.h"
#include "ViewPanel3D.h"
#include "SliceViewPanel.h"
#include "niftiimagereader.h"
#include "niiobject.h"


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->initLeft();
    this->initMiddle();
    this->initRight();

    this->selectedOrgan = nullptr;
    this->segmentation = nullptr;
    vtkNew<vtkGenericOpenGLRenderWindow> window;
    this->view3d->setRenderWindow(window.Get());
    labelEditor = new LabelEditorDialog(this);

    // Camera
    vtkSmartPointer<vtkCamera> camera = vtkSmartPointer<vtkCamera>::New();
    camera->SetViewUp(0, 1, 0);
    camera->SetPosition(0, 0, 10);
    camera->SetFocalPoint(0, 0, 0);

    // Renderer
    m_renderer = vtkSmartPointer<vtkRenderer>::New();
    m_renderer->SetActiveCamera(camera);
    m_renderer->SetBackground(.8, .8, .8);
    this->view3d->renderWindow()->AddRenderer(m_renderer);
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_actionOpen_MainImage_triggered(){
    QFileDialog* dialog = new QFileDialog(this);
    dialog->setDirectory("../itksnap/");
    QString fileName = dialog->getOpenFileName(this, "选择文件");
    qDebug() << "You select " << fileName;
    if(!fileName.length()){
        return;
    }
    this->loadMainImage(fileName);
}



void MainWindow::on_actionOpen_Segmentation_triggered(){
    QFileDialog* dialog = new QFileDialog(this);
    dialog->setDirectory("../itksnap/");
    QString fileName = dialog->getOpenFileName(this, "选择分割结果文件");
    qDebug() << "You select " << fileName;
    if(!fileName.length()){
        return;
    }
    this->loadSegmentation(fileName);
    /*
    QFile file(fileName);

    //currentFile = fileName;
    if (!file.open(QIODevice::ReadOnly | QFile::Text)) {
        QMessageBox::warning(this, "Warning", "Cannot open file: " + file.errorString());
        return;
    }
    setWindowTitle(fileName);
    QTextStream in(&file);
    QString text = in.readAll();
    qDebug() << text;
    //ui->textEdit->setText(text);
    file.close();
    */
    //QFileDialog dialog(this, tr("Open File"));
    //initializeImageFileDialog(dialog, QFileDialog::AcceptOpen);
    //while (dialog.exec() == QDialog::Accepted && !loadFile(dialog.selectedFiles().first())) {}
}

void MainWindow::initLeft(){
    // 初始化左侧，加入各个标签，对应的颜色和透明度
    this->toolBox = new QToolBox(this->centralWidget());
    this->toolBox->setObjectName("toolBox");
    ui->leftLayout->insertWidget(1, this->toolBox);

    this->organLable = new OrganLabelContainer(this->toolBox);
    this->organLable->setObjectName("organLabelContainer");
    QVBoxLayout* organLabelLayout = new QVBoxLayout(this->organLable);
    organLabelLayout->setMargin(0);
    organLabelLayout->setSpacing(5);

    /*
    this->other = new QGroupBox();
    QVBoxLayout* otherLayout = new QVBoxLayout(this->other);
    otherLayout->addWidget(new QLabel("OTHER"));
    */
    //this->organLable->setMinimumHeight(800);
    toolBox->setMinimumHeight(600);
    toolBox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);  // setMinimumHeight(800);
    toolBox->setMaximumWidth(400);
    toolBox->addItem(this->organLable, "器官标签");
}

void MainWindow::initMiddle(){
    ViewPanel3D* panel = new ViewPanel3D();
    this->ui->middleLayout->addWidget(panel);
    this->view3d = panel->getView3d();
    connect(panel, SIGNAL(btnExpandClicked(bool)), this, SLOT(expand3DView(bool)));
    tridPanel = panel;
}

void MainWindow::initRight(){
    SliceViewPanel* slice1 =  creataSliceViewPanel("sliceViewPanelA", ":/new/Resources/dl_axial.png");  // Transverse section 横断面
    SliceViewPanel* slice2 = creataSliceViewPanel("sliveViewPanelS", ":/new/Resources/dl_sagittal.png");  // Sagittal section 矢状面，把人体分成左右两面的解剖面
    SliceViewPanel* slice3 = creataSliceViewPanel("sliceViewPanelC", ":/new/Resources/dl_coronal.png");  // Coronal section 冠状面，把人体分成前后两面的解剖面

    this->ui->rightLayout->addWidget(slice1);
    this->ui->rightLayout->addWidget(slice2);
    this->ui->rightLayout->addWidget(slice3);

    this->sliceViewPanelXY = slice1;
    this->sliceViewPanelYZ = slice2;
    this->sliceViewPanelXZ = slice3;

    m_sliceRenderer1 = slice1->getRenderer();
    m_sliceRenderer2 = slice2->getRenderer();
    m_sliceRenderer3 = slice3->getRenderer();

    // 连接按钮的信号与槽
    connect(slice1, SIGNAL(btnExpandClicked(bool, SliceViewPanel*)), this, SLOT(expandSliceView(bool, SliceViewPanel*)));
    connect(slice2, SIGNAL(btnExpandClicked(bool, SliceViewPanel*)), this, SLOT(expandSliceView(bool, SliceViewPanel*)));
    connect(slice3, SIGNAL(btnExpandClicked(bool, SliceViewPanel*)), this, SLOT(expandSliceView(bool, SliceViewPanel*)));
}


void MainWindow::removeDataset(){
    // 移除view3D中的对象
    vtkActor* actor;
    vtkActorCollection* actors = this->m_renderer->GetActors();
    if(actors != nullptr){
        while((actor = actors->GetLastActor()) != nullptr){
            this->m_renderer->RemoveActor(actor);
        }
    }
    this->view3d->renderWindow()->Render();

}

void MainWindow::loadSegmentation(QString filePath){
    std::cout << "load thread:" << QThread::currentThreadId() << endl;
    this->removeDataset();

    NIIObject *mask = new NIIObject(filePath.toStdString());  // 必须用new，否则当前函数推出后，进程对象会被销毁，子线程直接挂了！！！
    if(this->segmentation)
        delete this->segmentation;

    this->segmentation = mask;
    workerThread = new QThread;
    mask->moveToThread(workerThread);
    //connect(&workerThread, &QThread::finished, this, nullptr);
    connect(this, SIGNAL(renderSegmentation(vtkRenderer*)), mask, SLOT(surfaceRendering(vtkRenderer*)));
    connect(mask, SIGNAL(renderDone()), this, SLOT(segmentationRenderDone()));
    workerThread->start();
    emit renderSegmentation(this->m_renderer);
    //mask->surfaceRendering(this->m_renderer);
    //workerThread.wait();
}


void MainWindow::loadMainImage(const QString &filePath){
    /**/
    using PixelType = int;  //定义像素类型
    constexpr unsigned int Dimension = 3;  // 定义图像维度

    // nii Reader
    NiftiImageReader<PixelType, Dimension>* reader = new NiftiImageReader<PixelType, Dimension>(filePath.toStdString());

    // 读取切片
    NiftiImageReader<PixelType, Dimension>::ImageType::Pointer slice = reader->extractSlice(2, 0, 1);
    //std::cout << "Slice is :\n" << slice << endl;
    //std::cout << "Slice's size is: " << reader->getImageSize(slice)<< std::endl;

    // 直接读取一个二维切片还未完成
    /*NiftiImageReader<PixelType, Dimension>::ImageType::Pointer oneSlice = reader->extractOneSlice(2, 0);
    std::cout << "One Slice is :\n" << oneSlice << endl;
    std::cout << "One Slice's size is: " << reader->getImageSize(oneSlice) << endl;
    */

    //std::cout << reader->itkToVtk(slice) << endl;
    vtkImageViewer2* viewer;
    viewer = reader->show3dImage_XY(reader->image, this->sliceViewPanelXY->getVTKView()->renderWindow());
    this->sliceViewPanelXY->setSliceViewer(viewer);

    viewer = reader->show3dImage_YZ(reader->image, this->sliceViewPanelYZ->getVTKView()->renderWindow());
    this->sliceViewPanelYZ->setSliceViewer(viewer);

    viewerXZ = reader->show3dImage_XZ(reader->image, this->sliceViewPanelXZ->getVTKView()->renderWindow());
    this->sliceViewPanelXZ->setSliceViewer(viewer);
}


void MainWindow::expand3DView(bool expanded){
    int stretch = expanded ? 4 : 0;

    this->sliceViewPanelXY->setVisible(expanded);
    this->sliceViewPanelYZ->setVisible(expanded);
    this->sliceViewPanelXZ->setVisible(expanded);
    this->ui->Layout->setStretch(2, stretch);
}


void MainWindow::expandSliceView(bool expanded, SliceViewPanel* slicePanel){
    QString name = slicePanel->objectName();
    QList<SliceViewPanel*> list;
    list << this->sliceViewPanelXY << this->sliceViewPanelYZ << this->sliceViewPanelXZ;

    int stretch = expanded ? 8 : 0;
    this->ui->Layout->setStretch(1, stretch);
    this->tridPanel->setVisible(expanded);

    list.removeOne(slicePanel);
    this->ui->Layout->setStretch(2, 16 - 2 - stretch);
    for(int i=0; i < list.length(); i++){
        list.at(i)->setVisible(expanded);
    }
}


SliceViewPanel* MainWindow::creataSliceViewPanel(std::string objectName, std::string originalIconPath){
    SliceViewPanel* panel = new SliceViewPanel();
    panel->setExpandBtnOriginIcon(originalIconPath);
    panel->setObjectName(QString::fromStdString(objectName));
    vtkRenderer* renderer = vtkRenderer::New();
    vtkSmartPointer<vtkRenderWindowInteractor> rwi =
        vtkSmartPointer<vtkRenderWindowInteractor>::New();
    panel->getVTKView()->renderWindow()->SetInteractor(rwi);
    panel->setRenderer(renderer);
    return panel;
}


void MainWindow::segmentationRenderDone(){
    if(workerThread->isRunning()){
        workerThread->exit();
    }
    qDebug() << "MainWindow know renderer done, now in: " << QThread::currentThreadId();
    this->m_renderer->ResetCamera();
    this->m_renderer->Render();

    NIIObject *mask = this->segmentation;

    QVBoxLayout* organLabelLayout = dynamic_cast<QVBoxLayout*>(this->organLable->layout());
    clearLayout(organLabelLayout);
    for(int i = 0; i < mask->m_labels->length(); i++){
        organLabelLayout->addWidget(mask->m_labels->at(i)->m_rgba);
        mask->m_labels->at(i)->m_rgba->setLabelEditorDialog(labelEditor);
    }
    organLabelLayout->addItem(new QSpacerItem(20, 20, QSizePolicy::Minimum, QSizePolicy::Expanding));

    // 将LabelEditorDialog 的信后 和 NIIObject 的槽 连接起来
    // 当标签的颜色或透明度改变时修改可视化对象的颜色和透明度
    //LabelEditorDialog *labelEditor = mask->m_labels->at(0)->m_rgba->labelEditor;
    connect(labelEditor, SIGNAL(opacityChanged(int, int)), mask, SLOT(changeSurfaceOpacity(int, int)));
    connect(labelEditor, SIGNAL(colorChanged(int, int, int, int, int)), mask, SLOT(changeSurfaceColor(int, int, int, int, int)));

}




