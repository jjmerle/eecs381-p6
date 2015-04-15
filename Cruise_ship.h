#ifndef CRUISE_SHIP_H
#define CRUISE_SHIP_H

#include "Ship.h"

#include <memory>
#include <vector>

class Island;

class Cruise_ship: public Ship {
public:
    
    Cruise_ship(const std::string& name_, Point position_);
    
    void update() override;
    
    
    void set_destination_position_and_speed(Point destination_position, double speed) override;
    
    
    void set_course_and_speed(double course, double speed) override;
    
    void dock(std::shared_ptr<Island>) override;
    
    void describe() const override;
    
    void stop() override;
    
private:
    enum class Dock_action_e {REFUEL, SIGHTSEEING, SET_NEW_COURSE, NO_STATE};
    Dock_action_e dock_action;
    std::vector<std::shared_ptr<const Island>> visited_islands;
    std::shared_ptr<Island> start_island_ptr;
    std::shared_ptr<Island> next_island_ptr;
    double cruise_speed;
    
    //prints a message and resets the cruise state
    void cancel_cruise();
    
    //resets all variables involving the cruise
    void reset_cruise_state();
    
    //checks if currently is in cruise
    bool is_in_cruise() const;
    
    
};

#endif