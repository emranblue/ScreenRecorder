#include "settingsdialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QFileDialog>
#include <QSettings>
#include <QLabel>

#include <QComboBox>

SettingsDialog::SettingsDialog(QWidget *parent) : QDialog(parent) {
    setWindowTitle("Settings");

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    QFormLayout *formLayout = new QFormLayout();

    m_outputLocationEdit = new QLineEdit(this);
    QPushButton *browseButton = new QPushButton("Browse...", this);
    QHBoxLayout *outputLocationLayout = new QHBoxLayout();
    outputLocationLayout->addWidget(m_outputLocationEdit);
    outputLocationLayout->addWidget(browseButton);
    formLayout->addRow("Output Location:", outputLocationLayout);

    m_fileNameFormatComboBox = new QComboBox(this);
    m_fileNameFormatComboBox->addItem("recording_{timestamp}.mkv", "recording_%1.mkv");
    m_fileNameFormatComboBox->addItem("{timestamp}_recording.mkv", "%1_recording.mkv");
    m_fileNameFormatComboBox->addItem("ScreenRecord_{timestamp}.mkv", "ScreenRecord_%1.mkv");
    m_fileNameFormatComboBox->addItem("{timestamp}.mkv", "%1.mkv");
    m_fileNameFormatComboBox->addItem("custom_{index}.mkv", "custom_%1.mkv");
    formLayout->addRow("File Name Format:", m_fileNameFormatComboBox);

    m_prefixWidget = new QWidget(this);
    QHBoxLayout *prefixLayout = new QHBoxLayout(m_prefixWidget);
    prefixLayout->setContentsMargins(0, 0, 0, 0);
    m_prefixEdit = new QLineEdit(this);
    prefixLayout->addWidget(new QLabel("Prefix:", this));
    prefixLayout->addWidget(m_prefixEdit);
    formLayout->addRow(m_prefixWidget);

    mainLayout->addLayout(formLayout);

    QHBoxLayout *buttonLayout = new QHBoxLayout();
    QPushButton *saveButton = new QPushButton("Save", this);
    QPushButton *cancelButton = new QPushButton("Cancel", this);
    buttonLayout->addStretch();
    buttonLayout->addWidget(saveButton);
    buttonLayout->addWidget(cancelButton);
    mainLayout->addLayout(buttonLayout);

    connect(browseButton, &QPushButton::clicked, this, &SettingsDialog::browseOutputDirectory);
    connect(saveButton, &QPushButton::clicked, this, &SettingsDialog::saveSettings);
    connect(cancelButton, &QPushButton::clicked, this, &SettingsDialog::reject);
    connect(m_fileNameFormatComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &SettingsDialog::updatePrefixWidgetVisibility);

    loadSettings();
}

void SettingsDialog::browseOutputDirectory() {
    QString directory = QFileDialog::getExistingDirectory(this, "Select Output Directory", m_outputLocationEdit->text());
    if (!directory.isEmpty()) {
        m_outputLocationEdit->setText(directory);
    }
}

void SettingsDialog::saveSettings() {
    QSettings settings;
    settings.setValue("outputLocation", m_outputLocationEdit->text());
    settings.setValue("fileNameFormatIndex", m_fileNameFormatComboBox->currentIndex());
    if (m_fileNameFormatComboBox->itemData(m_fileNameFormatComboBox->currentIndex()).toString() == "custom_%1.mkv") {
        settings.setValue("prefix", m_prefixEdit->text());
    }
    accept();
}

void SettingsDialog::updatePrefixWidgetVisibility(int index) {
    if (m_fileNameFormatComboBox->itemData(index).toString() == "custom_%1.mkv") {
        m_prefixWidget->setVisible(true);
    } else {
        m_prefixWidget->setVisible(false);
    }
}

void SettingsDialog::loadSettings() {
    QSettings settings;
    m_outputLocationEdit->setText(settings.value("outputLocation", "/home/emran/Videos").toString());
    m_fileNameFormatComboBox->setCurrentIndex(settings.value("fileNameFormatIndex", 0).toInt());
    m_prefixEdit->setText(settings.value("prefix", "video").toString());
    updatePrefixWidgetVisibility(m_fileNameFormatComboBox->currentIndex());
}
