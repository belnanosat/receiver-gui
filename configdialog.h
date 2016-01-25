#ifndef CONFIGDIALOG_H
#define CONFIGDIALOG_H

#include <QDialog>

class QListWidget;
class QListWidgetItem;
class QStackedWidget;

class ConfigDialog : public QDialog
{
    Q_OBJECT

public:
    ConfigDialog();
    virtual ~ConfigDialog() {}

public slots:
    void changePage(QListWidgetItem *current, QListWidgetItem *previous);
    void updateSettings();

signals:
    void updatePlotSettings(int plot_id, QPair<int,QVector<int>> settings);

private:
    void createIcons();

    QListWidget *contentsWidget;
    QStackedWidget *pagesWidget;
};

#endif
