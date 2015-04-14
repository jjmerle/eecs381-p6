#include "Ship.h"
#include "Island.h"
#include "Utility.h"
#include "Model.h"

#include <iostream>
#include <string>
#include <memory>
#define NDEBUG
#include <cassert>

using std::cout;
using std::endl;
using std::string;
using std::shared_ptr;

Ship::Ship(const string& name_, Point position_, double fuel_capacity_,
           double maximum_speed_, double fuel_consumption_, int resistance_):
Sim_object(name_),
resistance(resistance_),
fuel(fuel_capacity_),
fuel_consumption(fuel_consumption_),
ship_state(State_e::STOPPED),
fuel_capacity(fuel_capacity_),
maximum_speed(maximum_speed_),
track_base(position_){}



bool Ship::can_move() const{
    return is_afloat() && ship_state != State_e::DEAD_IN_THE_WATER;
}

bool Ship::is_moving() const{
    return ship_state == State_e::MOVING_ON_COURSE || ship_state == State_e::MOVING_TO_POSITION;
}

bool Ship::is_docked() const{
    return ship_state == State_e::DOCKED;
}

bool Ship::is_afloat() const{
    return ship_state != State_e::SUNK;
}

bool Ship::can_dock(shared_ptr<Island> island_ptr) const{
    return ship_state == State_e::STOPPED &&
    cartesian_distance(island_ptr->get_location(), track_base.get_position()) <= .1;
}

/*** Interface to derived classes ***/

void Ship::update() {
    cout << get_name();
    switch (ship_state) {
        case State_e::MOVING_ON_COURSE:
        case State_e::MOVING_TO_POSITION:
            //if the ship is in either moving state, do action below
            calculate_movement();
            cout << " now at " << get_location();
            broadcast_current_state();
            break;
        case State_e::STOPPED:
            cout << " stopped at " << track_base.get_position();
            break;
        case State_e::DOCKED:
            cout << " docked at " << docked_island_ptr->get_name();
            break;
        case State_e::DEAD_IN_THE_WATER:
            cout << " dead in the water at " << track_base.get_position();
            break;
        case State_e::SUNK:
            cout << " sunk";
        default:
            assert(0); //all cases covered, should never reach
            break;
    }
    cout << endl;
}


void Ship::describe() const {
    cout << get_name() << " at " << track_base.get_position();
    if (is_afloat())
        cout << ", fuel: " << fuel << " tons, resistance: " << resistance << endl;
    switch (ship_state) {
        case State_e::SUNK:
            cout << " sunk";
            break;
        case State_e::MOVING_TO_POSITION:
            cout << "Moving to " << destination << " on " << track_base.get_course_speed();
            break;
        case State_e::MOVING_ON_COURSE:
            cout << "Moving on " << track_base.get_course_speed();
            break;
        case State_e::DOCKED:
            cout << "Docked at " << docked_island_ptr->get_name();
            break;
        case State_e::STOPPED:
            cout << "Stopped";
            break;
        case State_e::DEAD_IN_THE_WATER:
            cout << "Dead in the water";
            break;
        default:
            assert(0); //Should never reach
    }
    cout << endl;
}

void Ship::broadcast_current_state() {
    Model::get_instance().notify_location(get_name(), track_base.get_position());
    Model::get_instance().notify_speed(get_name(), track_base.get_speed());
    Model::get_instance().notify_fuel(get_name(), fuel);
    Model::get_instance().notify_course(get_name(), track_base.get_course());
}

/*** Command functions ***/

void Ship::set_destination_position_and_speed(Point destination_position, double speed){
    check_and_set_speed(speed);
    destination = destination_position;
    ship_state = State_e::MOVING_TO_POSITION;
    double heading = Compass_vector(get_location(), destination).direction;
    set_course(heading);
    cout << get_name() << " will sail on " << track_base.get_course_speed() << " to " << destination << endl;
}


void Ship::set_course_and_speed(double course, double speed){
    check_and_set_speed(speed);
    set_course(course);
    ship_state = State_e::MOVING_ON_COURSE;
    cout << get_name() << " will sail on " << track_base.get_course_speed() << endl;
}


void Ship::stop(){
    if (!can_move())
        throw Error("Ship cannot move!");
    set_speed(0.);
    ship_state = State_e::STOPPED;
    cout << get_name() << " stopping at " << track_base.get_position() << endl;
}


void Ship::dock(shared_ptr<Island> island_ptr){
    if (!can_dock(island_ptr))
        throw Error("Can't dock!");
    set_location(island_ptr->get_location());
    docked_island_ptr = island_ptr;
    ship_state = State_e::DOCKED;
    cout << get_name() << " docked at " << docked_island_ptr->get_name() << endl;
}


void Ship::refuel(){
    if (ship_state != State_e::DOCKED)
        throw Error("Must be docked!");
    double needed_fuel = fuel_capacity - fuel;
    if (needed_fuel < 0.005)
        fuel = fuel_capacity;
    else{
        fuel += get_docked_Island()->provide_fuel(needed_fuel);
        cout << get_name() << " now has " << fuel << " tons of fuel" << endl;
    }
    Model::get_instance().notify_fuel(get_name(), fuel);
}

