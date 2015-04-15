#include "Cruise_ship.h"
#include "Model.h"
#include "Island.h"
#include "Cruise_ship.h"
#include "Model.h"
#include "Geometry.h"
#include "Island.h"
#include "Utility.h"

//MITCH: remove
#include "Pirate_ship.h"

#include <vector>
#include <memory>
#include <algorithm>
#define NDEBUG
#include <cassert>
#include <iostream>
#include <string>

using std::shared_ptr;
using std::vector;
using std::sort;
using std::cout;
using std::endl;
using std::string;



Cruise_ship::Cruise_ship(const std::string& name_, Point position_):
Ship(name_, position_, 500, 15., 2., 0),
dock_action(Dock_action_e::NO_STATE),
cruise_speed(0.)
{}




void Cruise_ship::update() {
    Ship::update();
    if (!is_in_cruise()) {
        return;
    } else if (can_dock(next_island_ptr)) {
        dock(next_island_ptr);
        //If there is no next island to visit, end cruise
        if (!next_island_ptr){
            cout << get_name() << " cruise is over at " << get_docked_Island()->get_name() << endl;
            reset_cruise_state();
            return;
        }
    } else if (is_docked()) {
        switch (dock_action) {
            case Dock_action_e::REFUEL:
                refuel();
                dock_action = Dock_action_e::SIGHTSEEING;
                break;
            case Dock_action_e::SIGHTSEEING:
                dock_action = Dock_action_e::SET_NEW_COURSE;
                break;
            case Dock_action_e::SET_NEW_COURSE:
                Ship::set_destination_position_and_speed(next_island_ptr->get_location(), cruise_speed);
                cout << get_name() << " will visit " << next_island_ptr->get_name() << endl;
                dock_action = Dock_action_e::NO_STATE;
                break;
            default:
                assert(0); //SHOULD NEVER REACH
                break;
        }
    }
}


void Cruise_ship::dock(shared_ptr<Island> island_ptr){
    Ship::dock(island_ptr);
    dock_action = Dock_action_e::REFUEL;
    //Insert the island we just docked at into visited islands
    visited_islands.insert(visited_islands.end(), get_docked_Island());
    
    //Gets the next island to visit. Next island will be nullptr if nothing left to visit
    next_island_ptr = get_closest_island(get_location(), &visited_islands);
    //if we have no more islands to visit are we are not at the starting island, set the next destination to the starting island
    //This means that if there are no more islands to visit we will be sent to the starting island and
    //if we are docked at the starting island and there are no more islands to visit, the next island will be nullptr
    if (!next_island_ptr && get_docked_Island() != start_island_ptr) {
        next_island_ptr = start_island_ptr;
    }
}

//Must restart the dock staging if cruise forcefully left
void Cruise_ship::set_destination_position_and_speed(Point destination_position, double speed){
    if (is_in_cruise())
        cancel_cruise();
    cruise_speed = speed;
    //set the new course to the given position
    Ship::set_destination_position_and_speed(destination_position, speed);
    start_island_ptr = Model::get_instance().get_island_at(destination_position);
    //if the given point was the position of an island, then start_island_ptr will not be nullptr
    if (start_island_ptr) {
        //start cruise
        next_island_ptr = start_island_ptr;
        cout << get_name() << " will visit " << start_island_ptr->get_name() << endl
        << get_name() << " cruise will start and end at " << start_island_ptr->get_name() << endl;
    }
}

void Cruise_ship::set_course_and_speed(double course, double speed){
    Ship::set_course_and_speed(course, speed);
    if (is_in_cruise())
        cancel_cruise();
}

void Cruise_ship::stop(){
    Ship::stop();
    if (is_in_cruise())
        cancel_cruise();
}


void Cruise_ship::describe() const{
    cout << "\nCruise_ship ";
    Ship::describe();
    if (is_in_cruise()) {
        if (is_moving()) {
            cout << "On cruise to " << next_island_ptr->get_name();
        } else if (is_docked()){
            cout <<  "Waiting during cruise at " << get_docked_Island()->get_name();
        }
        cout << endl;
    }
}



///Private

void Cruise_ship::cancel_cruise(){
    cout << get_name() << " canceling current cruise" << endl;
    reset_cruise_state();
}


void Cruise_ship::reset_cruise_state(){
    visited_islands.clear();
    start_island_ptr.reset();
    next_island_ptr.reset();
    cruise_speed = 0.;
    dock_action = Dock_action_e::NO_STATE;
}

bool Cruise_ship::is_in_cruise() const{
    //If there is a starting island for the cruise, we know we are in a cruise
    return static_cast<bool>(start_island_ptr);
}