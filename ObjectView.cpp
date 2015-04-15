#include "ObjectView.h"
#include "Utility.h"
#include <iostream>
#include <map>
#include <string>
using std::cout;
using std::endl;
using std::map;
using std::string;


ObjectView::ObjectView(const string& name_) : GraphicView(size_default_c, scale_default_c, origin_default_c, true),
    name(name_) {}

// Update the location of a name in the View
void ObjectView::update_location(const string& name_, Point location) {
    object_locations[name_] = location;
    if(name == name_) {
        set_origin(Point(location.x - get_first_dimension_size(), location.y - get_first_dimension_size()));
    }
}

// update a removed object
void ObjectView::update_remove(const std::string& name_) {
    object_locations.erase(name_);
}

void ObjectView::print_map_heading() {
    cout << "ObjectView for Object " << name << " at location " << (*object_locations.find(name)).second << endl;
}