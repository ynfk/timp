#include "gradientdescent.h"
#include <cmath>

GDResult gradientDescent(double (*func)(double), double (*deriv)(double), double startX, double learningRate, double tolerance, int maxIterations) {
    GDResult result;
    double x = startX;
    for (int i = 0; i < maxIterations; i++) {
        double grad = deriv(x);
        double newX = x - learningRate * grad;
        if (std::abs(newX - x) < tolerance) {
            result.xMin = newX;
            result.fMin = func(newX);
            result.iterations = i + 1;
            result.converged = true;
            return result;
        }
        x = newX;
    }
    result.xMin = x;
    result.fMin = func(x);
    result.iterations = maxIterations;
    result.converged = false;
    return result;
}

double funcQuadratic(double x) { return x * x; }
double derivQuadratic(double x) { return 2 * x; }
double funcCubic(double x) { return x * x * x - 2 * x * x + x; }
double derivCubic(double x) { return 3 * x * x - 4 * x + 1; }
