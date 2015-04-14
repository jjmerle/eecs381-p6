

#include "Pirate_ship.h"
#include "Navigation.h"
#include "Sim_object.h"
#include "Utility.h"
#include "Island.h"
#include "Model.h"

//MITCH; get rid of!
#include "Cruiser.h"

#include <memory>
#include <algorithm>
#include <vector>
#include <map>
#include <iostream>
#include <functional>
#include <cassert>
#include <memory>
#include <iostream>

using std::shared_ptr;
using std::static_pointer_cast;
using std::dynamic_pointer_cast;
using std::vector;
using std::map;
using std::min;
using std::max;
using std::cout;
using std::endl;
using namespace std::placeholders;

//Once boarded and items stolen, the pirates will plant dynamite on the ship to sink it
const int dynamite_damage_c = 30;

Pirate_ship::Pirate_ship(const std::string& name_, Point position_):
Warship(name_, position_, 100, 6, 1, 30, 1, 10), dock_state(Dock_state_e::NO_ACTION), cargo(0.), cargo_capacity(20.), treasure(0.) {
}


//Maybe instead of money, just keep the cargo on hand.  If the ship is ever dead in the water, refuel it
void Pirate_ship::update(){
    //MITCH: change to state or strategy pattern based on whether cargo is full!!!!! ASK IF IT IS ACTUALLY USEFUL.
    //COULD HAVE STATES SCAVENGING, MOVING TO ISLAND
    //MITCH I CAN TOTALLY MAKE AN ENUM FOR THIS. maybe i feel like it may make room for errors
    Warship::update();
    if (!can_move()) {
        destination_island_ptr.reset();
    } else if (is_docked()) {
        switch (dock_state) {
            case Dock_state_e::REFUEL:
                cout << get_name() << " refueling" << endl;
                refuel();
                dock_state = Dock_state_e::SELL_CARGO;
                break;
            case Dock_state_e::SELL_CARGO:
                cout << get_name() << " selling cargo" << endl;
                sell_cargo();
                dock_state = Dock_state_e::NO_ACTION;
                break;
            case Dock_state_e::NO_ACTION:  //Will do nothing until given an action
                break;
            default:
                assert(0); //MITCH: DO I NEED?
                break;
        }
    } else if (get_target()) { //if attacking, and target still exists
        //If close enough, ship will be plundered and fuel (and cargo if any) will be stolen
        if (cartesian_distance(get_location(), get_target()->get_location()) < .1) {
            //will take the fuel and cargo if any
            cout << get_name() << " has plundered " << get_target()->get_name() << endl;
            cargo += get_target()->give_available_fuel(); //MITCH: PASS IN CARGO CAPACITY
            if (cargo > cargo_capacity) {
                cargo = cargo_capacity;
            }
            treasure += get_target()->give_treasure();
            stop_attack();
//            cout << get_target()->get_name() << " has been packed with explosives" << endl;
//            //the other ship will not react in the double dispatch form to this hit
//            get_target()->receive_hit(dynamite_damage_c, shared_from_this());
//            //Once the ship has been plundered, plants dynamite to sink it. If resistance is high enough the ship may still survive
//            ///Might need to add dynamite cause the pirate ship response may make it never go to island
        } else {
            if (target_in_range()){
                //target may be destroyed after this
                fire_at_target();
            }
            if (get_target()) {
                Ship::set_destination_position_and_speed(get_target()->get_location(), get_maximum_speed());
            }
        }
        //if the pirate ship is not returning to an island
    } else if (!destination_island_ptr){
        //if the ship has cargo to unload and can move
        if (cargo) {
            destination_island_ptr = get_closest_island(get_location());
            if (destination_island_ptr) { //safety if there are no islands
                //Now heading to island to unload
                Ship::set_destination_position_and_speed(destination_island_ptr->get_location(), get_maximum_speed());
                cout << get_name() << " is heading to " << destination_island_ptr->get_name() << endl;
            }
        }
    } else if (can_dock(destination_island_ptr)) {
        //Will dock & sell the cargo on the island. Next update will set a new course to next closest ship
        dock(destination_island_ptr);
    }
}

