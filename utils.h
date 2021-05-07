#ifndef UTILS_H
#define UTILS_H
#include <QFileDialog>
#include <vtkLookupTable.h>
#include <vtkImageData.h>
#include <vtkMarchingCubes.h>
#include <vtkSmoothPolyDataFilter.h>
#include <vtkPolyDataNormals.h>
#include <vtkPolyDataMapper.h>

static void initializeImageFileDialog(QFileDialog &dialog, QFileDialog::AcceptMode acceptMode);

vtkLookupTable* createLookUpTable(int min, int max);

vtkLookupTable* createColorTable(int min, int max);

vtkMarchingCubes* createSurfaces(vtkImageData* data);

vtkSmoothPolyDataFilter* createSmoother(vtkAlgorithmOutput* data, int iterNum);

vtkPolyDataNormals* createNormals(vtkAlgorithmOutput* data);

vtkPolyDataMapper* createMapper(vtkAlgorithmOutput* data);

vtkActor* createActor(vtkMapper* mapper);

extern const int PREDEFINED_COLOR_NUM;
extern const double PREDEFINED_COLOR[][4];

#endif // UTILS_H
