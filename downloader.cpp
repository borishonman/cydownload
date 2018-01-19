#include "downloader.h"
#include "logger.h"

Downloader::Downloader(QMainWindow* mw, QProgressBar* progBar)
{
    m_mainWindow = mw;
    m_progBar = progBar;
    m_file = NULL;
    m_reply = NULL;
    m_manager = new QNetworkAccessManager(m_mainWindow);
}

Downloader::~Downloader()
{
    delete m_manager;
}

void Downloader::startDownload(QString url, QString filePath)
{
    //create the file and open it for reading
    m_file = new QFile(filePath);
    if (!m_file->open(QIODevice::WriteOnly))
    {
        delete m_file;
        m_file = 0;
        return;
    }

    //create a network request to download stuff
    m_reply = m_manager->get(QNetworkRequest(url));

    //connect the signals to monitor the download
    m_mainWindow->connect(m_reply, SIGNAL(readyRead()), this, SLOT(httpReadyRead()));
    m_mainWindow->connect(m_reply, SIGNAL(downloadProgress(qint64,qint64)), this, SLOT(updateDownloadProgress(qint64,qint64)));
    m_mainWindow->connect(m_reply, SIGNAL(finished()), this, SLOT(httpDownloadFinished()));
}

void Downloader::httpReadyRead()
{
    if (m_file)
            m_file->write(m_reply->readAll());
}

void Downloader::updateDownloadProgress(qint64 bytesRead, qint64 totalBytes)
{
    if (totalBytes < 1)
    { //unable to determine file size, so display a busy indicator
        Logger::log("could not find size");
        m_progBar->setMinimum(0);
        m_progBar->setMaximum(0);
        return;
    }

    //set the progress bar value
    m_progBar->setMaximum(100);
    m_progBar->setValue(((float)bytesRead/(float)totalBytes)*100.0f);
}

void Downloader::httpDownloadFinished()
{
    //close the file
    m_progBar->setValue(0);
    m_file->flush();
    m_file->close();

    // get redirection url
    QVariant redirectionTarget = m_reply->attribute(QNetworkRequest::RedirectionTargetAttribute);
    if (!redirectionTarget.isNull())
    {
        QUrl newUrl = redirectionTarget.toUrl();
        m_reply->deleteLater();
        m_file->open(QIODevice::WriteOnly);
        m_file->resize(0);
        //create a network request to download stuff
        m_manager = new QNetworkAccessManager();
        m_reply = m_manager->get(QNetworkRequest(newUrl));
        return;
    }

    bool success = true;
    if (m_reply->error())
    { //something bad happened
        success = false;
        m_file->remove();
        Logger::log("Error: " + m_reply->errorString());
    }

    m_reply->deleteLater();
    m_reply = 0;
    delete m_file;
    m_file = 0;

    downloadComplete(success);
}
