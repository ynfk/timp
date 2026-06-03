#ifndef GRADIENTDESCENT_H
#define GRADIENTDESCENT_H

struct GDResult {
    double xMin;
    double fMin;
    int iterations;
    bool converged;
};

GDResult gradientDescent(double (*func)(double), double (*deriv)(double), double startX, double learningRate = 0.01, double tolerance = 1e-6, int maxIterations = 1000);
double funcQuadratic(double x);
double derivQuadratic(double x);
double funcCubic(double x);
double derivCubic(double x);

#endif
