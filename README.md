# libcfg

This is a simple wrapper over [yaml-cpp](https://github.com/jbeder/yaml-cpp) library to implement a configuration 
fetcher api. This implementation is primarily put together to integrate a simple and reusable mechanism for adding 
configuration to other C++ applications. Hence, it has a reasonably smaller scope compared to full capabilities of 
`yaml-cpp` and simpler api layer to instantiate the configuration fetcher `cfg::ConfigBase` and a 
`cfg::ConfigBase::Get<T>` method to fetch configurations for a given key string. The key can refer to a root-level 
configuration a configuration in an arbitrary depth for which individual level segments are assumed to be underscore(_) 
separated. In this documentation we describe  the usage of the api and integration of the library with 
[CMake](https://cmake.org/cmake/help/latest/). To get acquainted with the feature-rich `yaml-cpp` library please visit 
[yaml-cpp](https://github.com/jbeder/yaml-cpp) and [yaml-cpp tutorials](https://github.com/jbeder/yaml-cpp/wiki/Tutorial).

## API
---

The main engine of the api is `cfg::ConfigBase` which can be instantiated using an absolute path to a YAML config file. 
Instantiation may fail with runtime error if an invalid path is provided. Therefore, it is instantiated using api layer 
utility function `cfg::GetConfig_From`, which takes an [std::filesystem::path](https://en.cppreference.com/w/cpp/filesystem/path), 
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
    // ConfigBase is instantiated with file path
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
underscore(`_`) as delimiter. Hence, the combined key to fetch it would be `attributes_debug`. Other config items, 
which are at the same level as `debug` can be fetched using similar combination of keys e.g., `attributes_rgb` or 
`attributes_point` etc. This can be generalized for fetching config items for any arbitrary depths. Following are 
some examples,

- 
    ```yaml
    road:
        dims:
            length: 50.
    ```
    should be fetched using `road_dims_length`.

-
    ```yaml
    road:
        dims:
            ...
        color:
            ...
            saturation: 0.2
    ```
    should be fetched using `road_color_saturation`.

Internally, the entire configuration file is represented with a tree structure and each config item therefore is a node. 
The `cfg::ConfigBase` encapsulates implementation for parsing of combined key-segments and recursively fetching the 
targeted config item. Similar to the earlier api-layer function `cfg::ConfigBase::Get<T>` method encapsulates any 
exception, which might take place while fetching the config item and makes it available to the caller using an uniform 
interface using `boost::optional<T>`.

```cpp
const std::string ROAD_DIMS_WIDTH = "road_dims_width";
if(base.Get<double>(ROAD_DIMS_WIDTH).has_value())
{
    const double road_width = base.Get<double>(ROAD_DIMS_WIDTH).value();
}
```

Above example illustrates the most straightforward usage pattern. In other circumstances we might want to leverage the 
power of `boost::optional` to set some default value, when the targeted config item can not be fetched.

```cpp
double some_default_val = 345.23;
double expected_config_item = base.Get<double>("SOME_MISSING_CONFIG").get_value_or(some_default_val);
```

In the above example if the targeted config item can not be fetched we assign a default value to the config. We get 
this utility out of the box from `boost::optional`.

### Handling Sequences


