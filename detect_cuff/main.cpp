//
//  main.cpp
//  detect_cuff
//
//  Created by tsukky on 2015/10/13.
//  Copyright (c) 2015年 tsukky. All rights reserved.
//

#include <iostream>
#include "picojson.h"
#include <string>
#include <sstream>
#include <fstream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Triangle.h"
#include "vector"
#include "Edge.h"
#include "objLoader.h"

using namespace std;
using namespace picojson;

bool sameEdge(Edge &edge1, Edge &edge2) {
    if (edge1.p1 == edge2.p1 && edge1.p2 == edge2.p2) return true;
    if (edge1.p1 == edge2.p2 && edge1.p2 == edge2.p1) return true;
    return false;
}

int main(int argc, const char * argv[]) {
    
    string filename = "md_pants.obj";
    
    vector<glm::vec3> verts;
    vector<glm::vec3> normals;
    vector<unsigned int> faces;
    
    bool res = sapobj::loadOBJ(filename.c_str(), verts, faces, normals);

    vector<Edge> edges;
    vector<Edge> cuff_edges;
    
    for(int n = 0; n < faces.size(); n+=3){
        
        int p1 = faces[n];
        int p2 = faces[n+1];
        int p3 = faces[n+2];
        int i = n/3;
        Edge edge1 = Edge(p1, p2, i);
        Edge edge2 = Edge(p2, p3, i);
        Edge edge3 = Edge(p1, p3, i);
        edges.push_back(edge1);
        edges.push_back(edge2);
        edges.push_back(edge3);
        
    }
    
    // cuffの要素の抽出
    bool check;
    for (int i = 0; i < edges.size(); i++) {
        check = false;
        for (int j = 0; j < cuff_edges.size(); j++) {
            if (sameEdge(cuff_edges[j], edges[i])) {
                cuff_edges.erase(cuff_edges.begin() + j);
                check = true;
                break;
            }
        }
        if (check) continue;
        cuff_edges.push_back(edges[i]);
    }
    
    // edgesの作成
    vector<Edge> pure_edges;
    bool check2;
    for (int i = 0; i < edges.size(); i++) {
        check2 = false;
        for (int j = 0; j < pure_edges.size(); j++) {
            if (sameEdge(pure_edges[j], edges[i])) {
                check2 = true;
                break;
            }
        }
        if (check2) continue;
        pure_edges.push_back(edges[i]);
    }
    
    // cuffのグルーピング
    vector<vector<Edge>> cuffs;
    
    while (cuff_edges.size() != 0) {
        
        vector<Edge> cuff;
        int first_num = cuff_edges[0].p1;
        int search_num = cuff_edges[0].p2;
        cuff.push_back(cuff_edges[0]);
        cuff_edges.erase(cuff_edges.begin());
        
        // 最初の辺につながったら
        while (search_num != first_num) {
            
            for (int i = 0; i < cuff_edges.size(); i++) {
                if (search_num == cuff_edges[i].p1) {
                    search_num = cuff_edges[i].p2;
                    cuff.push_back(cuff_edges[i]);
                    cuff_edges.erase(cuff_edges.begin() + i);
                    break;
                } else if (search_num == cuff_edges[i].p2) {
                    search_num = cuff_edges[i].p1;
                    cuff.push_back(cuff_edges[i]);
                    cuff_edges.erase(cuff_edges.begin() + i);
                    break;
                }
            }
        }
        cuffs.push_back(cuff);
    }
    
    for (vector<Edge> cuff : cuffs) {
        cout << cuff.size() << endl;
    }
    
    vector<float> averages;
    vector<pair<float, int>> tmp_averages;
    vector<float> maxs;
    vector<float> tmp_maxs;
    vector<float> mins;
    vector<float> tmp_mins;
    vector<vector<Edge>> real_cuffs;
    
    for (int i = 0; i < cuffs.size(); i++) {
        float sum = 0.0;
        float average;
        float max = -100.0;
        float min = 1000.0;
        for (int j = 0; j < cuffs[i].size(); j++) {
            float p1 = verts[cuffs[i][j].p1].y;
            float p2 = verts[cuffs[i][j].p2].y;
            sum += p1;
            sum += p2;
            if (max < p1) max = p1;
            if (max < p2) max = p2;
            if (min > p1) min = p1;
            if (min > p2) min = p1;
        }
        average = sum / (2*cuffs[i].size());
        tmp_averages.push_back(make_pair(average, i));
        tmp_maxs.push_back(max);
        tmp_mins.push_back(min);
    }
    
    sort(tmp_averages.begin(), tmp_averages.end(), greater<pair<float, int>>());
    
    
    for (pair<float, int> ave : tmp_averages) {
        averages.push_back(ave.first);
        int i = ave.second;
        maxs.push_back(tmp_maxs[i]);
        mins.push_back(tmp_mins[i]);
        real_cuffs.push_back(cuffs[i]);
    }
    
    stringstream ss;
    ifstream inf;
    ofstream outf;
    
    
    // 服群を読み込む。ない場合は0を入れる(これはまだ)
    outf.open(filename.c_str(), ios::app);
    if (!outf.is_open()) return 1;
    
//    for (int i = 0; i < edges.size(); i++) {
//        outf << "edge " << edges[i].p1 << " " << edges[i].p2 << endl;
//    }
    
    for (int i = 0; i < real_cuffs.size(); i++) {
        for (int j = 0; j < real_cuffs[i].size(); j++) {
            outf << "c" << i << " " << real_cuffs[i][j].p1 << " " << real_cuffs[i][j].p2 << endl;
        }
    }
    
    for (int i = 0; i < mins.size(); i++) {
        outf << "mi " << mins[i] << endl;
    }
    
    for (int i = 0; i < maxs.size(); i++) {
        outf << "ma " << maxs[i] << endl;
    }
    
    for (int i = 0; i < averages.size(); i++) {
        outf << "av " << averages[i] << endl;
    }
    
    
    return 0;
}
