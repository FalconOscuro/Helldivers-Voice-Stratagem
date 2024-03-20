#include "HDVS/stratagem.h"

namespace hdvs {

std::string Stratagem::GetIconName() const
{
    if (icon != "")
        return icon + (icon.find('.') == std::string::npos ? ".png" : "");

    return name.substr(0, name.find(' ')) + ".png";
}

bool Stratagem::operator==(const Stratagem& rhs) const {
    // Compare tirggers? Un-ordered so == not viable
   return name == rhs.name && code == rhs.code && type == rhs.type; 
}

// Ordered array of Stratagem Types as seen in enum for conversion
const std::string TYPE_NAMES[Stratagem::NUM_TYPES] = {
    "special"       , "hellpod" , "explosive"   , "mission"     ,
    "support weapon", "eagle"   , "supply"      , "incendiary"  ,
    "offensive"     , "orbital" , "backpack"    , "defensive"   ,
    "sentry"        , "vehicle"
};

} // namespace hdvs

namespace YAML {

Node convert<hdvs::Stratagem>::encode(const hdvs::Stratagem& strat)
{
    Node node;

    node["name"] = strat.name;
    node["trigger"] = strat.trigger;
    node["code"] = strat.code;
    node["type"] = strat.type;
    node["enabled"] = strat.enabled;

    if (strat.icon != "")
        node["icon"] = strat.icon;

    return node;
}

bool convert<hdvs::Stratagem>::decode(const Node& node, hdvs::Stratagem& strat)
{
    if (!(node.IsMap() && node["name"] && node["trigger"] && node["code"] && node["type"]))
        return false;

    strat.name = node["name"].as<std::string>();
    strat.enabled = node["enabled"] ? node["enabled"].as<bool>() : false;
    strat.icon = node["icon"] ? node["icon"].as<std::string>() : "";

    // big and block of remaining conversions
    return (
        convert<std::vector<std::string>>::decode(node["trigger"], strat.trigger)
            && convert<std::vector<hdvs::Stratagem::Direction>>::decode(node["code"], strat.code)
            && convert<hdvs::Stratagem::Type>::decode(node["type"], strat.type)
    );
}

// this is overly complex, could've just used vector
Node convert<hdvs::Stratagem::Type>::encode(const hdvs::Stratagem::Type& type)
{
    Node node;

    // check flags individually, if existing, add to sequence
    for (char i = 0; i < hdvs::Stratagem::NUM_TYPES; i++)
        if ((1 << i) & type)
            node.push_back(hdvs::TYPE_NAMES[static_cast<int>(i)]);

    return node;
}

// this is overly complex, could've just used vector
bool convert<hdvs::Stratagem::Type>::decode(const Node& node, hdvs::Stratagem::Type& type)
{
    if (!node.IsSequence())
        return false;

    for (size_t i = 0; i < node.size(); i++)
    {
        std::string typeStr = node[i].as<std::string>();
        for (auto j = 0; j < hdvs::Stratagem::NUM_TYPES; j++)
            if (hdvs::TYPE_NAMES[j] == typeStr) // loop all type names
            {
                type = (hdvs::Stratagem::Type)(type | 1 << j);
                break;
                // error/warn if fail?
            }
    }

    return true;
}

Node convert<hdvs::Stratagem::Direction>::encode(const hdvs::Stratagem::Direction& dir)
{
    return Node((char)dir);
}

bool convert<hdvs::Stratagem::Direction>::decode(const Node& node, hdvs::Stratagem::Direction& dir)
{
    if (!node.IsScalar())
        return false;

    dir = (hdvs::Stratagem::Direction)node.as<char>();
    return true;
}

} // namespace YAML
