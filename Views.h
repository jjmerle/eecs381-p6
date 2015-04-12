#ifndef VIEWS_H
#define VIEWS_H
#include "View.h"
#include "Geometry.h"
#include "Utility.h"
#include <map>
#include <string>

class SailingView : public View {
public:
    // prints out the current map
    void draw() override;
    
    // Update ship fuel
    void update_fuel(const std::string& name, double fuel_) override;
    
    // Update the Sim_object in the View with 'name' == name
    void update_location(const std::string& name_, Point location) override;
    
    // Remove a Ship from the View
    void update_remove(const std::string& name) override;
    
    // Update ship speed
    void update_course_and_speed(const std::string& name, double course_, double speed_) override;

private:
    // Struct containing all data needed for SailingView
    struct SailingViewInfo {
        double fuel;
        double course;
        double speed;
    };
    
    std::map<std::string, SailingViewInfo> ship_sailing_data;
    
};

class GraphicView : public View {
public:
    GraphicView(int size_, double scale_, Point origin, bool draw_y);
    
    void draw() override;
    
protected:
    // Calculate subscripts of Sim_object on the map
    bool get_subscripts(int &ix, int &iy, Point location);
    int get_first_dimension_size() { return size; }
    double get_scale() { return scale; }
    Point get_origin() { return origin; }
    virtual void set_size(int size_) { size = size_; }
    virtual void set_scale(double scale_) { scale = scale_; }
    virtual void set_origin(Point origin_) { origin = origin_; }
    
private:
    int size;			// current size of the display
    double scale;		// distance per cell of the display
    Point origin;		// coordinates of the lower-left-hand corner
    bool draw_y_coordinates;
    // Template Pattern helpers
    // Print the top of the map
    virtual void print_map_heading() = 0;
    // Get x, y coordinates and name/points to map
    virtual std::map<std::string, Point> get_draw_info() = 0;
    // Get empty space from derived class
    virtual const char* const get_empty_space() = 0;
    // Get space with multiple ships from derived class
    virtual const char* const get_crowded_space() = 0;
    // Get the second dimension of the map
    virtual int get_second_dimension_size() = 0;
};

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
    void print_map_heading() override;
    // Get x, y coordinates and name/points to map
    std::map<std::string, Point> get_draw_info() override;
    // Locations of all Sim_objects in the simulation
    std::map<std::string, Point> object_locations;
    // Get empty space from derived class
    const char* const get_empty_space() override;
    // Get space with multiple ships from derived class
    const char* const get_crowded_space() override { return "**"; }
    // Get the second dimension of the map
    int get_second_dimension_size() override { return 3; }
    std::string name;
    bool is_afloat;
    Point ownship_location;
    double heading;
};

class SquareView : public GraphicView {
public:
    // default constructor sets the default size, scale, and origin
    SquareView();
    
    // Update the location of a name in the View
    void update_location(const std::string& name_, Point location) override;
    
    // Remove the name and its location; no error if the name is not present.
    void update_remove(const std::string& name) override;
    
private:
    // Print the top of the map
    void print_map_heading() override;
    // Get x, y coordinates and name/points to map
    std::map<std::string, Point> get_draw_info() override;
    // Get empty space from derived class
    const char* const get_empty_space() override { return empty_map_space_c; }
    // Get space with multiple ships from derived class
    const char* const get_crowded_space() override { return "* "; }
    // Get the second dimension of the map
    int get_second_dimension_size() override { return get_first_dimension_size(); }
    // Locations of all Sim_objects in the simulation
    std::map<std::string, Point> object_locations;
};

class MapView : public SquareView {
public:
    // default constructor sets the default size, scale, and origin
    MapView();
    
    // Update the location of a name in the View
    void update_location(const std::string& name_, Point location) override;
    
    // Remove the name and its location; no error if the name is not present.
    void update_remove(const std::string& name) override;
    
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
    // Print the top of the map
    void print_map_heading() override;
    // Get x, y coordinates and name/points to map
    std::map<std::string, Point> get_draw_info() override;
    // Get empty space from derived class
    const char* const get_empty_space() override { return empty_map_space_c; }
    // Get space with multiple ships from derived class
    const char* const get_crowded_space() override { return "* "; }
    // Get the second dimension of the map
    int get_second_dimension_size() override { return get_first_dimension_size(); }
};

class ObjectView : public SquareView {
public:
    ObjectView(const std::string& name_);
    
    // Update the location of a name in the View
    void update_location(const std::string& name_, Point location) override;
    
    // update a removed Ship
    void update_remove(const std::string& name_) override;
    
private:
    // Print the top of the map
    void print_map_heading() override;
    // Get x, y coordinates and name/points to map
    std::map<std::string, Point> get_draw_info() override;
    // Get empty space from derived class
    const char* const get_empty_space() override { return empty_map_space_c; }
    // Get space with multiple ships from derived class
    const char* const get_crowded_space() override { return "* "; }
    // Get the second dimension of the map
    int get_second_dimension_size() override { return get_first_dimension_size(); }
    std::string name;
};

#endif