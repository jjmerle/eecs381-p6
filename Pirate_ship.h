
#ifndef PIRATE_SHIP
#define PIRATE_SHIP

#include "Warship.h"
#include <iosfwd>
#include <memory>


class Point;
class Island;

class Pirate_ship : public Warship {
public:
    
    Pirate_ship(const std::string& name_, Point position_);
    
    void update() override;
    
    void describe() const override;
    
    //Returns the amount of all fuel and cargo combines
    //Sets the fuel and cargo to 0
    //sets the speed of the ship to 0 and the state to dead in the water
    //Notifies views of the speed
    double give_available_fuel() override;
    
    //Returns the amount of treasure and sets the treasure to 0
    double give_treasure() override;
    
    //Will fire at the current target and tells the target to respond if it is still afloat
    void fire_at_target() override;
    
    //clears the destination island and sets the dock state to refuel
    void dock(std::shared_ptr<Island> island_ptr) override;
    
    void attack(std::shared_ptr<Ship> target_ptr) override;
    void set_destination_position_and_speed(Point destination_position, double speed) override;
    void set_course_and_speed(double course, double speed) override;
    void stop() override;
    
private:
    enum class Dock_state_e {REFUEL, SELL_CARGO, NO_ACTION};
    //Keeps track of the current action when docked
    Dock_state_e dock_state;
    
    //exchanges the cargo for treasure in a 1:1 ratio (supposed to show the act of selling it)
    void sell_cargo();
    double cargo;
    double cargo_capacity;
    double treasure;
    //the destination where 
    std::shared_ptr<Island> destination_island_ptr;
};



#endif
