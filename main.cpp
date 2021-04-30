#include "mainwindow.h"
#include <QApplication>

#include <vtkAutoInit.h>
VTK_MODULE_INIT(vtkRenderingOpenGL2);//可能是vtkRenderingOpenGL,都试试 通过就行了
VTK_MODULE_INIT(vtkInteractionStyle);


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    return a.exec();
}
