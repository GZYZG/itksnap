#ifndef UTILS_H
#define UTILS_H
#include <QFileDialog>
#include <vtkLookupTable.h>
#include <vtkImageData.h>
#include <vtkMarchingCubes.h>
#include <vtkSmoothPolyDataFilter.h>
#include <vtkPolyDataNormals.h>
#include <vtkPolyDataMapper.h>
#include <vtkImageViewer2.h>
#include <vtkCommand.h>
#include <vtkSliderWidget.h>
#include <vtkSliderRepresentation2D.h>
#include <vtkDiscreteMarchingCubes.h>
#include <vtkDecimatePro.h>
#include <QLayout>

static void initializeImageFileDialog(QFileDialog &dialog, QFileDialog::AcceptMode acceptMode);

vtkLookupTable* createLookUpTable(int min, int max);

vtkLookupTable* createColorTable(int min, int max);

vtkMarchingCubes* createSurfaces(vtkImageData* data);

vtkSmoothPolyDataFilter* createSmoother(vtkAlgorithmOutput* data, int iterNum);

vtkPolyDataNormals* createNormals(vtkAlgorithmOutput* data);

vtkPolyDataMapper* createMapper(vtkAlgorithmOutput* data);

vtkActor* createActor(vtkMapper* mapper);

vtkDecimatePro* create_polygon_reducer(vtkDiscreteMarchingCubes* extractor);

void create2DITKImageReader(std::string type);

void clearLayout(QLayout* layout);

class vtkSliderCallback : public vtkCommand
{
public:
    static vtkSliderCallback *New()
    {
        return new vtkSliderCallback;
    }
    virtual void Execute(vtkObject *caller, unsigned long, void*)
    {
        vtkSliderWidget *sliderWidget =
            reinterpret_cast<vtkSliderWidget*>(caller);
        this->viewer->SetSlice(static_cast<vtkSliderRepresentation *>(sliderWidget->GetRepresentation())->GetValue());
    }
    vtkSliderCallback() {}
    vtkSmartPointer<vtkImageViewer2> viewer = nullptr;
};




extern const int PREDEFINED_COLOR_NUM;
extern const int PREDEFINED_COLOR[][4];  // 预定义14种颜色。采用rgba定义颜色，每个分量取值为0~255
extern QHash<QString, QString> dicom_tag;

#endif // UTILS_H
