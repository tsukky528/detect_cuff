//
//  objLoader.h
//  objLoader
//
//  Created by tsukky on 2015/12/03.
//  Copyright (c) 2015年 tsukky. All rights reserved.
//

#ifndef objLoader_objLoader_h
#define objLoader_objLoader_h

#include <vector>
#include <stdio.h>
#include <string>
#include <cstring>
#include <sstream>
#include <fstream>
#include <iostream>

#include <glm/glm.hpp>

namespace sapobj {
    bool loadOBJ(
                 const char * path,
                 std::vector<glm::vec3> & out_vertices,
                 std::vector<unsigned int> & out_faces,
                 std::vector<glm::vec3> & out_normals
                 ){
        printf("Loading OBJ file %s...\n", path);
        
        std::vector<unsigned int> vertexIndices, uvIndices, normalIndices;
        std::vector<glm::vec3> temp_vertices;
        std::vector<unsigned int> temp_faces;
        std::vector<glm::vec3> temp_normals;
        
        
        FILE * file = fopen(path, "r");
        if( file == NULL ){
            printf("Impossible to open the file ! Are you in the right path ? See Tutorial 1 for details\n");
            getchar();
            return false;
        }
        
        while( 1 ){
            
            char lineHeader[128];
            // read the first word of the line
            int res = fscanf(file, "%s", lineHeader);
            if (res == EOF)
                break; // EOF = End Of File. Quit the loop.
            
            // else : parse lineHeader
            
            if ( strcmp( lineHeader, "v" ) == 0 ){
                glm::vec3 vertex;
                fscanf(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z );
                out_vertices.push_back(vertex);
//            }else if ( strcmp( lineHeader, "vt" ) == 0 ){
//                glm::vec2 uv;
//                fscanf(file, "%f %f\n", &uv.x, &uv.y );
//                uv.y = -uv.y; // Invert V coordinate since we will only use DDS texture, which are inverted. Remove if you want to use TGA or BMP loaders.
//                temp_uvs.push_back(uv);
            }else if ( strcmp( lineHeader, "vn" ) == 0 ){
                glm::vec3 normal;
                fscanf(file, "%f %f %f\n", &normal.x, &normal.y, &normal.z );
                out_normals.push_back(normal);
            }else if ( strcmp( lineHeader, "f" ) == 0 ){
                std::string vertex1, vertex2, vertex3;
                unsigned int vertexIndex[3], uvIndex[3], normalIndex[3];
                int matches = fscanf(file, "%d/%d/%d %d/%d/%d %d/%d/%d\n", &vertexIndex[0], &uvIndex[0], &normalIndex[0], &vertexIndex[1], &uvIndex[1], &normalIndex[1], &vertexIndex[2], &uvIndex[2], &normalIndex[2] );
                if (matches != 9){
                    printf("File can't be read by our simple parser :-( Try exporting with other options\n");
                    return false;
                }
                out_faces.push_back(vertexIndex[0]-1);
                out_faces.push_back(vertexIndex[1]-1);
                out_faces.push_back(vertexIndex[2]-1);
//                vertexIndices.push_back(vertexIndex[0]);
//                vertexIndices.push_back(vertexIndex[1]);
//                vertexIndices.push_back(vertexIndex[2]);
//                uvIndices    .push_back(uvIndex[0]);
//                uvIndices    .push_back(uvIndex[1]);
//                uvIndices    .push_back(uvIndex[2]);
//                normalIndices.push_back(normalIndex[0]);
//                normalIndices.push_back(normalIndex[1]);
//                normalIndices.push_back(normalIndex[2]);
            }else{
                // Probably a comment, eat up the rest of the line
//                std::cout << "ddd" << std::endl;
                char stupidBuffer[1000];
                fgets(stupidBuffer, 1000, file);
            }
            
        }
        
        return true;
    }
    
    bool loadOBJ2(
                 std::stringstream& ss,
                 std::vector<glm::vec3> & out_vertices,
                 std::vector<unsigned int> & out_faces,
                 std::vector<glm::vec3> & out_normals
                 ){
        
        
        std::vector<unsigned int> vertexIndices, uvIndices, normalIndices;
        
        std::string str;
        
        while( getline(ss, str) ){
            
            char lineHeader[128];
            // read the first word of the line
            int res = sscanf(str.c_str(), "%s", lineHeader);
            
            if (res == EOF)
                break; // EOF = End Of File. Quit the loop.
            
            // else : parse lineHeader
            
            if ( strcmp( lineHeader, "v" ) == 0 ){
                glm::vec3 vertex;
                std::string s;
                sscanf(str.c_str(), "%s %f %f %f\n", &s, &vertex.x, &vertex.y, &vertex.z );
                out_vertices.push_back(vertex);
            }else if ( strcmp( lineHeader, "vn" ) == 0 ){
                glm::vec3 normal;
                std::string s;
                sscanf(str.c_str(), "%s %f %f %f\n", &s, &normal.x, &normal.y, &normal.z );
                out_normals.push_back(normal);
            }else if ( strcmp( lineHeader, "f" ) == 0 ){
                std::string vertex1, vertex2, vertex3;
                std::string s;
                unsigned int vertexIndex[3], uvIndex[3], normalIndex[3];
                int matches = sscanf(str.c_str(), "%s %d/%d/%d %d/%d/%d %d/%d/%d\n", &s, &vertexIndex[0], &uvIndex[0], &normalIndex[0], &vertexIndex[1], &uvIndex[1], &normalIndex[1], &vertexIndex[2], &uvIndex[2], &normalIndex[2] );
                out_faces.push_back(vertexIndex[0]);
                out_faces.push_back(vertexIndex[1]);
                out_faces.push_back(vertexIndex[2]);
            }
        }
        return true;
    }
    
    std::stringstream exportObj(std::stringstream &ss, std::vector<glm::vec3> &out_vertices) {
        std::stringstream result;
        
        std::string str;
        int max_verts = out_vertices.size();
        int i = 0;
//        std::cout << ss.str() << std::endl;
        ss.clear();
        ss.seekg(0);
        while( getline(ss, str) ){
            
            char lineHeader[128];
            // read the first word of the line
            int res = sscanf(str.c_str(), "%s", lineHeader);
            
            if (res == EOF)
                break; // EOF = End Of File. Quit the loop.
            
            // else : parse lineHeader
            
            if ( strcmp( lineHeader, "v" ) == 0 ){
                glm::vec3 vertex;
                std::string s;
                result << "v " << out_vertices[i].x << " " << out_vertices[i].y << " " << out_vertices[i].z << std::endl;
                out_vertices.push_back(vertex);
                i++;
            }else{
                result << str << std::endl;
            }
        }

        return result;
    }

}


#endif
