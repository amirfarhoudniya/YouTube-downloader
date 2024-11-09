#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QProcess>
#include <QStringList>
#include <QFileDialog>
#include <QStandardPaths>

using namespace std ;

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_download_pushButton_clicked();
    void on_url_lineEdit_textChanged(const QString &arg1);
    void on_audio_radioButton_clicked();
    void on_video_radioButton_clicked();
    void on_choose_pushButton_clicked();
    void getOutput();
    void getError();
    void checkStatus();

    void on_stop_pushButton_clicked();

private:
    Ui::MainWindow *ui;
    QProcess *downloader = new QProcess(this);

    void enablePushButtons(bool enableDownloadPushButton, bool enableStopPushButton);
    bool checkOptions(QString &videoLink);
    void setStatus(QString message, bool isError);
    QStringList writeParameters(QString &videoLink, QString &path);
};

#endif // MAINWINDOW_H
