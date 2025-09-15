#include <iostream>
#include <vector>
#include <cmath>
#include <limits>
#include "bits/stdc++.h"
using namespace std;

struct Vector2 {
    double x, y;
    Vector2(double x = 0, double y = 0) : x(x), y(y) {}
    Vector2 operator-(const Vector2& v) const { return Vector2(x - v.x, y - v.y); }
    Vector2 operator+(const Vector2& v) const { return Vector2(x + v.x, y + v.y); }
    Vector2 operator*(double t) const { return Vector2(x * t, y * t); }
    double dot(const Vector2& v) const { return x * v.x + y * v.y; }
    double length() const { return sqrt(x * x + y * y); }
};

struct Ray {
    Vector2 origin, direction;
    Ray(Vector2 origin, Vector2 direction) : origin(origin), direction(direction) {}
};

struct Object {
    virtual double intersect(const Ray& ray) const = 0;
};

struct Circle : public Object {
    Vector2 center;
    double radius;
    Circle(Vector2 center, double radius) : center(center), radius(radius) {}
    double intersect(const Ray& ray) const override {
        Vector2 oc = ray.origin - center;
        double a = ray.direction.dot(ray.direction);
        double b = 2.0 * oc.dot(ray.direction);
        double c = oc.dot(oc) - radius * radius;
        double discriminant = b * b - 4 * a * c;
        if (discriminant < 0) return -1;
        else return (-b - sqrt(discriminant)) / (2.0 * a);
    }
};

struct Line : public Object {
    Vector2 p1, p2;
    Line(Vector2 p1, Vector2 p2) : p1(p1), p2(p2) {}
    double intersect(const Ray& ray) const override {
        Vector2 v1 = ray.origin - p1;
        Vector2 v2 = p2 - p1;
        Vector2 v3 = Vector2(-ray.direction.y, ray.direction.x);

        double dot = v2.dot(v3);
        if (fabs(dot) < 1e-6) return -1;

        double t1 = (v2.x * v1.y - v2.y * v1.x) / dot;
        double t2 = v1.dot(v3) / dot;

        if (t1 >= 0 && t2 >= 0 && t2 <= 1) {
            return t1;
        }

        return -1;
    }
};

int main() {
    // freopen("0.in", "r", stdin);
    // freopen("0.out", "w", stdout);

    ifstream inFile("0.in");
    ofstream outFile("0.out");

    if (!inFile) {
        cerr << "Failed to open input file!" << endl;
        return 1;
    }

    if (!outFile) {
        cerr << "Failed to open output file!" << endl;
        return 1;
    }

    int N;
    cin >> N;
    cout << "N: " << N << endl;

    vector<Object*> objects;
    for (int i = 0; i < N; ++i) {
        string type;
        cin >> type;
        if (type == "CIRCLE") {
            double x, y, r;
            cin >> x >> y >> r;
            objects.push_back(new Circle(Vector2(x, y), r));
        } else if (type == "LINE") {
            double x1, y1, x2, y2;
            cin >> x1 >> y1 >> x2 >> y2;
            objects.push_back(new Line(Vector2(x1, y1), Vector2(x2, y2)));
        }
    }

    int M;
    cin >> M;
    cout << "M: " << M << endl;

    for (int i = 0; i < M; ++i) {
        double ox, oy, dx, dy;
        cin >> ox >> oy >> dx >> dy;
        Ray ray(Vector2(ox, oy), Vector2(dx, dy));

        double minDist = numeric_limits<double>::max();
        for (const auto& obj : objects) {
            double dist = obj->intersect(ray);
            if (dist > 0 && dist < minDist) {
                minDist = dist;
            }
        }

        if (minDist == numeric_limits<double>::max()) {
            cout << -1 << endl;
        } else {
            cout << minDist << endl;
        }
    }

    for (auto obj : objects) {
        delete obj;
    }

    return 0;
}
