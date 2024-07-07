#ifndef FILECLASS_H
#define FILECLASS_H
#include <QFile>
#include <QString>
#include <QDebug>
#include "memory.h"

extern Memory memory;

class FileClass
{
public:
    FileClass();
    FileClass(QString);
    void WriteToFile(const Memory&, uint16_t, uint16_t);
    bool ReadFromFile(uint16_t);
    QFile file;
};

QVector<QString> ReadInputFile(const QString &filename) ;

#endif // FILECLASS_H
