#ifndef CONVERTTHREAD_H
#define CONVERTTHREAD_H

#include <QThread>
#include <QtCore>
#include <QStringList>
#include <QString>

#define BYTE_2_MATTXT 0
#define BYTE_2_MAT 2
#define BYTE_2_TXT 1
//#define TXT_2_MAT 3

class ConvertThread : public QThread
{
    Q_OBJECT
public:
    explicit ConvertThread(QObject *parent = 0);
    void run();
    void getName(QStringList names);
    void getChannel(int channel);
    void getProtocol(int a);
    void Convert(QString fileName);
//    double FormatBit(quint8 *a,int ByteNum);
//    void Convert(QString fileName,QString prl);
signals:
    QString convertComplete(QString);
    QString openError(QString);
    void workDone();
public slots:
private:
    QStringList mlist;
    int mchannel;
    int protocal;


};

#endif // CONVERTTHREAD_H
