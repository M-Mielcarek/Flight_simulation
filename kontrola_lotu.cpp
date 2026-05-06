#include <iostream>
#include <fstream>
#include <cmath>
#include <iomanip>
#include <vector>
/*-------------------------------------------------
Symulator wiatru i lotu samolotu.
W pliku wiatr.txt jest siatka predkosci wiatru,
W pliku samolot.txt sa dane o kursie samolotu.
-------------------------------------------------*/
struct Vec2 {
    float x, y;
};

class Wnd_Generator {
private:
    const float A = 200.0f;
    const float B = 200.0f;

    const int Nw = 40;
    const int Nk = 40;

    const float alpha = 2.0f;
    const float beta = 10.0f;

public:
    float Odleglosc(float x1, float y1, float x2, float y2);
    float wiatrScalar(float r);
    Vec2 wiatrVector(float x, float y);
    Vec2 CorrectCourse(Vec2 from, Vec2 to, Vec2 wiatr, float Vs_samolot);

    void SavewiatrGrid();
    void AnalyzeLot();
};

float Wnd_Generator::Odleglosc(float x1, float y1, float x2, float y2) {
    return std::sqrt((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1));
}

float Wnd_Generator::wiatrScalar(float r) {
    float raw = std::pow(r, alpha) * std::exp(-r / beta);
    float rMax = alpha * beta;
    float maxVal = std::pow(rMax, alpha) * std::exp(-rMax / beta);

    return 100.0f * raw / maxVal;
}

Vec2 Wnd_Generator::wiatrVector(float x, float y) {
    float Cx = A / 2.0f;
    float Cy = B / 2.0f;

    float dx = x - Cx;
    float dy = y - Cy;

    float r = std::sqrt(dx * dx + dy * dy);
    float V = wiatrScalar(r);

    float wx = -dy;
    float wy = dx;

    float len = std::sqrt(wx * wx + wy * wy);

    if (len == 0)
        return {0, 0};

    wx = wx / len * V;
    wy = wy / len * V;

    return {wx, wy};
}

Vec2 Wnd_Generator::CorrectCourse(Vec2 from, Vec2 to, Vec2 wiatr, float Vs_samolot) {
    float dx = to.x - from.x;
    float dy = to.y - from.y;

    float odleglosc = std::sqrt(dx * dx + dy * dy);

    float ux = dx / odleglosc;
    float uy = dy / odleglosc;

    float vsx = ux * Vs_samolot - wiatr.x;
    float vsy = uy * Vs_samolot - wiatr.y;

    float len = std::sqrt(vsx * vsx + vsy * vsy);

    vsx = vsx / len * Vs_samolot;
    vsy = vsy / len * Vs_samolot;

    return {vsx, vsy};
}

void Wnd_Generator::SavewiatrGrid() {
    std::ofstream file("wiatr.txt");

    float stepX = A / Nw;
    float stepY = B / Nk;

    file << std::fixed << std::setprecision(1);

    for (int i = 0; i < Nw; i++) {
        for (int j = 0; j < Nk; j++) {

            float x = i * stepX;
            float y = j * stepY;

            float Cx = A / 2.0f;
            float Cy = B / 2.0f;

            float r = Odleglosc(x, y, Cx, Cy);
            float V = wiatrScalar(r);

            file << std::setw(6) << V;
        }
        file << "\n";
    }

    file.close();
}

void Wnd_Generator::AnalyzeLot() {
    std::ofstream file("samolot.txt");

    std::vector<Vec2> route = {{10, 10},{21, 37},{60, 50},{120, 140},{180, 180}};

    float Vs_samolot = 50.0f;

    float rMax = alpha * beta;

    file << "Maksymalna predkosc wiatru = 100\n";
    file << "Maksymalne r = " << rMax << "\n\n";

    for (size_t i = 0; i < route.size() - 1; i++) {

        Vec2 from = route[i];
        Vec2 to = route[i + 1];

        float odleglosc = Odleglosc(from.x, from.y, to.x, to.y);

        Vec2 wiatr = wiatrVector(from.x, from.y);
        float wiatrKat = atan2(wiatr.y, wiatr.x);

        Vec2 samolot = CorrectCourse(from, to, wiatr, Vs_samolot);
        float samolotKat = atan2(samolot.y, samolot.x);

        file << "Odcinek nr " << i + 1 << "\n";

        file << "Dystans = " << odleglosc << "\n";

        file << "Wektor wiatru = (" << wiatr.x << ", " << wiatr.y << ")\n";

        file << "Azymut wiatru= " << wiatrKat << "\n";

        file << "Wektor samolotu = (" << samolot.x << ", " << samolot.y << ")\n";

        file << "Kurs samolotu (kat) = " << samolotKat << "\n\n";
    }

    file.close();
}

int main() {
    Wnd_Generator WG;

    WG.SavewiatrGrid();
    WG.AnalyzeLot();

    return 0;
}