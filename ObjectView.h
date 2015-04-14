#ifndef OBJECT_VIEW_H
#define OBJECT_VIEW_H
#include "GraphicView.h"
#include <map>
#include <string>

class ObjectView : public GraphicView { // TODO - what happens when ship is gone? update_remove
public:
    ObjectView(const std::string& name_);
    
    // Update the location of a name in the View
    void update_location(const std::string& name_, Point location) override;
    
    // update a removed Ship
    void update_remove(const std::string& name_) override;
    
private:
    // Templated Design Interface functions
    // Print the top of the map
    void print_map_heading() const override;
    // Get x, y coordinates and name/points to map
    std::map<std::string, Point> get_draw_info() const override { return object_locations; }
    
    // Locations of all Sim_objects in the simulation
    std::map<std::string, Point> object_locations;
    // Name of centered Object in the View
    std::string name;
};

#endif