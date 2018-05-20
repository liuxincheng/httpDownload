#include "downloadmanager.h"
#include <QFile>
#include <QDebug>
#include <QFileInfo>
#include <QDir>

#define DOWNLOAD_FILE_SUFFIX	"_tmp"

DownLoadManager::DownLoadManager(QObject *parent)
    : QObject(parent)
    , m_networkManager(NULL)
    , m_url(QUrl(""))
    , m_fileName("")
    , m_isSupportBreakPoint(false)
    , m_bytesReceived(0)
    , m_bytesTotal(0)
    , m_bytesCurrentReceived(0)
    , m_isStop(true)
{
    m_networkManager = new QNetworkAccessManager(this);
}

DownLoadManager::~DownLoadManager()
{}

// 设置是否支持断点续传;
void DownLoadManager::setDownInto(bool isSupportBreakPoint)
{
    m_isSupportBreakPoint = isSupportBreakPoint;
}

// 获取当前下载链接;
QString DownLoadManager::getDownloadUrl()
{
    return m_url.toString();
}

// 开始下载文件;
void DownLoadManager::downloadFile(QString url , QString fileName)
{
    // 防止多次点击开始下载按钮，进行多次下载，只有在停止标志变量为true时才进行下载;
    if (m_isStop)
    {
        m_isStop = false;
        m_url = QUrl::fromUserInput(url);

        // 从url 中获取文件名，但不是都有效;
        // QFileInfo fileInfo(m_url.path());
        // QString fileName = fileInfo.fileName();

        // 将当前文件名设置为临时文件名，下载完成时修改回来;
        m_fileName = fileName + DOWNLOAD_FILE_SUFFIX;

        // 如果当前下载的字节数为0那么说明未下载过或者重新下载
        // 则需要检测本地是否存在之前下载的临时文件，如果有则删除
        if (m_bytesCurrentReceived <= 0)
        {
            removeFile(m_fileName);
        }

        QNetworkRequest request;
        request.setUrl(m_url);

        // 如果支持断点续传，则设置请求头信息;
        if (m_isSupportBreakPoint)
        {
            QString strRange = QString("bytes=%1-").arg(m_bytesCurrentReceived);
            request.setRawHeader("Range", strRange.toLatin1());
        }

        // 请求下载;
        m_reply = m_networkManager->get(request);

        connect(m_reply, SIGNAL(downloadProgress(qint64, qint64)), this, SLOT(onDownloadProgress(qint64, qint64)));
        connect(m_reply, SIGNAL(readyRead()), this, SLOT(onReadyRead()));
        connect(m_reply, SIGNAL(finished()), this, SLOT(onFinished()));
        connect(m_reply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(onError(QNetworkReply::NetworkError)));
    }
}

// 下载进度信息;
void DownLoadManager::onDownloadProgress(qint64 bytesReceived, qint64 bytesTotal)
{
    if (!m_isStop)
    {
        m_bytesReceived = bytesReceived;
        if(m_bytesTotal == 0) m_bytesTotal = bytesTotal;
        // 更新下载进度;(加上 m_bytesCurrentReceived 是为了断点续传时之前下载的字节)
        emit signalDownloadProcess(m_bytesCurrentReceived, m_bytesTotal);
    }
}

// 获取下载内容，保存到文件中;
void DownLoadManager::onReadyRead()
{
    if (!m_isStop)
    {
        QFile file(m_fileName);
        if (file.open(QIODevice::WriteOnly | QIODevice::Append))
        {
            file.write(m_reply->readAll());
            m_bytesCurrentReceived = file.size();
        }
        file.close();
    }
}

// 下载完成;
void DownLoadManager::onFinished()
{
    m_isStop = true;
    QVariant statusCode = m_reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);

    if (m_reply->error() == QNetworkReply::NoError)
    {
        // 重命名临时文件;
        QFileInfo fileInfo(m_fileName);
        if (fileInfo.exists())
        {
            int index = m_fileName.lastIndexOf(DOWNLOAD_FILE_SUFFIX);
            QString realName = m_fileName.left(index);
            QFileInfo realfile(realName);
            if(realfile.exists()) QFile::remove(realName);
            QFile::rename(m_fileName, realName);
        }
    }
    else
    {
        // 有错误输出错误;
        QString strError = m_reply->errorString();
        qDebug() << "__________" + strError;
    }
    reset();
    stopWork();

    emit signalReplyFinished(statusCode.toInt());
}

// 下载过程中出现错误，关闭下载，并上报错误，这里未上报错误类型，可自己定义进行上报;
void DownLoadManager::onError(QNetworkReply::NetworkError code)
{
    QString strError = m_reply->errorString();
    qDebug() << "Status Code: " << code << strError;

    closeDownload();
    emit signalDownloadError();
}

// 停止下载工作;
void DownLoadManager::stopWork()
{
    m_isStop = true;
    if (m_reply != NULL)
    {
        disconnect(m_reply, SIGNAL(readyRead()), this, SLOT(onReadyRead()));
        disconnect(m_reply, SIGNAL(downloadProgress(qint64, qint64)), this, SLOT(onDownloadProgress(qint64, qint64)));
        disconnect(m_reply, SIGNAL(finished()), this, SLOT(onFinished()));
        disconnect(m_reply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(onError(QNetworkReply::NetworkError)));
        m_reply->abort();
        m_reply->deleteLater();
        m_reply = NULL;
    }
}

// 暂停下载按钮被按下,暂停当前下载;
void DownLoadManager::stopDownload()
{
    // 这里m_isStop变量为了保护多次点击暂停下载按钮，导致m_bytesCurrentReceived 被不停累加;
    if (!m_isStop)
    {
        stopWork();
    }
}

// 重置参数;
void DownLoadManager::reset()
{
    m_bytesCurrentReceived = 0;
    m_bytesReceived = 0;
    m_bytesTotal = 0;
}

// 删除文件;
void DownLoadManager::removeFile(QString fileName)
{
    // 删除已下载的临时文件;
    QFileInfo fileInfo(fileName);
    if (fileInfo.exists())
    {
        QFile::remove(fileName);
    }
}

// 关闭下载，重置参数，并删除下载的临时文件;
void DownLoadManager::closeDownload()
{
    stopWork();
    reset();
    removeFile(m_fileName);
}
