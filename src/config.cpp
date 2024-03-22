#include "HDVS/config.h"

namespace hdvs {

bool Config::operator==(const Config& cfg) const {
    return cfg.diallingSpeed == diallingSpeed && cfg.keys == keys;
}

bool Config::Keys::operator==(const Keys& rhs) const {
    return up == rhs.up && down == rhs.down
        && left == rhs.left && right == rhs.right
            && stratagem == rhs.stratagem;
}

} // namespace hdvs

namespace YAML {

Node convert<hdvs::Config>::encode(const hdvs::Config& cfg)
{
    Node node;

    node["dialling-speed"] = cfg.diallingSpeed;
    node["keys"] = cfg.keys;

    return node;
}

bool convert<hdvs::Config>::decode(const Node& node, hdvs::Config& cfg)
{
    if (!(node.IsMap() && node["keys"] && node["dialling-speed"]))
        return false;

    cfg.diallingSpeed = node["dialling-speed"].as<float>();

    return convert<hdvs::Config::Keys>::decode(node["keys"], cfg.keys);
}

Node convert<hdvs::Config::Keys>::encode(const hdvs::Config::Keys& keys)
{
    Node node;

    node["up"]        = keys.up;
    node["down"]      = keys.down;
    node["left"]      = keys.left;
    node["right"]     = keys.right;
    node["stratagem"] = keys.stratagem;

    return node;
}

bool convert<hdvs::Config::Keys>::decode(const Node& node, hdvs::Config::Keys& keys)
{
    if (!(node.IsMap() && node["up"] && node["down"] && node["left"] && node["right"] && node["stratagem"]))
        return false;

    keys.up         = node["up"].as<int>();
    keys.down       = node["down"].as<int>();
    keys.left       = node["left"].as<int>();
    keys.right      = node["right"].as<int>();
    keys.stratagem  = node["stratagem"].as<int>();

    return true;
}

} // namespace YAML
