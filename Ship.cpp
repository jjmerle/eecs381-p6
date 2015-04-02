#include "Ship.h"
#include "Island.h"
#include "Model.h"
#include "Utility.h"
#include <memory>
#include <iostream>
#include <iomanip>
using std::cout;
using std::endl;
using std::shared_ptr;
using std::setprecision;

const char* const ship_move_error_c = "Ship cannot move!";
const char* const ship_speed_error_c = "Ship cannot go that fast!";
const char* const ship_attack_error_c = "Cannot attack!";

// initialize, then output constructor message
Ship::Ship(const std::string& name_, Point position_, double fuel_capacity_,
    double maximum_speed_, double fuel_consumption_, int resistance_) :
    Sim_object(name_), fuel(fuel_capacity_), fuel_consumption(fuel_consumption_),
    fuel_capacity(fuel_capacity_), maximum_speed(maximum_speed_),
    resistance(resistance_), ship_state(Ship_State_e::STOPPED), docked_island(nullptr),
    track_base(position_) { }

// Return true if ship can move (it is not dead in the water or in the process or sinking);
bool Ship::can_move() const {
    return is_afloat() && ship_state != Ship_State_e::DEAD_IN_THE_WATER;
}

// Return true if ship is moving;
bool Ship::is_moving() const {
    return  ship_state == Ship_State_e::MOVING_ON_COURSE ||
            ship_state == Ship_State_e::MOVING_TO_POSITION;
}

// Return true if ship is docked;
bool Ship::is_docked() const {
    return ship_state == Ship_State_e::DOCKED;
}

// Return true if ship is afloat (not in process of sinking), false if not
bool Ship::is_afloat() const {
    return ship_state != Ship_State_e::SUNK;
}

// Return true if the ship is Stopped and the distance to the supplied island
// is less than or equal to 0.1 nm
bool Ship::can_dock(shared_ptr<Island> island_ptr) const {
    return ship_state == Ship_State_e::STOPPED &&
            cartesian_distance(get_location(), island_ptr->get_location()) <= .1;
}


// Broadcast all state to Views
void Ship::broadcast_current_state() {
    Model::get_instance().notify_location(get_name(), get_location());
    Model::get_instance().notify_fuel(get_name(), fuel);
    Model::get_instance().notify_course_and_speed(get_name(), track_base.get_course(), track_base.get_speed());
}

// Broadcast current location to Views
void Ship::broadcast_current_location() {
    Model::get_instance().notify_location(get_name(), get_location());
}

// Broadcast current fuel to Views
void Ship::broadcast_current_fuel() {
    Model::get_instance().notify_fuel(get_name(), fuel);
}

// Broadcast current course and speed to Views
void Ship::broadcast_current_course_and_speed() {
    Model::get_instance().notify_course_and_speed(get_name(), track_base.get_course(), track_base.get_speed());
}

/*** Interface to derived classes ***/
// Update the state of the Ship
void Ship::update() {
    cout << get_name();
    if(is_afloat()) {
        switch(ship_state) {
            case Ship_State_e::MOVING_ON_COURSE: // Drop through
            case Ship_State_e::MOVING_TO_POSITION:
                calculate_movement();
                cout << " now at " << get_location();
                broadcast_current_state(); // all state must be updated
                break;
            case Ship_State_e::STOPPED:
                cout << " stopped at " << track_base.get_position();
                break;
            case Ship_State_e::DOCKED:
                cout << " docked at " << get_docked_Island()->get_name();
                break;
            case Ship_State_e::DEAD_IN_THE_WATER:
                cout << " dead in the water at " << track_base.get_position();
                break;
            default:
                cout << default_switch_error_c << endl;
                break;
        };
    } else {
        cout << " sunk";
    }
    cout << endl;
}

// output a description of current state to cout
void Ship::describe() const {
    cout << get_name() << " at " << track_base.get_position();
    if(!is_afloat()) {
        cout << " sunk";
    } else {
        cout << ", fuel: " << fuel << " tons, resistance: " << resistance << endl;
        switch(ship_state) {
            case Ship_State_e::MOVING_TO_POSITION:
                cout << "Moving to " << destination <<  " on " << track_base.get_course_speed();
                break;
            case Ship_State_e::MOVING_ON_COURSE:
                cout << "Moving on " << track_base.get_course_speed();
                break;
            case Ship_State_e::DOCKED:
                cout << "Docked at " << get_docked_Island()->get_name();
                break;
            case Ship_State_e::STOPPED:
                cout << "Stopped";
                break;
            case Ship_State_e::DEAD_IN_THE_WATER:
                cout << "Dead in the water";
                break;
            default:
                cout << default_switch_error_c << endl;
                break;
        };
    }
    cout << endl;
}

