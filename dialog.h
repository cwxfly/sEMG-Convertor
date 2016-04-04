#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>
#include "convertthread.h"
#include "mat.h"

namespace Ui {
class Dialog;
}

class Dialog : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog(QWidget *parent = 0);
    ~Dialog();
private:
    Ui::Dialog *ui;
    int fileNumber;
    int channel_number;
    QStringList desFileNames;
    QStringList souFileNames;
    ConvertThread *mthread;
    int m_protocol;
signals:

private slots:
    void chooseCh(int);
    void chooseProtocol(int);
    void openFile();
    void convertFile();
    void on_openError(QString Info);
    void updateList(QString name);
    void on_workDone();
};

#endif // DIALOG_H
