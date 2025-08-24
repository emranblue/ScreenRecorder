#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>

class QLineEdit;
class QPushButton;
class QComboBox;
class QWidget;

class SettingsDialog : public QDialog {
    Q_OBJECT

public:
    explicit SettingsDialog(QWidget *parent = nullptr);

private slots:
    void browseOutputDirectory();
    void saveSettings();
    void updatePrefixWidgetVisibility(int index);

private:
    void loadSettings();

    QLineEdit *m_outputLocationEdit;
    QComboBox *m_fileNameFormatComboBox;
    QLineEdit *m_prefixEdit;
    QWidget *m_prefixWidget;
};

#endif // SETTINGSDIALOG_H
