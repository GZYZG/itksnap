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


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->initLeft();
    this->initMiddle();
    this->initRight();


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
    QString fileName = QFileDialog::getOpenFileName(this, "Open the file");
    this->loadMainImage(fileName);
    QFile file(fileName);

    //currentFile = fileName;
    if (!file.open(QIODevice::ReadOnly | QFile::Text)) {
        QMessageBox::warning(this, "Warning", "Cannot open file: " + file.errorString());
        return;
    }
    setWindowTitle(fileName);
    /*QTextStream in(&file);
    QString text = in.readAll();
    qDebug() << text;
    //ui->textEdit->setText(text);
    file.close();
    */
    //QFileDialog dialog(this, tr("Open File"));
    //initializeImageFileDialog(dialog, QFileDialog::AcceptOpen);
    //while (dialog.exec() == QDialog::Accepted && !loadFile(dialog.selectedFiles().first())) {}
}



void MainWindow::on_actionOpen_Segmentation_triggered(){
    QString fileName = QFileDialog::getOpenFileName(this, "Open the file");
    this->loadDataset(fileName);
    QFile file(fileName);
    qDebug() << "You select " << fileName;
    //currentFile = fileName;
    if (!file.open(QIODevice::ReadOnly | QFile::Text)) {
        QMessageBox::warning(this, "Warning", "Cannot open file: " + file.errorString());
        return;
    }
    setWindowTitle(fileName);
    /*QTextStream in(&file);
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
    qDebug() << ui->leftLayout->children().size();

    QStringList colorNames = QColor::colorNames();

    for(int i=0; i < 10; i++){

        QColor color(colorNames[i]);
        OrganLabelEditor* widget = new OrganLabelEditor("label-"+QString::number(i), &color);
        //QLabel* widget = new QLabel("haha");
        organLabelLayout->addWidget(widget);
        qDebug() << i << widget << "-th label created!";
    }
    organLabelLayout->addItem(new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding));

    this->other = new QGroupBox();
    QVBoxLayout* otherLayout = new QVBoxLayout(this->other);
    otherLayout->addWidget(new QLabel("OTHER"));
    otherLayout->addWidget(new Window());
    ColorWheel* colorWheel = new ColorWheel();
    otherLayout->addWidget(colorWheel);

    connect(colorWheel, SIGNAL(colorChange(const QColor&)), this, SLOT(colorwheel(const QColor&)));

    toolBox->addItem(this->organLable, "器官标签");
    toolBox->addItem(this->other, "其他");


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
    m_sliceRenderer1->SetBackground(.8, .8, .8);
    this->sliceView1->getVTKView()->renderWindow()->AddRenderer(m_sliceRenderer1);
}


void MainWindow::removeDataset(){
    // 移除view3D中的对象
    vtkActor* actor = this->m_renderer->GetActors()->GetLastActor();
    if(actor != nullptr){
        this->m_renderer->RemoveActor(actor);
    }
    this->view3d->renderWindow()->Render();
}


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

void MainWindow::loadDataset(QString filePath){
    this->removeDataset();

    // Create reader
    //vtkSmartPointer<vtkDataSetReader> reader = vtkSmartPointer<vtkDataSetReader>::New();
    //reader->SetFileName(filePath.toStdString().c_str());

    vtkNew<vtkNIFTIImageReader> reader;// = vtkSmartPointer<vtkNIFTIImageReader>::New();
    reader->SetFileName(filePath.toStdString().c_str());
    // Read the file
    reader->Update();

    // Add data set to 3D view
    vtkAlgorithmOutput* dataSet = reader->GetOutputPort();
    if (dataSet != nullptr) {
        vtkImageData* data = reader->GetOutput();//->GetScalarRange();
        double* range = data->GetScalarRange();
        qDebug() << "Scalar range is "<< range[0] << " " << range[1];
        this->addDataset(reader);
    }
}


void MainWindow::loadMainImage(const QString &filePath){
    /**/
    qDebug() << "You select :" << filePath;
    using PixelType = int;  //定义像素类型
    constexpr unsigned int Dimension = 3;  // 定义图像维度

    // nii Reader
    NiftiImageReader<PixelType, Dimension>* reader = new NiftiImageReader<PixelType, Dimension>(filePath.toStdString());

    // 读取切片
    NiftiImageReader<PixelType, Dimension>::ImageType::Pointer slice = reader->extractSlice(2, 0, 1);
    //std::cout << "Slice is :\n" << slice << endl;
    std::cout << "Slice's size is: " << reader->getImageSize(slice)<< std::endl;

    // 直接读取一个二维切片还未完成
    /*NiftiImageReader<PixelType, Dimension>::ImageType::Pointer oneSlice = reader->extractOneSlice(2, 0);
    std::cout << "One Slice is :\n" << oneSlice << endl;
    std::cout << "One Slice's size is: " << reader->getImageSize(oneSlice) << endl;
    */

    std::cout << reader->itkToVtk(slice) << endl;

    reader->show3dImage(reader->image, this->sliceView1->getVTKView()->renderWindow());

    //std::cout << slices->GetLargestPossibleRegion().Slice(0) << endl;
    //dynamic_cast<QGraphicsView*>(this->sliceView1)
}


void MainWindow::colorwheel(const QColor& color){
    QRgb rgb = (&color)->rgb();
    qDebug() << "hahah" << qRed(rgb) << ", " << qGreen(rgb) << ", " << qBlue(rgb);
}




