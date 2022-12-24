#ifndef TYPES_HPP
#define TYPES_HPP

#include <assert.h>
#include <ostream>
#include <vector>
#include <initializer_list>

#include <yaml-cpp/yaml.h>

namespace cfg
{
    /// @brief Universal variable length vector base type, which can be (de)serialized to and from YAML::Node.
    /// Intended to be specialized according to need for defining config values.
    /// @tparam T Value type
    /// @tparam len Length
    template <typename T, size_t len>
    struct Vec
    {
    protected:
        std::vector<T> _buffer; // Internal container

    public:
        // @brief Main constructor
        Vec(std::initializer_list<T> const &list)
        {
            _buffer.reserve(len);
            _buffer = list;
        }

        /// @brief Convenient constructor for default initialization of derived types
        Vec(std::vector<T> const &vec)
        {
            _buffer.reserve(len);
            _buffer = vec;
        }

        /// @brief Subscript operator overload as accessor
        T operator[](size_t idx) const
        {
            assert(idx >= 0 && idx < len);
            return _buffer[idx];
        }

        /// @brief Subscript operator overload as mutator
        T &operator[](size_t idx)
        {
            assert(idx >= 0 && idx < len);
            return _buffer[idx];
        }

        /// @brief Equality operator overload
        bool operator==(Vec<T, len> const &other) const
        {
            for (size_t idx = 0; idx < len; idx++)
            {
                if (_buffer[idx] != other._buffer[idx])
                {
                    return false;
                }
            }
            return true;
        }

        /// @brief Inequality operator overload
        bool operator!=(Vec<T, len> const &other) const
        {
            return !this == other;
        }
    };

    /// @brief Specialization for integer vector of given length
    template <size_t len>
    struct VecD : public Vec<double, len>
    {
        /// @brief Main constructor
        VecD(std::initializer_list<double> const &list)
            : Vec<double, len>(list) {}

        /// @brief Default constructor (required for deserialization)
        VecD()
            : Vec<double, len>(std::vector<double>(len)) {}
    };

    /// @brief Specializes for double vector of given length
    template <size_t len>
    struct VecI : public Vec<int, len>
    {
        /// @brief Main constructor
        VecI(std::initializer_list<int> const &list)
            : Vec<int, len>(list) {}

        /// @brief Default constructor (required for deserialization)
        VecI()
            : Vec<int, len>(std::vector<int>(len)) {}
    };

    /// @brief Specialization for string vector of given length
    template <size_t len>
    struct VecStr : public Vec<std::string, len>
    {
        /// @brief Main constructor
        VecStr(std::initializer_list<std::string> const &list)
            : Vec<std::string, len>(list) {}

        /// @brief Default constructor (required for deserialization)
        VecStr()
            : Vec<std::string, len>(std::vector<std::string>(len)) {}
    };

    /// Convenient type aliases for three-element vector types, commonly used to represent
    /// RGB color, coordinates, dimensions etc.
    using Vec3I = cfg::VecI<3>;
    using Vec3D = cfg::VecD<3>;
    using Vec3Str = cfg::VecStr<3>;
} // namespace cfg

namespace YAML
{
    /// @brief Implements (de)serialization of Vec<T, size_t> to/from YAML::Node
    template <typename T, size_t len>
    struct convert<cfg::Vec<T, len>>
    {
        /// @brief Serializes Vec<T, size_t> to YAML::Node
        static Node encode(cfg::Vec<T, len> const &vec)
        {
            Node node;
            if (len > 0)
            {
                for (size_t idx = 0; idx < len; idx++)
                {
                    node.push_back(vec[idx]);
                }
            }
            return node;
        }

        /// @brief Deserializes YAML::Node to Vec<T, size_t>
        static bool decode(Node const &node, cfg::Vec<T, len> &vec)
        {
            if (!node.IsSequence() || node.size() != len)
            {
                return false;
            }
            if (len > 0)
            {
                for (size_t idx = 0; idx < len; idx++)
                {
                    vec[idx] = node[idx].as<T>();
                }
            }
            return true;
        }
    };

    /// @brief (De)serialization double vector to/from YAML::Node
    template <size_t len>
    struct convert<cfg::VecD<len>>
    {
        static Node encode(cfg::VecD<len> const &vec)
        {
            return convert<cfg::Vec<double, len>>::encode(vec);
        }

        static bool decode(Node const &node, cfg::VecD<len> &vec)
        {
            return convert<cfg::Vec<double, len>>::decode(node, vec);
        }
    };

    /// @brief (De)serialization integer vector to/from YAML::Node
    template <size_t len>
    struct convert<cfg::VecI<len>>
    {
        static Node encode(cfg::VecI<len> const &vec)
        {
            return convert<cfg::Vec<int, len>>::encode(vec);
        }

        static bool decode(Node const &node, cfg::VecI<len> &vec)
        {
            return convert<cfg::Vec<int, len>>::decode(node, vec);
        }
    };

    /// @brief (De)serialization string vector to/from YAML::Node
    template <size_t len>
    struct convert<cfg::VecStr<len>>
    {
        static Node encode(cfg::VecStr<len> const &vec)
        {
            return convert<cfg::Vec<std::string, len>>::encode(vec);
        }

        static bool decode(Node const &node, cfg::VecStr<len> &vec)
        {
            return convert<cfg::Vec<std::string, len>>::decode(node, vec);
        }
    };
} // namespace YAML

#endif // TYPES_HPP