
#include <QtWidgets>

#include "pages.h"

QVector<QString> telemetry_values = {
    QObject::tr("Packet id"),
    QObject::tr("Internal temperature"),
    QObject::tr("External temperature"),
    QObject::tr("Alittude"),
    QObject::tr("Latitude"),
    QObject::tr("Longitude"),
    QObject::tr("Pressure"),
    QObject::tr("Voltage"),
    QObject::tr("Radiation"),
};

ConfigurationPage::ConfigurationPage(QWidget *parent)
    : QWidget(parent)
{
    QGroupBox *configGroup = new QGroupBox(tr("Plot #1 configuration"));

    QLabel *oxLabel = new QLabel(tr("Ox:"));
    QComboBox *serverCombo = new QComboBox;
    for (int i = 0; i < telemetry_values.size(); ++i) {
        serverCombo->addItem(telemetry_values[i]);
    }
    QLabel* oyLabel = new QLabel(tr("Oy:"));
    QListWidget* oyList = new QListWidget(this);
    for (int i = 0; i < telemetry_values.size(); ++i) {
        oyList->addItem(telemetry_values[i]);
    }
    oyList->setSelectionMode(QAbstractItemView::MultiSelection);

    QVBoxLayout *configLayout = new QVBoxLayout;
    configLayout->addWidget(oxLabel);
    configLayout->addWidget(serverCombo);
    configLayout->addWidget(oyLabel);
    configLayout->addWidget(oyList);
    configGroup->setLayout(configLayout);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(configGroup);
    mainLayout->addStretch(1);
    setLayout(mainLayout);
}

UpdatePage::UpdatePage(QWidget *parent)
    : QWidget(parent)
{
    QGroupBox *updateGroup = new QGroupBox(tr("Package selection"));
    QCheckBox *systemCheckBox = new QCheckBox(tr("Update system"));
    QCheckBox *appsCheckBox = new QCheckBox(tr("Update applications"));
    QCheckBox *docsCheckBox = new QCheckBox(tr("Update documentation"));

    QGroupBox *packageGroup = new QGroupBox(tr("Existing packages"));

    QListWidget *packageList = new QListWidget;
    QListWidgetItem *qtItem = new QListWidgetItem(packageList);
    qtItem->setText(tr("Qt"));
    QListWidgetItem *qsaItem = new QListWidgetItem(packageList);
    qsaItem->setText(tr("QSA"));
    QListWidgetItem *teamBuilderItem = new QListWidgetItem(packageList);
    teamBuilderItem->setText(tr("Teambuilder"));

    QPushButton *startUpdateButton = new QPushButton(tr("Start update"));

    QVBoxLayout *updateLayout = new QVBoxLayout;
    updateLayout->addWidget(systemCheckBox);
    updateLayout->addWidget(appsCheckBox);
    updateLayout->addWidget(docsCheckBox);
    updateGroup->setLayout(updateLayout);

    QVBoxLayout *packageLayout = new QVBoxLayout;
    packageLayout->addWidget(packageList);
    packageGroup->setLayout(packageLayout);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(updateGroup);
    mainLayout->addWidget(packageGroup);
    mainLayout->addSpacing(12);
    mainLayout->addWidget(startUpdateButton);
    mainLayout->addStretch(1);
    setLayout(mainLayout);
}
