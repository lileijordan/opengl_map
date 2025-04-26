#pragma once
#include <mutex>
#include <condition_variable>
#include <memory>
#include <queue>
#include <tuple>
#include <string>
#include <iostream>
#include <vector>

#include "feature.hpp"

void request_tile_task(std::mutex *mtx,
                       std::condition_variable *cv,
                       std::shared_ptr<std::queue<std::tuple<unsigned int,
                                                             unsigned int, unsigned int>>>
                           tile_queue,
                       std::shared_ptr<std::queue<std::tuple<unsigned int, unsigned int, unsigned int,std::shared_ptr<std::vector<Feature>>>>>
                           data_queue)
;


