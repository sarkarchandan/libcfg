#include <filesystem>
#include <unordered_map>

#include <catch2/catch_test_macros.hpp>

#include "cfg.hpp"
#include "types.hpp"

// Path to config file needs to be adjusted keeping in mind that when we execute test from build dir that would be
// the context of the executable binary.
const std::filesystem::path t_config_path = "../../tests/test_config.yaml";

TEST_CASE("config must be read from valid config file")
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

TEST_CASE("config cannot be read using empty or invalid key")
{
    cfg::ConfigBase cfg_base = {std::filesystem::absolute(t_config_path)};
    const std::string EMPTY_KEY = "";
    const std::string FIRST_LEVEL_INVALID_KEY = "invalid";
    const std::string SECOND_LEVEL_INVALID_KEY = "attributes_invalid";
    const std::string THIRD_LEVEL_INVALID_KEY = "road_color_invalid";
    const std::string ALL_LEVELS_INVALID = "all_levels_invalid";
    REQUIRE_FALSE(cfg_base.Get<double>("").has_value());
    REQUIRE_FALSE(cfg_base.Get<cfg::Vec3i>(SECOND_LEVEL_INVALID_KEY).has_value());
    REQUIRE_FALSE(cfg_base.Get<double>(THIRD_LEVEL_INVALID_KEY).has_value());
    REQUIRE_FALSE(cfg_base.Get<int>(ALL_LEVELS_INVALID).has_value());
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
    // Validate simple non-nested configuration
    REQUIRE(cfg_base.Get<double>(PI).has_value());
    REQUIRE(cfg_base.Get<double>(PI).value() == e_val_map[PI]);
    // Validate single nested configuration
    REQUIRE(cfg_base.Get<std::string>(ATTRIBUTES_NAME).has_value());
    REQUIRE(cfg_base.Get<std::string>(ATTRIBUTES_NAME).value() == e_attr_name);
    REQUIRE(cfg_base.Get<bool>(ATTRIBUTES_DEBUG).has_value());
    REQUIRE(cfg_base.Get<bool>(ATTRIBUTES_DEBUG).value() == e_attr_debug);
    // Validate double nested configuration
    REQUIRE(cfg_base.Get<double>(ROAD_DIMS_LENGTH).has_value());
    REQUIRE(cfg_base.Get<double>(ROAD_DIMS_LENGTH).value() == e_val_map[ROAD_DIMS_LENGTH]);
    REQUIRE(cfg_base.Get<double>(ROAD_DIMS_WIDTH).has_value());
    REQUIRE(cfg_base.Get<double>(ROAD_DIMS_WIDTH).value() == e_val_map[ROAD_DIMS_WIDTH]);
    REQUIRE(cfg_base.Get<double>(ROAD_DIMS_HEIGHT).has_value());
    REQUIRE(cfg_base.Get<double>(ROAD_DIMS_HEIGHT).value() == e_val_map[ROAD_DIMS_HEIGHT]);
    REQUIRE(cfg_base.Get<double>(ROAD_COLOR_HUE).has_value());
    REQUIRE(cfg_base.Get<double>(ROAD_COLOR_HUE).value());
    REQUIRE(cfg_base.Get<double>(ROAD_COLOR_SATURATION).has_value());
    REQUIRE(cfg_base.Get<double>(ROAD_COLOR_SATURATION).value() == e_val_map[ROAD_COLOR_SATURATION]);
    REQUIRE(cfg_base.Get<double>(ROAD_COLOR_VALUE).has_value());
    REQUIRE(cfg_base.Get<double>(ROAD_COLOR_VALUE).value() == e_val_map[ROAD_COLOR_VALUE]);
    // Validate vector configuration
    REQUIRE(cfg_base.Get<cfg::Vec3d>(ATTRIBUTES_POINT).has_value());
    REQUIRE(cfg_base.Get<cfg::Vec3d>(ATTRIBUTES_POINT).value() == e_point_xyz);
    REQUIRE(cfg_base.Get<cfg::Vec3i>(ATTRIBUTES_RGB).has_value());
    REQUIRE(cfg_base.Get<cfg::Vec3i>(ATTRIBUTES_RGB).value() == e_rgb);
    REQUIRE(cfg_base.Get<cfg::Vec3b>(ATTRIBUTES_FLAGS).has_value());
    REQUIRE(cfg_base.Get<cfg::Vec3b>(ATTRIBUTES_FLAGS).value() == e_flags);
    REQUIRE(cfg_base.Get<cfg::Vec3Str>(ATTRIBUTES_NAMES).has_value());
    REQUIRE(cfg_base.Get<cfg::Vec3Str>(ATTRIBUTES_NAMES).value() == e_names);
}
