#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGroupBox>
#include <QToolBox>
#include <QListWidget>


// 定义独立于实现的类名
typedef QGroupBox OrganLabelContainer ;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    QToolBox* toolBox;  // 左侧的toolbox，用于放置可折叠的面板
    OrganLabelContainer* organLable;  // 器官可视化设置面板
    QGroupBox* other;

    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void initLeft();

private slots:
    void on_pushButton_clicked();
    void on_actionOpen_triggered();
    void colorwheel(const QColor& color);

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H