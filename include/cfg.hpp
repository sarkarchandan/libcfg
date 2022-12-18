#ifndef CFG_HPP
#define CFG_HPP

#include <iostream>
#include <string>
#include <queue>
#include <filesystem>

#include <yaml-cpp/yaml.h>

namespace cfg
{
    /// @brief Implements a wrapper over yaml-cpp library to implement a utility method to fetch configurations from
    /// file.
    class ConfigBase
    {
        YAML::Node _config;
        std::string _default_delimiter;

        /// @brief Parses hierarchical key string into a queue of identifiers for individual levels in the
        /// hierarchical configuration structure. This is done depending on a delimiter separating the combined
        /// keys in a single string.
        std::queue<std::string> parseConfig(std::string const &_key) const
        {
            std::queue<std::string> buffer;
            size_t start = 0;
            size_t end = _key.find(_default_delimiter);
            while (end != std::string::npos)
            {
                buffer.push(_key.substr(/*pos*/ start, /*count*/ end - start));
                start = (end + _default_delimiter.length());
                end = _key.find(_default_delimiter, start);
            }
            buffer.push(_key.substr(start, end));
            return buffer;
        }

        /// @brief Recursively fetches keys from hierarchical yaml nodes using separated key segments.
        /// Yaml-cpp supports a number of node types such as null, scalar, sequence maps etc. A map denotes
        /// existence of a single key-value pair or existence of nested key-value pairs. When we exhaust all
        /// key segments and simultaneously currently traversed node is not a map, it means we have reached a
        /// node which denotes a scalar, a sequence or absence of a value, at which point we return the node.
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
        /// @brief Constructs ConfigBase from a YAML configuration file and a default delimiter for hierarchical
        /// configurations.
        ConfigBase(std::filesystem::path const &_cfg_path, std::string const &_def_delim = "_")
            : _default_delimiter(_def_delim)
        {
            try
            {
                _config = YAML::LoadFile(std::filesystem::absolute(_cfg_path));
            }
            catch (const std::exception &e)
            {
                std::cerr << e.what() << "\n";
                throw std::runtime_error("invalid config file");
            }
        }
        ~ConfigBase() {}

        /// @brief Implements accessor for config values against specified key
        /// @tparam T configuration value type
        /// @param key configuration key
        /// @return configuration value
        template <typename T>
        T Get(std::string const &key) const
        {
            if (key.find(_default_delimiter) == std::string::npos)
            {
                if (_config[key].IsNull())
                {
                    throw std::runtime_error("config not found");
                }
                return _config[key].as<T>();
            }
            std::queue<std::string> _segments = parseConfig(key);
            YAML::Node val = fetch(_config, _segments);
            if (val.IsNull())
            {
                throw std::runtime_error("config not found");
            }
            return val.as<T>();
        }
    };
} // namespace cfg

#endif // CFG_HPP