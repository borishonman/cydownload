#ifndef HELPERS_H
#define HELPERS_H

#include <QString>
#include <QStandardPaths>
#include <QDir>
#include "types.h"

#define DATA_DIR QStandardPaths::standardLocations(QStandardPaths::AppDataLocation).at(0)
#define REPO_URL_FILE "repos.ini"
#define REPO_URL_FILE_PATH QDir::cleanPath(DATA_DIR + QDir::separator() + REPO_URL_FILE)

class helpers
{
public:
    static QString urlCombine(QString a, QString b);
    static bool decompress(QString inFile, QString outFile);
    static Package getLatestPackage(PackageVersions packageVers);
    static QString bytesToHuman(qint64 sz);
    static void addRepoToFile(QString url, QString file);
    static void delRepoFromFile(QString url, QString file);
};

#endif // HELPERS_H
