#include <filesystem>
#include <cstdint>

#include <catch2/catch_test_macros.hpp>

// Path to config file needs to be adjusted keeping in mind that when we execute test from build dir that would be
// the context of the executable binary.
const std::filesystem::path t_config_path = "../../tests/test_config_custom.yaml";
