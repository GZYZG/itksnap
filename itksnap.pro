#-------------------------------------------------
#
# Project created by QtCreator 2021-04-29T17:44:34
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = itksnap
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += c++11

CONFIG +=USE_VTK
CONFIG +=USE_ITK

SOURCES += \
        main.cpp \
        mainwindow.cpp \
        organlabeleditor.cpp \
        colorlisteditor.cpp \
        window.cpp  \
        ColorWheel.cpp \
        LabelEditorDialog.cpp \
        abstractmodel.cpp \
        utils.cpp \
        label.cpp  \
        LabelEditorDialog.cpp  \
        ViewPanel3D.cpp  \
        SliceViewPanel.cpp


HEADERS += \
        mainwindow.h \
        organlabeleditor.h \
        colorlisteditor.h  \
        window.h  \
        ColorWheel.h  \
        LabelEditorDialog.h \
        abstractmodel.h \
        utils.h \
        label.h \
        ViewPanel3D.h  \
        SliceViewPanel.h \
    niftiimagereader.h




FORMS += \
        mainwindow.ui \
        LabelEditorDialog.ui \
        ViewPanel3D.ui  \
        SliceViewPanel.ui

INCLUDEPATH += D:/codeprograms/mybuilds/vtk9.0install/include/vtk-9.0
INCLUDEPATH += D:/codeprograms/itk_install/include/ITK-5.1


QMAKE_LIBDIR +=  D:/Windows Kits/10/Lib/10.0.17763.0/um/x64
LIBS+=kernel32.lib \
      user32.lib \
      gdi32.lib \
      winspool.lib \
      comdlg32.lib \
      advapi32.lib \
      shell32.lib \
      ole32.lib \
      oleaut32.lib \
      uuid.lib \
      odbc32.lib \
      odbccp32.lib \

LIBS += $$quote(D:/codeprograms/mybuilds/vtk9.0install/lib/*.lib)
LIBS += $$quote(D:/codeprograms/itk_install/lib/*.lib)
#LIBS += $$quote(D:/Windows Kits/10/Lib/10.0.17763.0/um/x64/*.lib)




# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    resources.qrc
