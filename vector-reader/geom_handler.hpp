#pragma once

#include <vector>
#include <vtzero/vector_tile.hpp>
#include <memory>

#include "feature.hpp"



class GeomHandler
{
public:
    std::shared_ptr<Feature> sfeature;

    GeomHandler() = default;

    GeomHandler(std::shared_ptr<Feature> ft):sfeature(ft){

    }

    ~GeomHandler() = default;
    GeomType type;
    
    void points_begin(const uint32_t)  noexcept
    {
        type = GeomType::POINT;
    }

    void points_point(const vtzero::point point);

    void points_end() const noexcept
    {
    }

    void linestring_begin(const uint32_t)  noexcept
    {
        type = LINE;
        tmp_coord_ptr = std::make_shared<std::vector<int>>();
    }

    void linestring_point(const vtzero::point point);
    void linestring_end()  noexcept
    {
        sfeature->coords.push_back(std::move(*tmp_coord_ptr));

        tmp_coord_ptr.reset();
    }

    void ring_begin(const uint32_t)  noexcept
    {
        type = POLYGON;

        if (tmp_coord_ptr == nullptr)
        {
            tmp_coord_ptr = std::make_shared<std::vector<int>>();
        }
    }
    void ring_point(const vtzero::point point);
    void ring_end(const vtzero::ring_type rt)  noexcept
    {
        sfeature->coords.push_back(std::move(*tmp_coord_ptr));
        tmp_coord_ptr.reset();
    }

private:
    std::shared_ptr<std::vector<int>> tmp_coord_ptr;

    // std::shared_ptr<std::vector<std::vector<int>>> coords;



};