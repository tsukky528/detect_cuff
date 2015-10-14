/*
 * LICENCE
 * copyright 2015 ~ ****
 * Some rights reserved.
 * Author: TSUKIYAMA
 * Released under CC BY
 */
#pragma once

#include<array>

using namespace std;

class Triangle {
public:
    Triangle() : p1(0), p2(1), p3(2) {}
    Triangle(const int _p1, const int _p2, const int _p3) : p1(_p1), p2(_p2), p3(_p3) {
        edges[0][0] = p1;
        edges[0][1] = p1;
        edges[1][0] = p1;
        edges[1][1] = p1;
        edges[2][0] = p1;
        edges[2][1] = p1;
        
    }
    ~Triangle() {}
    
    int p1, p2, p3;
    
    array<array<int, 2>, 3> edges;
};

