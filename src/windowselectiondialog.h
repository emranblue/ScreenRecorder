#ifndef WINDOWSELECTIONDIALOG_H
#define WINDOWSELECTIONDIALOG_H

#include <QDialog>
#include <QListWidget>
#include <QPushButton>
#include <QRect>

struct WindowInfo {
    WId id;
    QString title;
    QRect geometry;
};

class WindowSelectionDialog : public QDialog {
    Q_OBJECT

public:
    explicit WindowSelectionDialog(QWidget *parent = nullptr);

signals:
    void windowSelected(const QRect &geometry);

private slots:
    void onAccepted();

private:
    void populateWindowList();

    QListWidget *m_listWidget;
    QPushButton *m_okButton;
    QPushButton *m_cancelButton;

    QList<WindowInfo> m_windows;
};

#endif // WINDOWSELECTIONDIALOG_H
