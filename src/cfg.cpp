#include "cfg.hpp"

boost::optional<cfg::ConfigBase> cfg::GetConfig_From(std::filesystem::path const &_abs_path)
{
    try
    {
        cfg::ConfigBase base = cfg::ConfigBase(_abs_path);
        return base;
    }
    catch (YAML::BadFile const &bf)
    {
        std::cerr << bf.what() << '\n';
        return boost::none;
    }
}