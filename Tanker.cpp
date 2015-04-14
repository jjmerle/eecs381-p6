#include "Tanker.h"
#include "Island.h"
#include "Utility.h"
#include <memory>
#include <string>
#include <iostream>
using std::cout;
using std::endl;
using std::shared_ptr;
using std::static_pointer_cast;
using std::string;

const char* const tanker_no_cargo_destinations_c = " now has no cargo destinations";

// initialize, the output constructor message
Tanker::Tanker(const string& name_, Point position_) :
    Ship(name_, position_, 100, 10., 2., 0), cargo(0), cargo_capacity(1000),
    cargo_state(Cargo_State_e::NO_CARGO_DESTINATIONS), load_destination(nullptr),
    unload_destination(nullptr) { }

// This class overrides these Ship functions so that it can check if this Tanker has assigned cargo destinations.
// if so, throw an Error("Tanker has cargo destinations!"); otherwise, simply call the Ship functions.
void Tanker::set_destination_position_and_speed(Point destination, double speed) {
    if(cargo_state != Cargo_State_e::NO_CARGO_DESTINATIONS) {
        throw Error("Tanker has cargo destinations!");
    }
    Ship::set_destination_position_and_speed(destination, speed);
}

void Tanker::set_course_and_speed(double course, double speed) {
    if(cargo_state != Cargo_State_e::NO_CARGO_DESTINATIONS) {
        throw Error("Tanker has cargo destinations!");
    }
    Ship::set_course_and_speed(course, speed);
}

// Set the loading and unloading Island destinations
// if both cargo destination are already set, throw Error("Tanker has cargo destinations!").
// if they are the same, leave at the set values, and throw Error("Load and unload cargo destinations are the same!")
// if both destinations are now set, start the cargo cycle
void Tanker::update_loading() {
    if(load_destination && unload_destination) {
        if(is_docked()) {
            if(cartesian_distance(get_location(), load_destination->get_location()) <= .1) {
                cargo_state = Cargo_State_e::LOADING;
                return;
            } else if(cartesian_distance(get_location(), unload_destination->get_location()) <= .1) {
                cargo_state = Cargo_State_e::UNLOADING;
                return;
            }
        }
        if(!is_moving()) {
            if(!cargo && can_dock(load_destination)) {
                dock(load_destination);
                cargo_state = Cargo_State_e::LOADING;
                return;
            } else if(cargo > 0 && can_dock(unload_destination)) {
                dock(unload_destination);
                cargo_state = Cargo_State_e::UNLOADING;
                return;
            }
        }
        if(!cargo) {
            Ship::set_destination_position_and_speed(load_destination->get_location(), get_maximum_speed());
            cargo_state = Cargo_State_e::MOVING_TO_LOADING;
            return;
        } else if(cargo > 0){
            Ship::set_destination_position_and_speed(unload_destination->get_location(), get_maximum_speed());
            cargo_state = Cargo_State_e::MOVING_TO_UNLOADING;
            return;
        }
    }

}

void Tanker::set_load_destination(shared_ptr<Island> load_destination_) {
    if(cargo_state != Cargo_State_e::NO_CARGO_DESTINATIONS) {
        throw Error("Tanker has cargo destinations!");
    }
    load_destination = load_destination_;
    if(load_destination == unload_destination) {
        throw Error("Load and unload cargo destinations are the same!");
    }
    cout << get_name() << " will load at " << load_destination->get_name() << endl;
    update_loading();
}

void Tanker::set_unload_destination(shared_ptr<Island> unload_destination_) {
    if(cargo_state != Cargo_State_e::NO_CARGO_DESTINATIONS) {
        throw Error("Tanker has cargo destinations!");
    }
    unload_destination = unload_destination_;
    if(load_destination == unload_destination) {
        throw Error("Load and unload cargo destinations are the same!");
    }
    cout << get_name() << " will unload at " << unload_destination->get_name() << endl;
    update_loading();
}

// when told to stop, clear the cargo destinations and stop
void Tanker::stop() {
    Ship::stop();
    unload_destination = load_destination = nullptr;
    cargo_state = Cargo_State_e::NO_CARGO_DESTINATIONS;
    cout << get_name() << tanker_no_cargo_destinations_c << endl;
}

void Tanker::update() {
    Ship::update();
    if(!can_move()) {
        cargo_state = Cargo_State_e::NO_CARGO_DESTINATIONS;
        unload_destination = load_destination = nullptr;
        cout << get_name() << tanker_no_cargo_destinations_c << endl;
    }
    else if(cargo_state == Cargo_State_e::NO_CARGO_DESTINATIONS) { return; }
    else if(cargo_state == Cargo_State_e::MOVING_TO_LOADING && !is_moving() &&
       can_dock(load_destination)) {
        dock(load_destination);
        cargo_state = Cargo_State_e::LOADING;
    }
    else if(cargo_state == Cargo_State_e::MOVING_TO_UNLOADING && !is_moving() &&
       can_dock(unload_destination)) {
        dock(unload_destination);
        cargo_state = Cargo_State_e::UNLOADING;
    }
    else if(cargo_state == Cargo_State_e::LOADING) {
        Ship::refuel();
        double cargo_needed = cargo_capacity - cargo;
        if(cargo_needed < .005) {
            cargo = cargo_capacity;
            Ship::set_destination_position_and_speed(unload_destination->get_location(), get_maximum_speed());
            cargo_state = Cargo_State_e::MOVING_TO_UNLOADING;
        } else {
            cargo += load_destination->provide_fuel(cargo_needed);
            cout << get_name() << " now has " << cargo << " of cargo" << endl;
        }
    }
    else if(cargo_state == Cargo_State_e::UNLOADING) {
        if(cargo == 0.0) {
            Ship::set_destination_position_and_speed(load_destination->get_location(), get_maximum_speed());
            cargo_state = Cargo_State_e::MOVING_TO_LOADING;
        } else {
            unload_destination->accept_fuel(cargo);
            cargo = 0.0;
        }
    }
    return;
}

void Tanker::describe() const {
    cout << "\nTanker ";
    Ship::describe();
    cout << "Cargo: " << cargo << " tons";
    switch(cargo_state) {
        case Cargo_State_e::NO_CARGO_DESTINATIONS:
            cout << ", no cargo destinations";
            break;
        case Cargo_State_e::LOADING:
            cout << ", loading";
            break;
        case Cargo_State_e::UNLOADING:
            cout << ", unloading";
            break;
        case Cargo_State_e::MOVING_TO_LOADING:
            cout << ", moving to loading destination";
            break;
        case Cargo_State_e::MOVING_TO_UNLOADING:
            cout << ", moving to unloading destination";
            break;
        default:
            cout << default_switch_error_c << endl;
            break;
    };
    cout << endl;
}

void Tanker::receive_hit(int hit_force, std::shared_ptr<Cruiser> attacker_ptr) {
    Ship::receive_hit(hit_force, static_pointer_cast<Ship>(shared_from_this()));
    cout << "Tanker responding to Cruiser now" << endl;
}
void Tanker::receive_hit(int hit_force, std::shared_ptr<Pirate_ship> attacker_ptr) {
    Ship::receive_hit(hit_force, static_pointer_cast<Ship>(shared_from_this()));
    cout << "Tanker responding to Pirate Ship now" << endl;
}