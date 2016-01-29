#include "mainwindow.h"

#include <algorithm>

#include <QScrollBar>
#include <QFile>
#include <google/protobuf/text_format.h>
#include <marble/MarbleWidget.h>
#include <marble/GeoDataPlacemark.h>
#include <marble/GeoDataDocument.h>
#include <marble/MarbleModel.h>
#include <marble/GeoDataTreeModel.h>
#include <marble/GeoDataCoordinates.h>

#include "ui_mainwindow.h"
#include "proto/telemetry.pb.h"
#include "mymarblewidget.h"
#include "configdialog.h"
#include <qcustomplot.h>

static QVector<double (*)(const TelemetryPacket&)> telemetry_values = {
    TelemetryPacketWrapper::packet_id,
    TelemetryPacketWrapper::temperature_internal,
    TelemetryPacketWrapper::temperature_external,
    TelemetryPacketWrapper::altitude,
    TelemetryPacketWrapper::latitude,
    TelemetryPacketWrapper::longitude,
    TelemetryPacketWrapper::pressure,
    TelemetryPacketWrapper::bmp180_temperature,
    TelemetryPacketWrapper::voltage,
    TelemetryPacketWrapper::radiation,
};

static float ConvertDS18B20Temperature(int16_t raw_temperature) {
    float temperature = raw_temperature;
    temperature /= 16.0f;
    return temperature;
}

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    timer.start();
    ui->setupUi(this);
    child = new QProcess(this);
    QStringList arguments;
    //child->start("../flightsim/flightsim.py", arguments);
    //child->waitForStarted();
    serial.setBaudRate(QSerialPort::Baud38400);
    serial.setPortName("/dev/ttyUSB0");
    while (!serial.open(QIODevice::ReadOnly));
    qDebug() << serial.errorString() << serial.error();
    qDebug() << child->state() << child->errorString();
    connect(&serial, SIGNAL(readyRead()), this, SLOT(displayInputText()));
    mapWidget = new MyMarbleWidget(ui->tab_2);
    mapWidget->setObjectName(QStringLiteral("MarbleWidget"));
    mapWidget->setGeometry(QRect(50, 30, 891, 391));
    mapWidget->setMapThemeId("earth/openstreetmap/openstreetmap.dgml");
    ui->verticalLayout_2->addWidget(mapWidget);
    place = new Marble::GeoDataPlacemark( "BelNanoSat-2" );
    place->setCoordinate( 26.0783, 44.4671, 0, Marble::GeoDataCoordinates::Degree );
    //place->setBalloonVisible(true);
    //place->setZoomLevel(10);
    Marble::GeoDataDocument *document = new Marble::GeoDataDocument;
    document->append( place );

    // Add the document to MarbleWidget's tree model
    mapWidget->model()->treeModel()->addDocument( document );

    customPlot = new QCustomPlot(ui->tab_5);
    customPlot->setInteraction(QCP::iSelectPlottables, true);
    customPlot->addGraph();
    customPlot->addGraph();
    ui->gridLayout_3->addWidget(customPlot, 0, 0);
    customPlot2 = new QCustomPlot(ui->tab_5);
    customPlot2->setInteraction(QCP::iSelectPlottables, true);
    customPlot2->addGraph();
    customPlot2->addGraph();
    ui->gridLayout_3->addWidget(customPlot2, 0, 1);


    customPlot3 = new QCustomPlot(ui->tab_5);
    customPlot3->setInteraction(QCP::iSelectPlottables, true);
    customPlot3->addGraph();
    customPlot3->addGraph();
    ui->gridLayout_3->addWidget(customPlot3, 1, 0);

    customPlot4 = new QCustomPlot(ui->tab_5);
    customPlot4->setInteraction(QCP::iSelectPlottables, true);
    customPlot4->addGraph();
    customPlot4->addGraph();
    ui->gridLayout_3->addWidget(customPlot4, 1, 1);

    plots.push_back(customPlot);
    plots.push_back(customPlot2);
    plots.push_back(customPlot3);
    plots.push_back(customPlot4);
    plot_configs.resize(4);
    plot_configs[0].first = 0;
    plot_configs[1].first = 0;
    plot_configs[2].first = 0;
    plot_configs[3].first = 0;

    ui->progressBar->setMinimum(0);
    ui->progressBar->setMaximum(100);

    log_file = new QFile("/tmp/cansat-log.txt");
    if (!log_file->open(QIODevice::WriteOnly)) {
        qDebug() << "failed to open log file!";
    }

    plot_configs.resize(4);

    connect(ui->checkBox, SIGNAL(stateChanged(int)), this, SLOT(toggleMapCenter(int)));

    configDialog = new ConfigDialog();
    connect(ui->pushButton, SIGNAL(clicked(bool)), configDialog, SLOT(show()));
    connect(configDialog, SIGNAL(updatePlotSettings(int,QPair<int,QVector<int> >)),
            this, SLOT(updatePlotSettings(int, QPair<int, QVector<int> >)));
}

