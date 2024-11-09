#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // main window's style
    this->setWindowTitle("YouTube Downloader");
    this->setFixedSize(this->size());
    this->setStyleSheet("background-color:indigo ; color:white ; font:bold ;") ;

    // Quality comboBox items and style
    ui->quality_comboBox->addItem("1080");
    ui->quality_comboBox->addItem("720");
    ui->quality_comboBox->addItem("480");
    ui->quality_comboBox->setCurrentIndex(1);
    ui->quality_comboBox->setCursor(Qt::PointingHandCursor);

    // RadioButtons style
    ui->video_radioButton->setCursor(Qt::PointingHandCursor);
    ui->audio_radioButton->setCursor(Qt::PointingHandCursor);
    ui->playList_radioButton->setCursor(Qt::PointingHandCursor);
    ui->singleVideo_radioButton->setCursor(Qt::PointingHandCursor);

    // Default value of radioButtons
    ui->video_radioButton->setEnabled(true);
    ui->singleVideo_radioButton->setEnabled(true);

    // PushButtons style
    ui->download_pushButton->setCursor(Qt::PointingHandCursor);
    ui->choose_pushButton->setCursor(Qt::PointingHandCursor);
    ui->stop_pushButton->setCursor(Qt::PointingHandCursor);
    enablePushButtons(false , false) ;

    // checkBox's style
    ui->subtitle_checkBox->setCursor(Qt::PointingHandCursor) ;

    // Default path
    ui->path_lineEdit->setText("$HOME/Downloads/YouTube Downloader");

    // Progress bar
    ui->download_progress->setValue(0);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_download_pushButton_clicked()
{
    QString videoLink = ui->url_lineEdit->text();
    QString path = ui->path_lineEdit->text();

    enablePushButtons(false , true);

    if (!checkOptions(videoLink)) return; //check the options

    QStringList args = writeParameters(videoLink, path); //write request parameters based on options selected

    delete downloader;
    downloader = new QProcess(this); //make a new downloader


    if (downloader->state() == QProcess::NotRunning) {
        ui->download_progress->setValue(0); //set progressBar to zero

        setStatus("Start downloading", false);
        downloader->start("yt-dlp", args); //start downloading process

        connect(downloader, &QProcess::readyReadStandardOutput, this, &MainWindow::getOutput);
        connect(downloader, &QProcess::readyReadStandardError, this, &MainWindow::getError);
        QObject::connect(downloader, &QProcess::finished , this, &MainWindow::checkStatus );
    }
}

void MainWindow::enablePushButtons(bool enableDownloadPushButton, bool enableStopPushButton)
{
    if (enableDownloadPushButton) {
        ui->download_pushButton->setEnabled(true);
        ui->download_pushButton->setFixedSize(100, 40);
        ui->download_pushButton->setStyleSheet("background-color:orange; color:black; border-radius: 10px ;");

        ui->stop_pushButton->setEnabled(false);
        ui->stop_pushButton->setFixedSize(60, 30);
        ui->stop_pushButton->setStyleSheet("background-color:gray; color:black; border-radius: 10px ;");
    } else if (enableStopPushButton) {
        ui->download_pushButton->setEnabled(false);
        ui->download_pushButton->setFixedSize(60, 30);
        ui->download_pushButton->setStyleSheet("background-color:gray; color:black; border-radius: 10px ;");

        ui->stop_pushButton->setEnabled(true);
        ui->stop_pushButton->setFixedSize(100, 40);
        ui->stop_pushButton->setStyleSheet("background-color:red; color:black; border-radius: 10px ;");
    } else {
        ui->download_pushButton->setEnabled(false);
        ui->download_pushButton->setFixedSize(80, 40);
        ui->download_pushButton->setStyleSheet("background-color:gray; color:black; border-radius: 10px ;");

        ui->stop_pushButton->setEnabled(false);
        ui->stop_pushButton->setFixedSize(80, 40);
        ui->stop_pushButton->setStyleSheet("background-color:gray; color:black; border-radius: 10px ;");
    }
}

void MainWindow::on_url_lineEdit_textChanged(const QString &arg1)
{
    if (arg1.isEmpty()) {
        enablePushButtons(false, false);
    } else {
        enablePushButtons(true, false);
    }
}

bool MainWindow::checkOptions(QString &videoLink)
{
    //url lineEdit can't be empty!
    if (ui->url_lineEdit->text().isEmpty()) {
        ui->url_lineEdit->setStyleSheet("color:red");
        ui->url_lineEdit->setPlaceholderText("Can't be empty");
        return false;
    }

    //path lineEdit can't be empty!
    if (ui->path_lineEdit->text().isEmpty()) {
        ui->path_lineEdit->setStyleSheet("color:red");
        ui->path_lineEdit->setPlaceholderText("Can't be empty");
        return false;
    }

    //is the url link single video ?
    if (ui->singleVideo_radioButton->isChecked()) {
        if (!videoLink.contains("watch") && !videoLink.contains("shorts")) {
            setStatus("[ERROR] This URL doesn't seem to contain any video. Is it a playlist? Please change to playlist mode.", true);
            return false;
        }
    }

    //is the url link playList ?
    if (ui->playList_radioButton->isChecked()) {
        if (!videoLink.contains("list")) {
            setStatus("[ERROR] This URL doesn't seem to be a playlist. Maybe it's a video? Change to video mode and retry.", true);
            return false;
        }
    }

    return true;
}