/*** Command functions ***/
// Start moving to a destination position at a speed
// may throw Error("Ship cannot move!")
// may throw Error("Ship cannot go that fast!")
void Ship::set_destination_position_and_speed(Point destination_position, double speed) {
    if(!can_move()) {
        throw Error(ship_move_error_c);
    }
    if(speed > maximum_speed) {
        throw Error(ship_speed_error_c);
    }
    // reset docked state
    if(is_docked()) {
        docked_island = nullptr;
    }
    destination = destination_position;
    Compass_vector compass_vec(get_location(), destination);
    
    track_base.set_speed(speed);
    track_base.set_course(compass_vec.direction);
    ship_state = Ship_State_e::MOVING_TO_POSITION;
    
    broadcast_current_course_and_speed();
    cout << get_name() << " will sail on " << track_base.get_course_speed() << " to " << destination << endl;
}

// Start moving on a course and speed
// may throw Error("Ship cannot move!")
// may throw Error("Ship cannot go that fast!");
void Ship::set_course_and_speed(double course, double speed) {
    if(!can_move()) {
        throw Error(ship_move_error_c);
    }
    if(speed > maximum_speed) {
        throw Error(ship_speed_error_c);
    }
    if(is_docked()) {
        docked_island = nullptr;
    }
    track_base.set_course(course);
    track_base.set_speed(speed);
    ship_state = Ship_State_e::MOVING_ON_COURSE;
    
    broadcast_current_course_and_speed();
    cout << get_name() << " will sail on " << track_base.get_course_speed() << endl;
}

// Stop moving
// may throw Error("Ship cannot move!")
void Ship::stop() {
    if(!can_move()) {
        throw Error(ship_move_error_c);
    }
    track_base.set_speed(0);
    ship_state = Ship_State_e::STOPPED;
    broadcast_current_course_and_speed();
    cout << get_name() << " stopping at " << get_location() << endl;
}

// dock at an Island - set our position = Island's position, go into Docked state
// may throw Error("Can't dock!");
void Ship::dock(shared_ptr<Island> island_ptr) {
    if(ship_state != Ship_State_e::STOPPED ||
       cartesian_distance(get_location(), island_ptr->get_location()) > 0.1) {
        throw Error("Can't dock!");
    }
    track_base.set_position(island_ptr->get_location());
    docked_island = island_ptr;
    
    broadcast_current_location();
    ship_state = Ship_State_e::DOCKED;
    cout << get_name() << " docked at " << island_ptr->get_name() << endl;
}

// Refuel - must already be docked at an island; fill takes as much as possible
// may throw Error("Must be docked!");
void Ship::refuel() {
    if(is_docked()) {
        double required_fuel = fuel_capacity - fuel;
        if( required_fuel < .005 ) {
            fuel = fuel_capacity;
        } else {
            shared_ptr<Island> island = get_docked_Island();
            fuel += island->provide_fuel(required_fuel);
            cout << get_name() << " now has " << fuel << " tons of fuel" << endl;
        }
        broadcast_current_fuel();
    } else {
        throw Error("Must be docked!");
    }
}

/*** Fat interface command functions ***/
// These functions throw an Error exception for this class
// will always throw Error("Cannot load at a destination!");
void Ship::set_load_destination(shared_ptr<Island>) {
    throw Error("Cannot load at a destination!");
}

// will always throw Error("Cannot unload at a destination!");
void Ship::set_unload_destination(shared_ptr<Island>) {
    throw Error("Cannot unload at a destination!");
}

// will always throw Error("Cannot attack!");
void Ship::attack(shared_ptr<Ship> in_target_ptr) {
    throw Error(ship_attack_error_c);
}

// will always throw Error("Cannot attack!");
void Ship::stop_attack() {
    throw Error(ship_attack_error_c);
}

// interactions with other objects
// receive a hit from an attacker
void Ship::receive_hit(int hit_force, shared_ptr<Ship> attacker_ptr) {
    resistance -= hit_force;
    cout << get_name() << " hit with " << hit_force << ", resistance now " << resistance << endl;
    if(resistance < 0) {
        ship_state = Ship_State_e::SUNK;
        track_base.set_speed(0.);
        cout << get_name() << " sunk" << endl;
        Model::get_instance().notify_gone(get_name());
        Model::get_instance().remove_ship(shared_from_this());
    }
}

double Ship::get_maximum_speed() const {
    return maximum_speed;
}

// return pointer to the Island currently docked at, or nullptr if not docked
shared_ptr<Island> Ship::get_docked_Island() const {
    return docked_island;
}

/* Private Function Definitions */

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
but you may change the variable names or state names if you wish (e.g. movement_state).
*/
void Ship::calculate_movement()
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
    if(ship_state == Ship_State_e::MOVING_TO_POSITION && destination_distance <= distance_possible) {
		// yes, make our new position the destination
		track_base.set_position(destination);
		// we travel the destination distance, using that much fuel
		double fuel_required = destination_distance * fuel_consumption;
		fuel -= fuel_required;
		track_base.set_speed(0.);
        ship_state = Ship_State_e::STOPPED;
		}
	else {
		// go as far as we can, stay in the same movement state
		// simply move for the amount of time possible
		track_base.update_position(time_possible);
		// have we used up our fuel?
		if(full_fuel_required >= fuel) {
			fuel = 0.0;
			track_base.set_speed(0.);
            ship_state = Ship_State_e::DEAD_IN_THE_WATER;
			}
		else {
			fuel -= full_fuel_required;
			}
		}
}

