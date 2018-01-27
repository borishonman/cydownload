#include "repodialog.h"
#include <algorithm>
#include <QFileDialog>
#include <QMessageBox>
#include "ui_repodialog.h"
#include "helpers.h"
#include "logger.h"
#include <stdlib.h>

RepoDialog::RepoDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::RepoDialog)
{
    ui->setupUi(this);
}

RepoDialog::~RepoDialog()
{
    delete ui;
    delete m_dler;
}

void RepoDialog::initialize(Repo repo, SectionList sections)
{
    m_repo = repo;
    m_sections = sections;
    m_dler = new Downloader((QMainWindow*)parent(), ui->progressBar);
    connect(m_dler, SIGNAL(downloadComplete(bool, QString)), this, SLOT(downloadComplete(bool, QString)));
    setWindowTitle(m_repo.origin);

    //populate the repo section
    ui->txt_repo_name->setText(repo.origin);
    ui->txt_repo_description->textCursor().document()->setPlainText(repo.description);

    //populate the sections
    SectionList::iterator it;
    for (it = sections.begin();it != sections.end(); it++)
    {
        QTreeWidgetItem *itm = new QTreeWidgetItem(QStringList(it->first));
        PackageList::iterator itt;
        for (itt = it->second.begin();itt != it->second.end();itt++)
        {
            int t = itt->second.size();
            Package latestPackage = helpers::getLatestPackage(itt->second);
            QTreeWidgetItem *itm2 = new QTreeWidgetItem(QStringList(latestPackage.name + " (" + latestPackage.version + ")"));
            itm->insertChild(0, itm2);
            m_packageMap[itm2] = itt->second;
        }
        ui->tree_sections->insertTopLevelItem(0, itm);
    }
}

void RepoDialog::on_tree_sections_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous)
{
    //if a section header was clicked, or the same item was clicked, do nothing
    if (m_packageMap.find(current) == m_packageMap.end())
    {
        ui->txt_pkg_name->setText("");
        ui->txt_pkg_maintainer->setText("");
        ui->txt_pkg_description->textCursor().document()->setPlainText("");
        ui->cmb_pkg_version->setCurrentIndex(-1);
        ui->btn_download->setText("Download .deb");
        ui->btn_download->setEnabled(false);
        disablePackageInfoPanel();
        return;
    }

    //get the clicked package
    PackageVersions clickedPackage = m_packageMap[current];

    //fill the versions combo box
    PackageVersions::iterator it;
    for (it=clickedPackage.begin();it != clickedPackage.end();it++)
    {
        ui->cmb_pkg_version->insertItem(0, it->second.version);
    }

    //get the latest package
    Package latestPackage = helpers::getLatestPackage(clickedPackage);

    //fill the rest of the package info
    ui->txt_pkg_name->setText(latestPackage.name);
    ui->txt_pkg_maintainer->setText(latestPackage.maintainer);
    ui->txt_pkg_description->textCursor().document()->setPlainText(latestPackage.description);
    ui->cmb_pkg_version->setCurrentText(latestPackage.version);
    ui->txt_pkg_filesize->setText(helpers::bytesToHuman(atol(latestPackage.size.toStdString().c_str())));

    //enable the download buttons
    enablePackageInfoPanel();

    if (latestPackage.tags.contains("cydia::commercial"))
    {
        ui->btn_download->setText("Cannot download commercial packages!");
        ui->btn_download->setEnabled(false);
    }
    else
    {
        ui->btn_download->setText("Download .deb");
        ui->btn_download->setEnabled(true);
    }
}

