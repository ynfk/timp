#include "spline.h"
#include <QFile>
#include <QTextStream>
#include <cmath>
#include <algorithm>

SplineResult interpolateCubicSpline(const QVector<QPointF>& points, int outputCount) {
    SplineResult result;

    if (points.size() < 2) {
        result.success = false;
        result.error = "Need at least 2 points";
        return result;
    }

    // Сортируем точки по X
    QVector<QPointF> sorted = points;
    std::sort(sorted.begin(), sorted.end(),
              [](const QPointF& a, const QPointF& b) { return a.x() < b.x(); });

    int n = sorted.size() - 1;  // количество интервалов

    if (n < 1) {
        result.success = false;
        result.error = "Invalid points";
        return result;
    }

    QVector<double> x(n + 1), y(n + 1);
    for (int i = 0; i <= n; i++) {
        x[i] = sorted[i].x();
        y[i] = sorted[i].y();
    }

    // Вычисляем коэффициенты сплайна
    QVector<double> a(n), b(n), c(n + 1), d(n);

    // Копируем y в a
    for (int i = 0; i < n; i++) {
        a[i] = y[i];
    }

    // Шаги
    QVector<double> h(n);
    for (int i = 0; i < n; i++) {
        h[i] = x[i + 1] - x[i];
    }

    // Прогонка для natural spline
    QVector<double> alpha(n);
    for (int i = 1; i < n; i++) {
        alpha[i] = (3.0 / h[i]) * (y[i + 1] - y[i]) - (3.0 / h[i - 1]) * (y[i] - y[i - 1]);
    }

    QVector<double> l(n + 1), mu(n + 1), z(n + 1);
    l[0] = 1.0;
    mu[0] = 0.0;
    z[0] = 0.0;

    for (int i = 1; i < n; i++) {
        l[i] = 2.0 * (x[i + 1] - x[i - 1]) - h[i - 1] * mu[i - 1];
        mu[i] = h[i] / l[i];
        z[i] = (alpha[i] - h[i - 1] * z[i - 1]) / l[i];
    }

    l[n] = 1.0;
    z[n] = 0.0;
    c[n] = 0.0;

    for (int j = n - 1; j >= 0; j--) {
        c[j] = z[j] - mu[j] * c[j + 1];
        b[j] = (y[j + 1] - y[j]) / h[j] - h[j] * (c[j + 1] + 2.0 * c[j]) / 3.0;
        d[j] = (c[j + 1] - c[j]) / (3.0 * h[j]);
    }

    // Генерируем выходные точки
    double xMin = x[0];
    double xMax = x[n];
    double step = (xMax - xMin) / (outputCount - 1);

    for (int i = 0; i < outputCount; i++) {
        double xVal = xMin + i * step;
        result.x.append(xVal);

        // Находим интервал
        int interval = 0;
        for (int j = 0; j < n; j++) {
            if (xVal >= x[j] && xVal <= x[j + 1]) {
                interval = j;
                break;
            }
        }

        double dx = xVal - x[interval];
        double yVal = a[interval] + b[interval] * dx + c[interval] * dx * dx + d[interval] * dx * dx * dx;
        result.y.append(yVal);
    }

    result.success = true;
    return result;
}

bool saveSplineToCSV(const SplineResult& result, const QString& path) {
    QFile file(path);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) return false;

    QTextStream out(&file);
    out << "x,y\n";
    for (int i = 0; i < result.x.size(); i++) {
        out << result.x[i] << "," << result.y[i] << "\n";
    }
    return true;
}

QVector<QPointF> loadPointsFromCSV(const QString& path) {
    QVector<QPointF> points;
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) return points;

    QTextStream in(&file);
    bool isFirstLine = true;

    while (!in.atEnd()) {
        QString line = in.readLine().trimmed();
        if (isFirstLine) {
            isFirstLine = false;
            continue;
        }
        if (line.isEmpty()) continue;

        QStringList parts = line.split(',');
        if (parts.size() >= 2) {
            bool xOk, yOk;
            double x = parts[0].toDouble(&xOk);
            double y = parts[1].toDouble(&yOk);
            if (xOk && yOk) {
                points.append(QPointF(x, y));
            }
        }
    }
    return points;
}