/*** Fat interface command functions ***/


void Ship::set_load_destination(shared_ptr<Island>){
    throw Error("Cannot load at a destination!");
}

void Ship::set_unload_destination(shared_ptr<Island>){
    throw Error("Cannot unload at a destination!");
}

void Ship::attack(shared_ptr<Ship> in_target_ptr){
    throw Error("Cannot attack!");
}

void Ship::stop_attack(){
    throw Error("Cannot attack!");
}

// interactions with other objects

void Ship::receive_hit(int hit_force, shared_ptr<Ship> attacker_ptr){
    cout << "Ship::receive_hit()" << endl;
    resistance -= hit_force;
    cout << get_name() << " hit with " << hit_force << ", resistance now " << resistance << endl;
    if (resistance < 0.) { //If the ship resistance is negative, set it to sunk
        set_speed(0.);
        ship_state = State_e::SUNK;
        cout << get_name() << " sunk" << endl;
        //Remove ship from the views and model
        Model::get_instance().notify_gone(get_name());
        Model::get_instance().remove_ship(shared_from_this());
    }
}

// return pointer to the Island currently docked at, or nullptr if not docked
shared_ptr<Island> Ship::get_docked_Island() const{
    return docked_island_ptr;
}


double Ship::get_maximum_speed() const{
    return maximum_speed;
}

//MITCH: PROJECT 6 ASK ABOUT THIS!!!! THIS CHANGES SSSTUFFF COULD BE BADDD
double Ship::give_available_fuel(){
    double fuel_to_give = fuel;
    fuel = 0.;
    set_speed(0.);
    ship_state = State_e::DEAD_IN_THE_WATER;
    Model::get_instance().notify_fuel(get_name(), fuel);
    return fuel_to_give;
}


/* Private Function Definitions */


void Ship::set_speed(double speed){
    track_base.set_speed(speed);
    Model::get_instance().notify_speed(get_name(), track_base.get_speed());
}

void Ship::check_and_set_speed(double speed){
    if (!can_move())
        throw Error("Ship cannot move!");
    if (speed > get_maximum_speed())
        throw Error("Ship cannot go that fast!");
    if (is_docked())
        docked_island_ptr = nullptr;
    set_speed(speed);
}

void Ship::set_course(double course){
    track_base.set_course(course);
    Model::get_instance().notify_course(get_name(), course);
}

void Ship::set_location(Point location){
    track_base.set_position(location);
    Model::get_instance().notify_location(get_name(), track_base.get_position());
}


/*
 Calculate the new position of a ship based on how it is moving, its speed, and
 fuel state. This function should be called only if the state is
 MOVING_TO_POSITION or MOVING_ON_COURSE.
 
 Track_base has an update_position(double time) function that computes the new position
 of an object after the specified time has elapsed. If the Ship is going to move
 for a full time unit (one hour), then it will get go the "full step" distance,
 so update_position would be called with time = 1.0. If we can move less than that,
 e.g. due to not enough fuel, update position  will be called with the corresponding
 time less than 1.0.
 
 For clarity in specifying the computation, this code assumes the specified private variable names,
 but you may change the variable names or enum class names, or state names if you wish (e.g. movement_state).
 */
void Ship:: calculate_movement()
{
    // Compute values for how much we need to move, and how much we can, and how long we can,
    // given the fuel state, then decide what to do.
    double time = 1.0;	// "full step" time
    // get the distance to destination
    double destination_distance = cartesian_distance(get_location(), destination);
    // get full step distance we can move on this time step
    double full_distance = track_base.get_speed() * time;
    // get fuel required for full step distance
    double full_fuel_required = full_distance * fuel_consumption;	// tons = nm * tons/nm
    // how far and how long can we sail in this time period based on the fuel state?
    double distance_possible, time_possible;
    if(full_fuel_required <= fuel) {
        distance_possible = full_distance;
        time_possible = time;
    }
    else {
        distance_possible = fuel / fuel_consumption;	// nm = tons / tons/nm
        time_possible = (distance_possible / full_distance) * time;
    }
    
    // are we are moving to a destination, and is the destination within the distance possible?
    if(ship_state == State_e::MOVING_TO_POSITION && destination_distance <= distance_possible) {
        // yes, make our new position the destination
        track_base.set_position(destination);
        // we travel the destination distance, using that much fuel
        double fuel_required = destination_distance * fuel_consumption;
        fuel -= fuel_required;
        track_base.set_speed(0.);
        ship_state = State_e::STOPPED;
    }
    else {
        // go as far as we can, stay in the same movement state
        // simply move for the amount of time possible
        track_base.update_position(time_possible);
        // have we used up our fuel?
        if(full_fuel_required >= fuel) {
            fuel = 0.0;
            track_base.set_speed(0.);
            ship_state = State_e::DEAD_IN_THE_WATER;
        }
        else {
            fuel -= full_fuel_required;
        }
    }
}