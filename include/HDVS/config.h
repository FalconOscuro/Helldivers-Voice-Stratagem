#ifndef CONFIG_H
#define CONFIG_H

#include <yaml-cpp/yaml.h>
#include <string>

#include <thread>

namespace hdvs {

struct Config
{
    float diallingSpeed;
    size_t tick_rate_ms = 50;

    struct Keys {
        int up;
        int down;
        int left;
        int right;

        int stratagem;

        bool operator==(const Keys& rhs) const;
    };

    Keys keys;

    struct Listen {
        uint32_t n_threads = std::min((unsigned int)4, std::thread::hardware_concurrency());
        uint32_t step_ms = 3000;
        uint32_t length_ms = 6000;
        uint32_t keep_ms = 200;
        uint32_t capture_id = -1;
        uint32_t max_tokens = 32; // max tokens per chunk

        bool use_gpu = true;

        std::string language = "en";
        std::string model = "./data/models/ggml-tiny.en.bin";
    };
    Listen listen;

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
