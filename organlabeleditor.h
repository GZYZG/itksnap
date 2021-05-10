#ifndef ORGANLABELEDITOR_H
#define ORGANLABELEDITOR_H

#include <QString>
#include <QWidget>
#include <QLabel>
#include <QPushButton>

#include "LabelEditorDialog.h"


class OrganLabelEditor : public QWidget{
    Q_OBJECT
public:
    static uint count;
    static LabelEditorDialog* labelEditor;

    uint index;

    QString organName;
    //float opacity;
    QLabel* nameLabel;
    QPushButton* colorLabel;
    QPushButton* opacityLabel;
    QLayout* layout;
    int *rgba;   // 0~255


    OrganLabelEditor(QWidget* parent=0);
    OrganLabelEditor(QString organName, QWidget* parent=0);
    OrganLabelEditor(QString organName, QColor* color, QWidget* parent=0);

    ~OrganLabelEditor();

    void setOpacity(int opacity);
    void setColor(int r, int g, int b, int a);

    int getOpacity();
    int* getColor();

public slots:
    void organColorLabelClicked(bool);

};

#endif // ORGANLABELEDITOR_H
