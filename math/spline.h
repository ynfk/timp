#ifndef SPLINE_H
#define SPLINE_H

#include <QVector>
#include <QPointF>
#include <QString>

struct SplineResult {
    QVector<double> x;
    QVector<double> y;
    bool success;
    QString error;
};

SplineResult interpolateCubicSpline(const QVector<QPointF>& points, int outputCount = 100);
bool saveSplineToCSV(const SplineResult& result, const QString& path);
QVector<QPointF> loadPointsFromCSV(const QString& path);

#endif
