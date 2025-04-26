#pragma once

#include <glm/mat4x4.hpp>
#include "feature.hpp"

#include <memory>
#include <queue>
#include <mutex>
#include <tuple>


class Map
{
public:
    Map() = delete;
    Map(const Map &) = delete;
    Map &operator=(const Map &) = delete;
    Map(Map &&) = delete;

    ~Map() = default;

    // Map(float longitude, float latitude, float zoomLevel, float pitchLevel, float bearingLevel);

    Map(std::mutex * mtx,
    std::shared_ptr<std::queue<std::tuple<unsigned int, unsigned int, unsigned int,std::shared_ptr<std::vector<Feature>>>>> p_data_queue)
        :  pmtx(mtx), data_queue(p_data_queue)
    {
    }


    void set_mutex(std::mutex * mtx){
        this->pmtx = mtx;
    }

    void set_position(float longitude, float latitude)
    {
        lng = longitude;
        lat = latitude;
    }

    void set_zoom(float zoomLevel)
    {
        zoom = zoomLevel;
    }

    void set_pitch(float pitchLevel)
    {
        pitch = pitchLevel;
    }
    void set_bearing(float bearingLevel)
    {
        bearing = bearingLevel;
    }

    const glm::mat4 get_view() const
    {
        return view;
    }
    const glm::mat4 get_projection() const
    {
        return projection;
    }

    void update_data();

private:
    float lng;
    float lat;
    float zoom;

    float pitch;
    float bearing;

    glm::mat4 view;
    glm::mat4 projection;

    std::mutex * pmtx;

    std::shared_ptr<std::queue<std::tuple<unsigned int, unsigned int, unsigned int,std::shared_ptr<std::vector<Feature>>>>> data_queue;
};