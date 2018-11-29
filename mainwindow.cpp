#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDragEnterEvent>
#include <QMimeData>
#include <QFile>
#include <QUrl>
#include <QDebug>
#include <QFileDialog>
#include <QMessageBox>
#include <QFileInfo>
#include <QIODevice>
#include <Windows.h>
#include <string>
#include <QDir>
#include <iostream>
using namespace std;
using namespace cv;
QString imgPath,txtPath;


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{


    textEdit = new QTextEdit;
    setCentralWidget(textEdit);
    textEdit->setAcceptDrops(false); //禁用textEdit控件的放下操作
    setAcceptDrops(true);//启用textEdit控件的放下操作
    setWindowTitle(tr("Text Editor"));
    ui->setupUi(this);
    renders = new TcQrencode();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_clicked()
{  //
   // QString m = ui->lineEdit->text();
   // QImage a = renders->encodeImage(m,8);//350*350=25*16 16
   // a.save("xx.jpg","jpg",100);
   // ui->label_2->setPixmap(QPixmap::fromImage(a));
   // qDebug()<<imgPath<<txtPath;
    readFile(txtPath,imgPath);
}

void MainWindow::dragEnterEvent(QDragEnterEvent *e)
{
    if(e->mimeData()->hasFormat("text/uri-list")) //只能打开文本文件
        e->acceptProposedAction(); //可以在这个窗口部件上拖放对象
}

//拖动选择文件
void MainWindow::dropEvent(QDropEvent *e) //释放对方时，执行的操作
{
    QFileInfo fileinfo;//文件信息
    QList<QUrl> urls = e->mimeData()->urls();
    if(urls.isEmpty())
        return ;

    QString fileName = urls.first().toLocalFile();
    fileinfo = QFileInfo(fileName);

    if(fileName.isEmpty())
        return;
    if(fileinfo.suffix()=="jpg" || fileinfo.suffix()=="png" ){
        QImage logo;
        logo.load(fileName);
        ui->label->setPixmap(QPixmap::fromImage(logo));
        imgPath=fileName;
        ui->lineEdit_2->setText(fileName);
    }else{
        txtPath=fileName;
        ui->lineEdit->setText(fileName);
    }
}

//txtPath imgPath
bool MainWindow::readFile(const QString &fileName,const QString &fileName2) //读文件
{
    QFile file(fileName);
    QString line;//一行数据
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
           return false;
    QTextStream in(&file);
    while (1)
    {
        line = in.readLine();
        if (line.isNull()){
            break;
        }
        //()<<line;
        QStringList sl=line.split(" ");
        //qDebug()<<sl;
        mkQrcode(line);//生成二维码
    }
    QMessageBox::information(NULL, tr("img"), tr("已经生成完毕"));
    return true;
}

//make Qrcode
void MainWindow::mkQrcode(QString line){

    QStringList listInfo;
    line.replace("\t"," ");
    QStringList sl =line.split(" ");
    QString info;
    foreach (info, sl) {
        if(info!=""){
            listInfo.append(info);
        }
    }

    QString PattDirName=listInfo[0];
    QString ChildDirName=listInfo[1];
    QString NameNext;
    QString urlPre=listInfo[2];

    urlPre=urlPre.section("",0,-5);
    qint32 num=listInfo[3].toInt();
    qint32 i=1;
    QString url;
    while(i<=num){

        if(i<10){
            NameNext="00"+ QString("%1").arg(i);
            url=urlPre+NameNext;
        }
        else if(i>=10 && i<100){
            NameNext="0"+ QString("%1").arg(i);
            url=urlPre+NameNext;
        }else{
            NameNext=QString("%1").arg(i);
            url=urlPre+NameNext;
        }
        i+=1;
        QImage a = renders->encodeImage(url,14);//350*350=25*16 16
        a.save("./xx.png");
        Mat CodeImg=imread("./xx.png");//Qimage转Mat不方便,重新读取图
        string c_str = imgPath.toStdString();
        Mat logoImg=imread(c_str);

        //加logo
        Mat ROIimage = CodeImg(cv::Rect(128,128,logoImg.cols,logoImg.rows));
        logoImg.copyTo(ROIimage);

        //存储起来
        QDir * temp=new QDir();
        bool exist1 = temp->exists("./imgData");
        if(!exist1){
            temp->mkdir("./imgData");
        }
        bool exist = temp->exists("./imgData/"+PattDirName);
        if(!exist){
            temp->mkdir("./imgData/"+PattDirName);
        }
        exist=temp->exists("./imgData/"+PattDirName+"/"+ChildDirName);
        if(!exist){
            temp->mkdir("./imgData/"+PattDirName+"/"+ChildDirName);
        }

        QString filePath="./imgData/"+PattDirName+"/"+ChildDirName+"/"+NameNext+".jpg";
        string fpath=filePath.toStdString();
        char *p=(char*)fpath.c_str();
        bool wFlag=imwrite(UTF8ToGBK(p),CodeImg);
    }
}

//选择txt文件
void MainWindow::on_pushButton_2_clicked()
{
    QString path = QFileDialog::getOpenFileName(this, tr("Open txt"), ".", tr("Txt Files(*.txt)"));
           if(path.length() == 0) {
                    QMessageBox::information(NULL, tr("Path"), tr("You didn't select any files."));
           } else {
                 //  QMessageBox::information(NULL, tr("Path"), tr("You selected ") + path);
           }
    txtPath=path;
    ui->lineEdit->setText(path);
    qDebug()<<path;
}


//选择框选择img
void MainWindow::on_pushButton_3_clicked()
{
    QString path = QFileDialog::getOpenFileName(this, tr("Open Image"), ".", tr("Image Files(*.jpg *.png)"));
           if(path.length() == 0) {
                   QMessageBox::information(NULL, tr("Path"), tr("You didn't select any files."));
           } else {
//                   QMessageBox::information(NULL, tr("Path"), tr("You selected ") + path);
           }

    QImage img;
    img.load (path);
    ui->label->setPixmap(QPixmap::fromImage(img));//显示logo
    ui->lineEdit_2->setText(path);//显示logo路径
    imgPath=path;
    qDebug()<<path;
}

string MainWindow::UTF8ToGBK( char* strUTF8)
{
    int len = MultiByteToWideChar(CP_UTF8, 0, strUTF8, -1, NULL, 0);
    wchar_t* wszGBK = new wchar_t[len+1];
    memset(wszGBK, 0, len*2+2);
    MultiByteToWideChar(CP_UTF8, 0, strUTF8, -1, wszGBK, len);
    len = WideCharToMultiByte(CP_ACP, 0, wszGBK, -1, NULL, 0, NULL, NULL);
    char* szGBK = new char[len+1];
    memset(szGBK, 0, len+1);
    WideCharToMultiByte(CP_ACP, 0, wszGBK, -1, szGBK, len, NULL, NULL);
    string strTemp(szGBK);
    if(wszGBK) delete[] wszGBK;
    if(szGBK) delete[] szGBK;
    return strTemp;
}

