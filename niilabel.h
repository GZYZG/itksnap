#ifndef NIILABEL_H
#define NIILABEL_H

#include <vtkProperty.h>
#include <vtkActor.h>
#include <vtkSmoothPolyDataFilter.h>
#include <vtkDiscreteMarchingCubes.h>
#include <vtkDecimatePro.h>
#include <vtkPolyDataNormals.h>
#include <vtkPolyDataMapper.h>

#include "organlabeleditor.h"

class NIILabel{
public:
    vtkDiscreteMarchingCubes *m_surfaceExtractor;
    vtkDecimatePro* m_reducer;
    vtkSmoothPolyDataFilter *m_smoother;
    vtkPolyDataNormals* m_normals;

    vtkMapper* m_mapper;
    vtkActor *m_actor;
    vtkProperty *m_property;
    OrganLabelEditor* m_rgba;  // 采用0~255表示颜色和透明度，但是在vtk中颜色和透明度是用0~1表示的
    int m_smoothness;
    int m_labelValue;

    NIILabel(OrganLabelEditor* rgba, int labelValue, int smoothness);
    ~NIILabel();

    void setOpacity(int opacity);
    void setColor(int r, int g, int b, int a);

    int getOpacity();
    int* getColor();

private:

    NIILabel();

};

#endif // NIILABEL_H
