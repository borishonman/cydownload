#include <QMessageBox>
#include <QStandardPaths>
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "helpers.h"
#include "logger.h"
#include "repodialog.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //initialize the logger
    Logger::initialize(ui->txt_log);

    //initialize the downloader
    m_dl = new Downloader(this, ui->progressBar);
    connect(m_dl, SIGNAL(downloadComplete(bool,QString)), this, SLOT(downloadComplete(bool,QString)));

    //initialize the download tracker
    m_dling = NONE;

    //make sure the temp dir was created
    if (!m_tmpDir.isValid())
    {
        QMessageBox b;
        b.setText("ERROR: Could not create temporary files directory");
        b.exec();
        exit(1);
    }

    //get the path to the data directory, create it if not exists
    m_dataDir = DATA_DIR;
    if (!QDir(m_dataDir).exists())
        QDir().mkdir(m_dataDir);

    //read the saved repo URLs
    QFile f(REPO_URL_FILE_PATH);
    if (f.exists() && f.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        ui->cmb_repo_urls->insertItem(0, "Select a Repository");
        while (!f.atEnd())
        {
            QString curUrl = f.readLine().trimmed();
            ui->cmb_repo_urls->insertItem(1, curUrl);
        }
        f.close();
        ui->cmb_repo_urls->setCurrentIndex(0);
    }
}

MainWindow::~MainWindow()
{
    delete ui;
    delete m_dl;
}

void MainWindow::downloadComplete(bool success, QString errMsg)
{
    switch (m_dling)
    {
    case RELEASE:
        if (!success)
        {
            m_dl->startDownload(helpers::urlCombine(m_url, "dists/stable/main/binary-iphoneos-arm/Release"), QDir::cleanPath(m_tmpDir.path() + QDir::separator() + "Release"));
            m_dling = RELEASE_DEBIAN;
            break;
        }
        else
            Logger::log("Done");
        //start downloading the package list
        Logger::log("Getting package list...");
        m_dling = PACKAGES;
        m_dl->startDownload(helpers::urlCombine(m_url, "Packages.bz2"), QDir::cleanPath(m_tmpDir.path() + QDir::separator() + "Packages.bz2"));
        break;
    case RELEASE_DEBIAN:
        if (!success)
            Logger::log("No Release file could be found, repo information will be blank");
        else
            Logger::log("Done");
        //start downloading the package list
        Logger::log("Getting package list...");
        m_dling = PACKAGES;
        m_dl->startDownload(helpers::urlCombine(m_url, "Packages.bz2"), QDir::cleanPath(m_tmpDir.path() + QDir::separator() + "Packages.bz2"));
        break;
    case PACKAGES:
        if (!success)
        {
            m_dling = PACKAGES_DEBIAN;
            m_dl->startDownload(helpers::urlCombine(m_url, "dists/stable/main/binary-iphoneos-arm/Packages.bz2"), QDir::cleanPath(m_tmpDir.path() + QDir::separator() + "Packages.bz2"));
            break;
        }
        //extract the Packages list
        if (!helpers::decompress(QDir::cleanPath(m_tmpDir.path() + QDir::separator() + "Packages.bz2"), QDir::cleanPath(m_tmpDir.path() + QDir::separator() + "Packages")))
        {
            Logger::log("Error extracting Packages.bz2");
            break;
        }
        Logger::log("Done");
        repoInfoDownloaded();
        m_dling = PACKAGE;
        break;
    case PACKAGES_DEBIAN:
        if (!success)
        {
            Logger::log(errMsg);
            Logger::log("Not a valid Cydia repository!");
            m_dling = NONE;
            break;
        }
        //extract the Packages list
        if (!helpers::decompress(QDir::cleanPath(m_tmpDir.path() + QDir::separator() + "Packages.bz2"), QDir::cleanPath(m_tmpDir.path() + QDir::separator() + "Packages")))
        {
            Logger::log("Error extracting Packages.bz2");
            break;
        }
        Logger::log("Done");
        repoInfoDownloaded();
        m_dling = PACKAGE;
        break;
    case PACKAGE:
        break;
    case NONE:
        break;
    }
}

