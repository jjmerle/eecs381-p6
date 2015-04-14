#ifndef GRAPHIC_VIEW_H
#define GRAPHIC_VIEW_H
#include "View.h"
#include "Utility.h"
#include <map>

class GraphicView : public View {
public:
    void draw() override;
    
protected:
    // Pseudo-abstract Constructor
    GraphicView(int size_, double scale_, Point origin, bool draw_y);
    
    // Calculate subscripts of Sim_object on the map
    bool get_subscripts(int &ix, int &iy, Point location);
    int get_first_dimension_size() const { return size; }
    double get_scale() const { return scale; }
    Point get_origin() const { return origin; }
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
    virtual void print_map_heading() const = 0;
    // Get x, y coordinates and name/points to map
    virtual std::map<std::string, Point> get_draw_info() const = 0;
    // Get empty space from derived class
    virtual const char* const get_empty_space() const { return empty_map_space_c; }
    // Get space with multiple ships from derived class
    virtual const char* const get_crowded_space() const { return crowded_map_space_c; }
    // Get the second dimension of the map
    virtual int get_second_dimension_size() const { return size; }
};

#endif