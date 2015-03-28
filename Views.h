#ifndef VIEWS_H
#define VIEWS_H
#include "View.h"
#include "Geometry.h"
#include <map>
#include <string>

class SailingView : public View {
public:
    // Struct containing all data needed for SailingView
    struct SailingViewInfo {
        double fuel;
        double course;
        double speed;
    };
    
    // Update the location of a name in the View
    void update(const std::string& name, Point) override;
    
    // Remove the name and its location; no error if the name is not present.
    void update_remove(const std::string& name) override;
    
    // prints out the current map
    void draw() override;
    
private:
    std::map<std::string, SailingViewInfo> ship_sailing_data;
    
};

class GraphicView : public View {
public:
    GraphicView(Point origin_, double size_, double scale_);
    ~GraphicView();
    
    // Update the location of a name in the View
    void update(const std::string& name, Point location) override;
    
    // Remove the name and its location; no error if the name is not present.
    void update_remove(const std::string& name) override;
    
    // modify the display parameters
    // if the size is out of bounds will throw Error("New map size is too big!")
    // or Error("New map size is too small!")
    void set_size(int size_);
    
protected:
    Point origin;		// coordinates of the lower-left-hand corner
    double scale;		// distance per cell of the display
    int size;			// current size of the display
    // Locations of all Sim_objects in the simulation
    std::map<std::string, Point> object_locations;
    // Calculate subscripts of Sim_object on the map
    bool get_subscripts(int &ix, int &iy, Point location);
    
};

class MapView : public GraphicView {
public:
    // default constructor sets the default size, scale, and origin
    MapView();
    ~MapView();

    // prints out the current map
    void draw() override;
    
    // Discard the saved information - drawing will show only a empty pattern
    void clear();
    
    // If scale is not postive, will throw Error("New map scale must be positive!");
    void set_scale(double scale_);
    
    // any values are legal for the origin
    void set_origin(Point origin_);
    
    // set the parameters to the default values
    void set_defaults();
    
protected:
    // Calculate the cell subscripts corresponding to the location parameter, using the
    // current size, scale, and origin of the display.
    // Return true if the location is within the map, false if not
    bool get_subscripts(int &ix, int &iy, Point location);
};

class BridgeView : public GraphicView {
public:
    BridgeView();
    ~BridgeView();
    
    // prints out the current map
    void draw() override;
    
};

#endif