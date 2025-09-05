#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <discpp.h>

using namespace std;

int NRHS;
const int p = 8, NZ = 2;
const float EPS = 0.0001;

const int n = 600, m = n + 1;
float xout[m] = { NAN }, yout1[m] = { NAN }, yout2[m] = { NAN }, testout[m] = { NAN };
float miny, maxy, ystep;

const int maxX = 100;

void runExplicitEuler(int, float, float, float[], float[], float[]);
void runImplicitEuler(int, float, float, float[], float[], const float[]);
void computeMinMax(int, const float[], const float[], const float[], float&, float&, float&);

void explicitEuler(float&, float, float[]);
void implicitEuler(float&, float, float[]);
void deriv(float, float[], float[]);

bool checkConvergence(float, float, float); // Функция для проверки сходимости

void runRungeKutta(int, float, float, float[], float[]);
void rungeKuttaStep(float&, float, float[]);
void plotHyperbola();


int main() {

    runExplicitEuler(n, 0.0, 1.0, xout, yout1, testout);
    runImplicitEuler(n, 0.0, 1.0, xout, yout2, testout);
    computeMinMax(m, yout1, yout2, testout, miny, maxy, ystep);


    float youtRK[m] = { NAN };
    runRungeKutta(n, 0.0, 1.0, xout, youtRK);

    float diffExplicit = fabs(youtRK[n] - yout1[n]);
    float diffImplicit = fabs(youtRK[n] - yout2[n]);


    cout << setprecision(8) << "\nFinal Y-difference between explEuler and RungeKutta: " << diffExplicit
        << (diffExplicit < EPS ? " OK" : " Too large") << endl;

    cout << "\nFinal Y-difference between implEuler and RungeKutta: " << diffImplicit
        << (diffImplicit < EPS ? " OK" : " Too large") << endl;

    plotHyperbola();

    return EXIT_SUCCESS;
}

void runExplicitEuler(int n, float xbeg, float xend, float xout[], float yout1[], float testout[]) {
    float x = xbeg;
    float y[NZ] = { 1.0, 0.5 };  // y1(0) = 1, y2(0) = 0.5
    float h = (xend - xbeg) / n;

    xout[0] = x;
    yout1[0] = y[0];
    NRHS = 0;

    cout << "EULER EXPLICIT SCHEME, with h = " << h << endl;
    cout << "FOR X = " << setw(p) << fixed << setprecision(4) << x
        << ", Y = " << setw(p) << y[0] << endl;

    for (int i = 0; i < n; ++i) {
        explicitEuler(x, h, y);

        // Проверка сходимости на каждом шаге
        if (checkConvergence(y[0], yout1[i], EPS)) {
            cout << "Convergence reached at step " << i << ", X = " << x << ", Y = " << y[0] << endl;
            break; // Выход из цикла, если сходимость достигнута
        }

        xout[i + 1] = x;
        yout1[i + 1] = y[0];
        cout << "FOR X = " << setw(p) << x
            << ", Y = " << y[0] << endl;
    }

    cout << "NRHS = " << NRHS << endl;
}

void runImplicitEuler(int n, float xbeg, float xend, float xout[], float yout2[], const float testout[]) {
    float x = xbeg;
    float y[NZ] = { 1.0, 0.5 };  // y1(0) = 1, y2(0) = 0.5
    float h = (xend - xbeg) / n;

    xout[0] = x;
    yout2[0] = y[0];
    NRHS = 0;

    cout << "\nEULER IMPLICIT SCHEME, with h = " << h << endl;
    cout << "FOR X = " << setw(p) << fixed << setprecision(4) << x
        << ", Y = " << setw(p) << y[0] << endl;

    for (int i = 0; i < n; ++i) {
        implicitEuler(x, h, y);

        // Проверка сходимости на каждом шаге
        if (checkConvergence(y[0], yout2[i], EPS)) {
            cout << "Convergence reached at step " << i << ", X = " << x << ", Y = " << y[0] << endl;
            break; // Выход из цикла, если сходимость достигнута
        }

        xout[i + 1] = x;
        yout2[i + 1] = y[0];
        cout << "FOR X = " << setw(p) << x
            << ", Y = " << y[0] << endl;
    }

    cout << "NRHS = " << NRHS << endl;
}

