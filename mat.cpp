#include "mat.h"
#include <QDebug>
Mat::Mat(QString name,QList<double> ls,long row,long col)
{
    std::string strname = name.toStdString();
    m_fileName = strname.c_str();

    x.mrows = row;
    x.ncols = col;
    x.imagf = 0;
    x.namelen = 5;
    x.type = 0000;

    for (long i=0;i<col;i++)
    {
        for (long j=0;j<row;j++)
        {
            m_deslist.append(ls.value(col*j + i)); //矩阵转置想清楚
        }
    }
}

Mat::~Mat()
{

}

bool Mat::Write()
{
    FILE *fp = NULL;
    fp = fopen(m_fileName,"wb");
    if (!fp)
        return false;
    else
    {
        const char *p_name = "data";

        fwrite(&x,sizeof(Fmatix),1,fp);

        fwrite(p_name,sizeof(char),x.namelen,fp);

        int mn = x.mrows*x.ncols;

//        Node<qint16> *p;
//        p = m_deslist.phead->next;
        for (int i=0;i<mn;i++)
        {
            fwrite(&m_deslist.at(i),sizeof(double),1,fp);
//            p = p->next;
        }
        fclose(fp);
        m_deslist.clear();
        return true;
    }
}
