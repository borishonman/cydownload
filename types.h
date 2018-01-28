#ifndef TYPES_H
#define TYPES_H

#include <QString>
#include <map>

typedef struct _Repo
{
    QString url;
    QString origin;
    QString label;
    QString suite;
    QString version;
    QString codename;
    QString architectures;
    QString components;
    QString description;
} Repo;

typedef struct _Package
{
    QString packageid;
    QString version;
    QString section;
    QString maintainer;
    QString depends;
    QString architecture;
    QString filename;
    QString size;
    QString installedsize;
    QString md5sum;
    QString description;
    QString name;
    QString author;
    QString website;
    QString depiction;
    QString tags;
public:
    static _Package null()
    {
        _Package pkg;
        pkg.packageid = "";
        pkg.version = "";
        pkg.section = "";
        pkg.maintainer = "";
        pkg.depends = "";
        pkg.architecture = "";
        pkg.filename = "";
        pkg.size = "";
        pkg.installedsize = "";
        pkg.md5sum = "";
        pkg.description = "";
        pkg.name = "";
        pkg.author = "";
        pkg.website = "";
        pkg.depiction = "";
        return pkg;
    }
} Package;

struct strCmp {
    bool operator()(QString const &a, QString const &b) const {
        return a > b;
    }
};

typedef std::map<QString, Package> PackageVersions;
typedef std::map<QString, PackageVersions, strCmp> PackageList;
typedef std::map<QString, PackageList, strCmp> SectionList;

#endif // TYPES_H
