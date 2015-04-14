#include "MapView.h"
#include "Utility.h"
#include <iostream>
#include <map>
#include <string>
using std::cout;
using std::endl;
using std::map;
using std::string;

MapView::MapView() : GraphicView(size_default_c, scale_default_c, origin_default_c, true) { }


// update location of a valid object
void MapView::update_location(const string &name_, Point location) {
    object_locations[name_] = location;
}

// update a removed object
void MapView::update_remove(const std::string& name_) {
    object_locations.erase(name_);
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
// modify the display parameters
void MapView::set_defaults() {
    GraphicView::set_size(size_default_c);
    GraphicView::set_scale(scale_default_c);
    GraphicView::set_origin(origin_default_c);
}

// Clear the map of Points
void MapView::clear() { 
    object_locations.clear();
}

void MapView::print_map_heading() const {
    cout << "Display size: " << get_first_dimension_size() << ", scale: " << get_scale() << ", origin: " << get_origin() << endl;
    bool first_outside = false; // Bool to track if we have an Object outside of the Map
    int x, y;
    for(const auto pair : object_locations) {
        if(!get_subscripts(x, y, pair.second)) {
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
}