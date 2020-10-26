#ifndef FLIGHTPLAN_H
#define FLIGHTPLAN_H

#include <string>
#include <vector>
#include "waypoint.h"
#include "block.h"
#include <memory>
#include "tinyxml2.h"

struct BlockGroup {
    string group_name;
    vector<shared_ptr<Block>> blocks;
};

struct Exception {
    string cond;
    string deroute;
};

class FlightPlan
{
public:



    FlightPlan();
    FlightPlan(std::string uri);

    vector<shared_ptr<Waypoint>>& getWaypoints() {return  waypoints;}
    shared_ptr<Waypoint> getWaypoint(uint8_t id);
    vector<shared_ptr<Block>>& getBlocks() {return  blocks;}
    vector<shared_ptr<BlockGroup>> getGroups();
    vector<shared_ptr<Exception>> getExeptions() {return  exceptions;}
    shared_ptr<Block> getBlock(uint8_t id);
    double getDefaultAltitude() {return defaultAlt;}
    double getGroundAlt() {return ground_alt;}
    void setGroundAlt(double ga) {ground_alt = ga;}
    shared_ptr<Waypoint> getOrigin() {return origin;}

private:
    void parse_exceptions(tinyxml2::XMLElement* exs);
    void parse_waypoints(tinyxml2::XMLElement* wps);
    void parse_blocks(tinyxml2::XMLElement* blks);
    void parse_block_stages(tinyxml2::XMLElement* blk, shared_ptr<Block> block);

    std::vector<shared_ptr<Waypoint>> waypoints;
    std::vector<shared_ptr<Exception>> exceptions;
    std::vector<shared_ptr<Block>> blocks;
    shared_ptr<Waypoint> origin;

    double defaultAlt;
    string name;
    double max_dist_from_home;
    double ground_alt;
    double security_height;


};

#endif // FLIGHTPLAN_H
