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
    LabelEditorDialog* labelEditor;

    uint index;

    QString organName;
    //float opacity;
    QLabel* nameLabel;
    QPushButton* colorLabel;
    QPushButton* opacityLabel;
    QLayout* layout;
    int *rgba;   // 0~255


    OrganLabelEditor(QWidget* parent=nullptr);
    OrganLabelEditor(std::string organName, QWidget* parent=nullptr);
    OrganLabelEditor(std::string organName, QColor* color, QWidget* parent=nullptr);
    ~OrganLabelEditor();

    void setOpacity(int opacity);
    void setColor(int r, int g, int b, int a);
    void setLabelEditorDialog(LabelEditorDialog* dialog);

    int getOpacity();
    int* getColor();

public slots:
    void organColorLabelClicked(bool);

};

#endif // ORGANLABELEDITOR_H
