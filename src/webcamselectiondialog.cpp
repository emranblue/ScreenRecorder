#include "webcamselectiondialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QProcess>
#include <QDebug>
#include <QDir>
#include <QSet>

WebcamSelectionDialog::WebcamSelectionDialog(QWidget *parent)
    : QDialog(parent) {
    setWindowTitle("Select a Webcam");

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    m_listWidget = new QListWidget(this);
    mainLayout->addWidget(m_listWidget);

    QHBoxLayout *buttonLayout = new QHBoxLayout();
    m_okButton = new QPushButton("OK", this);
    m_cancelButton = new QPushButton("Cancel", this);
    buttonLayout->addWidget(m_okButton);
    buttonLayout->addWidget(m_cancelButton);
    mainLayout->addLayout(buttonLayout);

    connect(m_okButton, &QPushButton::clicked, this, &WebcamSelectionDialog::onAccepted);
    connect(m_cancelButton, &QPushButton::clicked, this, &QDialog::reject);

    populateWebcamList();
}

void WebcamSelectionDialog::populateWebcamList() {
    QSet<QString> foundDevicePaths;

    // First, try to get device names from v4l2-ctl
    QProcess process;
    process.start("v4l2-ctl", QStringList() << "--list-devices");
    process.waitForFinished();

    if (process.exitCode() == 0) {
        QString output = process.readAllStandardOutput();
        qDebug() << "v4l2-ctl output:\n" << output;

        QStringList lines = output.split('\n', Qt::SkipEmptyParts);
        QString currentDeviceName;

        for (const QString &line : lines) {
            if (!line.startsWith("\t")) {
                currentDeviceName = line.trimmed();
            } else {
                QString devicePath = line.trimmed();
                if (devicePath.startsWith("/dev/video")) {
                    WebcamInfo info = {devicePath, currentDeviceName};
                    m_webcams.append(info);
                    foundDevicePaths.insert(devicePath);
                    qDebug() << "Found webcam (v4l2-ctl):" << info.deviceName << info.devicePath;
                }
            }
        }
    } else {
        qDebug() << "v4l2-ctl not found or failed. Will rely on /dev/video* scan.";
    }

    // Always scan /dev/video* for devices, and add any not found by v4l2-ctl
    QDir devDir("/dev");
    QStringList videoDevices = devDir.entryList(QStringList() << "video*", QDir::System);
    for (const QString &deviceName : videoDevices) {
        QString devicePath = "/dev/" + deviceName;
        if (!foundDevicePaths.contains(devicePath)) {
            WebcamInfo info = {devicePath, devicePath}; // Use path as name if no descriptive name from v4l2-ctl
            m_webcams.append(info);
            qDebug() << "Found webcam (/dev/video*):" << info.deviceName << info.devicePath;
        }
    }

    // Populate the list widget
    for (const auto &webcam : m_webcams) {
        QListWidgetItem *item = new QListWidgetItem(QString("%1 (%2)").arg(webcam.deviceName, webcam.devicePath), m_listWidget);
        item->setData(Qt::UserRole, QVariant::fromValue(webcam.devicePath));
    }

    if (m_webcams.isEmpty()) {
        QListWidgetItem *item = new QListWidgetItem("Could not find any webcams.\nMake sure your webcam is connected.", m_listWidget);
        item->setForeground(Qt::red);
        m_okButton->setEnabled(false);
    }
}

void WebcamSelectionDialog::onAccepted() {
    QListWidgetItem *currentItem = m_listWidget->currentItem();
    if (currentItem) {
        QString selectedDevice = currentItem->data(Qt::UserRole).toString();
        qDebug() << "Selected webcam:" << selectedDevice;
        emit webcamSelected(selectedDevice);
    }
    accept();
}
