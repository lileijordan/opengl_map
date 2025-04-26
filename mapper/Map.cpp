#include "Map.hpp"
#include <iostream>
#include <tuple>
#include <string>
#include <queue>

// Map::Map(float longitude,float latitude,float zoomLevel,float pitchLevel = 0.f,float bearingLevel =0.f)
// :lng(longitude),lat(latitude),zoom(zoomLevel),pitch(pitchLevel),bearing(bearingLevel)
// {

// }

void Map::update_data(){
    std::lock_guard<std::mutex> lock(*pmtx);

    if (this->data_queue->size() > 0)
    {
        auto [x, y, z, features] = this->data_queue->front();
        this->data_queue->pop();

        std::cout << "x = " << x << ", y = " << y << ", z = " << z << std::endl;
    }
     
}
