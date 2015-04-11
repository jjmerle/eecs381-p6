#include "Cruiser.h"
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

void Cruiser::receive_hit(int hit_force, shared_ptr<Ship> attacker_ptr) {
    Ship::receive_hit(hit_force, attacker_ptr);
    if(is_afloat() && !is_attacking()) { // TODO - put this in response?
        Warship::attack(attacker_ptr);
    }
    attacker_ptr->respond_to_attack(static_pointer_cast<Cruiser>(shared_from_this()));
}
