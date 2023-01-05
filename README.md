# libcfg

![Build & Test](https://github.com/sarkarchandan/libcfg/actions/workflows/c-cpp.yml/badge.svg)

This is a simple wrapper over [yaml-cpp](https://github.com/jbeder/yaml-cpp) library to implement a configuration 
fetcher api. This project is primarily created to integrate a simple and reusable mechanism for adding 
configuration to other C++ applications. Hence, it has a reasonably smaller scope compared to full capabilities of 
`yaml-cpp` and it has a simpler api layer to instantiate the configuration fetcher engine `cfg::ConfigBase` and a 
`cfg::ConfigBase::Get<T>` method to fetch configuration against a given key string. The key can refer to a root-level 
configuration or a configuration in an arbitrary depth, for which individual level segments are assumed to be dot(.) 
separated. In this documentation we describe the usage of the api and integration of the library with 
[CMake](https://cmake.org/cmake/help/latest/). To get acquainted with the feature-rich `yaml-cpp` library please visit 
[yaml-cpp](https://github.com/jbeder/yaml-cpp) and [yaml-cpp tutorials](https://github.com/jbeder/yaml-cpp/wiki/Tutorial).

## API Layer

The main engine of the api is `cfg::ConfigBase`, which can be instantiated using an **absolute path** to a YAML config 
file. Instantiation may fail with runtime error if an invalid path is provided. Therefore, it is instantiated using api 
layer utility function `cfg::GetConfig_From`, which takes an [std::filesystem::path](https://en.cppreference.com/w/cpp/filesystem/path), 
handles any possible exception and returns `cfg::ConfigBase` instance wrapped inside a 
[boost::optional](https://www.boost.org/doc/libs/1_81_0/libs/optional/doc/html/index.html) value. The optional value is 
used to hide the exception from the caller and establish the convention that the configuration fetcher engine may not 
be obtained if something unexpected happens.

```cpp
const std::filesystem::path config_path = "<../config_file>.yaml";
const cfg::ConfigBase base = cfg::GetConfig_From(std::filesystem::absolute(config_path)).value();
```

Above snippet force-unwraps the optional value, which may fail if the value is not present. We can check wrapped value 
like the following snippet before unwrapping.

```cpp
const std::filesystem::path config_path = "<../config_file>.yaml";
if(cfg::GetConfig_From(config_path).has_value())
{
    // ConfigBase engine is instantiated with file path
}
```

Once instantiated, `cfg::ConfigBase::Get<T>` method should be used to fetch config values from file. This method 
follows a specific convention for keys, which needs special attention. Let's look at an example YAML config file.

```yaml
pi: 3.14159
attributes:
  name: "some name"
  debug: true
  point: [2.3, 5.2, 5.9]
  rgb: [255, 255, 255]
  names: ["tom", "dick", "harry"]
road:
  dims:
    length: 50.
    width: 12.
    height: 5.1
  color:
    hue: 0.2
    saturation: 0.2
    value: 0.2
```

In this file `pi` is a valid root-level key and it can be used as is to fetch corresponding value. However, to fetch 
the boolean value for `debug` config item we need to combine it with the root-level key-segment `attributes` using 
dot(`.`) as delimiter. Hence, the combined key to fetch would be `attributes.debug`. Other config items, 
which are at the same level as `debug` can be fetched using similar combination of keys e.g., `attributes.rgb` or 
`attributes.point` etc. This can be generalized for fetching config items for any arbitrary depths. Following are 
some examples,

- 
    ```yaml
    road:
        dims:
            length: 50.
    ```
    should be fetched using `road.dims.length`.

-
    ```yaml
    road:
        dims:
            ...
        color:
            ...
            saturation: 0.2
    ```
    should be fetched using `road.color.saturation`.

Internally, the entire configuration file is represented with a tree structure and each config item therefore is a node. 
This convention is implemented by the [yaml-cpp](https://github.com/jbeder/yaml-cp) library. The `cfg::ConfigBase` 
encapsulates the implementation of parsing the combined key-segments and recursively fetching the targeted config item. 
Similar to the earlier api-layer function to instantiate `cfg::ConfigBase`, the method `cfg::ConfigBase::Get<T>` 
encapsulates any exception, which might take place while fetching the config item and makes it available to the caller 
using the uniform interface using `boost::optional<T>`.

```cpp
const std::string ROAD_DIMS_WIDTH = "road.dims.width";
if(base.Get<double>(ROAD_DIMS_WIDTH).has_value())
{
    const double road_width = base.Get<double>(ROAD_DIMS_WIDTH).value();
}
```

Above example illustrates the most straightforward usage pattern. In other circumstances we might want to leverage the 
power of `boost::optional` to set some default value, when the targeted config item cannot be fetched.

```cpp
double some_default_val = 345.23;
double expected_config_item = base.Get<double>("SOME.MISSING.CONFIG").get_value_or(some_default_val);
```

In the above example if the targeted config item cannot be fetched we assign a default value to the config item. We get 
this utility out of the box from `boost::optional`. This convention of optional instantiation and fetching of values 
are incorporated by taking motivation from the usage pattern of the [Option](https://doc.rust-lang.org/std/option/enum.Option.html) 
enum type from the standard library of [Rust](https://www.rust-lang.org/) language.

### Handling Sequential Configurations

This library implements some additional utilities to deal with configuration items, which are sequences of values. The 
core implementation of (de)serialization of sequential values are reused from [yaml-cpp](https://github.com/jbeder/yaml-cp) 
library. To deal with sequential values as configurations we expose following types, some of which can be used as is with 
`cfg::ConfigBase::Get<T>` method to fetch config values. Others represent interfaces which can be used to implement 
custom sequential types.

- `cfg::Vec3I`, `cfg::Vec3D`, `cfg::Vec3Str` are concrete utility types, which represent commonly used **three-elements sequences** of 
integer, double and std::string types respectively. Each of these can be used as is to fetch three-element config items 
from YAML config file.

    ```yaml
    ...
    attributes:
        ...
        point: [2.3, 5.2, 5.9]
        rgb: [255, 255, 255]
        names: ["tom", "dick", "harry"]
        ...
    ...
    ```
    ```cpp
    const std::filesystem::path config_path = "<../config_file>.yaml";
    const std::string ATTRIBUTES_POINT = "attributes.point";
    const std::string ATTRIBUTES_RGB = "attributes.rgb";
    const std::string ATTRIBUTES_NAMES = "attributes.names";
    const cfg::ConfigBase base = cfg::GetConfig_From(std::filesystem::absolute(config_path)).value();

    auto val_vec_3d = base.Get<cfg::Vec3D>(ATTRIBUTES_POINT).value();
    auto val_vec_ei = base.Get<cfg::Vec3I>(ATTRIBUTES_RGB).value();
    auto val_vec_3str = base.Get<cfg::Vec3Str>(ATTRIBUTES_NAMES).value();
    ```
- `cfg::Vec<typename, size_t>` is an exposed helper interface to implement custom sequence types with arbitrary lengths. 
As shown in the below example, for any standard library types such as `int32_t`, `uint8_t` etc, it is a matter of 
specializing the interface and provide implementation of (de)serialization for the custom type. The implementation of 
(de)serialization in turn delegates to the same implementation for `cfg::Vec<typename, size_t>`, which uses 
`YAML::convert<typename>` type. The `YAML::convert<typename>` is defined in the [yaml-cpp](https://github.com/jbeder/yaml-cpp) 
library and it must be used to implement the (de)serialization of custom types as per the convention. Defining a 
sequential configuration with user defined types is possible in theory but it needs more involved implementation using 
the exposed interface and currently outside the scope of this library.

    ```yaml
    ...
    sequences:
        ...
        i32s: [-2147483648, 2147483647]
        ...
    ...
    ```
    ```cpp
    /// Specializes template cfg::Vec<typename, size_t> for a two-elements sequence of int32_t type
    template <>
    struct Vec2I32 : public cfg::Vec<int32_t, 2>
    {
        Vec2I32(std::initializer_list<int32_t> const &list)
            : cfg::Vec<int32_t, 2>(list) {}

        Vec2I32()
            : cfg::Vec<int32_t, 2>(std::vector<int32_t>(2)) {}
    };

    /// Implements (de)serialization of two-elements int32_t sequence
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

    const std::filesystem::path config_path = "<../config_file>.yaml";
    const std::string SEQUENCES_I32S = "sequences.i32s";
    const cfg::ConfigBase base = cfg::GetConfig_From(std::filesystem::absolute(config_path)).value();

    auto val_i32s = base.Get<Vec2I32>(SEQUENCES_I32S).value();
    ```

## Integration with CMake

...

## Acknowledgement of Helpful Resources

This is to acknowledge that this project is built on top of the following feature-rich open-source project(s).

- [yaml-cpp](https://github.com/jbeder/yaml-cpp)-`0.7.0` - A YAML parser and emitter in C++.

It has referred to resources made available in following open-source project(s) to setup continuos integration workflow.

- [install-boost](https://github.com/MarkusJx/install-boost) - Install boost on Github actions.
- [cmake-ci-setup](https://github.com/cristianadam/HelloWorld) - A C++ Hello World project, using CMake, and GitHub 
Actions accompanied by a [helpful blog post](https://cristianadam.eu/20191222/using-github-actions-with-c-plus-plus-and-cmake/).