void Pirate_ship::describe() const {
    cout << "\nPirate_ship ";
    Warship::describe();
    if (destination_island_ptr)
        cout << "Heading to " << destination_island_ptr->get_name() << " to sell cargo" << endl;
    cout << "Booty collected: $" << treasure << endl;
    cout << "Cargo on board: " << cargo << endl;
}

void Pirate_ship::dock(shared_ptr<Island> island_ptr){
    Ship::dock(island_ptr);
    //Successfully returned, so we no longer have a destination island
    destination_island_ptr.reset();
    dock_state = Dock_state_e::REFUEL;
}

////If any of these acrions occur returning to an island is no longer priority, so resets the destination island
void Pirate_ship::attack(shared_ptr<Ship> target_ptr){
    Warship::attack(target_ptr);
    destination_island_ptr.reset();
}
void Pirate_ship::set_destination_position_and_speed(Point destination_position, double speed){
    Ship::set_destination_position_and_speed(destination_position, speed);
    destination_island_ptr.reset();
}
void Pirate_ship::set_course_and_speed(double course, double speed){
    Ship::set_course_and_speed(course, speed);
    destination_island_ptr.reset();
}
void Pirate_ship::stop() {
    Ship::stop();
    destination_island_ptr.reset();
}
////


double Pirate_ship::give_available_fuel(){
    double fuel_to_give = Ship::give_available_fuel() + cargo;
    cargo = 0.;
    return fuel_to_give;
}

double Pirate_ship::give_treasure(){
    double treasure_to_give = treasure;
    treasure = 0.;
    return treasure_to_give;
}

//MITCH: ASK ABOUT MAKING FIRE AT TARGET VIRTUAL
void Pirate_ship::fire_at_target(){
    Warship::fire_at_target();
    //MITCH FIGURE OUT  since shared_from_this is inherited from ship level we must cast the pointer to pirate ships type
    //If the target is still afloat, respond
    if (get_target()){
        cout << get_target()->get_name() << " responding to " << get_name() << "'s attack" << endl;
        get_target()->handle_attacker(static_pointer_cast<Pirate_ship>(shared_from_this()));
    }
}


//Move straight towards ship to try to steal its treasure and cargo
//MITCH: IT ALREADY DOES THAT! THINK OF WHAT ELSE I CAN DO
//MITCH: MAKE THE ATTACKERS CONST?
void Pirate_ship::handle_attacker(shared_ptr<Pirate_ship> attacker_ptr){
    if (!is_attacking()) {
        attack(attacker_ptr);
        cout << get_name() << " now attacking " << attacker_ptr->get_name() << endl;
    }
}

void Pirate_ship::handle_attacker(shared_ptr<Cruiser> attacker_ptr){
    //If its not heading to an island, not attacking, and can move, go to closest island so cargo can be sold and pirates can escape
    //MITCH    get_target() != attacker_ptr &&
    if (!is_attacking() && can_move()) {
        if (!destination_island_ptr) { //Mitch: I should check for null island??? WILL NEVER BE THE CASE THOUGH. ASK IF I SHOULD TEST FOR
            destination_island_ptr = get_closest_island(get_location());
            set_destination_position_and_speed(destination_island_ptr->get_location(), get_maximum_speed());
        }
        cout << get_name() << " running away from " << attacker_ptr->get_name() << endl;
    }
}

//Private

void Pirate_ship::sell_cargo(){
    cout << get_name();
    if (cargo) {
        //exchanges cargo for treasure at a 1:1 ratio
        treasure += cargo;
        cargo = 0.;
        cout << " now has $" << treasure;
    } else {
        cout << "has no cargo to sell!";
    }
    cout << endl;
}


