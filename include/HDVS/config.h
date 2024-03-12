#ifndef CONFIG_H
#define CONFIG_H

#include <yaml-cpp/yaml.h>
#include <string>

namespace hdvs {

struct Config
{
    float diallingSpeed;

    struct Keys {
        std::string up;
        std::string down;
        std::string left;
        std::string right;

        std::string stratagem;

        bool operator==(const Keys& rhs) const;
    };

    Keys keys;

    // required by yaml-cpp
    bool operator==(const Config& cfg) const;
};

} // namespace hdvs

namespace YAML {
template<>
struct convert<hdvs::Config> {
    static Node encode(const hdvs::Config& cfg);

    static bool decode(const Node& node, hdvs::Config& cfg);
};

template<>
struct convert<hdvs::Config::Keys> {
    static Node encode(const hdvs::Config::Keys& keys);

    static bool decode(const Node& node, hdvs::Config::Keys& keys);
};

} // namespace YAML

#endif // CONFIG_H
