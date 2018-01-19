#ifndef DOWNLOADER_H
#define DOWNLOADER_H

#include <QProgressBar>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QFile>
#include <QMainWindow>
#include <QObject>

class Downloader : public QObject
{
    Q_OBJECT
public:
    explicit Downloader(QMainWindow* mw, QProgressBar* progBar);
    ~Downloader();
    void startDownload(QString url, QString filePath);
signals:
    void downloadComplete(bool success);
private slots:
    void httpReadyRead();
    void updateDownloadProgress(qint64 bytesRead, qint64 totalBytes);
    void httpDownloadFinished();
private:
    QMainWindow* m_mainWindow;
    QProgressBar* m_progBar;
    QNetworkAccessManager* m_manager;
    QNetworkReply* m_reply;
    QFile* m_file;
};

#endif // DOWNLOADER_H
