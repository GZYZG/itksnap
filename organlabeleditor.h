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
    float opacity;
    QLabel* nameLabel;
    QPushButton* colorLabel;
    QPushButton* opacityLabel;
    QLayout* layout;


    OrganLabelEditor(QWidget* parent=0);
    OrganLabelEditor(QString organName, QWidget* parent=0);
    OrganLabelEditor(QString organName, QColor* color, QWidget* parent=0);

    ~OrganLabelEditor();

public slots:
    void organColorLabelClicked(bool);

};

#endif // ORGANLABELEDITOR_H
