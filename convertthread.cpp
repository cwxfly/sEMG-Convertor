#include "convertthread.h"
#include "mat.h"
#include <QMessageBox>
#include <QtCore>
ConvertThread::ConvertThread(QObject *parent) :
    QThread(parent)
{

}
void ConvertThread::run()
{
    int fileNumber = mlist.length();
    for (int i=0;i<fileNumber;i++)
    {
        QString fileName = mlist.at(i);
        this->Convert(fileName);
    }
    emit workDone();
}

void ConvertThread::getName(QStringList names)
{
    mlist = names;
}

void ConvertThread::getChannel(int channel)
{
    mchannel = channel;
}

void ConvertThread::getProtocol(int a)
{
    protocal = a;
}
void ConvertThread::Convert(QString fileName)
{
    QFile sFile(fileName);
    if (!sFile.open(QIODevice::ReadOnly))
    {
        emit openError(fileName);
    }
    QDataStream in(&sFile);
    in.setVersion(QDataStream::Qt_5_0);
    in.setByteOrder( QDataStream::LittleEndian);
    // to txt & mat
    if (protocal == 0)
    {
        QString dFileName;
        QString dFileName2;
        QString tFileName;
        int sizeName;

        tFileName.append(sFile.fileName());
        sizeName = tFileName.size();
        tFileName.remove((sizeName-5),5);
        dFileName.append(tr("%1.%2").arg(tFileName).arg("txt"));
        dFileName2.append(tr("%1.%2").arg(tFileName).arg("mat"));
        QFile dFile ;
        dFile.setFileName(dFileName);

        if(!dFile.open(QIODevice::WriteOnly|QIODevice::Text))
        {
              emit openError(dFileName);
        }

        QTextStream out(&dFile);
        out.setIntegerBase(10);
        //64 & 128 通道设备解析
        if (mchannel == 64 || mchannel == 128)
        {
            QList<double> list;
            quint16 buffer=0;
            long row=0;
            double f_byte;
            while (!in.atEnd())
            {
                for (int i=0;i<mchannel;i++)
                {
                    in>>buffer;
//                    f_byte = (5000.0000/65536.0000)*buffer-2500.0000;
                    f_byte = double(buffer);
                    list.append(f_byte);
                    out<<f_byte<<" ";
                }
                row ++;
                out<<'\n';
             }
            sFile.close();
            dFile.close();
            emit convertComplete(dFileName);
            Mat matfile(dFileName2,list,row,mchannel);
            bool ok;
            ok = matfile.Write();
            if (ok)
                emit convertComplete(dFileName2);
            list.clear();
        }
        //何瑞星设备（新设备）数据解析
        else if (mchannel == 8)
        {
            QList<double> list;
            quint32 head =  0;
            qint16 buffer = 0;
            double f_byte;
            long row=0;

            while (!in.atEnd())
            {
                in >> head;
                if (head == 0x0055CCFF)
                {
                    for (int j=0;j<7;j++)
                    {
                        in >> buffer;
                        for (int i=0;i<mchannel;i++)
                        {
                            in >> buffer;
//                            f_byte = double(buffer*4000.0/32768.0);
                            f_byte = double(buffer);
                            list.append(f_byte);
                            out << f_byte << " ";
                        }
                    }
                    row ++;
                    out<<'\n';
                }
             }
            sFile.close();
            dFile.close();
            emit convertComplete(dFileName);
            Mat matfile(dFileName2,list,row,mchannel*7);
            bool ok;
            ok = matfile.Write();
            if (ok)
                emit convertComplete(dFileName2);
            list.clear();
        }
        //曹帅设备（老设备）数据解析
        else
        {
         in.setByteOrder( QDataStream::BigEndian);
         QList<double> list;

         quint32 head =  0;
         qint32 f_byte;
         long row = 0;
         double semg;
         double acc;
         quint8 data[3];
         quint8 signal[4];
         while (!in.atEnd())
         {
             in >> head;
             if (head == 0xFFAA5500)
             {
                 for (int i=0;i<2;i++)
                 {
                     in >> data[0]; //3bytes的随机数
                     in >> data[1];
                     in >> data[2];
                     for (int j=0;j<8;j++)
                     {
                        for (int m=0;m<3;m++)
                        {
                            in >> data[m];                            
                        }
                        f_byte = data[0]<<16 | data[1] << 8 | data[2];
                        if ((0x80 & data[0])) //处理符号位
                            f_byte = (0xff000000 | f_byte );
//                        semg = (f_byte*4500.0)/16777216.0;
                         semg = double(f_byte);
                        out << semg << " ";
                        list.append(semg);
                     }
                 }
                 for (int i=0;i<2;i++)
                 {
                     for (int j=0;j<4;j++)
                     {
                         in >> signal[j];
                     }

                     f_byte = ((((signal[0] & 0xc0)>>6)<<8) | signal[1]);// 加速度x轴
//                     acc = (f_byte * 4500.0)/1024;
                     acc = double(f_byte);
                     out << acc << " ";
                     list.append(acc);

                     f_byte = ((((signal[0] & 0x30)>>4)<<8) | signal[2]);// 加速度y轴
//                     acc = (f_byte * 4500.0)/1024;
                     acc = double(f_byte);
                     out << acc << " ";
                     list.append(acc);

                     f_byte = ((((signal[0] & 0x0c)>>2)<<8) | signal[3]);// 加速度z轴
//                     acc = (f_byte * 4500.0)/1024;
                     acc = double(f_byte);
                     out << acc << " ";
                     list.append(acc);
                 }
                 out << '\n';
                 row ++;
             }
         }
         sFile.close();
         dFile.close();
         emit convertComplete(dFileName);

         Mat matfile(dFileName2,list,row,22);
         bool ok;
         ok = matfile.Write();
         if (ok)
             emit convertComplete(dFileName2);
         list.clear();
        }

    }
    // to mat
    else if (protocal == 2)
    {
        QString dFileName2;
        QString tFileName;
        int sizeName;

        tFileName.append(sFile.fileName());
        sizeName = tFileName.size();
        tFileName.remove((sizeName-5),5);
        dFileName2.append(tr("%1.%2").arg(tFileName).arg("mat"));
        if (mchannel == 64 || mchannel == 128)
        {
            QList<double> list;
            quint16 buffer=0;
            long row=0;
            double f_byte;
            while (!in.atEnd())
            {
                for (int i=0;i<mchannel;i++)
                {
                    in>>buffer;
//                    f_byte = (5000.0000/65536.0000)*buffer-2500.0000;
                    f_byte = double(buffer);
                    list.append(f_byte);
                }
                row ++;
             }
            sFile.close();
            Mat matfile(dFileName2,list,row,mchannel);
            bool ok;
            ok = matfile.Write();
            if (ok)
                emit convertComplete(dFileName2);
            list.clear();
        }
        else if (mchannel == 8)
        {
            QList<double> list;
            quint32 head =  0x00000000;
            qint16 buffer = 0;
            double f_byte;
            long row=0;

            while (!in.atEnd())
            {
                in >> head;
                if (head == 0x0055CCFF)
                {
                    for (int j=0;j<7;j++)
                    {
                        in >> buffer;
                        for (int i=0;i<mchannel;i++)
                        {
                            in >> buffer;
//                            f_byte = (buffer / 32768.0)*4000.0;
                            f_byte= double(buffer);
                            list.append(f_byte);
                        }
                    }
                    row ++;
                }
             }
            sFile.close();
            Mat matfile(dFileName2,list,row,mchannel*7);
            bool ok;
            ok = matfile.Write();
            if (ok)
                emit convertComplete(dFileName2);
            list.clear();
        }
        else
        {
            in.setByteOrder( QDataStream::BigEndian);
            QList<double> list;

            long row = 0;
            quint32 head =  0;
            qint32 f_byte;
            double semg;
            double acc;
            quint8 data[3];
            quint8 signal[4];
            while (!in.atEnd())
            {
                in >> head;
                if (head == 0xFFAA5500)
                {
                    for (int i=0;i<2;i++)
                    {
                        in >> data[0]; //3bytes的随机数
                        in >> data[1];
                        in >> data[2];
                        for (int j=0;j<8;j++)
                        {
                           for (int m=0;m<3;m++)
                           {
                               in >> data[m];
                           }
                           f_byte = data[0]<<16 | data[1] << 8 | data[2];
                           if ((0x80 & data[0])) //处理符号位
                               f_byte = (0xff000000 | f_byte );
//                           semg = (f_byte*4500.0)/16777216.0;
                           semg = double(f_byte);
                           list.append(semg);
                        }
                    }
                    for (int i=0;i<2;i++)
                    {
                        for (int j=0;j<4;j++)
                        {
                            in >> signal[j];
                        }

                        f_byte = ((((signal[0] & 0xc0)>>6)<<8) | signal[1]);// 加速度x轴
//                        acc = (f_byte * 4500.0)/1024;
                        acc = double(f_byte);
                        list.append(acc);

                        f_byte = ((((signal[0] & 0x30)>>4)<<8) | signal[2]);// 加速度y轴
//                        acc = (f_byte * 4500.0)/1024;
                        acc = double(f_byte);
                        list.append(acc);

                        f_byte = ((((signal[0] & 0x0c)>>2)<<8) | signal[3]);// 加速度z轴
//                        acc = (f_byte * 4500.0)/1024;
                        acc = double(f_byte);
                        list.append(acc);
                    }
                    row ++;
                }
            }
            sFile.close();

            Mat matfile(dFileName2,list,row,22);
            bool ok;
            ok = matfile.Write();
            if (ok)
                emit convertComplete(dFileName2);
            list.clear();
        }

    }
    // to txt
    else
    {
        QString dFileName;
        QString tFileName;
        int sizeName;

        tFileName.append(sFile.fileName());
        sizeName = tFileName.size();
        tFileName.remove((sizeName-5),5);
        dFileName.append(tr("%1.%2").arg(tFileName).arg("txt"));
        QFile dFile ;
        dFile.setFileName(dFileName);

        if(!dFile.open(QIODevice::WriteOnly|QIODevice::Text))
        {
              emit openError(dFileName);
        }

        QTextStream out(&dFile);
        out.setIntegerBase(10);
        //64通道或128通道解析
        if (mchannel == 64 || mchannel == 128)
        {
            quint16 buffer=0;
            double f_byte;
            while (!in.atEnd())
            {
                for (int i=0;i<mchannel;i++)
                {
                    in>>buffer;
//                    f_byte = qRound((5000.0000/65536.0000)*buffer-2500.0000);
                    f_byte = double(buffer);
                    out<<f_byte<<" ";
                }
                out<<'\n';
             }
            sFile.close();
            dFile.close();
            emit convertComplete(dFileName);
        }
        //56通道解析 （何瑞星开发的设备）
        else if (mchannel == 8)
        {
            quint32 head =  0x00000000;
            qint16 buffer = 0;
            double f_byte;

            while (!in.atEnd())
            {
                in >> head;
                if (head == 0x0055CCFF)
                {
                    for (int j=0;j<7;j++)
                    {
                        in >> buffer;
                        for (int i=0;i<mchannel;i++)
                        {
                            in >> buffer;
//                            f_byte = qRound((buffer / 32768.0)*4000.0);
                            f_byte = double(buffer);
                            out << f_byte << " ";
                        }
                    }
                    out<<'\n';
                }
             }
            sFile.close();
            dFile.close();
            emit convertComplete(dFileName);
        }
        //扩展设备
        else
        {
            in.setByteOrder( QDataStream::BigEndian);

            quint32 head =  0;
            qint32 f_byte;
            double semg;
            double acc;
            quint8 data[3];
            quint8 signal[4];
            while (!in.atEnd())
            {
                in >> head;
                if (head == 0xFFAA5500)
                {
                    for (int i=0;i<2;i++)
                    {
                        in >> data[0]; //3bytes的随机数
                        in >> data[1];
                        in >> data[2];
                        for (int j=0;j<8;j++)
                        {
                           for (int m=0;m<3;m++)
                           {
                               in >> data[m];
                           }
                           f_byte = data[0]<<16 | data[1] << 8 | data[2];
                           if ((0x80 & data[0])) //处理符号位
                               f_byte = (0xff000000 | f_byte );
//                           semg = (f_byte*4500.0)/16777216.0;
                           semg = double(f_byte);
                           out << semg << " ";
                        }
                    }
                    for (int i=0;i<2;i++)
                    {
                        for (int j=0;j<4;j++)
                        {
                            in >> signal[j];
                        }

                        f_byte = ((((signal[0] & 0xc0)>>6)<<8) | signal[1]);// 加速度x轴
//                        acc = (f_byte * 4500.0)/1024;
                        acc = double(f_byte);
                        out << acc << " ";

                        f_byte = ((((signal[0] & 0x30)>>4)<<8) | signal[2]);// 加速度y轴
//                        acc = (f_byte * 4500.0)/1024;
                        acc = double(f_byte);
                        out << acc << " ";

                        f_byte = ((((signal[0] & 0x0c)>>2)<<8) | signal[3]);// 加速度z轴
//                        acc = (f_byte * 4500.0)/1024;
                        acc = double(f_byte);
                        out << acc << " ";
                    }
                    out << '\n';
                }
            }
            sFile.close();
            dFile.close();
            emit convertComplete(dFileName);

        }

    }
}
