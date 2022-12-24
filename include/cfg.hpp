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
    /// @brief Wraps yaml-cpp to implement a utility method to fetch config values from file.
    /// A config file could be arbitrarily large, hence YAML::Node tree is initialized onto heap.
    class ConfigBase
    {
        std::unique_ptr<YAML::Node> _root; // Root node of the node tree
        std::string _delimeter;            // Separator for key segments of hierarchical config items
        std::filesystem::path _path;       // Base path for config file

        /// @brief Parses combined key string into a queue of identifiers for individual levels in the
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

        /// @brief Recursively fetches keys from hierarchical yaml nodes using key segments. Yaml-cpp supports a
        /// number of node types such as null, scalar, sequence maps etc. A map denotes existence of a single
        /// key-value pair or nested key-value pairs. When we exhaust all key segments and currently traversed
        /// node is not a map, it means we have reached a node which denotes a scalar, a sequence or null value,
        /// at which point we return the node.
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

        /// @brief Constructor. Defined as private because this can throw but we don't want to handle the error in
        /// here. Instead we want to call the constructor from api layer and wrap the error handling there.
        explicit ConfigBase(std::filesystem::path const &_cfg_path)
            : _delimeter("_"), _path(_cfg_path)
        {
            _root = std::make_unique<YAML::Node>();
            *_root = YAML::LoadFile(_cfg_path);
        }

    public:
        ConfigBase() = delete;
        ~ConfigBase() {}

        /// @brief Copy constructor
        ConfigBase(ConfigBase const &_other)
            : _delimeter(_other._delimeter), _path(_other._path)
        {
            _root.release();
            _root = std::make_unique<YAML::Node>();
            *_root = YAML::LoadFile(_path);
        }

        /// @brief Copy assignment operator overload
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

        /// @brief Equality operator overload
        inline bool operator==(cfg::ConfigBase const &_other) const
        {
            return _delimeter == _other._delimeter && _path == _other._path && _root->Type() == _other._root->Type();
        }

        /// @brief Inequality operator overload
        inline bool operator!=(cfg::ConfigBase const &_other) const
        {
            return !(*this == _other);
        }

        /// @brief Common accessor api for config values against specified key. Key could be a simple one referring
        /// to a root-level configuration. It can also be a '_'-separated combination key-segments referring to a
        /// hierarchical combination.
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

        /// @brief Specifies cfg::GetConfig_From as friend function to hide the main constructor and enforce
        /// its usage as api to instantiate cfg::ConfigBase.
        friend boost::optional<cfg::ConfigBase> GetConfig_From(std::filesystem::path const &_abs_path);
    };

    /// @brief API to instantiate cfg::ConfigBase. Initialization of ConfigBase may fail if wrong filepath or
    /// malformed config file is provided.
    /// @param _abs_path absolute path to the config file
    /// @return optional cfg::ConfigBase
    boost::optional<cfg::ConfigBase> GetConfig_From(std::filesystem::path const &_abs_path);
} // namespace cfg

#endif // CFG_HPP