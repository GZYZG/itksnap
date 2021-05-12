#include "niilabel.h"

NIILabel::NIILabel(){
    this->m_surfaceExtractor = vtkDiscreteMarchingCubes::New();
    this->m_reducer =vtkDecimatePro::New();
    this->m_smoother = vtkSmoothPolyDataFilter::New();
    this->m_normals = vtkPolyDataNormals::New();
    this->m_mapper = vtkPolyDataMapper::New();
    this->m_actor = vtkActor::New();
    this->m_property = vtkProperty::New();
    this->m_rgba = nullptr;
    this->m_smoothness = 500;
    this->m_property = vtkProperty::New();

    this->m_labelValue = 0;



}

NIILabel::NIILabel(OrganLabelEditor* rgba, int labelValue, int smoothness) : NIILabel() {
    this->m_rgba = rgba;
    this->m_smoothness = smoothness;
    this->m_property->SetColor(rgba->rgba[0]/255., rgba->rgba[1]/255., rgba->rgba[2]/255.);
    this->m_property->SetOpacity(rgba->rgba[3]/255.);
    this->m_labelValue = labelValue;
}

void NIILabel::setOpacity(int opacity){
    this->m_rgba->setOpacity(opacity);
    this->m_property->SetOpacity(opacity/255.);
}

void NIILabel::setColor(int r, int g, int b, int a){
    this->m_rgba->setColor(r, g, b, a);
    this->m_property->SetColor(r/255., g/255., b/255.);
    this->setOpacity(a);
}

int NIILabel::getOpacity(){
    return this->m_rgba->getOpacity();
}

int* NIILabel::getColor(){
    return this->m_rgba->getColor();
}

NIILabel::~NIILabel(){
    m_surfaceExtractor->Delete();
    m_reducer->Delete();
    m_smoother->Delete();
    m_normals->Delete();
    m_mapper->Delete();
    m_actor->Delete();
    delete m_rgba;
    m_property->Delete();
    //std::cout << "NIILabel deleted " << endl;
}

