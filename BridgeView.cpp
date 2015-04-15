#include "BridgeView.h"
#include "Navigation.h"
#include <iostream>
#include <map>
#include <string>
using std::cout;
using std::endl;
using std::map;
using std::pair;
using std::string;

// default constructor sets the default size, scale, and origin, outputs constructor message
BridgeView::BridgeView(const string& name_) : GraphicView(19, 10, -90.0, false),name(name_), is_afloat(true) { }

// prints out the current map
map<string, Point> BridgeView::get_draw_info() const {
    map<string, Point> points_to_plot;
    if(is_afloat) {
        Point ownship_location = object_locations.find(name)->second;
        for(const pair<string, Point>& name_position_pair : object_locations) {
            if(name_position_pair.first != name) {
                Point othership_location = name_position_pair.second;
                Compass_position compass_pos(ownship_location, othership_location);
                if(compass_pos.range > 20 || compass_pos.range < 0.005) {
                    continue;
                }
                double bow_angle = compass_pos.bearing - heading;
                if(bow_angle < -180.0) { // TODO - fmod?
                    bow_angle += 360.0;
                } else if(bow_angle > 180.0) {
                    bow_angle -= 360.0;
                }
                if(bow_angle < -90.0 || bow_angle > 100.0) {
                    continue;
                }
                points_to_plot[name_position_pair.first] = Point(bow_angle, 0);
            }
        }
    }
    return points_to_plot;
}

void BridgeView::print_map_heading() {
    cout << "Bridge view from " << name;
    if(is_afloat) {
        cout << " position " << ownship_location << " heading " << heading;
    } else {
        cout << " sunk at " << ownship_location;
    }
    cout << endl;
}

// Update the location of a name in the View
void BridgeView::update_location(const string& name_, Point location) {
    object_locations[name_] = location;
    if(name == name_) {
        ownship_location = location;
    }
}

void BridgeView::update_remove(const string& name_) {
    object_locations.erase(name_);
    if(name == name_) {
        is_afloat = false;
    }
}

// Update ship heading
void BridgeView::update_course(const string& name_, double course_) {
    if(name == name_) {
        heading = course_;
    }
}

// Get empty space from derived class
const char* const BridgeView::get_empty_space() const {
    return (is_afloat) ? empty_map_space_c : "w-";
}
