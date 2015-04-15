#include "Controller.h"
#include "Model.h"
#include "Utility.h"
#include "View.h"
#include "MapView.h"
#include "BridgeView.h"
#include "SailingView.h"
#include "ObjectView.h"
#include "Ship_factory.h"
#include "Ship.h"
#include "Island.h"
#include "Geometry.h"

#include <iostream>
#include <string>
#include <algorithm>
#include <memory>
#include <map>
#include <functional>
#define NDEBUG
#include <cassert>

using std::cin;
using std::cout;
using std::endl;
using std::find;
using std::string;
using std::map;
using std::make_shared;
using std::mem_fn;
using std::pair;
using std::shared_ptr;

const char * const map_view_not_open_c = "Map view is not open!";
const char * const unrecognized_command_c = "Unrecognized command!";



//Will get a double from std input and through and clear cin if invalid input
static double read_double();

//same checks as get_double but also checks that the read in number is non_negative
static double read_speed();

//reads two doubles from cin and uses those as the x,y coordinates for the point
static Point read_Point();

//Reads an island name from from cin and uses that string to get the island ptr
//May throw island not found error
static shared_ptr<Island> get_island_ptr_from_input();

//Reads in the name of a ship and returns the pointer to it if available, else throws error
static shared_ptr<Ship> get_ship_ptr_from_input();



// create View object, run the program by acccepting user commands, then destroy View object
void Controller::run(){
    Model_destroyer model_destroyer;
    map<string, void(Controller::*)(shared_ptr<Ship>)> ship_commands{
        {"course", &Controller::course}, {"position", &Controller::position},
        {"destination", &Controller::destination}, {"load_at", &Controller::load_at},
        {"unload_at", &Controller::unload_at}, {"dock_at", &Controller::dock_at},
        {"attack", &Controller::attack}, {"refuel", &Controller::refuel},
        {"stop", &Controller::stop}, {"stop_attack", &Controller::stop_attack}
    };
    map<string, void(Controller::*)()> model_view_commands{
        {"default", &Controller::reset_map_to_default}, {"size", &Controller::size},
        {"zoom", &Controller::zoom}, {"pan", &Controller::pan},
        {"show", &Controller::show}, {"status", &Controller::status},
        {"go", &Controller::go}, {"create", &Controller::create},
        {"open_map_view", &Controller::open_map_view}, {"close_map_view", &Controller::close_map_view},
        {"open_sailing_view", &Controller::open_sailing_view}, {"close_sailing_view", &Controller::close_sailing_view},
        {"open_bridge_view", &Controller::open_bridge_view}, {"close_bridge_view", &Controller::close_bridge_view},
        {"open_object_view", &Controller::open_object_view}, {"close_object_view", &Controller::close_object_view}
    };
    string word;
    while (true) {
        cout << "\nTime " << Model::get_instance().get_time() << ": Enter command: ";
        //MITCH. Should I make this entry control the loop??? ASK kinda doubt
        cin >> word;
        try {
            //done this way to only have to search once through the map
            auto mv_command_it = model_view_commands.find(word);
            if (mv_command_it != model_view_commands.end()){
                //execute the command that is stored in the map iterator
                (this->*mv_command_it->second)();
            } else if (Model::get_instance().is_ship_present(word)) {
                //next word should be a ship command
                shared_ptr<Ship> ship_ptr = Model::get_instance().get_ship_ptr(word);
                cin >> word;
                //if the ship command is found, execute
                auto ship_command_it = ship_commands.find(word);
                if (ship_command_it != ship_commands.end()) {
                    (this->*ship_command_it->second)(ship_ptr);
                } else
                    throw Error(unrecognized_command_c);
            } else if (word == "quit"){
                break;
            } else {
                throw Error(unrecognized_command_c);
            }
        } catch (Error & e){
            getline(cin, word);
            cout << e.what() << endl;
        }
    }
    cout << "Done" << endl;
}



// Ship commands
void Controller::course(shared_ptr<Ship> ship_ptr){
    double heading = read_double();
    if (heading < 0.0 || heading >= 360.0)
        throw Error("Invalid heading entered!");
    ship_ptr->set_course_and_speed(heading, read_speed());
}


void Controller::position(shared_ptr<Ship> ship_ptr){
    Point point = read_Point();
    ship_ptr->set_destination_position_and_speed(point, read_speed());
}


void Controller::destination(shared_ptr<Ship> ship_ptr){
    Point location = get_island_ptr_from_input()->get_location();
    ship_ptr->set_destination_position_and_speed(location, read_speed());
}

void Controller::load_at(shared_ptr<Ship> ship_ptr){
    ship_ptr->set_load_destination(get_island_ptr_from_input());
}

void Controller::unload_at(shared_ptr<Ship> ship_ptr){
    ship_ptr->set_unload_destination(get_island_ptr_from_input());
}

void Controller::dock_at(shared_ptr<Ship> ship_ptr){
    ship_ptr->dock(get_island_ptr_from_input());
}

void Controller::attack(shared_ptr<Ship> ship_ptr){
    ship_ptr->attack(get_ship_ptr_from_input());
}

void Controller::refuel(shared_ptr<Ship> ship_ptr){
    ship_ptr->refuel();
}

void Controller::stop(shared_ptr<Ship> ship_ptr){
    ship_ptr->stop();
}

void Controller::stop_attack(shared_ptr<Ship> ship_ptr){
    ship_ptr->stop_attack();
}

//view commands

