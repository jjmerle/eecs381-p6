#include "Views.h"
#include "Model.h"
#include "Navigation.h"
#include "Utility.h"
#include <algorithm>
#include <cmath>
#include <iomanip>
#include <iostream>
#include <map>
#include <memory>
#include <string>
#include <vector>
using std::cout;
using std::endl;
using std::fill;
using std::for_each;
using std::map;
using std::setw;
using std::pair;
using std::shared_ptr;
using std::string;
using std::vector;

// ************************************** //
// ***** SailingView Implementation ***** //
// ************************************** //
void SailingView::draw()  {
    cout << "----- Sailing Data -----" << endl;
    cout << setw(10) << "Ship" << setw(10) << "Fuel" << setw(10) << "Course" << setw(10) << "Speed" << endl;
    for_each(ship_sailing_data.begin(), ship_sailing_data.end(),
             [](const pair<string, SailingViewInfo>& name_sail_data_pair) {
                 cout << setw(10)
                    << name_sail_data_pair.first <<         setw(10)
                    << name_sail_data_pair.second.fuel <<   setw(10)
                    << name_sail_data_pair.second.course << setw(10)
                    << name_sail_data_pair.second.speed <<  endl;
             });
}

// Update the Sim_object in the View with 'name' == name
void SailingView::update_location(const string& name, Point location) {
    if(Model::get_instance().is_ship_present(name) && ship_sailing_data.find(name) == ship_sailing_data.end()) {
        SailingViewInfo sail_data;
        ship_sailing_data[name] = sail_data;
    }

}

// Update ship fuel
void SailingView::update_fuel(const string& name, double fuel_) {
    ship_sailing_data[name].fuel = fuel_;
}

// Update ship speed
void SailingView::update_course_and_speed(const string& name, double course_, double speed_) {
    SailingViewInfo& sailing_data = ship_sailing_data[name];
    sailing_data.course = course_;
    sailing_data.speed = speed_;
}

// Update ship afloat state
void SailingView::update_remove(const string& name) {
    ship_sailing_data.erase(name);
}


// ************************************** //
// ***** GraphicView Implementation ***** //
// ************************************** //
const int size_default_c = 25; // TODO - Remove or put lower
const double scale_default_c = 2;
const Point point_default_c(-10, -10);
const string empty_space_c = ". ";
GraphicView::GraphicView(double size_, double scale_, Point origin_) :
    View(), size(size_), scale(scale_), origin(origin_) { }

// Save the supplied name and location for future use in a draw() call
// If the name is already present,the new location replaces the previous one.
void GraphicView::update_location(const string& name, Point location) {
    object_locations[name] = location;
}

// Remove the name and its location; no error if the name is not present.
void GraphicView::update_remove(const string& name) {
    object_locations.erase(name);
}

// Protected Functions
// *** Fat Interface Functions ** //
// Clear the map of Points
void GraphicView::clear() {
    object_locations.clear();
}

// modify the display parameters
void GraphicView::set_defaults() {
    size = size_default_c;
    scale = scale_default_c;
    origin = point_default_c;
}

/* *** Use this function to calculate the subscripts for the cell. */

/* *** This code assumes the specified private member variables. */
// Calculate the cell subscripts corresponding to the supplied location parameter,
// using the current size, scale, and origin of the display.
// This function assumes that origin is a  member variable of type Point,
// scale is a double value, and size is an integer for the number of rows/columns
// currently being used for the grid.
// Return true if the location is within the grid, false if not
bool GraphicView::get_subscripts(int &ix, int &iy, Point location)
{
    // adjust with origin and scale
    Cartesian_vector subscripts = (location - origin) / scale;
    // truncate coordinates to integer after taking the floor
    // floor function will produce integer smaller than even for negative values,
    // so - 0.05 => -1., which will be outside the array.
    ix = int(floor(subscripts.delta_x));
    iy = int(floor(subscripts.delta_y));
    // if out of range, return false
    if ((ix < 0) || (ix >= size) || (iy < 0) || (iy >= size)) {
        return false;
    }
    else
        return true;
}

const map<string, Point>& GraphicView::get_object_locations() {
    return object_locations;
}


// ********************************** //
// ***** MapView Implementation ***** //
// ********************************** //
// default constructor sets the default size, scale, and origin, outputs constructor message
MapView::MapView() : GraphicView(size_default_c, scale_default_c, point_default_c) { }

