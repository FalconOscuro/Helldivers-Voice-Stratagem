#ifndef STRATAGEM_H
#define STRATAGEM_H

#include <string>
#include <vector>
#include <yaml-cpp/yaml.h>

namespace hdvs {

struct Stratagem
{
    std::string name;
    std::string icon;
    std::vector<std::string> trigger;
    bool enabled;

    enum Direction {
        UP = 'U',
        DOWN = 'D',
        LEFT = 'L',
        RIGHT = 'R',
    }; // enum Direction
    std::vector<Direction> code;

    enum Type {
        NONE        = 0      ,
        SPECIAL     = 1      ,
        HELLPOD     = 1 << 1 ,
        EXPLOSIVE   = 1 << 2 ,
        MISSION     = 1 << 3 ,
        WEAPON      = 1 << 4 ,
        EAGLE       = 1 << 5 ,
        SUPPLY      = 1 << 6 ,
        INCENDIARY  = 1 << 7 ,
        OFFENSIVE   = 1 << 8 ,
        ORBIRTAL    = 1 << 9 ,
        BACKPACK    = 1 << 10,
        DEFENSIVE   = 1 << 11,
        SENTRY      = 1 << 12,
        VEHICLE     = 1 << 13,
    }; // enum Type
    static const char NUM_TYPES = 14;

    Type type;

    std::string GetIcon() const;
    bool operator==(const Stratagem& rhs) const;
}; // struct Stratagem

} // namespace hdvs

namespace YAML {

template<>
struct convert<hdvs::Stratagem> {
    static Node encode(const hdvs::Stratagem& strat);
    static bool decode(const Node& node, hdvs::Stratagem& strat);
}; // struct convert<hdvs::Stratagem>

template<>
struct convert<hdvs::Stratagem::Type> {
    static Node encode(const hdvs::Stratagem::Type& type);
    static bool decode(const Node& node, hdvs::Stratagem::Type& type);
}; // struct convert<hdvs::Stratagem::Type>

template<>
struct convert<hdvs::Stratagem::Direction> {
    static Node encode(const hdvs::Stratagem::Direction& dir);
    static bool decode(const Node& node, hdvs::Stratagem::Direction& dir);
}; // struct convert<hdvs::Stratagem::Direction>

} // namespace YAML

#endif // STRATAGEM_H
