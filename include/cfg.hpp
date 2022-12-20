#ifndef CFG_HPP
#define CFG_HPP

#include <iostream>
#include <string>
#include <queue>
#include <filesystem>
#include <memory>

#include <boost/optional.hpp>
#include <yaml-cpp/yaml.h>

namespace cfg
{
    /// @brief Implements a wrapper over yaml-cpp to implement a utility method to fetch config values from file.
    /// A config file could be arbitrarily large, hence we initialize the YAML::Node tree structure onto heap.
    class ConfigBase
    {
        std::unique_ptr<YAML::Node> _root; // Root node of the YAML config tree
        std::string _delimeter;            // Separator for key segments of hierarchical config items
        std::filesystem::path _path;       // Base path for config file

        /// @brief Parses hierarchical key string into a queue of identifiers for individual levels in the
        /// hierarchical config structure. This is done depending on a delimiter separating the combined
        /// keys in a single string.
        /// @example "road_color_saturation" => ["road" -> "color" -> "saturation"]
        std::queue<std::string> parseConfig(std::string const &_key) const
        {
            std::queue<std::string> buffer;
            size_t start = 0;
            size_t end = _key.find(_delimeter);
            while (end != std::string::npos)
            {
                buffer.push(_key.substr(/*pos*/ start, /*count*/ end - start));
                start = (end + _delimeter.length());
                end = _key.find(_delimeter, start);
            }
            buffer.push(_key.substr(start, end));
            return buffer;
        }

        /// @brief Recursively fetches keys from hierarchical yaml nodes using separated key segments.
        /// Yaml-cpp supports a number of node types such as null, scalar, sequence maps etc. A map denotes
        /// existence of a single key-value pair or existence of nested key-value pairs. When we exhaust all
        /// key segments and currently traversed node is not a map, it means we have reached a node which
        /// denotes a scalar, a sequence or null value, at which point we return the node.
        YAML::Node fetch(YAML::Node node, std::queue<std::string> &_segments) const
        {
            if (!node.IsMap() && _segments.empty())
            {
                return node;
            }
            std::string _key_seg = _segments.front();
            _segments.pop();
            return fetch(node[_key_seg], _segments);
        }

    public:
        /// @brief ConfigBase constructor
        explicit ConfigBase(std::filesystem::path const &_cfg_path)
            : _delimeter("_"), _path(_cfg_path)
        {
            _root = std::make_unique<YAML::Node>();
            *_root = YAML::LoadFile(_cfg_path);
        }
        ConfigBase() = delete;
        ~ConfigBase() {}

        ConfigBase(ConfigBase const &_other)
            : _delimeter(_other._delimeter), _path(_other._path)
        {
            _root.release();
            _root = std::make_unique<YAML::Node>();
            *_root = YAML::LoadFile(_path);
        }

        ConfigBase operator=(ConfigBase const &_other)
        {
            if (*this == _other)
            {
                return *this;
            }
            _delimeter = _other._delimeter;
            _path = _other._path;
            _root.release();
            _root = std::make_unique<YAML::Node>();
            *_root = YAML::LoadFile(_path);
            return *this;
        }

        inline bool operator==(cfg::ConfigBase const &_other) const
        {
            return _delimeter == _other._delimeter && _path == _other._path && _root->Type() == _other._root->Type();
        }

        inline bool operator!=(cfg::ConfigBase const &_other) const
        {
            return !(*this == _other);
        }

        /// @brief Common accessor api for config values against specified key.
        /// @tparam T configuration value type
        /// @param key config key
        /// @return optional config value
        template <typename T>
        boost::optional<T> Get(std::string const &key) const
        {
            if (key.find(_delimeter) == std::string::npos)
            {
                if (!(*_root)[key].IsDefined() || (*_root).IsNull())
                {
                    return boost::none;
                }
                return (*_root)[key].as<T>();
            }
            std::queue<std::string> _segments = parseConfig(key);
            // At this point key segments must not be empty
            assert(!_segments.empty());
            YAML::Node val = fetch((*_root), _segments);
            if (!val.IsDefined() || val.IsNull())
            {
                return boost::none;
            }
            return val.as<T>();
        }
    };

    /// @brief Instantiates cfg::ConfigBase while encapsulating the error handling.
    /// @param _abs_path absolute path to the config file
    /// @return optional cfg::ConfigBase
    boost::optional<cfg::ConfigBase> GetConfig_From(std::filesystem::path const &_abs_path)
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
} // namespace cfg

#endif // CFG_HPP