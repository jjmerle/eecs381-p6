#ifndef SAILING_VIEW_H
#define SAILING_VIEW_H
#include "View.h"
#include <map>
#include <string>


class SailingView : public View {
public:
    // prints out the current map
    void draw() override;
    
    // Update ship fuel
    void update_fuel(const std::string& name, double fuel_) override;
    
    // Remove a Ship from the View
    void update_remove(const std::string& name) override;
    
    // Update ship speed
    void update_course_and_speed(const std::string& name, double course_, double speed_) override;
    
private:
    // Struct containing all data needed for SailingView
    struct SailingViewInfo {
        double fuel;
        double course;
        double speed;
    };
    
    std::map<std::string, SailingViewInfo> ship_sailing_data;
    
};

#endif
