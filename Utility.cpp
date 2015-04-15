#include "Utility.h"
#include "Sim_object.h"
#include "Navigation.h"
#include "Ship.h"
#include "Island.h"



//MITCH IS THIS OKAY
#include "Model.h"


#include <memory>
#include <vector>
#include <algorithm>

using std::string;
using std::shared_ptr;
using std::vector;


const int min_ship_name_length_c = 2;



const char* const default_switch_error_c = "Error: reached default in switch statement!";
const char* const empty_map_space_c = ". ";
const char* const crowded_map_space_c = "* ";
const int size_default_c = 25;
const double scale_default_c = 2;
const Point origin_default_c(-10, -10);

string get_code(const string & name){
    if (name.length() < min_ship_name_length_c)
        return name;
    return name.substr(0, min_ship_name_length_c);
}


bool Objects_by_distance::operator () (std::shared_ptr<const Sim_object> obj_ptr1, std::shared_ptr<const Sim_object> obj_ptr2){
    
    double distance_to_obj1 = cartesian_distance(location, obj_ptr1->get_location());
    double distance_to_obj2 = cartesian_distance(location, obj_ptr2->get_location());
    if (distance_to_obj1 == distance_to_obj2)
        return obj_ptr1->get_name() < obj_ptr2->get_name();
    else
        return distance_to_obj1 < distance_to_obj2;
}


shared_ptr<Ship> get_closest_ship(shared_ptr<const Ship> ownship_ptr){
    Objects_by_distance ship_sort_by_distance(ownship_ptr->get_location());
    vector<shared_ptr<Ship>> ships;
    for (const auto & ships_name_ptr_pair : Model::get_instance().get_Ships()) {
        if (ships_name_ptr_pair.second != ownship_ptr) {
            ships.insert(lower_bound(ships.begin(), ships.end(), ships_name_ptr_pair.second,
                                     ship_sort_by_distance), ships_name_ptr_pair.second);
        }
    }
    if (!ships.empty()) {
        return ships.front();
    } else {
        return nullptr; //MITCH: IS THIS GOOD STYLE FOR A SMART POINTER?
    }
}

shared_ptr<Island> get_closest_island(Point location, vector<shared_ptr<const Island>> * exclusions){
    Objects_by_distance island_sort_by_distance(location);
    vector<shared_ptr<Island>> islands;
    //MITCH: CLEANUP. DONT CONTROL LOOP LIKE THIS i think
    for (const auto & island_name_ptr_pair : Model::get_instance().get_Islands()) {
        islands.insert(lower_bound(islands.begin(), islands.end(), island_name_ptr_pair.second,
                                   island_sort_by_distance), island_name_ptr_pair.second);
    }
    //Erase all excluded islands
    if (exclusions) {
        //MITCH: dont really need to erase but helps I guess
        sort(exclusions->begin(), exclusions->end(), island_sort_by_distance);
        auto erase_begin_it = set_difference(islands.begin(), islands.end(), exclusions->begin(), exclusions->end(), islands.begin());
        islands.erase(erase_begin_it, islands.end());
    }
    if (!islands.empty()) {
        return islands.front();
    } else {
        return nullptr; //MITCH: IS THIS GOOD STYLE FOR A SMART POINTER
    }
}
