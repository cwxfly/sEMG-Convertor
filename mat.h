#ifndef MAT_H
#define MAT_H
#include "linklist.h"
#include "linklist.cpp"
#include "QString"
#include <QList>
typedef struct
{
        long type;
        long mrows;
        long ncols;
        long imagf;
        long namelen;
}Fmatix;
class Mat
{
public:
    Mat(QString name,QList<double> ls,long row,long col);
    ~Mat();
    bool Write();
private:
    const char *m_fileName;
    QList<double> m_deslist;
    Fmatix x;
};

#endif // MAT_H
