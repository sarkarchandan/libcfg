#ifndef TYPES_HPP
#define TYPES_HPP

#include <ostream>
#include <vector>
#include <initializer_list>

#include <yaml-cpp/yaml.h>

namespace cfg
{
    /// @brief Defines generic three element vector type
    template <typename T>
    struct Vec3
    {
        T x, y, z;

        Vec3(std::initializer_list<T> const &list)
        {
            if (list.size() < 3)
            {
                throw std::runtime_error("in-sufficient number of elements provided");
            }
            std::vector<T> buffer = list;
            this->x = buffer[0];
            this->y = buffer[1];
            this->z = buffer[2];
        }

        bool operator==(Vec3 const &other) const
        {
            return this->x == other.x && this->y == other.y && this->z == other.z;
        }

        bool operator!=(Vec3 const &other) const
        {
            return !this == other;
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

    /// @brief Specializes three element vector type for bool
    struct Vec3b : Vec3<bool>
    {
        Vec3b(std::initializer_list<bool> const &list) : Vec3<bool>(list) {}
        Vec3b() : Vec3<bool>({false, false, false}) {}
    };

    /// @brief Specializes three element vector type for string
    struct Vec3Str : Vec3<std::string>
    {
        Vec3Str(std::initializer_list<std::string> const &list) : Vec3<std::string>(list) {}
        Vec3Str() : Vec3<std::string>({"", "", ""}) {}
    };
} // namespace cfg

/// @brief Overloads output stream operator for three element vector
template <typename T>
std::ostream &operator<<(std::ostream &os, cfg::Vec3<T> const &vec)
{
    os << "["
       << vec.x
       << ", "
       << vec.y
       << ", "
       << vec.z
       << "]";
    return os;
}

namespace YAML
{
    /// @brief Encapsulates (de)serialization of three element vector to/from YAML::Node
    template <typename T>
    struct convert<cfg::Vec3<T>>
    {
        static Node encode(cfg::Vec3<T> const &vec)
        {
            Node node;
            node.push_back(vec.x);
            node.push_back(vec.y);
            node.push_back(vec.z);
            return node;
        }

        static bool decode(Node const &node, cfg::Vec3<T> &vec)
        {
            if (!node.IsSequence() || node.size() != 3)
            {
                return false;
            }
            vec.x = node[0].as<T>();
            vec.y = node[1].as<T>();
            vec.z = node[2].as<T>();
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

    /// @brief Specializes (de)serialization of three element bool vector to/from YAML::Node
    template <>
    struct convert<cfg::Vec3b>
    {
        static Node encode(cfg::Vec3b const &vec)
        {
            return convert<cfg::Vec3<bool>>::encode(vec);
        }

        static bool decode(Node const &node, cfg::Vec3b &vec)
        {
            return convert<cfg::Vec3<bool>>::decode(node, vec);
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