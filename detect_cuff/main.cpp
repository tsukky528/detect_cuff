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

using namespace std;
using namespace picojson;

bool sameEdge(Edge &edge1, Edge &edge2) {
    if (edge1.p1 == edge2.p1 && edge1.p2 == edge2.p2) return true;
    if (edge1.p1 == edge2.p2 && edge1.p2 == edge2.p1) return true;
    return false;
}

int main(int argc, const char * argv[]) {
    
    string cloth0_path = "halfpants2.json";
    int num_verts;
    int num_faces;
    
    vector<glm::vec3> verts;
    vector<Triangle> faces;
    vector<Edge> edges;
    vector<Edge> cuff_edges;
    
    stringstream ss;
    ifstream inf;
    ofstream outf;

    
    // 服群を読み込む。ない場合は0を入れる(これはまだ)
    inf.open(cloth0_path.c_str(), ios::binary);
    if (!inf.is_open()) return 1;
    ss << inf.rdbuf();
    inf.close();
    
    value c0v;
    ss >> c0v;
    string err = get_last_error();
    if(!err.empty()) {
        cerr << err << endl;
        return -1;
    }
    
    picojson::array& cloth0VArray = c0v.get<object>()["vertices"].get<picojson::array>();
    picojson::array& cloth0FArray = c0v.get<object>()["faces"].get<picojson::array>();
    num_verts = (int) cloth0VArray.size() / 3;
    num_faces = (int) cloth0FArray.size() / 4;

    for(int m = 0; m < num_verts; m++){
        float x = cloth0VArray[m * 3].get<double>();
        float y = cloth0VArray[m * 3 + 1].get<double>();
        float z = cloth0VArray[m * 3 + 2].get<double>();
        verts.push_back(glm::vec3(x, y ,z));
    }
    
    for(int n = 1; n < cloth0FArray.size(); n+=4){
        int p1 = (int) cloth0FArray[n].get<double>();
        int p2 = (int) cloth0FArray[n+1].get<double>();
        int p3 = (int) cloth0FArray[n+2].get<double>();
        int fn = (n-1)/4;
        Edge edge1 = Edge(p1, p2, fn);
        Edge edge2 = Edge(p2, p3, fn);
        Edge edge3 = Edge(p1, p3, fn);
        edges.push_back(edge1);
        edges.push_back(edge2);
        edges.push_back(edge3);
        
        faces.push_back(Triangle(p1, p2, p3));
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
    
    cout << cuffs[0].size() << endl;
    cout << cuffs[1].size() << endl;
    cout << cuffs[2].size() << endl;
    
    object o;
    o = c0v.get<object>();
    
    // pure_edges要素追加
    picojson::array ar0;
    for (int i = 0; i < pure_edges.size(); i++) {
        ar0.push_back(value(float(pure_edges[i].p1)));
        ar0.push_back(value(float(pure_edges[i].p2)));
    }
    o.insert(make_pair("edges", value(ar0)));
    
    // cuff要素追加
    for (int i = 0; i < cuffs.size(); i++) {
        picojson::array ar;
        for (int j = 0; j < cuffs[i].size(); j++) {
            ar.push_back(value(float(cuffs[i][j].p1)));
            ar.push_back(value(float(cuffs[i][j].p2)));
        }
        string name = "cuff" + to_string(i);
        o.insert(make_pair(name, value(ar)));
    }
    
    value v(o);
    string resultString = v.serialize().c_str();
    cout << resultString << endl;
    
    ofstream ofs( cloth0_path, ios::out | ios::trunc );
    ofs << resultString << std::endl;
    
    return 0;
}
