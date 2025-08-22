#ifndef WEBCAMSELECTIONDIALOG_H
#define WEBCAMSELECTIONDIALOG_H

#include <QDialog>
#include <QListWidget>
#include <QPushButton>
#include <QString>

struct WebcamInfo {
    QString devicePath;
    QString deviceName;
};

class WebcamSelectionDialog : public QDialog {
    Q_OBJECT

public:
    explicit WebcamSelectionDialog(QWidget *parent = nullptr);

signals:
    void webcamSelected(const QString &devicePath);

private slots:
    void onAccepted();

private:
    void populateWebcamList();

    QListWidget *m_listWidget;
    QPushButton *m_okButton;
    QPushButton *m_cancelButton;

    QList<WebcamInfo> m_webcams;
};

#endif // WEBCAMSELECTIONDIALOG_H
