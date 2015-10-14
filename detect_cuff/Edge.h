//
//  Edge.h
//  detect_cuff
//
//  Created by tsukky on 2015/10/13.
//  Copyright (c) 2015年 tsukky. All rights reserved.
//

#ifndef detect_cuff_Edge_h
#define detect_cuff_Edge_h

#include<array>

using namespace std;

class Edge {
public:
    Edge() : p1(0), p2(1), is_cuff(true), fn(0) {}
    Edge(const int _p1, const int _p2, const int _fn) : p1(_p1), p2(_p2), fn(_fn) {
        is_cuff = true;
    }
    ~Edge() {}
    
    int p1, p2;
    bool is_cuff;
    int fn;
};



#endif
