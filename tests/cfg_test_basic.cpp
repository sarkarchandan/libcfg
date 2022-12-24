#include <filesystem>
#include <unordered_map>

#include <catch2/catch_test_macros.hpp>

#include "cfg.hpp"
#include "types.hpp"

SCENARIO("config must be read from valid config file")
{
    GIVEN("a valid file path")
    {
        // Path to config file needs to be adjusted keeping in mind that when we execute test from build dir
        // that would be the context of the executable binary.
        const std::filesystem::path t_config_path = "../../tests/test_config_basic.yaml";
        THEN("config base api can be obtained")
        {
            REQUIRE(cfg::GetConfig_From(std::filesystem::absolute(t_config_path)).has_value());
        }
    }
    GIVEN("an invalid file path")
    {
        const std::filesystem::path invalid_path = "invalid";
        THEN("config base api cannot be obtained")
        {
            REQUIRE_FALSE(cfg::GetConfig_From(invalid_path).has_value());
        }
    }
}

SCENARIO("config cannot be read using empty or invalid key")
{
    GIVEN("a config base api obtained from valid file path")
    {
        const std::filesystem::path t_config_path = "../../tests/test_config_basic.yaml";
        const cfg::ConfigBase base = cfg::GetConfig_From(std::filesystem::absolute(t_config_path)).value();
        WHEN("empty key is used")
        {
            const std::string EMPTY_KEY = "";
            THEN("config value cannot be obtained")
            {
                REQUIRE_FALSE(base.Get<double>(EMPTY_KEY).has_value());
            }
        }
        WHEN("invalid key is used")
        {
            const std::string FIRST_LEVEL_INVALID_KEY = "invalid";
            const std::string SECOND_LEVEL_INVALID_KEY = "attributes.invalid";
            const std::string THIRD_LEVEL_INVALID_KEY = "road.color.invalid";
            const std::string ALL_LEVELS_INVALID = "all.levels.invalid";
            THEN("config value cannot be obtained")
            {
                REQUIRE_FALSE(base.Get<cfg::Vec3I>(SECOND_LEVEL_INVALID_KEY).has_value());
                REQUIRE_FALSE(base.Get<double>(THIRD_LEVEL_INVALID_KEY).has_value());
                REQUIRE_FALSE(base.Get<int>(ALL_LEVELS_INVALID).has_value());
            }
        }
    }
}

SCENARIO("config cannot be read when value is malformed")
{
    GIVEN("a config base api obtained from valid file path")
    {
        const std::filesystem::path t_config_path = "../../tests/test_config_basic.yaml";
        const cfg::ConfigBase base = cfg::GetConfig_From(std::filesystem::absolute(t_config_path)).value();
        WHEN("value is malformed")
        {
            const std::string ERROR_MALFORMED = "error.malformed";
            THEN("config can not be obtained")
            {
                REQUIRE_FALSE(base.Get<cfg::Vec3I>(ERROR_MALFORMED).has_value());
            }
        }
    }
}

SCENARIO("config can be read from valid config file and keys")
{
    GIVEN("a config base api obtained from valid file path")
    {
        const std::filesystem::path t_config_path = "../../tests/test_config_basic.yaml";
        const cfg::ConfigBase base = cfg::GetConfig_From(std::filesystem::absolute(t_config_path)).value();
        WHEN("valid key is used")
        {
            // Define test key configurations
            const std::string PI = "pi";
            const std::string ATTRIBUTES_NAME = "attributes.name";
            const std::string ATTRIBUTES_DEBUG = "attributes.debug";
            const std::string ATTRIBUTES_POINT = "attributes.point";
            const std::string ATTRIBUTES_RGB = "attributes.rgb";
            const std::string ATTRIBUTES_NAMES = "attributes.names";
            const std::string ROAD_DIMS_LENGTH = "road.dims.length";
            const std::string ROAD_DIMS_WIDTH = "road.dims.width";
            const std::string ROAD_DIMS_HEIGHT = "road.dims.height";
            const std::string ROAD_COLOR_HUE = "road.color.hue";
            const std::string ROAD_COLOR_SATURATION = "road.color.saturation";
            const std::string ROAD_COLOR_VALUE = "road.color.value";
            THEN("config value can be obtained")
            {
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
                const cfg::Vec3D e_point_xyz = {2.3, 5.2, 5.9};
                const cfg::Vec3I e_rgb = {255, 255, 255};
                const cfg::Vec3Str e_names = {"tom", "dick", "harry"};
                const std::string e_attr_name = "some name";
                const bool e_attr_debug = true;
                // Validate simple non-nested configuration
                REQUIRE(base.Get<double>(PI).has_value());
                REQUIRE(base.Get<double>(PI).value() == e_val_map[PI]);
                // Validate single nested configuration
                REQUIRE(base.Get<std::string>(ATTRIBUTES_NAME).has_value());
                REQUIRE(base.Get<std::string>(ATTRIBUTES_NAME).value() == e_attr_name);
                REQUIRE(base.Get<bool>(ATTRIBUTES_DEBUG).has_value());
                REQUIRE(base.Get<bool>(ATTRIBUTES_DEBUG).value() == e_attr_debug);
                // Validate double nested configuration
                REQUIRE(base.Get<double>(ROAD_DIMS_LENGTH).has_value());
                REQUIRE(base.Get<double>(ROAD_DIMS_LENGTH).value() == e_val_map[ROAD_DIMS_LENGTH]);
                REQUIRE(base.Get<double>(ROAD_DIMS_WIDTH).has_value());
                REQUIRE(base.Get<double>(ROAD_DIMS_WIDTH).value() == e_val_map[ROAD_DIMS_WIDTH]);
                REQUIRE(base.Get<double>(ROAD_DIMS_HEIGHT).has_value());
                REQUIRE(base.Get<double>(ROAD_DIMS_HEIGHT).value() == e_val_map[ROAD_DIMS_HEIGHT]);
                REQUIRE(base.Get<double>(ROAD_COLOR_HUE).has_value());
                REQUIRE(base.Get<double>(ROAD_COLOR_HUE).value());
                REQUIRE(base.Get<double>(ROAD_COLOR_SATURATION).has_value());
                REQUIRE(base.Get<double>(ROAD_COLOR_SATURATION).value() == e_val_map[ROAD_COLOR_SATURATION]);
                REQUIRE(base.Get<double>(ROAD_COLOR_VALUE).has_value());
                REQUIRE(base.Get<double>(ROAD_COLOR_VALUE).value() == e_val_map[ROAD_COLOR_VALUE]);
                // Validate vector configuration
                REQUIRE(base.Get<cfg::Vec3D>(ATTRIBUTES_POINT).has_value());
                REQUIRE(base.Get<cfg::Vec3D>(ATTRIBUTES_POINT).value() == e_point_xyz);
                REQUIRE(base.Get<cfg::Vec3I>(ATTRIBUTES_RGB).has_value());
                REQUIRE(base.Get<cfg::Vec3I>(ATTRIBUTES_RGB).value() == e_rgb);
                REQUIRE(base.Get<cfg::Vec3Str>(ATTRIBUTES_NAMES).has_value());
                REQUIRE(base.Get<cfg::Vec3Str>(ATTRIBUTES_NAMES).value() == e_names);
            }
        }
    }
}
