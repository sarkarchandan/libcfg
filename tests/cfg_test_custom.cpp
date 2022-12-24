#include <filesystem>
#include <cstdint>

#include <catch2/catch_test_macros.hpp>

#include "types.hpp"
#include "cfg.hpp"

// Path to config file needs to be adjusted keeping in mind that when we execute test from build dir that would be
// the context of the executable binary.
const std::filesystem::path t_config_path = "../../tests/test_config_custom.yaml";

/// @brief Defines a custom uint8_t vector type
template <size_t len>
struct VecU8 : public cfg::Vec<uint8_t, len>
{
    VecU8(std::initializer_list<uint8_t> const &list)
        : cfg::Vec<uint8_t, len>(list) {}

    VecU8()
        : cfg::Vec<uint8_t, len>(std::vector<uint8_t>(len)) {}
};

/// @brief Implements (de)serialization for VecU8
template <size_t len>
struct YAML::convert<VecU8<len>>
{
    static Node encode(VecU8<len> const &vec)
    {
        return convert<cfg::Vec<uint8_t, len>>::encode(vec);
    }

    static bool decode(Node const &node, VecU8<len> &vec)
    {
        return convert<cfg::Vec<uint8_t, len>>::decode(node, vec);
    }
};

/// Defines convenient type aliases
using Vec3U8 = VecU8<3>;

SCENARIO("custom types can be used with sequence configurations")
{
    GIVEN("a valid config file")
    {
        REQUIRE(cfg::GetConfig_From(std::filesystem::absolute(t_config_path)).has_value());
        cfg::ConfigBase base = cfg::GetConfig_From(std::filesystem::absolute(t_config_path)).value();

        WHEN("a valid key for a uint8_t sequence is provided")
        {
            const std::string SEQUENCES_U8S = "sequences_u8s";
            REQUIRE(base.Get<Vec3U8>(SEQUENCES_U8S).has_value());
            THEN("config value should be correctly fetched")
            {
                const Vec3U8 e_rgb = {255, 255, 255};
                REQUIRE(base.Get<Vec3U8>(SEQUENCES_U8S).value() == e_rgb);
            }
        }
    }
}