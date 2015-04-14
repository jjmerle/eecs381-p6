#include "Warship.h"
#include "Utility.h"
#include <iostream>
#include <memory>
using std::cout;
using std::endl;
using std::string;
using std::shared_ptr;
using std::weak_ptr;

// initialize, then output constructor message
Warship::Warship(const string& name_, Point position_, double fuel_capacity_,
    double maximum_speed_, double fuel_consumption_, int resistance_,
    int firepower_, double maximum_range_) :
    Ship(name_, position_, fuel_capacity_, maximum_speed_, fuel_consumption_, resistance_),
    firepower(firepower_), maximum_range(maximum_range_),
    attack_state(Attack_State_e::NOTATTACKING) { }

// perform warship-specific behavior
void Warship::update() {
    Ship::update();
    if(is_attacking()) {
        shared_ptr<Ship> target_now = target.lock();
        if(target_now) {
            if(!is_afloat() || !target_now->is_afloat()) {
                stop_attack();
            } else {
                cout << get_name() << " is attacking" << endl;
            }
        } else {
            stop_attack();
        }

    }
}

// Warships will act on an attack and stop_attack command

// will	throw Error("Cannot attack!") if not Afloat
// will throw Error("Warship may not attack itself!")
// if supplied target is the same as this Warship
void Warship::attack(shared_ptr<Ship> target_ptr_) {
    if(!is_afloat()) {
        throw Error("Cannot attack!");
    }
    if(target_ptr_ == shared_from_this()) {
        throw Error("Warship may not attack itself!");
    }
    shared_ptr<Ship> target_now = target.lock();
    if(target_ptr_ == target_now) {
        throw Error("Already attacking this target!");
    }
    target = target_ptr_;
    attack_state = Attack_State_e::ATTACKING;
    cout << get_name() << " will attack " << target_ptr_->get_name() << endl;
}

// will throw Error("Was not attacking!") if not Attacking
void Warship::stop_attack() {
    if(!is_attacking()) {
        throw Error("Was not attacking!");
    }
    cout << get_name() << " stopping attack" << endl;
    attack_state = Attack_State_e::NOTATTACKING;
    target.reset();
}

void Warship::describe() const {
    Ship::describe();
    if(is_attacking()) {
        shared_ptr<Ship> target_now = target.lock();
        if(!target_now || !target_now->is_afloat()) {
            cout << "Attacking absent ship";
        } else {
            cout << "Attacking " << target_now->get_name();
        }
        cout << endl;
    }
}

// return true if this Warship is in the attacking state
bool Warship::is_attacking() const {
    return attack_state == Attack_State_e::ATTACKING;
}

// fire at the current target
void Warship::fire_at_target() {
    cout << get_name() << " fires" << endl;
    shared_ptr<Ship> target_now = target.lock();
    target_now->receive_hit(firepower, shared_from_this());
}

// is the current target in range?
bool Warship::target_in_range() const {
    shared_ptr<Ship> target_now = target.lock();
    return cartesian_distance(get_location(), target_now->get_location()) <= maximum_range;
}

// get the target
shared_ptr<Ship> Warship::get_target() const {
    return target.lock();
}