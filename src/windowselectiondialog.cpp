#include "windowselectiondialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QProcess>
#include <QDebug>

WindowSelectionDialog::WindowSelectionDialog(QWidget *parent)
    : QDialog(parent) {
    setWindowTitle("Select a Window");

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    m_listWidget = new QListWidget(this);
    mainLayout->addWidget(m_listWidget);

    QHBoxLayout *buttonLayout = new QHBoxLayout();
    m_okButton = new QPushButton("OK", this);
    m_cancelButton = new QPushButton("Cancel", this);
    buttonLayout->addWidget(m_okButton);
    buttonLayout->addWidget(m_cancelButton);
    mainLayout->addLayout(buttonLayout);

    connect(m_okButton, &QPushButton::clicked, this, &WindowSelectionDialog::onAccepted);
    connect(m_cancelButton, &QPushButton::clicked, this, &QDialog::reject);

    populateWindowList();
}

void WindowSelectionDialog::populateWindowList() {
    QProcess process;
    connect(&process, &QProcess::errorOccurred, this, [&](QProcess::ProcessError error){
        qDebug() << "wmctrl process error:" << error;
    });

    process.start("wmctrl", QStringList() << "-lG");
    process.waitForFinished();

    QString output = process.readAllStandardOutput();
    qDebug() << "wmctrl output:\n" << output;

    QStringList lines = output.split('\n', Qt::SkipEmptyParts);

    for (const QString &line : lines) {
        QStringList parts = line.split(' ', Qt::SkipEmptyParts);
        if (parts.size() >= 8) {
            WId id = parts[0].toULong(nullptr, 16);
            int x = parts[2].toInt();
            int y = parts[3].toInt();
            int width = parts[4].toInt();
            int height = parts[5].toInt();
            QString title = parts.mid(7).join(' ');

            // Ignore the desktop and other utility windows
            if (title.contains("Desktop") || title.contains("N/A")) {
                continue;
            }

            WindowInfo info = {id, title, QRect(x, y, width, height)};
            m_windows.append(info);

            qDebug() << "Found window:" << info.title << info.geometry;

            QListWidgetItem *item = new QListWidgetItem(title, m_listWidget);
            item->setData(Qt::UserRole, QVariant::fromValue(id));
        }
    }

    if (m_windows.isEmpty()) {
        QListWidgetItem *item = new QListWidgetItem("Could not find any windows.\nMake sure 'wmctrl' is installed.", m_listWidget);
        item->setForeground(Qt::red);
        m_okButton->setEnabled(false);
    }
}

void WindowSelectionDialog::onAccepted() {
    QListWidgetItem *currentItem = m_listWidget->currentItem();
    if (currentItem) {
        WId selectedId = currentItem->data(Qt::UserRole).value<WId>();
        for (const auto &win : m_windows) {
            if (win.id == selectedId) {
                qDebug() << "Selected window:" << win.title << win.geometry;
                emit windowSelected(win.geometry);
                break;
            }
        }
    }
    accept();
}