void computeMinMax(int m, const float yout1[], const float yout2[], const float testout[],
    float& miny, float& maxy, float& ystep) {
    miny = maxy = yout1[0];

    for (int i = 0; i < m; ++i) {
        miny = fmin(miny, fmin(yout1[i], fmin(yout2[i], testout[i])));
        maxy = fmax(maxy, fmax(yout1[i], fmax(yout2[i], testout[i])));
    }

    ystep = (maxy - miny) / 10.0;
}

void explicitEuler(float& x, float h, float y[]) {
    float dydx[NZ], yprev[NZ];
    deriv(x, y, dydx);

    for (int i = 0; i < NZ; i++) {
        yprev[i] = y[i];
        y[i] = yprev[i] + h * dydx[i];
    }

    x += h;
}

void implicitEuler(float& x, float h, float y[]) {
    float yprev[NZ], f1[NZ], f2[NZ], dydx[NZ];
    deriv(x, y, dydx);
    for (int i = 0; i < NZ; i++) {
        yprev[i] = y[i];
        f1[i] = dydx[i];
        y[i] = yprev[i] + h * f1[i];
    }

    x += h;
    deriv(x, y, dydx);
    for (int i = 0; i < NZ; i++) {
        f2[i] = dydx[i];
        y[i] = yprev[i] + h * (f1[i] + f2[i]) / 2.0;
    }
}


void deriv(float x, float y[], float dydx[]) {
    float num = y[0] * y[0];
    float denom = y[1] - x;

    if (fabs(denom) < EPS) denom = (denom >= 0 ? EPS : -EPS);

    NRHS += 1;

    dydx[0] = num / denom;
    dydx[1] = y[0] + 1.0;
}

bool checkConvergence(float ycurrent, float yprev, float eps) {
    return fabs(ycurrent - yprev) < eps;  // если разница меньше eps
}

void runRungeKutta(int n, float xbeg, float xend, float xout[], float yout[]) {
    float x = xbeg;
    float y[NZ] = { 1.0, 0.5 }; // начальные условия
    float h = (xend - xbeg) / n;

    xout[0] = x;
    yout[0] = y[0];

    cout << "\nRUNGE-KUTTA 4TH ORDER, with h = " << h << endl;
    cout << "FOR X = " << setw(p) << fixed << setprecision(4) << x
        << ", Y = " << setw(p) << y[0] << endl;

    for (int i = 0; i < n; ++i) {
        rungeKuttaStep(x, h, y);
        xout[i + 1] = x;
        yout[i + 1] = y[0];

        cout << "FOR X = " << setw(p) << x
            << ", Y = " << y[0] << endl;
    }
}

void rungeKuttaStep(float& x, float h, float y[]) {
    float k1[NZ], k2[NZ], k3[NZ], k4[NZ], yt[NZ];
    float x_half = x + h / 2.0, x_full = x + h;

    deriv(x, y, k1);
    for (int i = 0; i < NZ; ++i)
        yt[i] = y[i] + h * k1[i] / 2.0;

    deriv(x_half, yt, k2);
    for (int i = 0; i < NZ; ++i)
        yt[i] = y[i] + h * k2[i] / 2.0;

    deriv(x_half, yt, k3);
    for (int i = 0; i < NZ; ++i)
        yt[i] = y[i] + h * k3[i];

    deriv(x_full, yt, k4);

    for (int i = 0; i < NZ; ++i)
        y[i] += h / 6.0 * (k1[i] + 2 * k2[i] + 2 * k3[i] + k4[i]);

    x = x_full;
}

void plotHyperbola() {
    float x[maxX], y[maxX];

    for (int i = 0; i < maxX; ++i) {
        x[i] = i + 1;
        y[i] = 1.0f / (i + 1);
    }

    Dislin d;
    d.metafl("XWIN");
    d.disini();
    d.pagera();
    d.complx();

    d.name("X", "X");
    d.name("Y", "Y");
    d.titlin("График гиперболы y=1/x", 1);

    // Уменьшаем количество подписей по оси X
    float xStart = 1.0f;
    float xEnd = float(maxX);
    float xStep = 10.0f;  // Увеличиваем шаг по X

	d.height(35); // Размер шрифта

    d.graf(xStart, xEnd, xStart, xStep, 0.0, 1.0, 0.0, 0.1);

    d.color("BLUE");
    d.curve(x, y, maxX);

    d.disfin();
}