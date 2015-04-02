#ifndef CRUISE_SHIP_H
#define CRUISE_SHIP_H
/* It travels at the speed specified in the initial command. When it arrives at 
    an island, it docks, and then it stays for a few updates. On the first one, 
    it attempts to refuel from the island. On the next update it does nothing while 
    the passengers see the local sights. On the third update, it sets course for 
    its next destination (the closest unvisited island; in case of a tie, the first 
    in alphabetical order). When it has visited the last island, it returns to the 
    first island, the one it was originally sent to. When it arrives there, it 
    docks, does not try to refuel, and announces that the cruise is over.
 
    A Cruise_ship has fuel capacity and initial amount 500 tons, maximum speed 15., 
    fuel consumption 2 tons/nm, and resistance 0
 */
#include "Ship.h"
#include <vector>
#include <string>

class Cruise_ship : public Ship {
public:
    // Construct with name and Position
    Cruise_ship(const std::string& name_, Point position_);
    
    // Update Cruise_ship state
    void update() override;
    
    // Describe Cruise_ship state
    void describe() const override;
    
    /*** Command functions ***/
    // Start moving to a destination position at a speed
    void set_destination_position_and_speed(Point destination_position, double speed) override;
    // Start moving on a course and speed
    void set_course_and_speed(double course, double speed) override;
    // Stop moving
    void stop() override;
    
    
    
private:
    // Enums for current Cruise state
    enum class Cruise_State_e {
        NOT_CRUISING, CRUISING_TO_DESTINATION, REFUELING,
        DOCKED_SIGHTSEEING, LEAVING_ISLAND
    };
    
    int cruise_speed;
    Cruise_State_e cruise_state;
    std::shared_ptr<Island> first_destination;
    std::shared_ptr<Island> cruise_destination;
    std::vector<std::shared_ptr<Island>> islands;

    // Class helper functions
    void cancel_cruise();
};



#endif
