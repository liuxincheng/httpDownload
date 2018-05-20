#ifndef MYHTTPDOWNLOAD_H
#define MYHTTPDOWNLOAD_H

#include <QWidget>
#include <QTime>
#include "ui_myhttpdownload.h"

class DownLoadManager;

class MyHttpDownload : public QWidget
{
    Q_OBJECT

public:
    MyHttpDownload(QWidget *parent = 0);
    ~MyHttpDownload();

private:
    void initWindow();
    // ת����λ;
    QString transformUnit(qint64 bytes , bool isSpeed = false);
    // ת��ʱ��;
    QString transformTime(qint64 seconds);

private slots:
    void onStartDownload();
    void onStopDownload();
    void onCloseDownload();

    void onDownloadProcess(qint64 bytesReceived, qint64 bytesTotal);
    void onReplyFinished(int);

private:
    Ui::MyHttpDownloadClass ui;

    QString m_url;
    DownLoadManager* m_downloadManager;
    uint m_timeInterval;
    qint64 m_currentDownload;
    qint64 m_intervalDownload;
    QTime m_timeRecord;
    int m_nTime;
};

#endif // MYHTTPDOWNLOAD_H
