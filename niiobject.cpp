#include <qobject.h>
#include "niiobject.h"
#include "utils.h"
#include "printutils.h"

#include <exception>


NIIObject::NIIObject() : QObject(){
    m_file = "";
    m_reader = vtkNIFTIImageReader::New();
    //m_mapper = vtkNew<vtkPolyDataMapper>();
    m_extent = new int[6]{0};
    m_scalarRange = new double[2]{0};
    m_labels = new QList<NIILabel*>();
    m_renderer = nullptr;
}

NIIObject::NIIObject(std::string fileName) : NIIObject(){
    m_reader->SetFileName(fileName.c_str());
    m_reader->Update();  // 很快，不是时间瓶颈
    m_reader->GetDataExtent(m_extent);
    m_reader->GetOutput()->GetScalarRange(m_scalarRange);
    this->initLabels();
}


void NIIObject::surfaceRendering(vtkRenderer* renderer){
    // 比较耗时
    std::cout << "renerering in Thread:" << QThread::currentThreadId() << endl;
    QThread::sleep(10);
    if(m_renderer != renderer)  m_renderer = renderer;
    for(int i =1; i < this->m_labels->length(); i++){
        //std::cout << "renderering for NO." << i << " surface"<< endl;
        try {
            this->surfaceRendering(i, renderer);
        } catch (std::string e) {
            std::cerr << "exception occurs: " << e << endl;
        }

    }
    std::cout << "finished renderering" << endl;
    emit renderDone();
}

void NIIObject::surfaceRendering(int labelIndex, vtkRenderer* renderer){
    NIILabel *label = this->m_labels->at(labelIndex);
    label->m_surfaceExtractor->SetInputConnection(m_reader->GetOutputPort());
    label->m_surfaceExtractor->SetValue(0, label->m_labelValue);
    label->m_surfaceExtractor->Update();

    // if the cell size is 0 then there is no label_idx data
    if(label->m_surfaceExtractor->GetOutput()->GetMaxCellSize()){
        // setup reducer
        label->m_reducer->SetInputConnection(label->m_surfaceExtractor->GetOutputPort());
        label->m_reducer->SetTargetReduction(0.5);  // magic number
        label->m_reducer->PreserveTopologyOn();
        // setup smoother
        label->m_smoother->SetInputConnection(label->m_reducer->GetOutputPort());
        label->m_smoother->SetNumberOfIterations(label->m_smoothness);

        // setup normals
        label->m_normals->SetInputConnection(label->m_smoother->GetOutputPort());
        label->m_normals->SetFeatureAngle(60.0);
        // setup mapper
        label->m_mapper->SetInputConnection(label->m_normals->GetOutputPort());
        label->m_mapper->ScalarVisibilityOff();
        label->m_mapper->Update();
        // setup actor
        label->m_actor->SetMapper(label->m_mapper);
        label->m_actor->SetProperty(label->m_property);
        // add actor to renerer
        renderer->AddActor(label->m_actor);

        //printActorBound(label->m_actor->GetBounds());
    }
}

void NIIObject::initLabels(){
    int min = int(m_scalarRange[0]), max = int(m_scalarRange[1]);
    int n = max - min + 1;
    if(n > PREDEFINED_COLOR_NUM){
        std::cerr << "支持的标签类别数最大为 " << PREDEFINED_COLOR_NUM << "，但是读入的文件中标签类别数为 " << n << "，超出的标签不被显示!" << endl;
        n = PREDEFINED_COLOR_NUM;
    }
    for(int i = 0; i < n; i++){
        QColor* color = new QColor();
        int r = PREDEFINED_COLOR[i][0], g = PREDEFINED_COLOR[i][1], b = PREDEFINED_COLOR[i][2], a = PREDEFINED_COLOR[i][3];
        color->setRgb(r, g, b, a);
        OrganLabelEditor* organ = new OrganLabelEditor(CNLABELNAMES[i], color);
        organ->index = i;
        NIILabel* label = new NIILabel(organ, min+i, 200);
        this->m_labels->append(label);
    }
}

void NIIObject::changeSurfaceOpacity(int selectLabelIndex, int opacity){
    if(selectLabelIndex == -1)  return;
    //std::cout << "surface opacity changed, and index is " << selectLabelIndex << endl;
    NIILabel* label = this->m_labels->at(selectLabelIndex);
    label->setOpacity(opacity);
    m_renderer->GetRenderWindow()->Render();

}

void NIIObject::changeSurfaceColor(int selectLabelIndex, int r, int g, int b, int a){
    if(selectLabelIndex == -1)  return;
    //std::cout << "before surface color changed, and index is " << selectLabelIndex << endl;
    NIILabel *label = this->m_labels->at(selectLabelIndex);
    label->setColor(r, g, b, a);
    m_renderer->Render();
    m_renderer->GetRenderWindow()->Render();
    //std::cout << "surface color changed, and index is " << selectLabelIndex << endl;
}

NIIObject::~NIIObject(){
    delete m_scalarRange;
    delete m_extent;
    for(int i=0; i < m_labels->length(); i++){
        delete m_labels->at(i);
    }
    delete m_labels;

    m_reader->Delete();
    //std::cout << "hahaha 1" << endl;
    //m_mapper->Delete();
    //std::cout << "hahaha 2" << endl;
    m_renderer->Delete();
    std::cout << "niiobject deleted" << endl;
}

