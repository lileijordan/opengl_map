#include "geom_handler.hpp"

#include <vector>
#include <vtzero/vector_tile.hpp>
#include <memory>

void GeomHandler::points_point(const vtzero::point point)
{
    sfeature->coords.push_back({point.x, point.y});
}

void GeomHandler::linestring_point(const vtzero::point point)
{
    tmp_coord_ptr->push_back(point.x);
    tmp_coord_ptr->push_back(point.y);
}

void GeomHandler::ring_point(const vtzero::point point){
    tmp_coord_ptr->push_back(point.x);
    tmp_coord_ptr->push_back(point.y);
}