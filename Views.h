#ifndef VIEWS_H
#define VIEWS_H
#include "View.h"
#include "Geometry.h"
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
    // Good place for shared functionality
    // If shared base only has same member variables, NOT a reason to exist!
    // Shared member variables are NOT a reason to have a common class
    // Similar drawing, plotting
    // Print out grids with objects in them. Tell me size and scale, and where you
       // want objects (what where)
    // GraphicView can call BridgeView
    // Function in the base can call a function declared virtual in base to go to derived implementation
        // gives you ability to ask for info from derived class
    
    GraphicView(double size_, double scale_, Point origin_);
    
    // Update the location of a name in the View
    void update_location(const std::string& name_, Point location) override;
    
    // Remove the name and its location; no error if the name is not present.
    void update_remove(const std::string& name) override;
    
protected:
    // *** Fat Interface Functions for subclasses ** //
    // Clear the map of Points
    void clear();
    // modify the display parameters
    // if the size is out of bounds will throw Error("New map size is too big!")
    // or Error("New map size is too small!")
    void set_size(int size_) { size = size_; }
    void set_scale(double scale_) { scale = scale_; }
    void set_origin(Point origin_) { origin = origin_; }
    void set_defaults();
    // Calculate subscripts of Sim_object on the map
    bool get_subscripts(int &ix, int &iy, Point location); // Put equivalent (cut some out) - parameterized version of function?
    const std::map<std::string, Point>& get_object_locations();
    Point get_origin() { return origin; }
    double get_scale() { return scale; }
    int get_size() { return size; }
    
private:
    int size;			// current size of the display
    double scale;		// distance per cell of the display
    Point origin;		// coordinates of the lower-left-hand corner

    // Locations of all Sim_objects in the simulation
    std::map<std::string, Point> object_locations;
    
};

class MapView : public GraphicView {
public:
    // default constructor sets the default size, scale, and origin
    MapView();

    // prints out the current map
    void draw() override;
    
    // Discard the saved information - drawing will show only a empty pattern
    void clear();
    
    void set_size(int size_);
    
    // If scale is not postive, will throw Error("New map scale must be positive!");
    void set_scale(double scale_);
    
    // any values are legal for the origin
    void set_origin(Point origin_);
    
    // set the parameters to the default values
    void set_defaults();
};

class BridgeView : public GraphicView {
public:
    BridgeView(std::string name_);
    
    // prints out the current map
    void draw() override;
    
    // Update the location of a name in the View
    void update_location(const std::string& name_, Point location) override;
    
    // update a removed Ship
    void update_remove(const std::string& name_) override;
    
    // Update ship heading
    void update_course_and_speed(const std::string& name_, double course_, double) override;
    
private:
    std::string name;
    Point ownship_location;
    double heading;
    bool is_afloat;
};

#endif