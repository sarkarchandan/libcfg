#include <filesystem>
#include <unordered_map>

#include <catch2/catch_test_macros.hpp>

#include "cfg.hpp"
#include "types.hpp"

// Path to config file needs to be adjusted keeping in mind that when we execute test from build dir that would be
// the context of the executable binary.
const std::filesystem::path t_config_path = "../../tests/test_config.yaml";

TEST_CASE("Configurations cannot be read from invalid config file")
{
    try
    {
        cfg::ConfigBase cfg_base = std::filesystem::absolute("invalid path");
    }
    catch (std::runtime_error const &re)
    {
        REQUIRE(static_cast<std::string>(re.what()) == "invalid config file");
    }
}

TEST_CASE("Configurations cannot be read using empty key")
{
    cfg::ConfigBase cfg_base = {std::filesystem::absolute(t_config_path)};
    REQUIRE_THROWS_AS(cfg_base.get<double>(""), std::runtime_error);
}

TEST_CASE("Configurations cannot be read using invalid key")
{
    cfg::ConfigBase cfg_base = {std::filesystem::absolute(t_config_path)};
    REQUIRE_THROWS_AS(cfg_base.get<double>("invalid_key"), std::runtime_error);
}

TEST_CASE("Configurations can be read from valid config file")
{
    cfg::ConfigBase cfg_base = {std::filesystem::absolute(t_config_path)};
    // Define test key configurations
    const std::string PI = "pi";
    const std::string ATTRIBUTES_NAME = "attributes_name";
    const std::string ATTRIBUTES_DEBUG = "attributes_debug";
    const std::string ATTRIBUTES_POINT = "attributes_point";
    const std::string ATTRIBUTES_RGB = "attributes_rgb";
    const std::string ATTRIBUTES_FLAGS = "attributes_flags";
    const std::string ATTRIBUTES_NAMES = "attributes_names";
    const std::string ROAD_DIMS_LENGTH = "road_dims_length";
    const std::string ROAD_DIMS_WIDTH = "road_dims_width";
    const std::string ROAD_DIMS_HEIGHT = "road_dims_height";
    const std::string ROAD_COLOR_HUE = "road_color_hue";
    const std::string ROAD_COLOR_SATURATION = "road_color_saturation";
    const std::string ROAD_COLOR_VALUE = "road_color_value";

    // Define test values
    std::unordered_map<std::string, double> e_val_map = {
        {PI, 3.14159},
        {ROAD_DIMS_LENGTH, 50.},
        {ROAD_DIMS_WIDTH, 12.},
        {ROAD_DIMS_HEIGHT, 5.1},
        {ROAD_COLOR_HUE, 0.2},
        {ROAD_COLOR_SATURATION, 0.2},
        {ROAD_COLOR_VALUE, 0.2},
    };
    cfg::Vec3d e_point_xyz = {2.3, 5.2, 5.9};
    cfg::Vec3i e_rgb = {255, 255, 255};
    cfg::Vec3b e_flags = {true, false, true};
    cfg::Vec3Str e_names = {"tom", "dick", "harry"};
    std::string e_attr_name = "some name";
    bool e_attr_debug = true;
    // Validate simple configuration
    REQUIRE(cfg_base.get<double>(PI) == e_val_map[PI]);
    // Validate single nested configuration
    REQUIRE(cfg_base.get<std::string>(ATTRIBUTES_NAME) == e_attr_name);
    REQUIRE(cfg_base.get<bool>(ATTRIBUTES_DEBUG) == e_attr_debug);
    // Validate vector configuration
    REQUIRE(cfg_base.get<cfg::Vec3d>(ATTRIBUTES_POINT) == e_point_xyz);
    REQUIRE(cfg_base.get<cfg::Vec3i>(ATTRIBUTES_RGB) == e_rgb);
    REQUIRE(cfg_base.get<cfg::Vec3b>(ATTRIBUTES_FLAGS) == e_flags);
    REQUIRE(cfg_base.get<cfg::Vec3Str>(ATTRIBUTES_NAMES) == e_names);
    // Validate double nested configuration
    REQUIRE(cfg_base.get<double>(ROAD_DIMS_LENGTH) == e_val_map[ROAD_DIMS_LENGTH]);
    REQUIRE(cfg_base.get<double>(ROAD_DIMS_WIDTH) == e_val_map[ROAD_DIMS_WIDTH]);
    REQUIRE(cfg_base.get<double>(ROAD_DIMS_HEIGHT) == e_val_map[ROAD_DIMS_HEIGHT]);
    REQUIRE(cfg_base.get<double>(ROAD_COLOR_HUE) == e_val_map[ROAD_COLOR_HUE]);
    REQUIRE(cfg_base.get<double>(ROAD_COLOR_SATURATION) == e_val_map[ROAD_COLOR_SATURATION]);
    REQUIRE(cfg_base.get<double>(ROAD_COLOR_VALUE) == e_val_map[ROAD_COLOR_VALUE]);
}
