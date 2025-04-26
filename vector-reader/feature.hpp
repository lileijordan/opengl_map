#pragma once

#include <string>
#include <vector>
#include <map>
#include <any>


enum GeomType
{
    POINT = 1,
    LINE = 2,
    POLYGON = 3
};

class Feature
{
    public:
    GeomType gtype;
    std::string layer_name;
    std::string feature_id;
    std::map<std::string, std::any> properties;
    std::vector<std::vector<int>> coords;

    std::vector<std::vector<float>> coords_buffer;
    std::vector<std::uint32_t> indices;

    Feature() = default;
    Feature(const Feature &) = default;
    Feature(Feature && f){
        
    }


    private:


};