#include "earcut_proxy.hpp"
#include <vector>
#include <array>

#include "earcut.hpp"

#include <iostream>

#include <sstream>

std::vector<std::uint32_t> trianglize(const std::vector<float>& coords)
{
    std::vector<std::vector<std::array<float, 2>>> polygons;

    polygons.push_back({});
    polygons.push_back({});

    // std::stringstream ss;
    // ss <<"\"linestring(";

    // 先不考虑环的问题
    for(int i=0;i<coords.size();i+=2)
    {
        polygons[0].push_back({coords[i], coords[i+1]});

        // std::cout << "polygon[0].push({" << std::to_string(coords[i])<<","<<std::to_string(coords[i+1]) <<"});" << std::endl;

        // if (i > 0){
        //     ss << ",";
        // }

        // ss << std::to_string(coords[i]);
        // ss <<" ";
        // ss << std::to_string(coords[i+1]);

    }

    // ss <<")\"";

    // std::cout << ss.str() << std::endl;

    std::vector<std::uint32_t> indices = mapbox::earcut<std::uint32_t>(polygons);

    return indices;
}


std::vector<std::uint32_t> trianglize(const std::vector<int>& coords)
{
    std::vector<std::vector<std::array<int, 2>>> polygons;

    polygons.push_back({});
    polygons.push_back({});

    // 先不考虑环的问题
    for(int i=0;i<coords.size();i+=2)
    {
        polygons[0].push_back({coords[i], coords[i+1]});
    }

    std::vector<std::uint32_t> indices = mapbox::earcut<std::uint32_t>(polygons);

    return indices;
}