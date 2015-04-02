/* *** View class ***
The View class encapsulates the data and functions needed to generate the map
display, and control its properties. It has a "memory" for the names and locations
of the to-be-plotted objects.

Usage: 
1. Call the update_location function with the name and position of each object
to be plotted. If the object is not already in the View's memory, it will be added
along with its location. If it is already present, its location will be set to the 
supplied location. If a single object changes location, its location can be separately
updated with a call to update_location. 

2. Call the update_remove function with the name of any object that should
no longer be plotted. This must be done *after* any call to update_location that
has the same object name since update_location will add any object name supplied.

3. Call the draw function to print out the map.

4. As needed, change the origin, scale, or displayed size of the map
with the appropriate functions. Since the view "remembers" the previously updated
information, the draw function will print out a map showing the previous objects
using the new settings.
 */
#ifndef VIEW_H
#define VIEW_H
#include "Geometry.h"

class View {
public:
    View() {};
    // disallow copy/move construction or assignment
    View(View& other)=delete;
    View(View&& other)=delete;
    View& operator=(View& rhs)=delete;
    View& operator=(View&& rhs)=delete;
    
    // prints out the current map
    virtual void draw() = 0;
    
    // Remove the name and its location; no error if the name is not present.
    virtual void update_remove(const std::string& name_) = 0;
    
    // Update the Sim_object in the View with 'name' == name
    virtual void update_location(const std::string& name, Point location) = 0;
    
    // *** Fat Interface *** //
    // Default behavior is to do nothing //
    
    // Update ship fuel
    virtual void update_fuel(const std::string& name, double fuel_) { };
    
    // Update ship heading
    virtual void update_heading(const std::string& name_, double heading_) { };
    
    // Update ship speed
    virtual void update_course_and_speed(const std::string& name, double course_, double speed_) { };
};

#endif