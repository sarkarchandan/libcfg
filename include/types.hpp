#ifndef TYPES_HPP
#define TYPES_HPP

#include <assert.h>
#include <ostream>
#include <vector>
#include <initializer_list>

#include <yaml-cpp/yaml.h>

namespace cfg
{
    /// @brief Defines generic three element vector type. This is a base type, which is in turn specialized to
    /// create three separate concrete vector types, which can be deserialized from config file.
    template <typename T>
    struct Vec3
    {
    private:
        std::vector<T> _buffer; // Internal container

    public:
        /// @brief Constructor
        Vec3(std::initializer_list<T> const &list)
        {
            if (list.size() < 3)
            {
                throw std::runtime_error("in-sufficient number of elements provided");
            }
            _buffer = list;
        }

        /// @brief Equality operator overload
        bool operator==(Vec3 const &other) const
        {
            return _buffer[0] == other._buffer[0] &&
                   _buffer[1] == other._buffer[1] &&
                   _buffer[2] == other._buffer[2];
        }

        /// @brief Inequality operator overload
        bool operator!=(Vec3 const &other) const
        {
            return !this == other;
        }

        /// @brief Subscript operator overload as accessor
        T operator[](size_t idx) const
        {
            assert(idx >= 0 && idx < 3);
            return _buffer[idx];
        }

        /// @brief Subscript operator overload as modifier
        T &operator[](size_t idx)
        {
            assert(idx >= 0 && idx < 3);
            return _buffer[idx];
        }
    };

    /// @brief Specializes three element vector type for double
    struct Vec3d : Vec3<double>
    {
        Vec3d(std::initializer_list<double> const &list) : Vec3<double>(list) {}
        Vec3d() : Vec3<double>({0., 0., 0.}) {}
    };

    /// @brief Specializes three element vector type for int
    struct Vec3i : Vec3<int>
    {
        Vec3i(std::initializer_list<int> const &list) : Vec3<int>(list) {}
        Vec3i() : Vec3<int>({0, 0, 0}) {}
    };

    /// @brief Specializes three element vector type for string
    struct Vec3Str : Vec3<std::string>
    {
        Vec3Str(std::initializer_list<std::string> const &list) : Vec3<std::string>(list) {}
        Vec3Str() : Vec3<std::string>({"", "", ""}) {}
    };
} // namespace cfg

/// @brief Overloads output stream operator for generic three-element vector
template <typename T>
std::ostream &operator<<(std::ostream &os, cfg::Vec3<T> const &vec)
{
    os << "["
       << vec[0]
       << ", "
       << vec[1]
       << ", "
       << vec[2]
       << "]";
    return os;
}

namespace YAML
{
    /// @brief Implements (de)serialization of three element vector to/from YAML::Node
    template <typename T>
    struct convert<cfg::Vec3<T>>
    {
        /// @brief Implements marshalling of generic three-element vector to YAML::Node
        static Node encode(cfg::Vec3<T> const &vec)
        {
            Node node;
            node.push_back(vec[0]);
            node.push_back(vec[1]);
            node.push_back(vec[2]);
            return node;
        }

        /// @brief Implements unmarshalling of YAML::Node to generic three-element vector
        static bool decode(Node const &node, cfg::Vec3<T> &vec)
        {
            if (!node.IsSequence() || node.size() != 3)
            {
                return false;
            }
            vec[0] = node[0].as<T>();
            vec[1] = node[1].as<T>();
            vec[2] = node[2].as<T>();
            return true;
        }
    };

    /// @brief Specializes (de)serialization of three element double vector to/from YAML::Node
    template <>
    struct convert<cfg::Vec3d>
    {
        static Node encode(cfg::Vec3d const &vec)
        {
            return convert<cfg::Vec3<double>>::encode(vec);
        }

        static bool decode(Node const &node, cfg::Vec3d &vec)
        {
            return convert<cfg::Vec3<double>>::decode(node, vec);
        }
    };

    /// @brief Specializes (de)serialization of three element int vector to/from YAML::Node
    template <>
    struct convert<cfg::Vec3i>
    {
        static Node encode(cfg::Vec3i const &vec)
        {
            return convert<cfg::Vec3<int>>::encode(vec);
        }

        static bool decode(Node const &node, cfg::Vec3i &vec)
        {
            return convert<cfg::Vec3<int>>::decode(node, vec);
        }
    };

    /// @brief Specializes (de)serialization of three element string vector to/from YAML::Node
    template <>
    struct convert<cfg::Vec3Str>
    {
        static Node encode(cfg::Vec3Str const &vec)
        {
            return convert<cfg::Vec3<std::string>>::encode(vec);
        }

        static bool decode(Node const &node, cfg::Vec3Str &vec)
        {
            return convert<cfg::Vec3<std::string>>::decode(node, vec);
        }
    };
} // namespace YAML

#endif // TYPES_HPP