void RepoDialog::on_cmb_pkg_version_currentTextChanged(const QString &arg1)
{
    //if no package (or a section) was selected, do nothing
    if (m_packageMap.find(ui->tree_sections->currentItem()) == m_packageMap.end())
        return;

    //get the selected package
    PackageVersions clickedPackage = m_packageMap[ui->tree_sections->currentItem()];

    //get the selected package version
    PackageVersions::iterator it;
    for (it=clickedPackage.begin();it != clickedPackage.end();it++)
    {
        if (it->first == arg1)
        {
            //found it! update the display
            ui->txt_pkg_name->setText(it->second.name);
            ui->txt_pkg_maintainer->setText(it->second.maintainer);
            ui->txt_pkg_description->textCursor().document()->setPlainText(it->second.description);
            ui->txt_pkg_filesize->setText(helpers::bytesToHuman(atol(it->second.size.toStdString().c_str())));
        }
    }
}

void RepoDialog::enablePackageInfoPanel()
{
    ui->txt_pkg_name->setEnabled(true);
    ui->txt_pkg_maintainer->setEnabled(true);
    ui->txt_pkg_description->setEnabled(true);
    ui->btn_download->setEnabled(true);
}
void RepoDialog::disablePackageInfoPanel()
{
    ui->txt_pkg_name->setEnabled(false);
    ui->txt_pkg_maintainer->setEnabled(false);
    ui->txt_pkg_description->setEnabled(false);
    ui->btn_download->setEnabled(false);
}

void RepoDialog::on_btn_download_clicked()
{
    //ask for the directory to download to
    QFileDialog fd;
    fd.setFileMode(QFileDialog::Directory);
    fd.setOption(QFileDialog::ShowDirsOnly);
    fd.setWindowTitle("Select download folder");
    if (!fd.exec())
        return;
    QString dlDir = fd.selectedUrls().at(0).toLocalFile();

    //get the selected package
    PackageVersions clickedPackage = m_packageMap[ui->tree_sections->currentItem()];

    //get the selected package version
    PackageVersions::iterator it;
    QString pkgFile = "package.deb";
    QString url = "";
    for (it=clickedPackage.begin();it != clickedPackage.end();it++)
    {
        if (it->first == ui->cmb_pkg_version->currentText())
        {
            //found it! get the package filename
            pkgFile = it->second.packageid + +"_" + it->second.version + ".deb";
            url = helpers::urlCombine(m_repo.url, it->second.filename);
        }
    }

    //the file path to download to
    QString dlFile = QDir::cleanPath(dlDir + QDir::separator() + pkgFile);

    //start the download
    Logger::log("Downloading " + pkgFile);
    ui->btn_download->setText("downloading...");
    ui->tree_sections->setEnabled(false);
    m_dler->startDownload(url, dlFile);
}

void RepoDialog::downloadComplete(bool success, QString errMsg)
{
    QMessageBox mb;
    if (success)
    {
        mb.setText("Download complete!");
        Logger::log("Download complete");
    }
    else
    {
        mb.setText("Download failed! Please see log window");
        Logger::log("Download failed: " + errMsg);
    }
    mb.setStandardButtons(QMessageBox::Ok);
    mb.exec();
    ui->btn_download->setText("Download .deb");
    ui->tree_sections->setEnabled(true);
}

void RepoDialog::on_txt_search_textChanged(const QString &arg1)
{
    //clear the tree view
    ui->tree_sections->clear();

    //re-add everything that contains the search term
    SectionList::iterator it;
    for (it = m_sections.begin();it != m_sections.end(); it++)
    {
        QTreeWidgetItem *itm = new QTreeWidgetItem(QStringList(it->first));
        PackageList::iterator itt;
        for (itt = it->second.begin();itt != it->second.end();itt++)
        {
            Package latestPackage = helpers::getLatestPackage(itt->second);
            if (latestPackage.name.toLower().contains(arg1.toLower()) || arg1.size() == 0)
            {
                QTreeWidgetItem *itm2 = new QTreeWidgetItem(QStringList(latestPackage.name + " (" + latestPackage.version + ")"));
                itm->insertChild(0, itm2);
                m_packageMap[itm2] = itt->second;
            }
        }
        if (itm->childCount() > 0)
            ui->tree_sections->insertTopLevelItem(0, itm);
    }
}