void MainWindow::repoInfoDownloaded()
{
    //we have everything we need to display the repo information and packages list
    QString releaseFile = QDir::cleanPath(m_tmpDir.path() + QDir::separator() + "Release");
    QString packagesFile = QDir::cleanPath(m_tmpDir.path() + QDir::separator() + "Packages");

    //wipe any old release data
    m_repo.origin = "";
    m_repo.label = "";
    m_repo.suite = "";
    m_repo.version = "";
    m_repo.codename = "";
    m_repo.architectures = "";
    m_repo.components = "";
    m_repo.description = "";

    //parse the Release file, if it exists
    if (QFile::exists(releaseFile))
    {
        //open it
        QFile *rFile = new QFile(releaseFile);
        if (!rFile->open(QIODevice::ReadOnly | QIODevice::Text))
        {
            Logger::log("Failed opening Release file");
            return;
        }

        //read each line of the file
        while (!rFile->atEnd())
        {
            QString line = rFile->readLine();
            QStringList list = line.split(":");

            if (list.at(0).trimmed().toLower() == "origin")
                m_repo.origin = list.at(1).trimmed();
            else if (list.at(0).trimmed().toLower() == "label")
                m_repo.label = list.at(1).trimmed();
            else if (list.at(0).trimmed().toLower() == "suite")
                m_repo.suite = list.at(1).trimmed();
            else if (list.at(0).trimmed().toLower() == "version")
                m_repo.version = list.at(1).trimmed();
            else if (list.at(0).trimmed().toLower() == "codename")
                m_repo.codename = list.at(1).trimmed();
            else if (list.at(0).trimmed().toLower() == "architectures")
                m_repo.architectures = list.at(1).trimmed();
            else if (list.at(0).trimmed().toLower() == "components")
                m_repo.components = list.at(1).trimmed();
            else if (list.at(0).trimmed().toLower() == "description")
                m_repo.description = list.at(1).trimmed();
        }

        //close it
        rFile->close();
        delete rFile;
    }
    else
    {
        m_repo.origin = m_url;
        m_repo.description = "";
    }
    m_repo.url = m_url;

    //open the Packages file
    QFile *pFile = new QFile(packagesFile);
    if (!pFile->open(QIODevice::ReadOnly | QIODevice::Text))
    {
        Logger::log("Failed opening Packages file");
        return;
    }

    Package pkg = Package::null();
    //parse the Packages file
    while (!pFile->atEnd())
    {
        QString line = pFile->readLine();

        //check for new package
        if (line.startsWith("\n"))
        {
            m_packages.push_back(pkg);
            pkg = Package::null();
            continue;
        }

        QRegExp rx("^(.*): (.*)");
        rx.indexIn(line);

        if (rx.cap(1).trimmed().toLower() == "package")
            pkg.packageid = rx.cap(2).trimmed();
        else if (rx.cap(1).trimmed().toLower() == "version")
            pkg.version = rx.cap(2).trimmed();
        else if (rx.cap(1).trimmed().toLower() == "section")
            pkg.section = rx.cap(2).trimmed();
        else if (rx.cap(1).trimmed().toLower() == "maintainer")
            pkg.maintainer = rx.cap(2).trimmed();
        else if (rx.cap(1).trimmed().toLower() == "depends")
            pkg.depends = rx.cap(2).trimmed();
        else if (rx.cap(1).trimmed().toLower() == "architecture")
            pkg.architecture = rx.cap(2).trimmed();
        else if (rx.cap(1).trimmed().toLower() == "filename")
            pkg.filename = rx.cap(2).trimmed();
        else if (rx.cap(1).trimmed().toLower() == "size")
            pkg.size = rx.cap(2).trimmed();
        else if (rx.cap(1).trimmed().toLower() == "installed-size")
            pkg.installedsize = rx.cap(2).trimmed();
        else if (rx.cap(1).trimmed().toLower() == "md5sum")
            pkg.md5sum = rx.cap(2).trimmed();
        else if (rx.cap(1).trimmed().toLower() == "description")
            pkg.description = rx.cap(2).trimmed();
        else if (rx.cap(1).trimmed().toLower() == "name")
            pkg.name = rx.cap(2).trimmed();
        else if (rx.cap(1).trimmed().toLower() == "author")
            pkg.author = rx.cap(2).trimmed();
        else if (rx.cap(1).trimmed().toLower() == "website")
            pkg.website = rx.cap(2).trimmed();
        else if (rx.cap(1).trimmed().toLower() == "depiction")
            pkg.depiction = rx.cap(2).trimmed();
        else if (rx.cap(1).trimmed().toLower() == "tag")
            pkg.tags = rx.cap(2).trimmed();
    }

    //and close it
    pFile->close();
    delete pFile;

    //now split the packages into sections
    m_sections.empty();
    for (unsigned long i=0;i<m_packages.size();i++)
    {
        QString secName = m_packages[i].section;
        if (m_sections[secName].find(m_packages[i].name) != m_sections[secName].end())
        { //package was already added
            m_sections[secName][m_packages[i].name][m_packages[i].version] = m_packages[i];
        }
        else
        { //package was not already added, need to create a PackageVersions
            PackageVersions pkgVers;
            pkgVers[m_packages[i].version] = m_packages[i];
            m_sections[secName][m_packages[i].name] = pkgVers;
        }
    }

    //and empty the old package list
    m_packages.empty();

    //now we can open a new dialog with the repo contents
    RepoDialog *rd = new RepoDialog();
    rd->initialize(m_repo, m_sections);
    rd->exec();
    delete rd;
}

