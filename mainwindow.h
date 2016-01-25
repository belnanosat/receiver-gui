#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QProcess>
#include <QDebug>
#include <QTime>

#include "proto/telemetry.pb.h"
#include "telemetrypacketwrapper.h"
#include "qcustomplot.h"

namespace Marble {
class GeoDataPlacemark;
class MarbleWidget;
}

class QCustomPlot;
class MyMarbleWidget;
class QFile;
class ConfigDialog;

namespace Ui {
class MainWindow;
}

template <typename T1, typename T2>
void addPlot(QCustomPlot *plot, const QVector<TelemetryPacket>& packets, const QString& xlegend, const QString& ylegend,
             QVector<QPair<T1 (*)(const TelemetryPacket&),
                                         T2 (*)(const TelemetryPacket&)>> lst_tmp) {
    double xmin = std::numeric_limits<double>::max();
    double xmax = std::numeric_limits<double>::min();
    double ymin = std::numeric_limits<double>::max();
    double ymax = std::numeric_limits<double>::min();
    std::vector<QPair<T1 (*)(const TelemetryPacket&),
            T2 (*)(const TelemetryPacket&)>> lst(lst_tmp.begin(), lst_tmp.end());
    for (int j = 0; j < lst.size(); ++j) {
        QVector<double> x(packets.size());
        QVector<double> y(packets.size());
        for (int i = 0; i < packets.size(); ++i) {
            x[i] = lst[j].first(packets[i]);
            y[i] = lst[j].second(packets[i]);
        }

        plot->graph(j)->setData(x, y);
        xmin = std::min(xmin, *std::min_element(x.begin(), x.end()));
        xmax = std::max(xmax, *std::max_element(x.begin(), x.end()));
        ymin = std::min(ymin, *std::min_element(y.begin(), y.end()));
        ymax = std::max(ymax, *std::max_element(y.begin(), y.end()));
    }
    // give the axes some labels:
    plot->xAxis->setLabel(xlegend);
    plot->yAxis->setLabel(ylegend);
    // set axes ranges, so we see all data:
    plot->xAxis->setRange(xmin, xmax);
    plot->yAxis->setRange(ymin, ymax);
    plot->replot();
    plot->repaint();
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:
    void displayInputText();
    void toggleMapCenter(int state);
    void updatePlotSettings(int plot_id, QPair<int, QVector<int> > settings);

protected:
    bool readPacket(const QString& s, TelemetryPacket* packet);
    void processPacket(const TelemetryPacket& packet);
    void redrawPlots();

private:
    QFile *log_file;
    Ui::MainWindow *ui;
    QProcess *child;
    QString cur_packet;
    Marble::GeoDataPlacemark *place;
    MyMarbleWidget* mapWidget;
    QVector<TelemetryPacket> packets;
    QTime timer;
    QCustomPlot* customPlot, *customPlot2, *customPlot3, *customPlot4;
    int packet_id = 0;
    int packet_max_length = -1;
    QPainter* painter;
    ConfigDialog* configDialog;

    QVector <QPair<int, QVector<int> > > plot_configs;
    QVector <QCustomPlot*> plots;
};

#endif // MAINWINDOW_H
