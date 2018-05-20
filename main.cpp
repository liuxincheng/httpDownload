#include <QtWidgets/QApplication>
#include <QDebug>
#include <QTime>
#include "myhttpdownload.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MyHttpDownload w;
    w.show();
    return a.exec();
}
