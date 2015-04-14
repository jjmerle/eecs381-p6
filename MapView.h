#ifndef MAP_VIEW_H
#define MAP_VIEW_H
#include "GraphicView.h"
#include "Utility.h"
#include <map>

class MapView : public GraphicView {
public:
    // default constructor sets the default size, scale, and origin
    MapView();
    
    // Update the location of a name in the View
    void update_location(const std::string& name_, Point location) override;
    
    // update a removed Ship
    void update_remove(const std::string& name_) override;
    
    // Discard the saved information - drawing will show only a empty pattern
    void clear();
    
    void set_size(int size_) override;
    
    // If scale is not postive, will throw Error("New map scale must be positive!");
    void set_scale(double scale_) override;
    
    // any values are legal for the origin
    void set_origin(Point origin_) override;
    
    // set the parameters to the default values
    void set_defaults();
private:
    // Templated Design Interface functions
    // Print the top of the map
    void print_map_heading() const override;
    // Get x, y coordinates and name/points to map
    std::map<std::string, Point> get_draw_info() const override { return object_locations; }
    
    // Locations of all Sim_objects in the simulation
    std::map<std::string, Point> object_locations;
};

#endif
