#include "Model.h"
#include "Sim_object.h"
#include "Island.h"
#include "Ship.h"
#include "View.h"
#include "Ship_factory.h"
#include "Utility.h"
#include <algorithm>
#include <iostream>
#include <functional>
#include <set>
#include <memory>
#include <vector>
using std::any_of;
using std::cout;
using std::endl;
using std::for_each;
using std::pair;
using std::make_shared;
using std::mem_fn;
using std::vector;
using std::set;
using std::shared_ptr;
using std::string;
using namespace std::placeholders;

using island_pair = pair<string, shared_ptr<Island>>;
using ship_pair = pair<string, shared_ptr<Ship>>;

Model& Model::get_instance() {
    static Model m;
    return m;
}

// Sim_object Name Comparator
bool Model::Name_Comparator::operator()(shared_ptr<Sim_object> s1, shared_ptr<Sim_object> s2) {
    return s1->get_name() < s2->get_name();
}

void Model::create_and_insert_island(const std::string& name_, Point position_,
                          double fuel_, double production_rate_) {
    shared_ptr<Island> island_ptr = make_shared<Island>(name_, position_, fuel_, production_rate_);
    islands.insert(island_pair(name_, island_ptr));
    all_objects.insert(island_ptr);
}

void Model::create_and_insert_ship(const std::string& name, const std::string& type,
                                   Point initial_position) {
    shared_ptr<Ship> ship_ptr = create_ship(name, type, initial_position);
    ships.insert(ship_pair(name, ship_ptr));
    all_objects.insert(ship_ptr);
}

// create the initial objects, output constructor message
Model::Model() : time(0) {
    create_and_insert_island("Exxon", Point(10, 10), 1000, 200);
    create_and_insert_island("Shell", Point(0, 30), 1000, 200);
    create_and_insert_island("Bermuda", Point(20, 20));
    create_and_insert_island("Treasure_Island", Point(50, 5), 100, 5);
	
    create_and_insert_ship("Ajax", "Cruiser", Point (15, 15));
    create_and_insert_ship("Xerxes", "Cruiser", Point (25, 25));
    create_and_insert_ship("Valdez", "Tanker", Point (30, 30));

	cout << "Model constructed" << endl;
}

// destroy all objects, output destructor message
Model::~Model() {
    cout << "Model destructed" << endl;
}

// is name already in use for either ship or island?
// either the identical name, or identical in first two characters counts as in-use
bool Model::is_name_in_use(const std::string& name) const {
    return any_of(all_objects.begin(), all_objects.end(),
                  [&name](shared_ptr<Sim_object> so) {
                      string obj_name = so->get_name();
                      return (name[0] == obj_name[0] && name[1] == obj_name[1]);
                  });
}

// is there such an island?
bool Model::is_island_present(const std::string& name) const {
    return islands.find(name) != islands.end();
}

// will throw Error("Island not found!") if no island of that name
shared_ptr<Island> Model::get_island_ptr(const std::string& name) const {
    auto ip = islands.find(name);
    if(ip == islands.end()) {
        throw Error("Island not found!");
    } else {
        return ip->second;
    }
}

// is there such an ship?
bool Model::is_ship_present(const std::string& name) const {
    return ships.find(name) != ships.end();
}

// add a new ship to the list, and update the view
void Model::add_ship(shared_ptr<Ship> ship) {
    all_objects.insert(ship);
    ships.insert(ship_pair(ship->get_name(), ship));
    ship->broadcast_current_state();
}

// will throw Error("Ship not found!") if no ship of that name
shared_ptr<Ship> Model::get_ship_ptr(const std::string& name) const {
    auto ip = ships.find(name);
    if(ip == ships.end()) {
        throw Error("Ship not found!");
    } else {
        return ip->second;
    }
}

// tell all objects to describe themselves
void Model::describe() const {
    for_each(all_objects.begin(), all_objects.end(), mem_fn(&Sim_object::describe));
}

// increment the time, and tell all objects to update themselves
void Model::update() {
    ++time;
    for_each(all_objects.begin(), all_objects.end(), mem_fn(&Sim_object::update));
}

/* View services */
// Attaching a View adds it to the container and causes it to be updated
// with all current objects'location (or other state information.
void Model::attach(View* view) {
    view_list.push_back(view);
    for_each(all_objects.begin(), all_objects.end(), mem_fn(&Sim_object::broadcast_current_state));
}

// Detach the View by discarding the supplied pointer from the container of Views
// - no updates sent to it thereafter.
void Model::detach(View* view) {
    view_list.remove(view);
}

// notify the views about an object's location
void Model::notify_location(const std::string& name, Point location) {
    for_each(view_list.begin(), view_list.end(),
             [&name, &location](View* vp) { vp->update(name, location); });
    
}

// notify the views that an object is now gone
void Model::notify_gone(const std::string& name) {
    for_each(view_list.begin(), view_list.end(),
            [&name](View* vp) { vp->update_remove(name); });
}

// remove the Ship from the containers.
void Model::remove_ship(shared_ptr<Ship> ship_ptr) {
    all_objects.erase(ship_ptr);
    ships.erase(ship_ptr->get_name());
}

double Model::get_fuel_from_ship(shared_ptr<Ship> ship_ptr) { return ship_ptr->get_fuel(); }
double Model::get_course_from_ship(shared_ptr<Ship> ship_ptr) { return ship_ptr->get_course(); }
double Model::get_speed_from_ship(shared_ptr<Ship> ship_ptr) { return ship_ptr->get_speed(); }

// Return a set of Island location Points
vector<shared_ptr<Island>> Model::get_islands() {
    vector<shared_ptr<Island>> island_locations;
    for(island_pair island_pr : islands) {
        island_locations.push_back(island_pr.second);
    }
    return island_locations;
}

shared_ptr<Island> Model::get_island_from_location(Point location) {
    for(island_pair island_pr : islands) {
        if(island_pr.second->get_location() == location) {
            return island_pr.second;
        }
    }
    return nullptr;
}