// prints out the current map
void MapView::draw() {
    Point origin = get_origin();
    int size = get_size();
    double scale = get_scale();
    const map<string, Point>& object_locations = get_object_locations();
    cout << "Display size: " <<  size << ", scale: " << scale << ", origin: " << origin << endl;
    vector< vector<string> > matrix(size, vector<string>(size, empty_space_c));
    
    bool first_outside = false; // Bool to track if we have an Object outside of the Map
    // Mark the Matrix with Object Locations
    for(auto pair : object_locations) {
        int x, y;
        if(GraphicView::get_subscripts(x, y, pair.second)) {
            matrix[size-y-1][x] = (matrix[size-y-1][x] == empty_space_c) ?
            pair.first.substr(0, 2) : "* ";
        } else {
            if(!first_outside) {
                first_outside = true;
                cout << pair.first;
            } else {
                cout << ", " << pair.first;
            }
        }
    }
    if(first_outside) {
        cout << " outside the map" << endl;
    }
    
    // Output our Matrix
    int i = 0, y = ( (size-1) * scale) + origin.y;
    for(auto& row : matrix) {
        cout << setw(5);
        if(!((size-i-1) % 3)) { // Output Y-Column subscripts
            cout.precision(0);
            cout << setw(4) << y-i * scale;
        }
        cout  << " ";
        ++i;
        for(auto& cell : row) {
            cout << cell;
        }
        cout << endl;
    }
    
    cout << setw(6);
    for(int i = 0; i < size; ++i) {
        if(!(i % 3)) {
            cout.precision(0);
            cout << origin.x + i*scale; // Output X-Row subscripts
        }
        cout << setw(6);
    }
    cout << endl;
    cout.precision(2); // reset precision
}

void MapView::clear() {
    GraphicView::clear();
}

void MapView::set_size(int size_) {
    if(size_ > 30)
        throw Error("New map size is too big!");
    if(size_ <= 6)
        throw Error("New map size is too small!");
    GraphicView::set_size(size_);
}


void MapView::set_scale(double scale_) {
    if(scale_ <= 0.0)
        throw Error("New map scale must be positive!");
    GraphicView::set_scale(scale_);
}

// any values are legal for the origin
void MapView::set_origin(Point origin_) {
    GraphicView::set_origin(origin_);
}

// set the parameters to the default values
void MapView::set_defaults() {
    GraphicView::set_defaults();
}

// ************************************* //
// ***** BridgeView Implementation ***** //
// ************************************* //

// default constructor sets the default size, scale, and origin, outputs constructor message
BridgeView::BridgeView(string name_) : GraphicView(19, 10, -90.0), name(name_), is_afloat(true) { }

// prints out the current map
void BridgeView::draw() {
    const map<string, Point>& object_locations = get_object_locations();
    cout << "Bridge view from " << name;
    if(is_afloat) {
        cout << " position " << ownship_location << " heading " << heading;
    } else {
        cout << " sunk at " << ownship_location;
    }
    cout << endl;
    
    vector< vector<string> > matrix(3);
    
    if(is_afloat) {
        fill(matrix.begin(), matrix.end(), vector<string>(19, empty_space_c));
        Point ownship_location = object_locations.find(name)->second;
        for(const pair<string, Point>& name_position_pair : object_locations) {
            if(name_position_pair.first != name) {
                Point othership_location = name_position_pair.second;
                Compass_position compass_pos(ownship_location, othership_location);
                if(compass_pos.range > 20 || compass_pos.range < 0.005) {
                    continue;
                }
                double bow_angle = compass_pos.bearing - heading;
                if(bow_angle < -180.0) {
                    bow_angle += 360.0;
                } else if(bow_angle > 180.0) {
                    bow_angle -= 360.0;
                }
                if(bow_angle < -90.0 || bow_angle > 100.0) {
                    continue;
                }
                int x, y;
                if(get_subscripts(x, y, Point(bow_angle, 0))) {
                    matrix[2][x] = (matrix[2][x] == empty_space_c) ?
                    name_position_pair.first.substr(0, 2) : "**";
                } else {
                    cout << "x " << x << " y " << y << endl;
                }
                
            }
        }
    } else {
        fill(matrix.begin(), matrix.end(), vector<string>(19, "w-"));
    }

    
    for(auto& row : matrix) {
        cout << setw(7);
        for(auto& cell : row) {
            cout << cell;
        }
        cout << endl;
    }
    
    cout << setw(6);
    for(int i = -90; i <= 90; i += 30) {
        cout << i << setw(6);
    }
    cout << endl;

}

// Update the location of a name in the View
void BridgeView::update_location(const string& name_, Point location) {
    GraphicView::update_location(name_, location);
    if(name == name_) {
        ownship_location = location;
    }
}

void BridgeView::update_remove(const string& name_) {
    GraphicView::update_remove(name_);
    if(name == name_) {
        is_afloat = false;
    }
}

// Update ship heading
void BridgeView::update_course_and_speed(const string& name_, double course_, double) {
    if(name == name_) {
        heading = course_;
    }
}