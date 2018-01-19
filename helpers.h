#ifndef HELPERS_H
#define HELPERS_H

#include <QString>
#include "types.h"

class helpers
{
public:
    static QString urlCombine(QString a, QString b);
    static bool decompress(QString inFile, QString outFile);
    static Package getLatestPackage(PackageVersions packageVers);
    static QString bytesToHuman(qint64 sz);
};

#endif // HELPERS_H
