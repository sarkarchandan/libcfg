#include <filesystem>
#include <cstdint>

#include <catch2/catch_test_macros.hpp>

#include "types.hpp"
#include "cfg.hpp"

// Path to config file needs to be adjusted keeping in mind that when we execute test from build dir that would be
// the context of the executable binary.
const std::filesystem::path t_config_path = "../../tests/test_config_custom.yaml";

/// @brief Defines a custom three-element unsigned 8 bit integer vector type
struct Vec3U8 : public cfg::Vec<uint8_t, 3>
{
    Vec3U8(std::initializer_list<uint8_t> const &list)
        : cfg::Vec<uint8_t, 3>(list) {}

    Vec3U8()
        : cfg::Vec<uint8_t, 3>(std::vector<uint8_t>(3)) {}
};

/// @brief Defines a custom two-element 32 bit integer vector type
struct Vec2I32 : public cfg::Vec<int32_t, 2>
{
    Vec2I32(std::initializer_list<int32_t> const &list)
        : cfg::Vec<int32_t, 2>(list) {}

    Vec2I32()
        : cfg::Vec<int32_t, 2>(std::vector<int32_t>(2)) {}
};

/// @brief Implements (de)serialization for Vec3U8
template <>
struct YAML::convert<Vec3U8>
{
    static Node encode(Vec3U8 const &vec)
    {
        return convert<cfg::Vec<uint8_t, 3>>::encode(vec);
    }

    static bool decode(Node const &node, Vec3U8 &vec)
    {
        return convert<cfg::Vec<uint8_t, 3>>::decode(node, vec);
    }
};

/// @brief Implements (de)serialization for Vec2I32
template <>
struct YAML::convert<Vec2I32>
{
    static Node encode(Vec2I32 const &vec)
    {
        return convert<cfg::Vec<int32_t, 2>>::encode(vec);
    }

    static bool decode(Node const &node, Vec2I32 &vec)
    {
        return convert<cfg::Vec<int32_t, 2>>::decode(node, vec);
    }
};

SCENARIO("custom types can be used with sequence configurations")
{
    GIVEN("a valid config file")
    {
        REQUIRE(cfg::GetConfig_From(std::filesystem::absolute(t_config_path)).has_value());
        cfg::ConfigBase base = cfg::GetConfig_From(std::filesystem::absolute(t_config_path)).value();

        WHEN("a valid key for a custom uint8_t sequence is provided")
        {
            const std::string SEQUENCES_U8S = "sequences_u8s";
            REQUIRE(base.Get<Vec3U8>(SEQUENCES_U8S).has_value());
            THEN("config value should be correctly fetched")
            {
                const Vec3U8 e_rgb = {255, 255, 255};
                REQUIRE(base.Get<Vec3U8>(SEQUENCES_U8S).value() == e_rgb);
            }
        }
        WHEN("a valid key for a custom int32_t sequence is provided")
        {
            const std::string SEQUENCES_I32S = "sequences_i32s";
            REQUIRE(base.Get<Vec2I32>(SEQUENCES_I32S).has_value());
            THEN("config value should be correctly fetched")
            {
                const Vec2I32 e_vals = {-2147483648, 2147483647};
                REQUIRE(base.Get<Vec2I32>(SEQUENCES_I32S).value() == e_vals);
            }
        }
    }
}