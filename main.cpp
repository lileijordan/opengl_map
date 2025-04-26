#include <iostream>

#include "vector_tile_reader.hpp"
#include "task.hpp"
#include <vector>

#include <any>
#include <tuple>
#include <string>
#include <memory>
#include <sstream>
#include <fstream>

#include <mutex>
#include <condition_variable>
#include <thread>
#include <queue>
#include <chrono>

#include <spdlog/spdlog.h>

#include "feature.hpp"
#include "Map.hpp"

int main()
{

    std::mutex mtx;
    std::condition_variable cv;
    std::shared_ptr<std::queue<std::tuple<unsigned int, unsigned int, unsigned int>>> tile_queue =
        std::make_shared<std::queue<std::tuple<unsigned int, unsigned int, unsigned int>>>();

    std::shared_ptr<std::queue<std::tuple<unsigned int, unsigned int, unsigned int, std::shared_ptr<std::vector<Feature>>>>> data_queue = std::make_shared<std::queue<std::tuple<unsigned int, unsigned int, unsigned int, std::shared_ptr<std::vector<Feature>>>>>();

    Map map(&mtx, data_queue);

    std::thread t(request_tile_task, &mtx, &cv, tile_queue, data_queue);
    t.detach();

    std::thread t2([&]
                   {
        while(true)
        {

             map.update_data();
 

            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        } });

    t2.detach();

    mtx.lock();
    tile_queue->push(std::make_tuple(12, 18, 6));
    mtx.unlock();
    cv.notify_one();

    unsigned int x, y, z;

    while (true)
    {
        std::cin >> x;
        std::cin >> y;
        std::cin >> z;

        mtx.lock();
        tile_queue->push(std::make_tuple(x, y, z));
        mtx.unlock();
        cv.notify_one();
    }

    return 0;
}