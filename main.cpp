#include "mainwindow.h"
#include <QApplication>
#include <QFile>
#include <QTextStream>

#include <vtkAutoInit.h>
VTK_MODULE_INIT(vtkRenderingOpenGL2);
VTK_MODULE_INIT(vtkInteractionStyle);


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    QFile f(":qdarkstyle/light/style.qss");

    if (!f.exists())   {
        printf("Unable to set stylesheet, file not found\n");
    } else {
        f.open(QFile::ReadOnly | QFile::Text);
        QTextStream ts(&f);
        w.setStyleSheet(ts.readAll());
    }
    w.show();

    return a.exec();
}
