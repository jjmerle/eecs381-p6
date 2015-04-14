#include "Model.h"
#include "Island.h"
#include "Ship.h"
#include "Utility.h"
#include "Ship_factory.h"
#include "View.h"

#include <iostream>
#include <string>
#include <algorithm>
#define NDEBUG
#include <cassert>

using std::string;
using std::cout;
using std::endl;
using std::set;
using std::bind;
using namespace std::placeholders;
using std::shared_ptr;
using std::make_shared;
using std::vector;
using std::sort;
using std::mem_fn;
using std::inserter;
using std::lower_bound;
using std::map;


using Imap_t = map<string, shared_ptr<Island>>;
using Smap_t = map<string, shared_ptr<Ship>>;

//MITCH: double check. pretty sure
Model * Model::singleton_ptr = nullptr;

Model::Model():time(0){
    islands["Exxon"] = make_shared<Island>("Exxon", Point(10, 10), 1000, 200);
    islands["Shell"] = make_shared<Island>("Shell", Point(0, 30), 1000, 200);
    islands["Bermuda"] = make_shared<Island>("Bermuda", Point(20, 20));
    islands["Treasure_Island"] = make_shared<Island>("Treasure_Island", Point(50, 5), 100, 5);
    ships["Ajax"] = create_ship("Ajax", "Cruiser", Point (15, 15));
    ships["Xerxes"] = create_ship("Xerxes", "Cruiser", Point (25, 25));
    ships["Valdez"] = create_ship("Valdez", "Tanker", Point (30, 30));
    transform(islands.begin(), islands.end(), inserter(sim_objects, sim_objects.begin()), bind(&Imap_t::value_type::second, _1));
    transform(ships.begin(), ships.end(), inserter(sim_objects, sim_objects.begin()), bind(&Smap_t::value_type::second, _1));
}

Model & Model::get_instance() {
    if (!singleton_ptr)
        singleton_ptr = new Model;
    return *singleton_ptr;
}


bool Model::is_name_in_use(const string& name) const{
    return any_of(sim_objects.begin(), sim_objects.end(), [&name](shared_ptr<const Sim_object> sim_object_ptr){
        return get_code(sim_object_ptr->get_name()) == get_code(name);
    });
}


bool Model::is_island_present(const string& name) const{
    return islands.find(name) != islands.end();
}


shared_ptr<Island> Model::get_island_ptr(const string& name) const{
    if (!is_island_present(name))
        throw Error("Island not found!");
    return islands.find(name)->second;
}

bool Model::is_ship_present(const string& name) const {
    return ships.find(name) != ships.end();
}


void Model::add_ship(shared_ptr<Ship> ship_ptr){
    sim_objects.insert(ship_ptr);
    //adds the ship to both containers using the first two characters of the name as keys
    ships[ship_ptr->get_name()] = ship_ptr;
    //notify all views of the new ship
    ship_ptr->broadcast_current_state();
}


shared_ptr<Ship> Model::get_ship_ptr(const string& name) const{
    if (!is_ship_present(name))
        throw Error("Ship not found!");
    return ships.find(name)->second;
}

// tell all objects to describe themselves
void Model::describe() const {
    for_each(sim_objects.begin(), sim_objects.end(), mem_fn(&Sim_object::describe));
}

void Model::update(){
    ++time;
    for_each(sim_objects.begin(), sim_objects.end(), mem_fn(&Sim_object::update));
}


/* View services */


void Model::attach(shared_ptr<View> view_ptr_){
    assert(view_ptr_);
    view_ptrs.push_back(view_ptr_);
    //This is redundant but simpler design.  Told to do in project 4 spec
    for_each(sim_objects.begin(), sim_objects.end(), mem_fn(&Sim_object::broadcast_current_state));
}

// Detach the View by discarding the supplied pointer from the container of Views
// - no updates sent to it thereafter.
void Model::detach(shared_ptr<View> view_ptr_){
    auto view_it = find(view_ptrs.begin(), view_ptrs.end(), view_ptr_);
    if (view_it != view_ptrs.end())
        view_ptrs.erase(view_it);
}

void Model::notify_speed(const std::string& name, double speed){
    for_each(view_ptrs.begin(), view_ptrs.end(), bind(&View::update_speed, _1, cref(name), speed));
}

void Model::notify_fuel(const std::string& name, double fuel){
    for_each(view_ptrs.begin(), view_ptrs.end(), bind(&View::update_fuel, _1, cref(name), fuel));
}

void Model::notify_course(const std::string& name, double course){
    for_each(view_ptrs.begin(), view_ptrs.end(), bind(&View::update_course, _1, cref(name), course));
}


// notify the views about an object's location
void Model::notify_location(const string& name, Point location){
    for_each(view_ptrs.begin(), view_ptrs.end(), bind(&View::update_location, _1, cref(name), location));
}

// notify the views that an object is now gone
void Model::notify_gone(const string& name){
    for_each(view_ptrs.begin(), view_ptrs.end(), bind(&View::update_remove, _1, cref(name)));
}

// remove the Ship from the containers.
void Model::remove_ship(shared_ptr<Ship> ship_ptr){
    ships.erase(ship_ptr.get()->get_name());
    sim_objects.erase(ship_ptr);
}



void Model::draw(){
    for_each(view_ptrs.begin(), view_ptrs.end(), mem_fn(&View::draw));
}

//MITCH: SHOULD THIS BE IN CRUISE SHIP? IDK
shared_ptr<Island> Model::get_island_at(Point point){
    auto islands_it = find_if(islands.begin(), islands.end(), [point] (const Imap_t::value_type & island_name_ptr_pair){
        return island_name_ptr_pair.second->get_location() == point;
    });
    if (islands_it != islands.end())
        return islands_it->second;
    return nullptr;
}


const Imap_t & Model::get_Islands() const {
    return islands;
}

const Smap_t & Model::get_Ships() const {
    return ships;
}