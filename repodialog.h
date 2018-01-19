#ifndef REPODIALOG_H
#define REPODIALOG_H

#include <QDialog>
#include <QTreeWidgetItem>
#include "types.h"
#include "downloader.h"
#include <map>
#include <vector>

namespace Ui {
class RepoDialog;
}

class RepoDialog : public QDialog
{
    Q_OBJECT

public:
    explicit RepoDialog(QWidget *parent = 0);
    ~RepoDialog();

    void initialize(Repo repo, SectionList sections);

private slots:
    void on_tree_sections_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous);

    void on_cmb_pkg_version_currentTextChanged(const QString &arg1);

    void on_btn_download_clicked();

    void downloadComplete(bool success);

    void on_txt_search_textChanged(const QString &arg1);

private:
    Ui::RepoDialog *ui;
    Repo m_repo;
    Downloader* m_dler;
    SectionList m_sections;
    std::map<QTreeWidgetItem*, PackageVersions> m_packageMap;

    void enablePackageInfoPanel();
    void disablePackageInfoPanel();
};

#endif // REPODIALOG_H
