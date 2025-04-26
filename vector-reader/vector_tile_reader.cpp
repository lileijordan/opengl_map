#include "vector_tile_reader.hpp"
#include <iostream>
#include <vtzero/vector_tile.hpp>
#include <fstream>


std::shared_ptr<std::vector<Feature>> VectorTileReader::parse_vector_tile(const std::string& tile_data)
{
    std::shared_ptr<std::vector<Feature>> features = std::make_shared<std::vector<Feature>>();

    std::ofstream outfile("tile_data.mvt", std::ios::binary);
    outfile.write(tile_data.data(), tile_data.size());
    outfile.close();


    vtzero::vector_tile tile(tile_data.data(), tile_data.size());
    

    while (auto layer = tile.next_layer())
    {
        while (auto feature = layer.next_feature())
        {
            auto sfeature = std::make_shared<Feature>();
            sfeature->layer_name = std::string(layer.name());
            sfeature->feature_id = std::to_string(feature.id());
            sfeature->gtype = static_cast<GeomType>(feature.geometry_type());

            GeomHandler geom_handler(sfeature);
            // feature.visit(geom_handler);

            vtzero::decode_geometry(feature.geometry(), geom_handler);

            while (auto prop = feature.next_property())
            {
                std::string key = std::string(prop.key().data(),prop.key().size());

                // std::cout << "key = " << key << std::endl;

                auto value = prop.value();

                switch(value.type()){
                    case vtzero::property_value_type::string_value:
                        sfeature->properties[key] = std::string(value.string_value());
                        break;
                    case vtzero::property_value_type::float_value:
                        sfeature->properties[key] = value.float_value();
                        break;
                    case vtzero::property_value_type::double_value:
                        sfeature->properties[key] = value.double_value();
                        break;
                    case vtzero::property_value_type::int_value:
                        sfeature->properties[key] = value.int_value();
                        break;
                    case vtzero::property_value_type::uint_value:
                        sfeature->properties[key] = value.uint_value();
                        break;
                    case vtzero::property_value_type::sint_value:
                        sfeature->properties[key] = value.sint_value();
                        break;
                    case vtzero::property_value_type::bool_value:
                        sfeature->properties[key] = value.bool_value();
                        break;
                    default:
                        sfeature->properties[key] = std::string("unknown prop value");
                        break;
                }


            }
            // Handle the geometry type and coordinates


            features->push_back(*sfeature);
        }
    }

    return features;
}