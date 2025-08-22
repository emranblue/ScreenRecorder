#include <QApplication>
#include <QWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QFormLayout>
#include <QComboBox>
#include <QCheckBox>
#include <QScreen>
#include <QPixmap>
#include <QProcess>
#include <QDebug>
#include <QDateTime>
#include <QDir>
#include <QMenu>
#include <QContextMenuEvent>
#include <QAction>
#include <QLabel>

#include "selectionwidget.h"
#include "windowselectiondialog.h"
#include "webcamselectiondialog.h"

class ScreenRecorder : public QWidget {
    Q_OBJECT

public:
    ScreenRecorder(QWidget *parent = nullptr) : QWidget(parent), m_selectionWidget(nullptr), m_windowSelectionDialog(nullptr), m_selectedWebcam("/dev/video0") {
        setWindowTitle("Screen Recorder");
        setContextMenuPolicy(Qt::CustomContextMenu);
        connect(this, &QWidget::customContextMenuRequested, this, &ScreenRecorder::showContextMenu);


        QVBoxLayout *mainLayout = new QVBoxLayout(this);

        // Recording options
        QGroupBox *optionsGroupBox = new QGroupBox("Recording Options", this);
        QFormLayout *optionsLayout = new QFormLayout(optionsGroupBox);

        m_captureModeComboBox = new QComboBox(this);
        m_captureModeComboBox->addItem("Full Screen", QVariant::fromValue(CaptureMode::FullScreen));
        m_captureModeComboBox->addItem("Area Selection", QVariant::fromValue(CaptureMode::AreaSelection));
        m_captureModeComboBox->addItem("Window Selection", QVariant::fromValue(CaptureMode::WindowSelection));
        optionsLayout->addRow("Capture Mode:", m_captureModeComboBox);

        m_audioSourceComboBox = new QComboBox(this);
        m_audioSourceComboBox->addItem("None", QVariant::fromValue(AudioSource::None));
        m_audioSourceComboBox->addItem("Microphone", QVariant::fromValue(AudioSource::Microphone));
        m_audioSourceComboBox->addItem("System Audio", QVariant::fromValue(AudioSource::SystemAudio));
        optionsLayout->addRow("Audio Source:", m_audioSourceComboBox);

        QHBoxLayout *webcamLayout = new QHBoxLayout();
        m_webcamCheckBox = new QCheckBox("Include Webcam", this);
        webcamLayout->addWidget(m_webcamCheckBox);
        m_selectWebcamButton = new QPushButton("Select Webcam", this);
        webcamLayout->addWidget(m_selectWebcamButton);
        optionsLayout->addRow(webcamLayout);

        m_selectedWebcamLabel = new QLabel(QString("Selected Webcam: %1").arg(m_selectedWebcam), this);
        optionsLayout->addRow(m_selectedWebcamLabel);

        m_fpsComboBox = new QComboBox(this);
        m_fpsComboBox->addItem("15", QVariant::fromValue(15));
        m_fpsComboBox->addItem("30", QVariant::fromValue(30));
        m_fpsComboBox->addItem("60", QVariant::fromValue(60));
        optionsLayout->addRow("FPS:", m_fpsComboBox);


        mainLayout->addWidget(optionsGroupBox);

        // Controls
        QHBoxLayout *controlsLayout = new QHBoxLayout();
        m_startButton = new QPushButton("Start Recording", this);
        m_stopButton = new QPushButton("Stop Recording", this);
        m_stopButton->setEnabled(false);
        controlsLayout->addWidget(m_startButton);
        controlsLayout->addWidget(m_stopButton);
        m_aboutButton = new QPushButton("About", this);
        controlsLayout->addWidget(m_aboutButton);
        mainLayout->addLayout(controlsLayout);

        connect(m_startButton, &QPushButton::clicked, this, &ScreenRecorder::startRecording);
        connect(m_stopButton, &QPushButton::clicked, this, &ScreenRecorder::stopRecording);
        connect(m_selectWebcamButton, &QPushButton::clicked, this, &ScreenRecorder::openWebcamSelectionDialog);
        connect(m_aboutButton, &QPushButton::clicked, this, &ScreenRecorder::showAboutDialog);


        m_process = new QProcess(this);
        connect(m_process, &QProcess::errorOccurred, this, [=](QProcess::ProcessError error) {
            qDebug() << "Process error:" << error;
        });

        // Actions for context menu
        m_startAction = new QAction("Start Record", this);
        connect(m_startAction, &QAction::triggered, this, &ScreenRecorder::startRecording);
        m_stopAction = new QAction("Stop Record", this);
        m_stopAction->setEnabled(false);
        connect(m_stopAction, &QAction::triggered, this, &ScreenRecorder::stopRecording);
    }

private slots:
    void openWebcamSelectionDialog() {
        WebcamSelectionDialog dialog(this);
        connect(&dialog, &WebcamSelectionDialog::webcamSelected, this, &ScreenRecorder::onWebcamSelected);
        dialog.exec();
    }

    void onWebcamSelected(const QString &devicePath) {
        m_selectedWebcam = devicePath;
        m_selectedWebcamLabel->setText(QString("Selected Webcam: %1").arg(m_selectedWebcam));
    }

    void showContextMenu(const QPoint &pos) {
        QMenu menu(this);
        menu.addAction(m_startAction);
        menu.addAction(m_stopAction);
        menu.exec(mapToGlobal(pos));
    }

