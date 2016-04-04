#include "dialog.h"
#include "ui_dialog.h"
#include <QtGui>
#include <QFile>
#include <QFileDialog>
#include <QFileInfo>
#include <QMessageBox>
#include <QDataStream>
#include <QTextStream>
#include <QLabel>
#include <QStringListModel>

Dialog::Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog)
{
    ui->setupUi(this);
    setMaximumSize(600,400);
    setMinimumSize(600,400);
    ui->sourceFileList->setAcceptDrops(true);
    Qt::WindowFlags flags=Qt::Dialog;

    flags =Qt::WindowMinimizeButtonHint | Qt::WindowMaximizeButtonHint | Qt::WindowCloseButtonHint;
    setWindowFlags(flags);

    channel_number = 0;
    m_protocol = 0;
    fileNumber = 0;
    setWindowTitle(tr("sEMG Convertor"));
    ui->label->setText("<h3><font color = red>欢迎使用</font></h3>");

    ui->sourceFileList->setSelectionMode(QAbstractItemView::ExtendedSelection);
    ui->sourceFileList->setDragEnabled(true);
    ui->sourceFileList->viewport()->setAcceptDrops(true);
    ui->sourceFileList->setDropIndicatorShown(true);

    mthread = new ConvertThread(this);

//    QObject::connect(ui->radioButton,SIGNAL(clicked()),this,SLOT(ch64()));
 //   QObject::connect(ui->radioButton_2,SIGNAL(clicked()),this,SLOT(ch128()));
    QObject::connect(ui->comboBox,SIGNAL(currentIndexChanged(int)),this,SLOT(chooseCh(int)));
    QObject::connect(ui->comboBox_2,SIGNAL(currentIndexChanged(int)),this,SLOT(chooseProtocol(int)));
    QObject::connect(ui->chooseButton,SIGNAL(clicked()),this,SLOT(openFile()));
    QObject::connect(this->mthread,SIGNAL(openError(QString)),this,SLOT(on_openError(QString)));
    QObject::connect(this->mthread,SIGNAL(convertComplete(QString)),this,SLOT(updateList(QString)));
    QObject::connect(this->mthread,SIGNAL(workDone()),this,SLOT(on_workDone()));
    QObject::connect(ui->okButton,SIGNAL(clicked(bool)),this,SLOT(convertFile()));
    QObject::connect(ui->exitButton,SIGNAL(clicked()),this,SLOT(close()));
}
void Dialog::openFile()
{
    QFileDialog fileDialog(this);
    fileDialog.setFileMode(QFileDialog::ExistingFiles);
    fileDialog.setNameFilter("*.byte");

    if (fileDialog.exec())
    {
        souFileNames = fileDialog.selectedFiles();
    }

    QStringListModel *souFileModel = new QStringListModel;
    souFileModel->setStringList(souFileNames);
    fileNumber = souFileModel->rowCount();

    ui->sourceFileList->setModel(souFileModel);

    this->mthread->getName(this->souFileNames);    
}

void Dialog::convertFile()
{
    this->mthread->getChannel(this->channel_number);
    this->mthread->getProtocol(this->m_protocol);
    if (fileNumber < 1 or channel_number == 0)
    {
        if (fileNumber < 1)
            QMessageBox::warning(this,"警告","你必须先选择需要转换的文件",QMessageBox::Ok);
        else
            QMessageBox::warning(this,"警告","你必须先选择肌电信号的通道数",QMessageBox::Ok);
    }
    else
    {
        ui->label->setText("<h3><font color = yellow>转换中 > > ></font></h3>");
        this->mthread->start();
    }
}

void Dialog::on_openError(QString Info)
{
    QString sMessage = "Can't open" + Info;
    QMessageBox::warning(this,tr("Open File Wrong"),sMessage,QMessageBox::Ok);
}

void Dialog::updateList(QString name)
{
    this->desFileNames << name;
    QStringListModel *desFileModel = new QStringListModel;
    desFileModel->setStringList(desFileNames);
    ui->desFileList->setModel(desFileModel);
}

void Dialog::on_workDone()
{
    fileNumber = 0;
    ui->label->setText("<h3><font color = green>转换结束</font></h3>");
    this->mthread->wait(5);
    this->mthread->terminate();
    this->mthread->exit(0);
}

void Dialog::chooseCh(int i)
{
    if (i==0)
        channel_number = 0;
    else if (i == 1)
        channel_number = 64;
    else if (i == 2)
        channel_number = 128;
    else if (i == 3)
        channel_number = 8;
    else
        channel_number = 16;
}
void Dialog::chooseProtocol(int index)
{
    switch (index)
    {
        case 0:
            m_protocol = BYTE_2_MATTXT;
            break;
        case 1:
            m_protocol = BYTE_2_TXT;
            break;
        case 2:
            m_protocol = BYTE_2_MAT;
            break;
//        case 3:
//            m_protocol = TXT_2_MAT;
//            break;
    default:
        qDebug() << "Something wrong!";
        break;
    }
}
Dialog::~Dialog()
{
    delete ui;
}
