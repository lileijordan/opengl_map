#include "task.hpp"
#include <mutex>
#include <condition_variable>
#include <memory>
#include <queue>
#include <tuple>
#include <string>
#include <iostream>

#include <curl/curl.h>
#include <sstream>

#include "feature.hpp"
#include "vector_tile_reader.hpp"

#include "geosutils.hpp"

#include <zlib.h>

#include <spdlog/spdlog.h>

#include "earcut_proxy.hpp"

#include <sstream>
#include <fstream>

int unzip_data(const std::vector<char> &compressed_data, std::vector<char> &uncompressed_data)
{

    z_stream strm;
    int ret;
    unsigned char out[4096];
    unsigned int have;

    strm.zalloc = Z_NULL;
    strm.zfree = Z_NULL;
    strm.opaque = Z_NULL;
    ret = inflateInit2(&strm, 16 + MAX_WBITS);
    if (ret != Z_OK)
        return ret;

    strm.avail_in = compressed_data.size();
    strm.next_in = (unsigned char *)compressed_data.data();

    do
    {
        strm.avail_out = sizeof(out);
        strm.next_out = out;
        ret = inflate(&strm, Z_NO_FLUSH);
        if (ret == Z_STREAM_ERROR)
            return ret;
        have = sizeof(out) - strm.avail_out;
        uncompressed_data.insert(uncompressed_data.end(), out, out + have);

    } while (strm.avail_out == 0);

    ret = inflateEnd(&strm);
    return ret;
}

// 用于处理 libcurl 写入的数据
size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
    size_t totalSize = size * nmemb;
    std::string *str = static_cast<std::string *>(userp);
    str->append(static_cast<char *>(contents), totalSize);
    return totalSize;
}

std::string https_get_request(const std::string &url)
{
    CURL *curl;
    CURLcode res;
    std::string response;

    curl_global_init(CURL_GLOBAL_DEFAULT); // 初始化全局环境
    curl = curl_easy_init();               // 初始化 CURL 对象

    if (curl)
    {
        // curl_easy_setopt(curl, CURLOPT_URL, "https://api.mapbox.com/v4/mapbox.mapbox-bathymetry-v2,mapbox.mapbox-streets-v8,mapbox.mapbox-terrain-v2,mapbox.mapbox-models-v1/15/16372/10894.vector.pbf?sku=101sAqin0jEw5&access_token=pk.eyJ1IjoibGlsZWlqb3JkYW4iLCJhIjoiY2luc2Z1a2UxMTEybnUya2pheDdwZjhxOSJ9._ENu7hjywKHQZMcj9S24vA"); // 设置 URL
        // https://webst01.is.autonavi.com/appmaptile?style=6&x=214373&y=107747&z=18

        // curl_easy_setopt(curl, CURLOPT_URL, "https://webst01.is.autonavi.com/appmaptile?style=6&x=214373&y=107747&z=18");

        // "https://tiles-a.basemaps.cartocdn.com/vectortiles/carto.streets/v1/10/164/396.mvt"
        // curl_easy_setopt(curl, CURLOPT_URL,"https://tiles-a.basemaps.cartocdn.com/vectortiles/carto.streets/v1/10/164/396.mvt");

        // std::cout <<"----" << std::endl;

        curl_easy_setopt(curl, CURLOPT_URL, url.data());

        // 如果你担心 SSL 证书问题（例如本地没根证书），可以临时关闭验证（生产环境不要这样）
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L); // 不验证对方证书
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L); // 不验证主机名

        // 设置写入回调
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

        // 执行请求
        res = curl_easy_perform(curl);
        if (res != CURLE_OK)
        {
            std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;
        }

        curl_easy_cleanup(curl); // 清理资源
    }

    curl_global_cleanup();

    return response;
}