    void startRecording() {
        m_startButton->setEnabled(false);
        m_stopButton->setEnabled(true);
        m_startAction->setEnabled(false);
        m_stopAction->setEnabled(true);

        CaptureMode selectedMode = m_captureModeComboBox->currentData().value<CaptureMode>();

        switch (selectedMode) {
            case CaptureMode::FullScreen:
            {
                QScreen *screen = QGuiApplication::primaryScreen();
                startFfmpeg(screen->geometry());
                break;
            }
            case CaptureMode::AreaSelection:
                m_selectionWidget = new SelectionWidget();
                connect(m_selectionWidget, &SelectionWidget::areaSelected, this, &ScreenRecorder::onAreaSelected);
                m_selectionWidget->show();
                break;
            case CaptureMode::WindowSelection:
                m_windowSelectionDialog = new WindowSelectionDialog(this);
                connect(m_windowSelectionDialog, &WindowSelectionDialog::windowSelected, this, &ScreenRecorder::onWindowSelected);
                if (m_windowSelectionDialog->exec() == QDialog::Rejected) {
                    stopRecording();
                }
                break;
        }
    }

    void onAreaSelected(const QRect &rect) {
        startFfmpeg(rect);
    }

    void onWindowSelected(const QRect &rect) {
        startFfmpeg(rect);
    }

    void startFfmpeg(const QRect &captureRect) {
        AudioSource audioSource = m_audioSourceComboBox->currentData().value<AudioSource>();
        bool includeWebcam = m_webcamCheckBox->isChecked();

        QString outputDir = "/home/emran/Videos";
        QDir dir(outputDir);
        if (!dir.exists()) {
            dir.mkpath(".");
        }

        QString timestamp = QDateTime::currentDateTime().toString("yyyy-MM-dd_hh-mm-ss");
        QString outputFileName = QString("%1/recording_%2.mkv").arg(outputDir, timestamp);


        QStringList args;
        args << "-y"; // Overwrite output file if it exists
        args << "-r" << m_fpsComboBox->currentText(); // Set FPS
        
        // Main input (screen)
        args << "-f" << "x11grab";
        args << "-s" << QString::number(captureRect.width()) + "x" + QString::number(captureRect.height());
        args << "-i" << QString(":0.0+%1,%2").arg(captureRect.x()).arg(captureRect.y());

        int audioInputIndex = 1;
        if (includeWebcam) {
            args << "-f" << "v4l2" << "-i" << m_selectedWebcam;
            audioInputIndex = 2;
        }

        // Audio source
        bool captureAudio = false;
        switch(audioSource) {
            case AudioSource::Microphone:
                args << "-f" << "alsa" << "-i" << "default";
                captureAudio = true;
                break;
            case AudioSource::SystemAudio:
                args << "-f" << "pulse" << "-i" << "default";
                captureAudio = true;
                break;
            case AudioSource::None:
            default:
                break;
        }

        if (includeWebcam) {
            QString filter = "[1:v]scale=320:240[webcam];[0:v][webcam]overlay=W-w-10:H-h-10[outv]";
            args << "-filter_complex" << filter;
            args << "-map" << "[outv]";
            if (captureAudio) {
                args << "-map" << QString::number(audioInputIndex) + ":a";
            }
        } else {
            if (captureAudio) {
                args << "-map" << "0:v" << "-map" << "1:a";
            }
        }

        // Encoding settings
        args << "-c:v" << "libx264" << "-preset" << "ultrafast" << "-qp" << "0";
        if (captureAudio) {
            args << "-c:a" << "aac" << "-b:a" << "128k";
        }
        
        args << outputFileName;

        QString command = "ffmpeg " + args.join(" ");
        qDebug() << "Starting ffmpeg with command:" << command;
        m_process->start("ffmpeg", args);
    }

    void stopRecording() {
        m_startButton->setEnabled(true);
        m_stopButton->setEnabled(false);
        m_startAction->setEnabled(true);
        m_stopAction->setEnabled(false);
        if (m_process->state() == QProcess::Running) {
            m_process->write("q\n");
            m_process->waitForFinished();
        }
    }

private slots:
    void showAboutDialog() {
        QDialog *aboutDialog = new QDialog(this);
        aboutDialog->setWindowTitle("About");
        QVBoxLayout *layout = new QVBoxLayout(aboutDialog);
        QLabel *nameLabel = new QLabel("Developed by: Emran Hossain", aboutDialog);
        layout->addWidget(nameLabel);
        aboutDialog->setLayout(layout);
        aboutDialog->exec();
    }

private:
    enum class CaptureMode {
        FullScreen,
        AreaSelection,
        WindowSelection
    };

    enum class AudioSource {
        None,
        Microphone,
        SystemAudio
    };

    QPushButton *m_startButton;
    QPushButton *m_stopButton;
    QComboBox *m_captureModeComboBox;
    QComboBox *m_audioSourceComboBox;
    QCheckBox *m_webcamCheckBox;
    QPushButton *m_selectWebcamButton;
    QLabel *m_selectedWebcamLabel;
    QComboBox *m_fpsComboBox;
    QProcess *m_process;
    SelectionWidget *m_selectionWidget;
    WindowSelectionDialog *m_windowSelectionDialog;
    QAction *m_startAction;
    QAction *m_stopAction;
    QString m_selectedWebcam;
    QPushButton *m_aboutButton;
};

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    ScreenRecorder recorder;
    recorder.show();

    return app.exec();
}

#include "main.moc"