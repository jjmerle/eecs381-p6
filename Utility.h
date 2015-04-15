#ifndef UTILITY_H
#define UTILITY_H

#include "Geometry.h"
#include <exception>
#include <string>
#include <vector>
#include <memory>

class Sim_object;
class Ship;
class Island;

class Error : public std::exception {
public:
    Error(const char* msg_) : msg(msg_) {}
    const char* what() const noexcept override
    {return msg;}
private:
    const char* msg;
};

//gives the minimum length for a string and
//if a name matches from 0 <= i < min_ship_name_length_c
//its creation will be prevented
//also number of characters that are printed on the map
extern const int min_ship_name_length_c;


// Error message for reaching 'default' in switch statements
extern const char* const default_switch_error_c;
extern const char* const empty_map_space_c;
extern const char* const crowded_map_space_c;
extern const int size_default_c;
extern const double scale_default_c;
extern const Point origin_default_c;

//gets map code based off first min_ship_name_length_c characters of the name.
//Will return a string of size min_ship_name_length_c or less
std::string get_code(const std::string & name);

//MITCH: MAYBE MOVE IMPLEMENTATION TO CPP
struct Objects_by_distance {
    Objects_by_distance(Point location_):location(location_) {}
    
    bool operator () (std::shared_ptr<const Sim_object> obj_ptr1, std::shared_ptr<const Sim_object> obj_ptr2);
private:
    Point location;
};

//MITCH: note that th vector is not const since it must be remordered
std::shared_ptr<Ship> get_closest_ship(std::shared_ptr<const Ship> ownship_ptr);

std::shared_ptr<Island> get_closest_island(Point location, std::vector<std::shared_ptr<const Island>> * exclusions = nullptr);


#endif