#ifndef MYMARBLEWIDGET
#define MYMARBLEWIDGET

#include <QApplication>
#include <marble/MarbleWidget.h>
#include <marble/GeoPainter.h>
#include <marble/GeoDataLineString.h>

using namespace Marble;

class MyMarbleWidget : public MarbleWidget
{
    public:
    MyMarbleWidget(QWidget* parent = 0) : MarbleWidget(parent) {

    }

    void addCoordinate(double longitude, double latitude) {
        coordinates.push_back(GeoDataCoordinates(longitude, latitude, 0, Marble::GeoDataCoordinates::Degree));
    }

    virtual void customPaint(GeoPainter* painter);
private:
    QVector<GeoDataCoordinates> coordinates;
};

void MyMarbleWidget::customPaint(GeoPainter* painter) {
    GeoDataLineString shapeNoTessellation( NoTessellation );
    for (int i = 0; i + 1 < coordinates.size(); ++i) {
        shapeNoTessellation << coordinates[i] << coordinates[i + 1];
    }

    QPen pen;
    pen.setWidth(3);
    pen.setColor(Qt::red);
    painter->setPen(pen);
    painter->drawPolyline( shapeNoTessellation );
}

#endif // MYMARBLEWIDGET

