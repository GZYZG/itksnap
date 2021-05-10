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
#include "colorlisteditor.h"
#include "window.h"
#include "ColorWheel.h"
#include "utils.h"
#include "viewpanel3d.h"
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
    vtkNew<vtkGenericOpenGLRenderWindow> window;
    this->view3d->setRenderWindow(window.Get());

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
    this->toolBox = new QToolBox();
    ui->leftLayout->insertWidget(1, this->toolBox);

    this->organLable = new OrganLabelContainer();
    QVBoxLayout* organLabelLayout = new QVBoxLayout(this->organLable);
    organLabelLayout->setMargin(0);
    organLabelLayout->setSpacing(0);

    /* // 预先设置的标签
    QStringList colorNames = QColor::colorNames();
    for(int i=0; i < 10; i++){

        QColor color(colorNames[i]);
        OrganLabelEditor* widget = new OrganLabelEditor("label-"+QString::number(i), &color);
        //QLabel* widget = new QLabel("haha");
        organLabelLayout->addWidget(widget);
        qDebug() << i << widget << "-th label created!";
    }
    organLabelLayout->addItem(new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding));
    */

    /*
    this->other = new QGroupBox();
    QVBoxLayout* otherLayout = new QVBoxLayout(this->other);
    otherLayout->addWidget(new QLabel("OTHER"));
    otherLayout->addWidget(new Window());
    ColorWheel* colorWheel = new ColorWheel();
    otherLayout->addWidget(colorWheel);

    connect(colorWheel, SIGNAL(colorChange(const QColor&)), this, SLOT(colorwheel(const QColor&)));
    toolBox->addItem(this->other, "其他");
    */
    //this->organLable->setMinimumHeight(800);
    toolBox->setMinimumHeight(600);
    toolBox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);  // setMinimumHeight(800);
    toolBox->addItem(this->organLable, "器官标签");



}

void MainWindow::initMiddle(){
    ViewPanel3D* panel = new ViewPanel3D();
    this->ui->middleLayout->insertWidget(1, panel);
    this->view3d = panel->getView3d();
}

void MainWindow::initRight(){
    SliceViewPanel* slice1 = new SliceViewPanel();
    //QGraphicsView* slice1 = new QGraphicsView(this->centralWidget());
    slice1->setObjectName("sliceViewT");  // Transverse section 横断面
    SliceViewPanel* slice2 = new SliceViewPanel();
    slice2->setObjectName("sliveViewS");  // Sagittal section 矢状面，把人体分成左右两面的解剖面
    SliceViewPanel* slice3 = new SliceViewPanel();
    slice3->setObjectName("sliceViewC");  // Coronal section 冠状面，把人体分成前后两面的解剖面

    this->ui->rightLayout->insertWidget(0, slice1);
    this->ui->rightLayout->insertWidget(1, slice2);
    this->ui->rightLayout->insertWidget(2, slice3);

    this->sliceView1 = slice1;
    this->sliceView2 = slice2;
    this->sliceView3 = slice3;

    m_sliceRenderer1 = vtkSmartPointer<vtkRenderer>::New();
    this->sliceView1->getVTKView()->renderWindow()->AddRenderer(m_sliceRenderer1);

    m_sliceRenderer2 = vtkSmartPointer<vtkRenderer>::New();
    this->sliceView2->getVTKView()->renderWindow()->AddRenderer(m_sliceRenderer2);

    m_sliceRenderer3 = vtkSmartPointer<vtkRenderer>::New();
    this->sliceView3->getVTKView()->renderWindow()->AddRenderer(m_sliceRenderer3);
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

// 废弃
void MainWindow::addDataset(vtkImageAlgorithm* reader){
    vtkMarchingCubes* surfaces = createSurfaces(reader->GetOutput());
    vtkSmoothPolyDataFilter* smoother = createSmoother(surfaces->GetOutputPort(), 50);
    vtkPolyDataNormals* normals = createNormals(smoother->GetOutputPort());
    vtkPolyDataMapper* mapper = createMapper(normals->GetOutputPort());
    double* range = reader->GetOutput()->GetScalarRange();
    mapper->SetScalarRange(range[0], range[1]);
    vtkLookupTable* lut = createColorTable(int(range[0]), int(range[1]));
    mapper->SetLookupTable(lut);
    vtkActor* actor = createActor(mapper);
    this->m_renderer->AddActor(actor);
    this->m_renderer->ResetCamera(actor->GetBounds());

    this->m_renderer->Render();
}

void MainWindow::loadSegmentation(QString filePath){
    this->removeDataset();

    NIIObject *mask = new NIIObject(filePath.toStdString());
    mask->surfaceRendering(this->m_renderer);
    this->m_renderer->ResetCamera();
    this->m_renderer->Render();

    QVBoxLayout* organLabelLayout = dynamic_cast<QVBoxLayout*>(this->organLable->layout());
    clearLayout(organLabelLayout);
    for(int i = 0; i < mask->m_labels->length(); i++){
        organLabelLayout->addWidget(mask->m_labels->at(i)->m_rgba);
    }
    organLabelLayout->addItem(new QSpacerItem(20, 20, QSizePolicy::Minimum, QSizePolicy::Expanding));

    // 将LabelEditorDialog 的信后 和 NIIObject 的槽 连接起来
    // 当标签的颜色或透明度改变时修改可视化对象的颜色和透明度
    LabelEditorDialog *labelEditor = mask->m_labels->at(0)->m_rgba->labelEditor;
    connect(labelEditor, SIGNAL(opacityChanged(int, int)), mask, SLOT(changeSurfaceOpacity(int, int)));
    connect(labelEditor, SIGNAL(colorChanged(int, int, int, int, int)), mask, SLOT(changeSurfaceColor(int, int, int, int, int)));

    /* Add data set to 3D view
    vtkNew<vtkNIFTIImageReader> reader;// = vtkSmartPointer<vtkNIFTIImageReader>::New();
    reader->SetFileName(filePath.toStdString().c_str());
    // Read the file
    reader->Update();
    vtkAlgorithmOutput* dataSet = reader->GetOutputPort();
    if (dataSet != nullptr) {
        vtkImageData* data = reader->GetOutput();//->GetScalarRange();
        double* range = data->GetScalarRange();
        qDebug() << "Scalar range is "<< range[0] << " " << range[1];
        this->addDataset(reader);
    }*/
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
    std::cout << "XY" << endl;
    reader->show3dImage_XY(reader->image, this->sliceView1->getVTKView()->renderWindow());
    std::cout << "YZ" << endl;
    reader->show3dImage_YZ(reader->image, this->sliceView2->getVTKView()->renderWindow());
    std::cout << "XZ" << endl;
    reader->show3dImage_XZ(reader->image, this->sliceView3->getVTKView()->renderWindow());

    //std::cout << slices->GetLargestPossibleRegion().Slice(0) << endl;
    //dynamic_cast<QGraphicsView*>(this->sliceView1)
}


void MainWindow::colorwheel(const QColor& color){
    QRgb rgb = (&color)->rgb();
    qDebug() << "hahah" << qRed(rgb) << ", " << qGreen(rgb) << ", " << qBlue(rgb);
}




