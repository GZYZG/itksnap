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
    this->colorLabel = new QPushButton("颜色");
    this->opacityLabel = new QPushButton("25%");
    this->opacity = 1.0;

    // 设置属性
    this->nameLabel->setMaximumWidth(100);
    this->colorLabel->setObjectName("organLabel-"+QString::number(OrganLabelEditor::count));
    this->colorLabel->setMaximumWidth(30);
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
    this->setMinimumWidth(15);
    this->layout->setSpacing(5);
    this->layout->setMargin(2);
    this->index = OrganLabelEditor::count;
    OrganLabelEditor::count += 1;
}

OrganLabelEditor::OrganLabelEditor(QString organName, QWidget* parent) : OrganLabelEditor(parent){
    this->organName = organName;
    this->nameLabel->setText(this->organName);
}

OrganLabelEditor::OrganLabelEditor(QString organName, QColor* color, QWidget* parent) : OrganLabelEditor(parent){
    QRgb rgb = color->rgb();
    int r = qRed(rgb), g = qGreen(rgb), b = qBlue(rgb);
    QString style = QString().sprintf("QPushButton{background-color:rgb(%d,%d,%d);}", r, g, b);
    this->colorLabel->setStyleSheet(style);
    this->organName = organName;
    this->nameLabel->setText(this->organName);
}

void OrganLabelEditor::organColorLabelClicked(bool clicked){
    qDebug() << "You are clicking " << this->index << "-th organ color label clicked=" << clicked;
    QString file;
    if(OrganLabelEditor::labelEditor == 0){
        OrganLabelEditor::labelEditor = new LabelEditorDialog(this);

    }


    labelEditor->show();

}

OrganLabelEditor::~OrganLabelEditor(){

}


