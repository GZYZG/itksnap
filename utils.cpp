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

    for(int i=0; i < n; i++){
        if( i < PREDEFINED_COLOR_NUM){
            r = PREDEFINED_COLOR[i][0];
            g = PREDEFINED_COLOR[i][1];
            b = PREDEFINED_COLOR[i][2];
            opacity = PREDEFINED_COLOR[i][3];
        }else{
            QColor color = QColor(colorNames[i]);
            QRgb rgb = (&color)->rgb();
            r = qRed(rgb);
            g = qGreen(rgb);
            b = qBlue(rgb);
            opacity = 255*(1 - i / n);
        }

        clut->SetTableValue(i/255, r/255, g/255, b/255, opacity/255);
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
    surface->SetNumberOfContours(n);
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

void clearLayout(QLayout* layout){
    QLayoutItem *child;
    while((child = layout->takeAt(0)) != nullptr){
        if(child->widget()){
            child->widget()->setParent(nullptr);
        } else if(child->layout()){
            clearLayout(child->layout());
        }

        delete child;
    }
}


const int PREDEFINED_COLOR_NUM = 14;
const int PREDEFINED_COLOR[PREDEFINED_COLOR_NUM][4] = {{0, 0, 0, 0},
                                {255, 0, 0, 255},
                                {0, 255, 0, 255},
                                {0, 0, 255, 255},
                                {255, 255, 0, 255},
                                {0, 255, 255, 255},
                                {255, 0, 255, 255},
                                {25, 239, 213, 255},
                                {0, 0, 205, 255},
                                {205, 133, 63, 255},
                                {210, 180, 140, 255},
                                {102, 205, 170, 255},
                                {0, 0, 128, 255},
                                {0, 139, 139, 255}};

QHash<QString, QString> dicom_tag = {
    {"Image Type", "0008|0008"},
    {"Instance Creation Time", "0008|0013"},
    {"SOP Class UID", "0008|0016"},
    {"SOP Instance UID", "0008|0018"},
    {"Study Date", "0008|0020"},
    {"Series Date", "0008|0021"},
    {"Content Date", "0008|0023"},
    {"Study Time", "0008|0030"},
    {"Series Time", "0008|0031"},
    {"Content Time", "0008|0033"},
    {"Accession Number", "0008|0050"},
    {"Modality", "0008|0060"},
    {"Manufacturer", "0008|0070"},
    {"Institution Name", "0008|0080"},
    {"Referring Physician's Name", "0008|0090"},
    {"Station Name", "0008|1010"},
    {"Study Description", "0008|1030"},
    {"Series Description", "0008|103e"},
    {"Institutional Department Name", "0008|1040"},
    {"Manufacturer's Model Name", "0008|1090"},
    {"Recommended Display Frame Rate", "0008|2144"},
    {"Patient's Name", "0010|0010"},
    {"Patient ID", "0010|0020"},
    {"Patient's Birth Date", "0010|0030"},
    {"Patient's Sex", "0010|0040"},
    {"Contrast/Bolus Agent", "0018|0010"},
    {"Cine Rate", "0018|0040"},
    {"KVP", "0018|0060"},
    {"Device Serial Number", "0018|1000"},
    {"Software Version(s)", "0018|1020"},
    {"Protocol Name", "0018|1030"},
    {"Frame Time", "0018|1063"},
    {"Frame Delay", "0018|1066"},
    {"Distance Source to Detector", "0018|1110"},
    {"Distance Source to Patient", "0018|1111"},
    {"Table Motion", "0018|1134"},
    {"Exposure Time", "0018|1150"},
    {"X-Ray Tube Current", "0018|1151"},
    {"Radiation Setting", "0018|1155"},
    {"Imager Pixel Spacing", "0018|1164"},
    {"Positioner Motion", "0018|1500"},
    {"Positioner Primary Angle", "0018|1510"},
    {"Positioner Secondary Angle", "0018|1511"},
    {"Shutter Shape", "0018|1600"},
    {"Shutter Left Vertical Edge", "0018|1602"},
    {"Shutter Right Vertical Edge", "0018|1604"},
    {"Shutter Upper Horizontal Edge", "0018|1606"},
    {"Shutter Lower Horizontal Edge", "0018|1608"},
    {"Study Instance UID", "0020|000d"},
    {"Series Instance UID", "0020|000e"},
    {"Study ID", "0020|0010"},
    {"Series Number", "0020|0011"},
    {"Instance Number", "0020|0013"},
    {"Patient Orientation", "0020|0020"},
    {"Laterality", "0020|0060"},
    {"Samples per Pixel", "0028|0002"},
    {"Photometric Interpretation", "0028|0004"},
    {"Number of Frames", "0028|0008"},
    {"Frame Increment Pointer", "0028|0009"},
    {"Rows", "0028|0010"},
    {"Columns", "0028|0011"},
    {"Bits Allocated", "0028|0100"},
    {"Bits Stored", "0028|0101"},
    {"High Bit", "0028|0102"},
    {"Pixel Representation", "0028|0103"},
    {"Pixel Intensity Relationship", "0028|1040"},
    {"Window Center", "0028|1050"},
    {"Window Width", "0028|1051"},
    {"Lossy Image Compression", "0028|2110"},
    {"Performed Procedure Step Start Date", "0040|0244"},
    {"Performed Procedure Step Start Time", "0040|0245"},
    {"Performed Procedure Step ID", "0040|0253"},
    {"Distance Source to Entrance", "0040|0306"},
    {"Curve Dimensions", "5000|0005"},
    {"Number of Points", "5000|0010"},
    {"Type of Data", "5000|0020"},
    {"Axis Units", "5000|0030"},
    {"Data Value Representation", "5000|0103"},
    {"Minimum Coordinate Value", "5000|0104"},
    {"Maximum Coordinate Value", "5000|0105"},
    {"Curve Data Descriptor", "5000|0110"},
    {"Coordinate Start Value", "5000|0112"},
    {"Coordinate Step Value", "5000|0114"},
    {"Curve Data", "5000|3000"}
};
