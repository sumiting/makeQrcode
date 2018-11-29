#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "tcQrencode.h"
#include <QTextEdit>
#include "opencv2/opencv.hpp"
#include <iostream>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_pushButton_clicked();
    void on_pushButton_2_clicked();

    void on_pushButton_3_clicked();

protected:
    void dragEnterEvent(QDragEnterEvent *e);
    void dropEvent(QDropEvent *e);


private:
    bool readFile(const QString &fileName,const QString &fileName2);
    void mkQrcode(QString line);
    std::string MainWindow::UTF8ToGBK(char* strUTF8);
    QTextEdit *textEdit;
    Ui::MainWindow *ui;
    TcQrencode* renders;

};

#endif // MAINWINDOW_H
