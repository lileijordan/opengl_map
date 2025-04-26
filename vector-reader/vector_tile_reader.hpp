#ifndef __VECTOR_TILE_READER_HPP__
#define __VECTOR_TILE_READER_HPP__

#include <string>
#include "geom_handler.hpp"
#include <vector>
#include "feature.hpp"

class VectorTileReader {
public:

    VectorTileReader() = default;
    ~VectorTileReader() = default;

    std::shared_ptr<std::vector<Feature>> parse_vector_tile(const std::string& tile_data);


private:
    
};


#endif