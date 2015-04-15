#include "SailingView.h"
#include "Utility.h"
#include <iostream>
#include <iomanip>
#include <string>
using std::cout;
using std::endl;
using std::pair;
using std::setw;
using std::string;

const int sailng_data_set_width_c = 10;

void SailingView::draw()  {
    cout << "----- Sailing Data -----" << endl;
    cout << setw(sailng_data_set_width_c) << "Ship" << setw(sailng_data_set_width_c)
    << "Fuel" << setw(sailng_data_set_width_c) << "Course"
    << setw(sailng_data_set_width_c) << "Speed" << endl;
    for_each(ship_sailing_data.begin(), ship_sailing_data.end(),
             [](const pair<string, SailingViewInfo>& name_sail_data_pair) {
                 cout << setw(sailng_data_set_width_c)
                 << name_sail_data_pair.first <<         setw(sailng_data_set_width_c)
                 << name_sail_data_pair.second.fuel <<   setw(sailng_data_set_width_c)
                 << name_sail_data_pair.second.course << setw(sailng_data_set_width_c)
                 << name_sail_data_pair.second.speed <<  endl;
             });
}

// Update ship fuel
void SailingView::update_fuel(const string& name, double fuel_) {
    ship_sailing_data[name].fuel = fuel_;
}

// Update ship course
void SailingView::update_course(const string& name, double course_) {
    SailingViewInfo& sailing_data = ship_sailing_data[name];
    sailing_data.course = course_;
}

void SailingView::update_speed(const string& name, double speed_) {
    SailingViewInfo& sailing_data = ship_sailing_data[name];
    sailing_data.speed = speed_;
}

// Update ship afloat state
void SailingView::update_remove(const string& name) {
    ship_sailing_data.erase(name);
}

