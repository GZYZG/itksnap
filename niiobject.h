#ifndef NIIOBJECT_H
#define NIIOBJECT_H

#include <vtkNIFTIImageReader.h>
#include <vtkNew.h>
#include <vtkPolyDataMapper.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>

#include "organlabeleditor.h"
#include "niilabel.h"

class NIIObject : public QObject{
    Q_OBJECT
public:
    std::string m_file;
    double *m_scalarRange;
    int* m_extent;
    vtkNIFTIImageReader* m_reader;
    QList<NIILabel*>* m_labels;
    vtkPolyDataMapper *m_mapper;
    vtkRenderer *m_renderer;

    NIIObject(std::string fileName);
    void surfaceRendering(vtkRenderer* renderer);
    void surfaceRendering(int labelIndex, vtkRenderer* renderer);
    void initLabels();
    ~NIIObject();

private slots:
    void changeSurfaceOpacity(int selectLabelIndex, int opacity);
    void changeSurfaceColor(int selectLabelIndex, int r, int g, int b, int a);

private:
    NIIObject();
};

#endif // NIIOBJECT_H
