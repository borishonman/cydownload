#include "helpers.h"
#include <QFile>
#include <QTextStream>
#include <stdlib.h>
#include "bzip2/bzlib.h"

QString helpers::urlCombine(QString a, QString b)
{
    if (a.endsWith("/"))
        return a + b;
    else
        return a + "/" + b;
}

bool helpers::decompress(QString inFile, QString outFile)
{
    //open the output file for writing
    QFile *oFile = new QFile(outFile);
    if (!oFile->open(QIODevice::WriteOnly))
        return false;

    //open the input file for reading
    QFile *iFile = new QFile(inFile);
    if (!iFile->open(QIODevice::ReadOnly))
    {
        delete oFile;
        return false;
    }

    //open the input file
    BZFILE *bzIn = BZ2_bzdopen(iFile->handle(), "rb");
    if (bzIn == NULL)
    {
        delete oFile;
        delete iFile;
        return false;
    }

    //decompress the file
    int len;
    char buff[0x1000];
    while ((len = BZ2_bzread(bzIn, buff, 0x1000)) > 0)
    {
        oFile->write(buff, len);
    }

    //close file handles
    BZ2_bzclose(bzIn);
    oFile->flush();
    oFile->close();

    delete oFile;
    delete iFile;

    //return success
    return true;
}

Package helpers::getLatestPackage(PackageVersions packageVers)
{
    Package ret = packageVers.begin()->second;


    PackageVersions::iterator it;
    for (it=packageVers.begin();it != packageVers.end(); it++)
    {
        QString curVer = it->first;
        QString maxVer = ret.version;
        curVer.remove(".");
        maxVer.remove(".");
        float ver = atof(curVer.toStdString().c_str());
        if (ver > atof(maxVer.toStdString().c_str()))
            ret = it->second;
    }

    return ret;
}

QString helpers::bytesToHuman(qint64 sz)
{
    //get KB
    float kb = (float)sz / 1024.0f;
    if (kb < 1024)
        return QString::number(kb, 'f', 2) + " KB";

    //get MB
    float mb = (float)kb / 1024.0f;
    if (mb < 1024)
        return QString::number(mb, 'f', 2) + " MB";

    return QString::number(mb / 1024.0f, 'f', 2) + " GB";
}

void helpers::addRepoToFile(QString url, QString file)
{
    QFile f(file);

    //open the file for writing+append
    if (!f.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Append))
        return;

    //write to the file
    QTextStream stream( &f );
    stream << url << endl;

    //close the file
    stream.flush();
    f.flush();
    f.close();
}
void helpers::delRepoFromFile(QString url, QString file)
{
    QFile f(file);
    std::vector<QString> lines;

    //open the file for reading
    if (!f.open(QIODevice::ReadWrite | QIODevice::Text))
        return;

    //read the text from the file
    QTextStream in(&f);
    while(!in.atEnd())
    {
        QString l = in.readLine();
        if (l != url)
            lines.push_back(l);
    }
    in.flush();

    //write the text back to the file
    f.resize(0);
    QTextStream out( &f );
    for (unsigned long i=0;i<lines.size();i++)
    {
        out << lines[i] << endl;
    }
    out.flush();

    //close the file
    f.close();
}
