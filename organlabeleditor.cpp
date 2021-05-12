#include "organlabeleditor.h"
#include <QHBoxLayout>
#include <QColor>
#include <QDebug>
#include <qrgb.h>
#include <qtextcodec.h>

#include "LabelEditorDialog.h"
#include "mainwindow.h"


uint OrganLabelEditor::count = 0;
//LabelEditorDialog* OrganLabelEditor::labelEditor = 0;

OrganLabelEditor::OrganLabelEditor(QWidget* parent) : QWidget (parent){
    // 创建
    QTextCodec *codec = QTextCodec::codecForName("GBK");//修改这两行
    this->organName = codec->toUnicode("器官");
    this->nameLabel = new QLabel(this->organName);
    this->colorLabel = new QPushButton();
    this->opacityLabel = new QPushButton();

    this->rgba = new int[4]{0,0,0,1};

    // 设置各个部分的属性
    this->nameLabel->setMaximumWidth(120);
    this->colorLabel->setObjectName("organLabel-"+QString::number(OrganLabelEditor::count));
    this->colorLabel->setMaximumWidth(30);
    this->opacityLabel->setMaximumWidth(50);

    // 添加至布局
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
    this->setObjectName("OrganLabel-"+QString::number(this->index));
    OrganLabelEditor::count += 1;
}

OrganLabelEditor::OrganLabelEditor(std::string organName, QWidget* parent) : OrganLabelEditor(parent){
    QTextCodec *codec = QTextCodec::codecForName("GBK");//修改这两行
    this->organName = codec->toUnicode(organName.c_str());
    this->nameLabel->setText(this->organName);
}

OrganLabelEditor::OrganLabelEditor(std::string organName, QColor* color, QWidget* parent) : OrganLabelEditor(parent){
    QRgb rgba = color->rgba();
    int r = qRed(rgba), g = qGreen(rgba), b = qBlue(rgba), a = qAlpha(rgba);
    QString style = QString().sprintf("QPushButton{background-color:rgb(%d,%d,%d);}", r, g, b);
    this->colorLabel->setStyleSheet(style);
    QTextCodec *codec = QTextCodec::codecForName("GBK");//修改这两行
    this->organName = codec->toUnicode(organName.c_str());
    this->nameLabel->setText(this->organName);
    this->rgba = new int[4]{r, g, b, a};
    this->opacityLabel->setText(QString::number(int(100*a/255))+"%");
}

void OrganLabelEditor::organColorLabelClicked(bool clicked){
    //qDebug() << "You are clicking " << this->index << "-th organ color label clicked=" << this->organName;
    QColor color = QColor();

    color.setRgb(this->rgba[0], this->rgba[1], this->rgba[2], this->rgba[3]);
    labelEditor->selectLabelIndex = int(this->index);
    labelEditor->setColor(&color);
    labelEditor->setOpacity(this->rgba[3]);
    labelEditor->setInLabelDescription(this->organName);
    labelEditor->show();
}

OrganLabelEditor::~OrganLabelEditor(){
    delete nameLabel;
    delete colorLabel;
    delete rgba;
    delete opacityLabel;
    delete layout;
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


void OrganLabelEditor::setLabelEditorDialog(LabelEditorDialog *dialog){
    this->labelEditor = dialog;
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