void MainWindow::updatePlotSettings(int plot_id, QPair<int, QVector<int> > settings) {
    plot_configs[plot_id] = settings;
    redrawPlots();
    //addPlot(plot, packets, "Time, ms", "Height, m", {std::make_pair(&TelemetryPacket::packet_id, &TelemetryPacket::altitude)});
}

void MainWindow::redrawPlots() {
    for (int i = 0; i < plots.size(); ++i) {
        QVector<QPair<double(*)(const TelemetryPacket&), double(*)(const TelemetryPacket&)> > tmp;
        QPair<double(*)(const TelemetryPacket&), double(*)(const TelemetryPacket&)> cur;
        cur.first = telemetry_values[plot_configs[i].first];
        for (int j = 0; j < plot_configs[i].second.size(); ++j) {
            cur.second = telemetry_values[plot_configs[i].second[j]];
            tmp.push_back(cur);
        }
        addPlot(plots[i], packets, "Packet id", "Temperature", tmp);
    }
}

MainWindow::~MainWindow()
{
    log_file->close();
    delete log_file;
    delete ui;
}

void unstuffData(const QByteArray &data, QByteArray &unstuffed_data) {
    int next_zero = data[0];
    for (int i = 1; i < data.length(); ++i) {
        if (i < next_zero) {
            unstuffed_data[i - 1] = data[i];
        } else {
            unstuffed_data[i - 1] = 0;
            next_zero = i + data[i];
        }
    }
}

void MainWindow::displayInputText()  {
    QByteArray s = serial.read(256);
    //QTextStream stream(log_file);
    //stream << s;
    //ui->textEdit->setPlainText(ui->textEdit->toPlainText() + s);
    QScrollBar *sb = ui->textEdit->verticalScrollBar();
    sb->setValue(sb->maximum());
    cur_packet += s;
    if (packet_max_length > -1) {
        int percentage = std::min(100, cur_packet.length() * 100 / packet_max_length);
        ui->progressBar->setValue(percentage);
    }
    int i = 0;
    while (i < cur_packet.length()) {
        if (cur_packet[i] != 0) {
            ++i;
            continue;
        }
        // We've received a complete packet
        qDebug() << "packet: " << i + 1;
        packet_max_length = std::max(packet_max_length, i + 1);
        QByteArray tmp = cur_packet.left(i);
        QByteArray unstuffed_data(tmp.length() - 2, 0);
        unstuffData(tmp, unstuffed_data);
        TelemetryPacket packet;
        QPalette Pal(palette());
        bool correct = readPacket(unstuffed_data, &packet);
        if (correct) {
            Pal.setColor(QPalette::Background, Qt::green);
            processPacket(packet);
        } else {
            Pal.setColor(QPalette::Background, Qt::red);
        }
        ui->checksumStatus->setAutoFillBackground(true);
        ui->checksumStatus->setPalette(Pal);
        ui->checksumStatus->show();
        cur_packet = cur_packet.right(cur_packet.length() - i - 1);
    }
}

void MainWindow::toggleMapCenter(int state) {
    if (state && !packets.empty()) {
        const TelemetryPacket& packet = packets[packets.size() - 1];
        mapWidget->centerOn(Marble::GeoDataCoordinates(packet.longitude(), packet.latitude(), 0, Marble::GeoDataCoordinates::Degree));
    }
}

bool MainWindow::readPacket(const QByteArray &s, TelemetryPacket *packet) {
    if (s.length() >= 2) {
        uchar expected_checksum = s[s.length() - 1];
        uchar checksum = 0;
        for (int i = 0; i + 1 < s.length(); ++i) {
            checksum ^= s[i];
        }
        if (checksum != expected_checksum) {
            qDebug() << checksum << " " << expected_checksum;
            return false;
        }
        packet->ParseFromString(s.left(s.length() - 1).toStdString());
    } else {
        return false;
    }
    return true;
}

