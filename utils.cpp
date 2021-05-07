#include <QFileDialog>
#include <QStandardPaths>
#include <QImageReader>
#include <QImageWriter>
#include <QDebug>

#include <vtkActor.h>

#include "utils.h"

static void initializeImageFileDialog(QFileDialog &dialog, QFileDialog::AcceptMode acceptMode)
{
    static bool firstDialog = true;

    if (firstDialog) {
        firstDialog = false;
        const QStringList picturesLocations = QStandardPaths::standardLocations(QStandardPaths::PicturesLocation);
        dialog.setDirectory(picturesLocations.isEmpty() ? QDir::currentPath() : picturesLocations.last());
    }

    QStringList mimeTypeFilters;
    const QByteArrayList supportedMimeTypes = acceptMode == QFileDialog::AcceptOpen
        ? QImageReader::supportedMimeTypes() : QImageWriter::supportedMimeTypes();
    foreach (const QByteArray &mimeTypeName, supportedMimeTypes)
        mimeTypeFilters.append(mimeTypeName);
    mimeTypeFilters.sort();
    dialog.setMimeTypeFilters(mimeTypeFilters);
    dialog.selectMimeTypeFilter("image/jpeg");
    if (acceptMode == QFileDialog::AcceptSave)
        dialog.setDefaultSuffix("jpg");
}

vtkLookupTable* createColorTable(int min, int max){
    vtkLookupTable* clut = vtkLookupTable::New();
    int n = max-min+1;
    clut->SetNumberOfColors(n);
    QStringList colorNames = QColor::colorNames();
    double r, g, b, opacity;

    for(int i=1; i < n; i++){
        if( i < PREDEFINED_COLOR_NUM){
            r = PREDEFINED_COLOR[i][0];
            g = PREDEFINED_COLOR[i][1];
            b = PREDEFINED_COLOR[i][2];
            opacity = PREDEFINED_COLOR[i][3];
        }else{
            QColor color = QColor(colorNames[i]);
            QRgb rgb = (&color)->rgb();
            r = qRed(rgb)/255;
            g = qGreen(rgb)/255;
            b = qBlue(rgb)/255;
            opacity = 1 - i / n;
        }

        clut->SetTableValue(i, r, g, b, opacity);
        qDebug() << r << ", " << g <<", " << b << ", " << opacity;
    }

    clut->Build();
    return clut;
}

vtkLookupTable* createLookUpTable(int min, int max){
    vtkLookupTable* lut = vtkLookupTable::New();
    lut->SetTableRange(min, max);
    lut->SetHueRange(0 ,0);
    lut->SetSaturationRange(0, 0);
    lut->SetValueRange(0, 2);
    lut->Build();

    return lut;
}

vtkMarchingCubes* createSurfaces(vtkImageData* data){
    vtkMarchingCubes* surface = vtkMarchingCubes::New();
    surface->SetInputData(data);
    double* range = data->GetScalarRange();
    int min = int(range[0]), max = int(range[1]);
    int n = max - min + 1;
    for(int i = 0; i < n; i++){
       surface->SetValue(i, min + i);
    }

    surface->Update();
    return  surface;
}

vtkSmoothPolyDataFilter* createSmoother(vtkAlgorithmOutput* data, int iterNum){
    vtkSmoothPolyDataFilter* smoother = vtkSmoothPolyDataFilter::New();
    smoother->SetNumberOfIterations(iterNum);
    smoother->SetInputConnection(data);

    smoother->Update();
    return smoother;
}

vtkPolyDataNormals* createNormals(vtkAlgorithmOutput* data){
    vtkPolyDataNormals* normals = vtkPolyDataNormals::New();
    normals->SetInputConnection(data);
    normals->SetFeatureAngle(60);

    normals->Update();

    return normals;
}

vtkPolyDataMapper* createMapper(vtkAlgorithmOutput* data){

    vtkPolyDataMapper* mapper = vtkPolyDataMapper::New();
    mapper->SetInputConnection(data);
    mapper->SetScalarVisibility(true);

    mapper->Update();
    return mapper;
}


vtkActor* createActor(vtkMapper* mapper){
    vtkActor* actor = vtkActor::New();
    actor->SetMapper(mapper);

    return actor;
}

const int PREDEFINED_COLOR_NUM = 14;
const double PREDEFINED_COLOR[PREDEFINED_COLOR_NUM][4] = {{0., 0., 0., 0.},
                                {1., 0., 0., 1.},
                                {0., 1., 0., 1.},
                                {0., 0., 1., 1.},
                                {1., 1., 0., 1.},
                                {0, 1, 1, 1},
                                {1., 0., 1., 1},
                                {255/255., 239/255., 213/255., 1.},
                                {0., 0., 205/255., 1.},
                                {205/255, 133/255., 63/255., 1.},
                                {210/255., 180/255., 140/255., 1.},
                                {102/255., 205/255., 170/255., 1.},
                                {0., 0., 128/255., 1.},
                                {0., 139/255., 139/255., 1.}};
