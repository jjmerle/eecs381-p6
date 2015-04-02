/* Warship class
A Warship is a ship with firepower and range member variables, and some services for
protected classes to manage many of the details of warship behavior. This is an
abstract base class, so concrete classes derived from Warship must be declared.
*/
#ifndef WARSHIP_H
#define WARSHIP_H
#include "Ship.h"
#include <memory>

class Warship : public Ship {
public:
	// perform warship-specific behavior
	void update() override;

	// Warships will act on an attack and stop_attack command

	// will	throw Error("Cannot attack!") if not Afloat
	// will throw Error("Warship may not attack itself!")
    // if supplied target is the same as this Warship
    void attack(std::shared_ptr<Ship> target_ptr_) override;

	// will throw Error("Was not attacking!") if not Attacking
	void stop_attack() override;
	
	void describe() const override;

protected:
	// future projects may need additional protected members
    // initialize, then output constructor message
    // Protected Constructor to make Warship pseudo-abstract
    Warship(const std::string& name_, Point position_, double fuel_capacity_,
            double maximum_speed_, double fuel_consumption_, int resistance_,
            int firepower_, double maximum_range_);
    
	// return true if this Warship is in the attacking state
	bool is_attacking() const;
	
	// fire at the current target
	void fire_at_target();
		
	// is the current target in range?
	bool target_in_range() const;

	// get the target
    std::shared_ptr<Ship> get_target() const;
    
private:
    enum class Attack_State_e { ATTACKING, NOTATTACKING };
    
    int firepower;
    double maximum_range;
    Attack_State_e attack_state;
    std::weak_ptr<Ship> target;
};

#endif