std::string assemble_url(unsigned int x, unsigned int y, unsigned int z)
{
    std::stringstream oss;
    oss << "https://api.mapbox.com/v4/mapbox.mapbox-bathymetry-v2,mapbox.mapbox-streets-v8,mapbox.mapbox-terrain-v2,mapbox.mapbox-models-v1/" << std::to_string(z) << "/" << std::to_string(x) << "/" << std::to_string(y) << ".vector.pbf?sku=101DBxIy4T495&access_token=pk.eyJ1IjoibGlsZWlqb3JkYW4iLCJhIjoiY2luc2Z1a2UxMTEybnUya2pheDdwZjhxOSJ9._ENu7hjywKHQZMcj9S24vA";

    // oss << "https://api.maptiler.com/tiles/v3/"<<z <<"/"<<x <<"/"<<y <<".pbf?key=O7VbOY3zrXxBupgrQtdE&mtsid=b3f010d2-243b-4696-94ce-1a811b92cc3b";

    // std::cout << oss.str() << std::endl;

    return oss.str();
}

void request_tile_task(std::mutex *mtx, std::condition_variable *cv,
                       std::shared_ptr<std::queue<std::tuple<unsigned int,
                                                             unsigned int, unsigned int>>>
                           tile_queue,
                       std::shared_ptr<std::queue<std::tuple<unsigned int, unsigned int, unsigned int,std::shared_ptr<std::vector<Feature>>>>> data_queue)
{

    while (true)
    {

        spdlog::info("start task......");
        std::unique_lock<std::mutex> lock(*mtx);
        cv->wait(lock, [&]
                 { return tile_queue->size() > 0; });
        auto [x, y, z] = tile_queue->front();
        tile_queue->pop();
        lock.unlock();
        

        spdlog::info("get a http get request tile......");

        std::string data;

        spdlog::info("x = {},y = {}, z = {}",x,y,z);

        std::string mvt;

        if (x == 12 && y == 18 && z== 6){

            std::ifstream tile_tmp("/Users/leili/Downloads/test.pbf",std::ios::binary);

            std::stringstream ss;

            ss << tile_tmp.rdbuf();

            data = ss.str();

            tile_tmp.close();

            mvt = data;

        }else{

            std::string url = assemble_url(x, y, z);
            data = std::move(https_get_request(url));

                        // std::cout << "request data over..." << std::endl;
            spdlog::info("request data over...");
            std::vector<char> compress_data(data.begin(), data.end());
            std::vector<char> uncompress_data;
            int ret = unzip_data(compress_data, uncompress_data);

            mvt = std::string(uncompress_data.begin(),uncompress_data.end());

        }

        



        // std::string mvt(uncompress_data.begin(), uncompress_data.end());

        VectorTileReader vector_tile_reader;
        std::shared_ptr<std::vector<Feature>> features = vector_tile_reader.parse_vector_tile(mvt);

        spdlog::error("feature size = {}", features->size());


        for (auto &f : *features)
        {
            if (f.gtype == POINT || f.gtype == LINE)
            {

                auto vs = point_line_buffer(f, 10);
                //     f.coords_buffer.insert(f.coords_buffer.end(),
                //                            std::make_move_iterator(vs.begin()),
                //                            std::make_move_iterator(vs.end()));
                //       );

                //    vs.clear();

                auto indices = trianglize(vs);

                f.coords_buffer.push_back(std::move(vs));

                f.indices =std::move(indices);


                // spdlog::info("type = {},buffer size = {},index size = {}",f.gtype == POINT?1:2,f.coords_buffer.size(),f.indices.size());
            }else{
                auto indices = trianglize(f.coords[0]);

                f.indices =std::move(indices);


                // spdlog::info("type = {},polygon size = {},index size = {}",3,f.coords[0].size(),f.indices.size());

            }

        }


        mtx->lock();
        std::cout << std::this_thread::get_id()<<":" << features->size() << std::endl;
        data_queue->push(std::make_tuple(x, y, z, std::move(features)));
        mtx->unlock();

        spdlog::info("get a tile data over...");
    }
}