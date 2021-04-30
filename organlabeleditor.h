#ifndef ORGANLABELEDITOR_H
#define ORGANLABELEDITOR_H

#include <QString>
#include <QWidget>
#include <QLabel>
#include <QPushButton>


class OrganLabelEditor : public QWidget{
    Q_OBJECT
public:
    static uint count;

    uint index;

    QString organName;
    float opacity;
    QLabel* nameLabel;
    QPushButton* colorLabel;
    QLabel* opacityLabel;
    QLayout* layout;

    OrganLabelEditor();
    OrganLabelEditor(QString organName);
    OrganLabelEditor(QString organName, QColor* color);

    ~OrganLabelEditor();

public slots:
    void organColorLabelClicked(bool);

};

#endif // ORGANLABELEDITOR_H
