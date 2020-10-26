#include "flightplan.h"
#include "iostream"

#include <clocale>
#include "coordinatestransform.h"

using namespace std;
using namespace tinyxml2;

FlightPlan::FlightPlan(): origin()
{

}


FlightPlan::FlightPlan(string uri) : origin()
{
    setlocale(LC_ALL, "C"); // needed for stod() to use '.' as decimal separator instead of ',' (at least in France)
    XMLDocument doc;

    if(uri.substr(0,4) == "file") {
        string path = uri.substr(7, uri.length()-7);
        doc.LoadFile(path.c_str());
    }

    if(doc.Error()) {
        cerr << "Error parsing " << uri << ": " << doc.ErrorStr();
    }
    else {
        XMLElement* fp_root = doc.FirstChildElement( "dump" )->FirstChildElement( "flight_plan" );
        name = fp_root->Attribute("name");
        const char* lat0 = fp_root->Attribute("lat0");
        const char* lon0 = fp_root->Attribute("lon0");
        const char* defalt = fp_root->Attribute("alt");
        const char* max_dist_home = fp_root->Attribute("max_dist_from_home");
        const char* gnd_alt = fp_root->Attribute("ground_alt");
        const char* sec_h = fp_root->Attribute("security_height");

        defaultAlt = stod(defalt);
        max_dist_from_home = stod(max_dist_home);
        ground_alt = stod(gnd_alt);
        security_height = stod(sec_h);

        origin = make_shared<Waypoint>("__ORIGIN", 0, stod(lat0), stod(lon0), defaultAlt);

        XMLElement* wps = fp_root->FirstChildElement("waypoints");
        parse_waypoints(wps);
        XMLElement* blks = fp_root->FirstChildElement("blocks");
        parse_blocks(blks);

        XMLElement* exs = fp_root->FirstChildElement("exceptions");
        if(exs) {
            parse_exceptions(exs);
        }

    }
}

void FlightPlan::parse_exceptions(XMLElement* exs) {
    for(auto ex=exs->FirstChildElement(); ex!=nullptr; ex=ex->NextSiblingElement()) {
        const char* cond = ex->Attribute("cond");
        const char* deroute = ex->Attribute("deroute");

        auto e = make_shared<Exception>();
        e->cond = cond;
        e->deroute = deroute;

        exceptions.push_back(e);
    }
}

void FlightPlan::parse_waypoints(XMLElement* wps) {
    CoordinatesTransform::get()->init_WGS84_UTM(origin->getLat(), origin->getLon());

    uint8_t wp_id = 1;
    for(auto wp=wps->FirstChildElement(); wp!=nullptr; wp=wp->NextSiblingElement()) {
        auto waypoint = make_shared<Waypoint>(wp, wp_id, origin, defaultAlt);
        waypoints.push_back(waypoint);
        ++wp_id;
    }
}

void FlightPlan::parse_blocks(tinyxml2::XMLElement* blks) {

    for(auto blk=blks->FirstChildElement(); blk!=nullptr; blk=blk->NextSiblingElement()) {
        auto block = make_shared<Block>(blk);
        blocks.push_back(block);
    }

    std::sort(blocks.begin(), blocks.end(),
              [](auto lblock, auto rblock) {
                    return lblock->getNo() < rblock->getNo();
    });
}

shared_ptr<Waypoint> FlightPlan::getWaypoint(uint8_t id) {
    for(auto& wp: waypoints) {
        if(wp->getId() == id) {
            return wp;
        }
    }
    throw runtime_error("No waypoint with id " + to_string(id));
}

shared_ptr<Block> FlightPlan::getBlock(uint8_t no) {
    for(auto& bl: blocks) {
        if(bl->getNo() == no) {
            return bl;
        }
    }
    throw runtime_error("No block with no " + to_string(no));
}

vector<shared_ptr<BlockGroup>> FlightPlan::getGroups()
{
    if(blocks.size() == 0) {
        return vector<shared_ptr<BlockGroup>>();
    }

    vector<shared_ptr<BlockGroup>> v;

    shared_ptr<BlockGroup> bg = make_shared<BlockGroup>();
    bg->group_name = blocks[0]->getGroup();
    v.push_back(bg);

    for(auto b: blocks) {
        if(b->getGroup() != v[v.size()-1]->group_name) {
            shared_ptr<BlockGroup> bg = make_shared<BlockGroup>();
            bg->group_name = b->getGroup();
            v.push_back(bg);
        }
        v[v.size()-1]->blocks.push_back(b);
    }

    return v;
}