void Controller::open_bridge_view(){
    shared_ptr<Ship> ship_ptr = get_ship_ptr_from_input();
    shared_ptr<BridgeView> new_bridge_view_ptr = make_shared<BridgeView>(ship_ptr->get_name());
    auto bridge_view_insertion_it = bridge_views.insert(pair<string, shared_ptr<BridgeView>>(ship_ptr->get_name(), new_bridge_view_ptr));
    if (bridge_view_insertion_it.second) {
        Model::get_instance().attach(new_bridge_view_ptr);
        all_views.push_back(new_bridge_view_ptr);
    } else {
        throw Error("Bridge view is already open for that ship!");
    }
}


void Controller::close_bridge_view(){
    string ship_name;
    cin >> ship_name;
    auto bridge_view_it = bridge_views.find(ship_name);
    if (bridge_view_it == bridge_views.end()) {
        throw Error("Bridge view for that ship is not open!");
    }
    Model::get_instance().detach(bridge_view_it->second);
    all_views.erase(find(all_views.begin(), all_views.end(), bridge_view_it->second));
    bridge_views.erase(ship_name);
}


void Controller::reset_map_to_default(){
    if (!map_view_ptr) {
        throw Error(map_view_not_open_c);
    }
    map_view_ptr->set_defaults();
}


void Controller::size(){
    int size;
    cin >> size;
    if (!cin) {
        cin.clear();
        throw Error("Expected an integer!");
    }
    if (!map_view_ptr) {
        throw Error(map_view_not_open_c);
    }
    map_view_ptr->set_size(size);
}


void Controller::zoom(){
    if (!map_view_ptr) {
        throw Error(map_view_not_open_c);
    }
    map_view_ptr->set_scale(read_double());
}


void Controller::pan(){
    if (!map_view_ptr) {
        throw Error(map_view_not_open_c);
    }
    map_view_ptr->set_origin(read_Point());
}

void Controller::show(){
    for_each(all_views.begin(), all_views.end(), mem_fn(&View::draw));
}

//Model commands

void Controller::status(){
    Model::get_instance().describe();
}

void Controller::go(){
    Model::get_instance().update();
}


void Controller::create(){
    string new_ship_name;
    cin >> new_ship_name;
    if (static_cast<int>(new_ship_name.length()) < min_ship_name_length_c)
        throw Error("Name is too short!");
    if (Model::get_instance().is_name_in_use(new_ship_name))
        throw Error("Name is already in use!");
    string ship_type;
    cin >> ship_type;
    Model::get_instance().add_ship(create_ship(new_ship_name, ship_type, read_Point()));
}


void Controller::open_map_view(){
    if (map_view_ptr) {
        throw Error("Map view is already open!");
    }
    map_view_ptr = make_shared<MapView>();
    Model::get_instance().attach(map_view_ptr);
    all_views.push_back(map_view_ptr);
}

void Controller::close_map_view(){
    if (!map_view_ptr) {
        throw Error("Map view is not open!");
    }
    Model::get_instance().detach(map_view_ptr);
    map_view_ptr.reset();
    all_views.erase(find(all_views.begin(), all_views.end(), map_view_ptr));
}

void Controller::open_sailing_view(){
    if (sailing_view_ptr) {
        throw Error("Sailing data view is already open!");
    }
    sailing_view_ptr = make_shared<SailingView>();
    Model::get_instance().attach(sailing_view_ptr);
    all_views.push_back(sailing_view_ptr);
}

void Controller::close_sailing_view(){
    if (!sailing_view_ptr) {
        throw Error("Sailing data view is not open!");
    }
    Model::get_instance().detach(sailing_view_ptr);
    sailing_view_ptr.reset();
    all_views.erase(find(all_views.begin(), all_views.end(), sailing_view_ptr));
}

void Controller::open_object_view(){
    shared_ptr<Ship> ship_ptr = get_ship_ptr_from_input();
    shared_ptr<ObjectView> new_object_view_ptr = make_shared<ObjectView>(ship_ptr->get_name());
    auto object_view_insertion_it = object_views.insert(pair<string, shared_ptr<ObjectView>>(ship_ptr->get_name(), new_object_view_ptr));
    if (object_view_insertion_it.second) {
        Model::get_instance().attach(new_object_view_ptr);
        all_views.push_back(new_object_view_ptr);
    } else {
        throw Error("object view is already open for that ship!");
    }
}


void Controller::close_object_view(){
    string ship_name;
    cin >> ship_name;
    auto object_view_it = object_views.find(ship_name);
    if (object_view_it == object_views.end()) {
        throw Error("object view for that ship is not open!");
    }
    Model::get_instance().detach(object_view_it->second);
    all_views.erase(find(all_views.begin(), all_views.end(), object_view_it->second));
    object_views.erase(ship_name);
}


/////Helpers
////////////////////////////////////////////////////////////////

static double read_speed(){
    double d = read_double();
    if (d < 0.)
        throw Error("Negative speed entered!");
    return d;
}

static double read_double(){
    double d;
    cin >> d;
    if (!cin) {
        cin.clear();
        throw Error("Expected a double!");
    }
    return d;
}

static Point read_Point(){
    double x = read_double();
    double y = read_double();
    return Point(x,y);
}

static shared_ptr<Island> get_island_ptr_from_input(){
    string island_name;
    cin >> island_name;
    return Model::get_instance().get_island_ptr(island_name);
}


static shared_ptr<Ship> get_ship_ptr_from_input(){
    string ship_name;
    cin >> ship_name;
    return Model::get_instance().get_ship_ptr(ship_name);
}