void MainWindow::setStatus(QString message, bool isError)
{
    //show messages in status label
    if (isError) {
        ui->status_label->setStyleSheet("color:red;");
        ui->status_label->setText(message);
    } else {
        ui->status_label->setStyleSheet("color:green;");
        ui->status_label->setText(message);
    }
}

QStringList MainWindow::writeParameters(QString &videoLink, QString &path)
{
    //make a download request
    QStringList args = { videoLink, "--paths", path, "--no-color" };

    QString kbps;
    args << "--format";


    if (ui->video_radioButton->isChecked()) { //video format selected(mp4)
        switch (ui->quality_comboBox->currentIndex()) {
        case 0:
            args << "bv[ext=mp4][height=1080]+ba[ext=m4a]";
            break;
        case 1:
            args << "bv[ext=mp4][height=720]+ba[ext=m4a]";
            break;
        case 2:
            args << "bv[ext=mp4][height=480]+ba[ext=m4a]";
            break;
        }
    } else if (ui->audio_radioButton->isChecked()) { //audio format selected(mp3)
        args << "ba[ext=m4a]" << "-x" << "--audio-format" << "mp3" << "--audio-quality";
        switch (ui->quality_comboBox->currentIndex()) {
        case 0:
            kbps = "320k";
            break;
        case 1:
            kbps = "256k";
            break;
        case 2:
            kbps = "192k";
            break;
        }
        args << kbps;
    }

    if (ui->video_radioButton->isChecked() && ui->subtitle_checkBox->isChecked()) { //subtitle
        args << "--write-subs" << "--sub-langs" << "all" << "--convert-subs" << "srt";
    }
    if (ui->singleVideo_radioButton->isChecked()) {
        args << "--no-playlist";
    } else {
        args << "--yes-playlist";
    }

    QString playList;
    if (ui->playList_radioButton->isChecked()) { //playList selected
        if (ui->video_radioButton->isChecked()) { //video playList
            playList = "[Playlist] %(playlist)s/%(playlist_index)s - %(title)s [%(height)sp].%(ext)s";
        } else {
            playList = "[Playlist] %(playlist)s/%(playlist_index)s - %(title)s [" + kbps + "].%(ext)s";
        }
    } else {
        if (ui->video_radioButton->isChecked()) { //audio playList
            playList = "%(title)s [%(height)sp].%(ext)s";
        } else {
            playList = "%(title)s [" + kbps + "].%(ext)s";
        }
    }

    args << "-o" << playList;

    return args;
}

void MainWindow::on_audio_radioButton_clicked()
{
    ui->quality_comboBox->clear();

    // Audio qualities
    ui->quality_comboBox->addItem("320K");
    ui->quality_comboBox->addItem("256K");
    ui->quality_comboBox->addItem("192K");
    ui->quality_comboBox->setCurrentIndex(0);
}

void MainWindow::on_video_radioButton_clicked()
{
    ui->quality_comboBox->clear();

    // Video qualities
    ui->quality_comboBox->addItem("1080");
    ui->quality_comboBox->addItem("720");
    ui->quality_comboBox->addItem("480");
    ui->quality_comboBox->setCurrentIndex(1);
}

void MainWindow::getOutput()
{
    //get download logs
    QString output = QString::fromLocal8Bit(downloader->readAllStandardOutput());
    string tmp = output.toStdString();
    size_t found = tmp.find("[download]");

    // Extract the frag parameter and calculate the percentage
    size_t fragPos = tmp.find("(frag ");
    if (fragPos != string::npos) {
        size_t slashPos = tmp.find('/', fragPos);
        size_t endPos = tmp.find(')', fragPos);
        if (slashPos != string::npos && endPos != string::npos) {
            string currentFragStr = tmp.substr(fragPos + 6, slashPos - fragPos - 6);
            string totalFragStr = tmp.substr(slashPos + 1, endPos - slashPos - 1);
            int currentFrag = atoi(currentFragStr.c_str());
            int totalFrag = atoi(totalFragStr.c_str());
            if (totalFrag > 0) {
                int fragPercent = (currentFrag * 100) / totalFrag;
                if(fragPercent > 97) {
                    /*in mp4 formats video file downloads frist,then audio will download.
                    so in 97% of video progressBar waits to audio to download then show the completion message.
                    (notice that the whole video downloads in background but
                     we show the process with this view to user) */
                    goto label1 ;
                } else {
                    //show percentage in progressBar
                    ui->download_progress->setValue(fragPercent);
                }
            }
        }
    }

    if (found != string::npos) {
        tmp.erase(0, found + 11);
    }

    label1:
    setStatus(QString::fromStdString(tmp), false);
}

void MainWindow::getError()
{
    //ger Error and show message in status label
    QString output = downloader->readAllStandardError();
    setStatus(output, true);
}

void MainWindow::checkStatus()
{
    //if download finished show message and enable download pushButton again
    if (downloader->exitStatus() == QProcess::NormalExit) {
        setStatus("Job finished!", false);
        ui->download_progress->setValue(100);
    }

    enablePushButtons(true , false);
}

void MainWindow::on_choose_pushButton_clicked()
{
    //choose path to save file
    QString dir = QFileDialog::getExistingDirectory(nullptr, QString(), ui->path_lineEdit->text());
    if (!dir.isEmpty()) {
        ui->path_lineEdit->setText(dir);
    }
}

void MainWindow::on_stop_pushButton_clicked()
{
    //cancel download
    if(downloader->state() == QProcess::Running)
    {
        downloader->kill();
        setStatus("Job canceled by user." , true);
        ui->download_progress->setValue(0);
    }

    enablePushButtons(true , false);
}