void MainWindow::on_btn_open_repo_clicked()
{
    QString networkUrl, fileUrl;

    //get the URL of the repo to connect to
    m_url = ui->cmb_repo_urls->currentText();

    /**Download Release**/
        networkUrl = helpers::urlCombine(m_url, "Release");
        fileUrl = QDir::cleanPath(m_tmpDir.path() + QDir::separator() + "Release");

        //start the download
        Logger::log("Getting repo information...");
        m_dling = RELEASE;
        m_dl->startDownload(networkUrl, fileUrl);
}

void MainWindow::on_btn_new_repo_clicked()
{
    m_dr = new dialogrepo();
    m_dr->exec();

    //add to the combo box
    ui->cmb_repo_urls->insertItem(ui->cmb_repo_urls->count(), m_dr->getText());
    ui->cmb_repo_urls->setCurrentIndex(ui->cmb_repo_urls->count()-1);
    ui->btn_open_repo->setEnabled(true);
    ui->btn_del_repo->setEnabled(true);

    //write it to the file
    helpers::addRepoToFile(m_dr->getText(), REPO_URL_FILE_PATH);

    delete m_dr;
}

void MainWindow::on_cmb_repo_urls_currentIndexChanged(int index)
{
    if (index == 0 || index == -1)
    {
        ui->btn_open_repo->setEnabled(false);
        ui->btn_del_repo->setEnabled(false);
        return;
    }

    ui->btn_open_repo->setEnabled(true);
    ui->btn_del_repo->setEnabled(true);
}

void MainWindow::on_btn_del_repo_clicked()
{
    QString url = ui->cmb_repo_urls->currentText();

    //remove the entry from the combo box
    ui->cmb_repo_urls->removeItem(ui->cmb_repo_urls->currentIndex());
    ui->cmb_repo_urls->setCurrentIndex(0);

    //remove it from the file too
    helpers::delRepoFromFile(url, REPO_URL_FILE_PATH);
}
