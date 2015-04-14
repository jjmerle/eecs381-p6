#ifndef BRIDGE_VIEW_H
#define BRIDGE_VIEW_H
#include "GraphicView.h"
#include "Utility.h"
#include <string>

class BridgeView : public GraphicView {
public:
    BridgeView(const std::string& name_);
    
    // Update the location of a name in the View
    void update_location(const std::string& name_, Point location) override;
    
    // update a removed Ship
    void update_remove(const std::string& name_) override;
    
    // Update ship heading
    void update_course_and_speed(const std::string& name_, double course_, double) override;
private:
    // Print the top of the map
    void print_map_heading() const override;
    // Get x, y coordinates and name/points to map
    std::map<std::string, Point> get_draw_info() const override;
    // Get empty space from derived class
    const char* const get_empty_space() const override;
    // Get space with multiple ships from derived class
    const char* const get_crowded_space() const override { return "**"; }
    // Get the second dimension of the map
    int get_second_dimension_size() const override { return 3; } // TODO - magic constant
    
    // Locations of all Sim_objects in the simulation
    std::map<std::string, Point> object_locations;
    std::string name;
    bool is_afloat;
    Point ownship_location;
    double heading;
};

#endif
