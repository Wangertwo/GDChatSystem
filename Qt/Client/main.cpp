#include "mainwindow.h"
#include <QApplication>
#include <QFile>
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QFile qss(":/style/style/style.qss");
    if(qss.open(qss.ReadOnly)) {
        qDebug() << "open qss success!";
        QString style = QLatin1String(qss.readAll());
        a.setStyleSheet(style);
        qss.close();
    }else {
        qDebug() << "open qss failed!";
    }

    QString fileName = "config.ini";
    QString app_path = QCoreApplication::applicationDirPath();
    // 找到配置路径
    QString config_path = QDir::toNativeSeparators(app_path + QDir::separator() + fileName);
    QSettings settings(config_path, QSettings::IniFormat);
    QString gate_host = settings.value("GateServer/host").toString();
    QString gate_port = settings.value("GateServer/port").toString();
    gate_url_prefix = "http://" + gate_host + ":" + gate_port;
    qDebug() << gate_url_prefix;

    MainWindow w;
    w.show();
    return a.exec();
}