void MainWindow::processPacket(const TelemetryPacket& packet) {
    packets.push_back(packet);
    place->setCoordinate( packet.longitude(), packet.latitude(), 0, Marble::GeoDataCoordinates::Degree );
    ui->lineEdit_altitude->setText(QString::number(packet.altitude()));
    ui->lineEdit_longitude->setText(QString::number(packet.longitude()));
    ui->lineEdit_latitude->setText(QString::number(packet.latitude()));
    ui->lineEdit_pressure->setText(QString::number(packet.pressure()));
    ui->lineEdit_bmp180_temperature->setText(QString::number(packet.bmp180_temperature() / 10.0));
    ui->lineEdit_internal_temperature->setText(QString::number(packet.temperature_internal()));
    ui->lineEdit_external_temperature->setText(QString::number(packet.temperature_external()));
    ui->lineEdit_voltage->setText(QString::number(packet.voltage()));
    ui->lineEdit_radiation->setText(QString::number(packet.radiation()));

    if (packet.has_ds18b20_temperature1()) {
        ui->lineEdit_ds18b20_temperature1->setText(
                    QString::number(ConvertDS18B20Temperature(packet.ds18b20_temperature1())));
    }
    if (packet.has_ds18b20_temperature2()) {
        ui->lineEdit_ds18b20_temperature2->setText(
                    QString::number(ConvertDS18B20Temperature(packet.ds18b20_temperature2())));
    }
    if (packet.has_ds18b20_temperature3()) {
        ui->lineEdit_ds18b20_temperature3->setText(
                    QString::number(ConvertDS18B20Temperature(packet.ds18b20_temperature3())));
    }
    if (packet.has_ds18b20_temperature4()) {
        ui->lineEdit_ds18b20_temperature4->setText(
                    QString::number(ConvertDS18B20Temperature(packet.ds18b20_temperature4())));
    }

    if (packet.has_acceleration_x()) {
        ui->lineEdit_accelerometer_x->setText(
                    QString::number(packet.acceleration_x()));
    }
    if (packet.has_acceleration_y()) {
        ui->lineEdit_accelerometer_y->setText(
                    QString::number(packet.acceleration_y()));
    }
    if (packet.has_acceleration_z()) {
        ui->lineEdit_accelerometer_z->setText(
                    QString::number(packet.acceleration_z()));
    }

    if (packet.has_gyroscope_x()) {
        ui->lineEdit_gyroscope_x->setText(
                    QString::number(packet.gyroscope_x()));
    }
    if (packet.has_gyroscope_y()) {
        ui->lineEdit_gyroscope_y->setText(
                    QString::number(packet.gyroscope_y()));
    }
    if (packet.has_gyroscope_z()) {
        ui->lineEdit_gyroscope_z->setText(
                    QString::number(packet.gyroscope_z()));
    }

    mapWidget->addCoordinate(packet.longitude(), packet.latitude());
    if (ui->checkBox->isChecked()) {
        mapWidget->centerOn(Marble::GeoDataCoordinates(packet.longitude(), packet.latitude(), 0, Marble::GeoDataCoordinates::Degree));
    }

    for (int i = 0; i < ui->listWidget->count(); ++i) {
        QListWidgetItem* item = ui->listWidget->item(i);
        if (packet.status() & (1 << i)) {
            item->setBackgroundColor(Qt::green);
        } else {
            item->setBackgroundColor(Qt::red);
        }
    }

    mapWidget->update();
    redrawPlots();
    //addPlot(customPlot, packets, "Time, ms", "Height, m", {std::make_pair(&TelemetryPacket::packet_id, &TelemetryPacket::altitude)});
    //addPlot<double, double>(customPlot, packets, "Time, ms", "Height, m", {QPair(&TelemetryPacketWrapper::packet_id, &TelemetryPacketWrapper::altitude)});
    //addPlot(customPlot2, packets, "Time, ms", "Internal and external temperature, C",
    //{std::make_pair(&TelemetryPacket::packet_id, &TelemetryPacket::temperature_internal),
    // std::make_pair(&TelemetryPacket::packet_id, &TelemetryPacket::temperature_external)});
    //addPlot(customPlot3, packets, "Time, ms", "Pressure, Ps", {std::make_pair(&TelemetryPacket::packet_id, &TelemetryPacket::pressure)});
    //addPlot(customPlot4, packets, "Time, ms", "Voltage, V", {std::make_pair(&TelemetryPacket::packet_id, &TelemetryPacket::voltage)});
}
