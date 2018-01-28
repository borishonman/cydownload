#include "helpers.h"
#include <QFile>
#include <QTextStream>
#include <stdlib.h>
#include "zlib/zlib.h"
#include "bzip2/bzlib.h"

#define CHUNK 1024

QString helpers::urlCombine(QString a, QString b)
{
    if (a.endsWith("/"))
        return a + b;
    else
        return a + "/" + b;
}



bool helpers::decompress(QString inFile, QString outFile)
{
    if (inFile.endsWith("bz2"))
        return decompress_bz2(inFile, outFile);
    else if (inFile.endsWith("gz"))
        return decompress_gz(inFile, outFile);
    else
        return false;
}

bool helpers::decompress_bz2(QString inFile, QString outFile)
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
    //delete iFile;

    //return success
    return true;
}

bool helpers::decompress_gz(QString inFile, QString outFile)
{
    int ret;
    unsigned have;
    z_stream strm;
    unsigned char in[CHUNK];
    unsigned char out[CHUNK];

    /* allocate inflate state */
    strm.zalloc = Z_NULL;
    strm.zfree = Z_NULL;
    strm.opaque = Z_NULL;
    strm.avail_in = 0;
    strm.next_in = Z_NULL;
    ret = inflateInit2(&strm, MAX_WBITS | 16);
    if (ret != Z_OK)
        return ret;

    //open the input file
    FILE *iFile = NULL;
    iFile = fopen(inFile.toStdString().c_str(), "rb");
    if (iFile == NULL)
        return false;

    //open the output file
    FILE *oFile = NULL;
    oFile = fopen(outFile.toStdString().c_str(), "wb");
    if (oFile == NULL)
        return false;


    /* decompress until deflate stream ends or end of file */
    do
    {
        strm.avail_in = fread(in, 1, CHUNK, iFile);
        if (ferror(iFile))
        {
            (void)inflateEnd(&strm);
            return Z_ERRNO;
        }
        if (strm.avail_in == 0)
            break;
        strm.next_in = in;
        /* run inflate() on input until output buffer not full */
        do
        {
            strm.avail_out = CHUNK;
            strm.next_out = out;
            ret = inflate(&strm, Z_NO_FLUSH);
            if (ret == Z_STREAM_ERROR)
                return false;
            switch (ret)
            {
            case Z_NEED_DICT:
                ret = Z_DATA_ERROR;     /* and fall through */
            case Z_DATA_ERROR:
            case Z_MEM_ERROR:
                (void)inflateEnd(&strm);
                return ret;
            }
            have = CHUNK - strm.avail_out;
            if (fwrite(out, 1, have, oFile) != have || ferror(oFile))
            {
                (void)inflateEnd(&strm);
                return Z_ERRNO;
            }
        } while (strm.avail_out == 0);
        /* done when inflate() says it's done */
    } while (ret != Z_STREAM_END);
    /* clean up and return */
    (void)inflateEnd(&strm);
    fclose(iFile);
    fclose(oFile);
    return ret == Z_STREAM_END;
}

Package helpers::getLatestPackage(PackageVersions packageVers)
{
    Package ret = packageVers.begin()->second;


    PackageVersions::iterator it;
    for (it=packageVers.begin();it != packageVers.end(); it++)
    {
        QString maxVer = ret.version;
        float ver = helpers::numberize(it->first);
        if (ver > helpers::numberize(maxVer))
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

int helpers::numberize (QString str)
{
    QString s = str;
    const char* cstr = str.toStdString().c_str();
    char* tmp;
    for (unsigned long i=0;i<strlen(cstr);i++)
    {
        if (cstr[i] != '0' && strtol(&(cstr[i]), &tmp, 10) == 0)
            s.remove(i, 1);
    }
    return atoi(s.toStdString().c_str());
}
