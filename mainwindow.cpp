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

// vtk
#include <vtkCamera.h>
#include <vtkDataSetMapper.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkProperty.h>
#include <vtkGenericOpenGLRenderWindow.h>
#include <vtkDataSetReader.h>
#include <vtkNIFTIImageReader.h>
#include <vtkImageData.h>


// 我的头文件
#include "organlabeleditor.h"
#include "colorlisteditor.h"
#include "window.h"
#include "ColorWheel.h"
#include "utils.h"


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->initLeft();
    //qDebug() << ui->view3d->objectName();

    vtkNew<vtkGenericOpenGLRenderWindow> window;
    ui->view3d->setRenderWindow(window.Get());

    // Camera
    vtkSmartPointer<vtkCamera> camera = vtkSmartPointer<vtkCamera>::New();
    camera->SetViewUp(0, 1, 0);
    camera->SetPosition(0, 0, 10);
    camera->SetFocalPoint(0, 0, 0);

    // Renderer
    m_renderer = vtkSmartPointer<vtkRenderer>::New();
    m_renderer->SetActiveCamera(camera);
    m_renderer->SetBackground(.8, .8, .8);
    ui->view3d->renderWindow()->AddRenderer(m_renderer);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_clicked()
{
    //QMessageBox::about(this, tr("About MDI"),
    //             tr("The <b>Notepad</b> example demonstrates how to code a basic "
    //               "text editor using QtWidgets"));
    //QWidget* content = ui->toolBox->widget(0);
    //int n = ui->verticalLayout->children().size();
    //qDebug << n;
    //ui->verticalLayout->insertWidget(0, new QLabel("haha-"+QString(n) ));  // , tr("haha"));

}



void MainWindow::on_actionOpen_triggered(){
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
    this->toolBox = new QToolBox();
    ui->leftLayout->insertWidget(1, this->toolBox);

    this->organLable = new OrganLabelContainer();
    QVBoxLayout* organLabelLayout = new QVBoxLayout(this->organLable);
    organLabelLayout->setMargin(5);
    qDebug() << ui->leftLayout->children().size();

    QStringList colorNames = QColor::colorNames();

    for(int i=0; i < 10; i++){

        QColor color(colorNames[i]);
        OrganLabelEditor* widget = new OrganLabelEditor("label-"+QString::number(i), &color);
        //QLabel* widget = new QLabel("haha");
        organLabelLayout->addWidget(widget);
        qDebug() << i << widget << "-th label created!";
    }

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


void MainWindow::removeDataset(){
    // 移除view3D中的对象
    vtkActor* actor = this->m_renderer->GetActors()->GetLastActor();
    if(actor != nullptr){
        this->m_renderer->RemoveActor(actor);
    }
    ui->view3d->renderWindow()->Render();
}


void MainWindow::addDataset(vtkImageAlgorithm* reader){
    vtkMarchingCubes* surfaces = createSurfaces(reader->GetOutput());
    vtkSmoothPolyDataFilter* smoother = createSmoother(surfaces->GetOutputPort(), 50);
    vtkPolyDataNormals* normals = createNormals(smoother->GetOutputPort());
    vtkPolyDataMapper* mapper = createMapper(normals->GetOutputPort());
    double* range = reader->GetOutput()->GetScalarRange();
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


void MainWindow::colorwheel(const QColor& color){
    qDebug() << "hahah" << color;
}




