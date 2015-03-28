#include "Cruise_ship.h"
#include "Model.h"
#include "Island.h"
#include "Geometry.h"
#include <algorithm>
#include <cassert>
#include <cmath>
#include <iostream>
#include <memory>
#include <set>
#include <string>
#include <vector>
using std::cout;
using std::endl;
using std::min_element;
using std::set;
using std::shared_ptr;
using std::string;
using std::vector;

// Class helper functions
void Cruise_ship::cancel_cruise() {
    cruise_speed = -1;
    first_destination = cruise_destination = nullptr;
    islands = Model::get_instance().get_islands();
    cruise_state = Cruise_State_e::NOT_CRUISING;
    cout <<  " canceling current cruise" << endl;
}

// Class Public Interface
Cruise_ship::Cruise_ship(const string& name_, Point position_) :
    Ship(name_, position_, 500, 15., 2, 0), cruise_speed(0),
    cruise_state(Cruise_State_e::NOT_CRUISING),
    islands(Model::get_instance().get_islands()) {
        cout << "Cruise_ship " << name_ << " constructed" << endl;
    }

// Update Cruise_ship state
void Cruise_ship::update() {
    Ship::update();
    if(!can_move() && cruise_state != Cruise_State_e::NOT_CRUISING) {
        cancel_cruise();
    } else {
        switch(cruise_state) {
            case Cruise_State_e::NOT_CRUISING:
                break;
            case Cruise_State_e::CRUISING_TO_DESTINATION:
                if(can_dock(cruise_destination)) {
                    dock(cruise_destination);
                    if(first_destination == cruise_destination) {
                        cruise_state = Cruise_State_e::NOT_CRUISING;
                    }
                    cruise_state = Cruise_State_e::REFUELING;
                }
                break;
            case Cruise_State_e::REFUELING:
                refuel();
                cruise_state = Cruise_State_e::DOCKED_SIGHTSEEING;
                break;
            case Cruise_State_e::DOCKED_SIGHTSEEING:
                cruise_state = Cruise_State_e::LEAVING_ISLAND;
                break;
            case Cruise_State_e::LEAVING_ISLAND:
                // Find the closest, non-visited Island (break ties lexicographically)
                Point current_location = get_location();
                auto closest_island_it =
                        min_element(islands.begin(), islands.end(),
                               [&current_location](shared_ptr<Island> island_ptr1, shared_ptr<Island> island_ptr2) {
                                   double potential_distance1 =
                                   cartesian_distance(current_location, island_ptr1->get_location());
                                   double potential_distance2 =
                                   cartesian_distance(current_location, island_ptr2->get_location());
                                   if(abs(potential_distance1 - potential_distance2) < .01) {
                                       return island_ptr1->get_name() < island_ptr2->get_name();
                                   } else {
                                       return potential_distance1 < potential_distance2;
                                   }
                               });
                Ship::set_destination_position_and_speed((*closest_island_it)->get_location(), cruise_speed);
                islands.erase(closest_island_it);
                cruise_state = Cruise_State_e::CRUISING_TO_DESTINATION;
                break;
            
        };
    }
}

void Cruise_ship::describe() const {
    Ship::describe();
    switch(cruise_state) {
        case Cruise_State_e::CRUISING_TO_DESTINATION:
            cout << "On cruise to " << cruise_destination->get_name() << endl;
            break;
        case Cruise_State_e::DOCKED_SIGHTSEEING: // Drop-through
        case Cruise_State_e::REFUELING:
        case Cruise_State_e::LEAVING_ISLAND:
            cout << "Waiting during cruise at " << endl;
            break;
        default:
            break;
    }
}

// Start moving to a destination position at a speed
void Cruise_ship::set_destination_position_and_speed(Point destination_position, double speed) {
    if(cruise_state != Cruise_State_e::NOT_CRUISING) {
        cancel_cruise();
    }
    Ship::set_destination_position_and_speed(destination_position, speed);
    shared_ptr<Island> possible_island = Model::get_instance().get_island_from_location(destination_position);
    if(possible_island) {
        cruise_destination = possible_island;
        islands  = Model::get_instance().get_islands();
        cruise_state = Cruise_State_e::CRUISING_TO_DESTINATION;
        cout << " cruise will start and end at " << cruise_destination->get_name() << endl;
    }
}

// Start moving on a course and speed
void Cruise_ship::set_course_and_speed(double course, double speed) {
    if(cruise_state != Cruise_State_e::NOT_CRUISING) {
        cancel_cruise();
    }
    Ship::set_course_and_speed(course, speed);
}

// Stop moving
void Cruise_ship::stop() {
    cruise_state = Cruise_State_e::NOT_CRUISING;
    Ship::stop();
}