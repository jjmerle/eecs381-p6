#include "Cruiser.h"
#include "Pirate_ship.h"
#include <iostream>
#include <memory>
using std::cout;
using std::endl;
using std::shared_ptr;
using std::static_pointer_cast;

// initialize, then output constructor message
Cruiser::Cruiser(const std::string& name_, Point position_) :
    Warship(name_, position_, 1000, 20, 10, 6, 3, 15) {}

void Cruiser::update() {
    Warship::update();
    if(is_afloat() && is_attacking()) {
        if(get_target() && target_in_range()) {
            fire_at_target();
        } else {
            cout << get_name() << " target is out of range" << endl;
            stop_attack();
        }
    }

}
void Cruiser::describe() const {
    cout << "\nCruiser ";
    Warship::describe();
}

//MITCH: ASK ABOUT MAKING FIRE AT TARGET VIRTUAL
void Cruiser::fire_at_target(){
    Warship::fire_at_target();
    //MITCH FIGURE OUT  since shared_from_this is inherited from ship level we must cast the pointer to pirate ships type
    //If the target is still afloat, respond
    shared_ptr<Ship> target_now = get_target();
    target_now->receive_hit(get_firepower(), static_pointer_cast<Cruiser>(shared_from_this()));
}

void Cruiser::receive_hit(int hit_force, shared_ptr<Cruiser> attacker_ptr) {
    Ship::receive_hit(hit_force, attacker_ptr);
    if(is_afloat() && !is_attacking()) { // TODO - put this in response?
        Warship::attack(attacker_ptr);
    }
}

void Cruiser::receive_hit(int hit_force, shared_ptr<Pirate_ship> attacker_ptr) {
    Ship::receive_hit(hit_force, attacker_ptr);
    if(is_afloat() && !is_attacking()) { // TODO - put this in response?
        Warship::attack(attacker_ptr);
    }
}