#include "GraphicView.h"
#include <cmath>
#include <map>
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>
using std::cout;
using std::endl;
using std::floor;
using std::map;
using std::setw;
using std::string;
using std::vector;

void GraphicView::draw() {
    
    print_map_heading();
    
    vector<vector<string>> matrix;
    for(int i=0; i<get_second_dimension_size(); ++i) { // get_size() is inlined
        matrix.push_back(vector<string>(get_first_dimension_size(), get_empty_space()));
    }
    
    map<string, Point> points_to_plot = get_draw_info();
    
    // Mark the Matrix with Object Locations
    for(const auto& name_point_pair : points_to_plot) {
        int x, y;
        if(get_subscripts(x, y, name_point_pair.second)) {
            matrix[get_second_dimension_size()-y-1][x] =
            (matrix[get_second_dimension_size()-y-1][x] == get_empty_space()) ?
            name_point_pair.first.substr(0, 2) : get_crowded_space();
        }

    }
    
    // Output our Matrix
    int i = 0, y = ( (size-1) * scale) + origin.y;
    for(const auto& row : matrix) {
        cout << setw(5);
        if(draw_y_coordinates && !((size-i-1) % 3)) { // Output Y-Column subscripts
            cout.precision(0);
            cout << setw(4) << y-i * scale;
        }
        cout  << " ";
        ++i;
        for(const auto& cell : row) { // TODO - ostream iterator
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

// Protected Functions
GraphicView::GraphicView(int size_, double scale_, Point origin_, bool draw_y) : View(),
size(size_), scale(scale_), origin(origin_), draw_y_coordinates(draw_y) {}

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