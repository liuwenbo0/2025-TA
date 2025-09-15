#include <iostream>
#include <vector>
#include <string>
#include <random>
#include <iomanip>
#include "windows.h"

// 使用命名空间以简化代码
using namespace std;

// 定义常量范围和特征开关
const double COORD_MIN = -1000.0;
const double COORD_MAX = 1000.0;
const double RADIUS_MIN = 1.0;
const double RADIUS_MAX = 100.0;

// 特征开关
const bool GENERATE_OVERLAPPING_CIRCLES = true;  // 生成部分重叠的圆
const bool GENERATE_INTERSECTING_RAYS = true;   // 生成与多个物体相交的光线
const bool GENERATE_NON_INTERSECTING_RAYS = false;  // 生成不与任何物体相交的光线

// 随机数生成器
mt19937 rng(42); // 固定种子以确保可重复性
uniform_real_distribution<double> dist_coord(COORD_MIN, COORD_MAX);
uniform_real_distribution<double> dist_radius(RADIUS_MIN, RADIUS_MAX);
uniform_int_distribution<int> dist_type(0, 1); // 0: CIRCLE, 1: LINE
uniform_real_distribution<double> dist_dir(-1.0, 1.0);

// 函数生成一个随机方向向量，确保不是零向量
pair<double, double> generate_direction() {
    double dx, dy;
    do {
        dx = dist_dir(rng);
        dy = dist_dir(rng);
    } while (abs(dx) < 1e-6 && abs(dy) < 1e-6);
    return {dx, dy};
}

int main() {
    SetConsoleOutputCP(CP_UTF8);

    // 设置输出精度
    cout << fixed << setprecision(6);
    
    // 生成物体数量 N (1 ≤ N ≤ 10000)
    uniform_int_distribution<int> dist_N(1, 10000);
    int N = dist_N(rng);
    cout << N << endl;
    
    // 生成 N 个物体
    for(int i = 0; i < N; ++i) {
        int type = dist_type(rng);
        if(type == 0) { // CIRCLE
            double x, y, r;
            if (GENERATE_OVERLAPPING_CIRCLES && i < N / 2) {
                // 生成部分重叠的圆
                x = dist_coord(rng) / 10;  // 把圆的中心约束在较小的范围内，以保证部分重叠
                y = dist_coord(rng) / 10;
                r = dist_radius(rng);
            } else {
                // 正常生成的圆
                x = dist_coord(rng);
                y = dist_coord(rng);
                r = dist_radius(rng);
            }
            cout << "CIRCLE " << x << " " << y << " " << r << "\n";
        }
        else { // LINE
            double x1 = dist_coord(rng);
            double y1 = dist_coord(rng);
            double x2 = dist_coord(rng);
            double y2 = dist_coord(rng);
            // 确保线段的两个端点不重合
            while(abs(x1 - x2) < 1e-6 && abs(y1 - y2) < 1e-6) {
                x2 = dist_coord(rng);
                y2 = dist_coord(rng);
            }
            cout << "LINE " << x1 << " " << y1 << " " << x2 << " " << y2 << "\n";
        }
    }
    
    // 生成光线数量 M (1 ≤ M ≤ 10000)
    uniform_int_distribution<int> dist_M(1, 10000);
    int M = dist_M(rng);
    M = min(1000000 / N, M);
    cout << M << endl;
    
    // 生成 M 条光线
    for(int i = 0; i < M; ++i) {
        double ox, oy, dx, dy;
        if (GENERATE_INTERSECTING_RAYS && i < M / 2) {
            // 生成与多个物体相交的光线
            ox = dist_coord(rng) / 10;  // 确保光线起点位于物体的中心附近
            oy = dist_coord(rng) / 10;
            pair<double, double> dir = generate_direction();
            dx = dir.first;
            dy = dir.second;
        } else if (GENERATE_NON_INTERSECTING_RAYS && i >= M / 2) {
            // 生成不与任何物体相交的光线
            ox = COORD_MAX + dist_coord(rng);  // 将光线起点放在远离物体的位置
            oy = COORD_MAX + dist_coord(rng);
            pair<double, double> dir = generate_direction();
            dx = dir.first;
            dy = dir.second;
        } else {
            // 正常生成光线
            ox = dist_coord(rng);
            oy = dist_coord(rng);
            pair<double, double> dir = generate_direction();
            dx = dir.first;
            dy = dir.second;
        }
        cout << ox << " " << oy << " " << dx << " " << dy << "\n";
    }
    
    return 0;
}
