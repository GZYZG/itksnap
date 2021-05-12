#ifndef NIIOBJECT_H
#define NIIOBJECT_H

#include <QThread>

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
    //vtkPolyDataMapper *m_mapper;
    vtkRenderer *m_renderer;

    NIIObject(std::string fileName);
    ~NIIObject();

    void surfaceRendering(int labelIndex, vtkRenderer* renderer);
    void initLabels();
    void clearLabels();

private slots:
    void changeSurfaceOpacity(int selectLabelIndex, int opacity);
    void changeSurfaceColor(int selectLabelIndex, int r, int g, int b, int a);
public slots:
    void surfaceRendering(vtkRenderer* renderer);

signals:
    void renderDone();

private:
    NIIObject();
};

class WorkerThread : public QThread
{
    Q_OBJECT
    void run() override {
        QString result;
        result = QString::fromStdString("work done...");
        emit resultReady(result);
    }
signals:
    void resultReady(const QString &s);
};

#endif // NIIOBJECT_H
