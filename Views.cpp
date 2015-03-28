#include "Views.h"
#include "Model.h"
#include "Utility.h"
#include <cmath>
#include <iomanip>
#include <iostream>
#include <memory>
#include <string>
#include <vector>
using std::cout;
using std::endl;
using std::setw;
using std::pair;
using std::shared_ptr;
using std::string;
using std::vector;

using SailingViewInfo = SailingView::SailingViewInfo;


// ************************************** //
// ***** SailingView Implementation ***** //
// ************************************** //

void SailingView::update(const string& name, Point) {
    shared_ptr<Ship> ship_ptr = Model::get_instance().get_ship_ptr(name);
    SailingViewInfo& data_for_ship = ship_sailing_data[name];
    data_for_ship.fuel = Model::get_instance().get_fuel_from_ship(ship_ptr);
    data_for_ship.course = Model::get_instance().get_course_from_ship(ship_ptr);
    data_for_ship.speed = Model::get_instance().get_speed_from_ship(ship_ptr);
}

void SailingView::draw()  {
    cout << "----- Sailing Data -----" << endl;
    for_each(ship_sailing_data.begin(), ship_sailing_data.end(),
             [](const pair<string, SailingViewInfo>& name_sail_data_pair) {
                 cout << name_sail_data_pair.first << setw(10)
                 << name_sail_data_pair.second.fuel << setw(10)
                 << name_sail_data_pair.second.course << setw(10)
                 << name_sail_data_pair.second.speed << endl;
             });
}

// ************************************** //
// ***** GraphicView Implementation ***** //
// ************************************** //

GraphicView::GraphicView(Point origin_, double size_, double scale_) :
    View(), origin(origin_), size(size_), scale(scale_) {
    cout << "GraphicView constructed" << endl;
}

GraphicView::~GraphicView() {
    cout << "GraphicView destructed" << endl;
}

// Save the supplied name and location for future use in a draw() call
// If the name is already present,the new location replaces the previous one.
void GraphicView::update(const string& name, Point location) {
    object_locations[name] = location;
}

// Remove the name and its location; no error if the name is not present.
void GraphicView::update_remove(const string& name) {
    auto location_it = object_locations.find(name);
    if(location_it != object_locations.end()) {
        object_locations.erase(location_it);
    }
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


// ********************************** //
// ***** MapView Implementation ***** //
// ********************************** //
static int size_default_c = 25;
static double mapview_scale_default_c = 2;
static Point mapview_point_default_c(-10, -10);
static string empty_space_c = ". ";

// default constructor sets the default size, scale, and origin, outputs constructor message
MapView::MapView() : GraphicView(mapview_point_default_c, size_default_c, mapview_scale_default_c) {
    cout << "MapView constructed" << endl;
}

MapView::~MapView() { // outputs destructor message
    cout << "MapView destructed" << endl;
}

// prints out the current map
void MapView::draw() {
    cout << "Display size: " <<  size << ", scale: " << scale << ", origin: " << origin << endl;
    vector< vector<string> > matrix(size, vector<string>(size, empty_space_c));
    
    bool first_outside = false; // Bool to track if we have an Object outside of the Map
    // Mark the Matrix with Object Locations
    for(auto pair : object_locations) {
        int x, y;
        if(get_subscripts(x, y, pair.second)) {
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

// Discard the saved information - drawing will show only a empty pattern
void MapView::clear() {
    object_locations.clear();
}

// modify the display parameters
// if the size is out of bounds will throw Error("New map size is too big!")
// or Error("New map size is too small!")
void GraphicView::set_size(int size_) {
    if(size_ > 30)
        throw Error("New map size is too big!");
    if(size_ <= 6)
        throw Error("New map size is too small!");
    size = size_;
}

// If scale is not postive, will throw Error("New map scale must be positive!");
void MapView::set_scale(double scale_) {
    if(scale_ <= 0.0)
        throw Error("New map scale must be positive!");
    scale = scale_;
}

// any values are legal for the origin
void MapView::set_origin(Point origin_) {
    origin = origin_;
}

// set the parameters to the default values
void MapView::set_defaults() {
    size = size_default_c;
    scale = mapview_scale_default_c;
    origin = mapview_point_default_c;
}

// ************************************* //
// ***** BridgeView Implementation ***** //
// ************************************* //

// default constructor sets the default size, scale, and origin, outputs constructor message
BridgeView::BridgeView() : GraphicView(-90.0, size_default_c, 10.0) {
    cout << "BridgeView constructed" << endl;
}

BridgeView::~BridgeView() { // outputs destructor message
    cout << "BridgeView destructed" << endl;
}

// prints out the current map
void BridgeView::draw() {

}
