#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTemporaryDir>
#include <vector>
#include <map>
#include "dialogrepo.h"
#include "downloader.h"
#include "types.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

    typedef enum _Downloading {RELEASE=0, RELEASE_DEBIAN, PACKAGES, PACKAGES_DEBIAN, PACKAGE, NONE} Downloading;

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void dialogrepo_accepted();
    void downloadComplete(bool success);

    void on_btn_open_clicked();

private:
    void repoInfoDownloaded();
    Ui::MainWindow *ui;
    dialogrepo *m_dr;
    QTemporaryDir m_tmpDir;
    QString m_url;
    Downloader *m_dl;
    Downloading m_dling;
    Repo m_repo;
    std::vector<Package> m_packages;
    SectionList m_sections;
};

#endif // MAINWINDOW_H
