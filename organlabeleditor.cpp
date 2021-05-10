#include "organlabeleditor.h"
#include <QHBoxLayout>
#include <QColor>
#include <QDebug>
#include <qrgb.h>

#include "LabelEditorDialog.h"


uint OrganLabelEditor::count = 0;
LabelEditorDialog* OrganLabelEditor::labelEditor = 0;

OrganLabelEditor::OrganLabelEditor(QWidget* parent) : QWidget (parent){
    // 初始化
    this->nameLabel = new QLabel("器官");
    this->colorLabel = new QPushButton();
    this->opacityLabel = new QPushButton();

    this->rgba = new int[4]{0,0,0,1};

    // 设置属性
    this->nameLabel->setMaximumWidth(80);
    this->colorLabel->setObjectName("organLabel-"+QString::number(OrganLabelEditor::count));
    this->colorLabel->setMaximumWidth(40);
    this->opacityLabel->setMaximumWidth(50);

    // 放入布局中
    this->layout = new QHBoxLayout(this);
    this->layout->addWidget(this->nameLabel);
    this->layout->addWidget(this->colorLabel);
    this->layout->addWidget(this->opacityLabel);

    // 信号与槽
    connect(this->colorLabel, SIGNAL(clicked(bool)), this, SLOT(organColorLabelClicked(bool)));

    // 设置整体属性
    this->setMaximumHeight(40);
    this->setMaximumWidth(200);
    this->layout->setSpacing(5);
    this->layout->setMargin(2);
    this->index = OrganLabelEditor::count;
    OrganLabelEditor::count += 1;
    if(OrganLabelEditor::labelEditor == 0){
        OrganLabelEditor::labelEditor = new LabelEditorDialog(this);

    }
}

OrganLabelEditor::OrganLabelEditor(QString organName, QWidget* parent) : OrganLabelEditor(parent){
    this->organName = organName;
    this->nameLabel->setText(this->organName);
}

OrganLabelEditor::OrganLabelEditor(QString organName, QColor* color, QWidget* parent) : OrganLabelEditor(parent){
    QRgb rgba = color->rgba();
    int r = qRed(rgba), g = qGreen(rgba), b = qBlue(rgba), a = qAlpha(rgba);
    QString style = QString().sprintf("QPushButton{background-color:rgb(%d,%d,%d);}", r, g, b);
    this->colorLabel->setStyleSheet(style);
    this->organName = organName;
    this->nameLabel->setText(this->organName);
    this->rgba = new int[4]{r, g, b, a};
    this->opacityLabel->setText(QString::number(int(100*a/255))+"%");
}

void OrganLabelEditor::organColorLabelClicked(bool clicked){
    qDebug() << "You are clicking " << this->index << "-th organ color label clicked=" << clicked;
    QString file;
    if(OrganLabelEditor::labelEditor == 0){
        OrganLabelEditor::labelEditor = new LabelEditorDialog(this);

    }

    QColor color = QColor();
    color.setRgb(this->rgba[0], this->rgba[1], this->rgba[2], this->rgba[3]);
    labelEditor->selectLabelIndex = int(this->index);
    labelEditor->setColor(&color);
    qDebug()  << "what setColor???" << endl;
    labelEditor->setOpacity(this->rgba[3]);
    qDebug()  << "what setOpacity???" << endl;
    labelEditor->show();
}

OrganLabelEditor::~OrganLabelEditor(){

}

void OrganLabelEditor::setOpacity(int opacity){
    this->rgba[3] = opacity;
    this->opacityLabel->setText(QString::number(int(100*opacity/255.))+"%");
}

void OrganLabelEditor::setColor(int r, int g, int b, int a){
    this->rgba[0] = r;
    this->rgba[1] = g;
    this->rgba[2] = b;
    this->rgba[3] = a;
    QString style = QString().sprintf("QPushButton{background-color:rgba(%d,%d,%d);}", r, g, b);
    this->colorLabel->setStyleSheet(style);
}

int OrganLabelEditor::getOpacity(){
    return this->rgba[3];
}

int* OrganLabelEditor::getColor(){
    int *color = new int[4];
    color[0] = this->rgba[0];
    color[1] = this->rgba[1];
    color[2] = this->rgba[2];
    color[3] = this->rgba[3];

    return color;
}


