#include "printutils.h"
#include <QString>
#include <QDebug>
#include <iostream>

void printActorBound(double *bound){
    QString s = QString().sprintf("[xmin=%.2f xmax=%.2f ymin=%.2f ymax=%.2f zmin=%.2f zmax=%.2f]",
                      bound[0], bound[1], bound[2], bound[3], bound[4], bound[5]);
    std::cout << s.toStdString() << std::endl;
}
