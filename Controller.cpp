#include "Controller.h"
#include "Model.h"
#include "Views.h"
#include "Ship.h"
#include "Island.h"
#include "Geometry.h"
#include "Ship_factory.h"
#include "Utility.h"
#include <exception>
#include <iostream>
#include <memory>
#include <string>
using std::cin;
using std::cout;
using std::endl;
using std::exception;
using std::shared_ptr;
using std::string;
using std::pair;

// File static functions
static Point read_point_from_cin();

using ship_fn_pair = pair<string, void(Controller::*)(shared_ptr<Ship> ship_ptr)>;
using mv_fn_pair = pair<string, void(Controller::*)()>;

// Helper functions (commands to be run)

void Controller::controller_set_course_and_speed(shared_ptr<Ship> ship_ptr) {
    double heading, speed;
    cin >> heading;
    if(cin.fail())
        throw Error(cmdline_double_error_c);
    if(heading < 0.0 || heading >= 360.0)
        throw Error("Invalid heading entered!");
    cin >> speed;
    if(cin.fail())
        throw Error(cmdline_double_error_c);
    if(speed < 0)
        throw Error(cmdline_negative_speed_error_c);
    ship_ptr->set_course_and_speed(heading, speed);
}

static Point read_point_from_cin() {
    double x, y;
    cin >> x;
    if(cin.fail())
        throw Error(cmdline_double_error_c);
    cin >> y;
    if(cin.fail())
        throw Error(cmdline_double_error_c);
    return Point(x, y);
}

void Controller::controller_set_destination_position_and_speed(shared_ptr<Ship> ship_ptr) {
    Point position_point = read_point_from_cin();
    double speed;
    cin >> speed;
    if(cin.fail())
        throw Error(cmdline_double_error_c);
    if(speed < 0)
        throw Error(cmdline_negative_speed_error_c);
    ship_ptr->set_destination_position_and_speed(position_point, speed);
}

void Controller::controller_set_island_destination(shared_ptr<Ship> ship_ptr) {
    string island_name;
    cin >> island_name;
    double speed;
    cin >> speed;
    if(cin.fail())
        throw Error(cmdline_double_error_c);
    if(speed < 0)
        throw Error(cmdline_negative_speed_error_c);
    shared_ptr<Island> island_destination = Model::get_instance().get_island_ptr(island_name);
    ship_ptr->set_destination_position_and_speed(island_destination->get_location(), speed);
}

void Controller::controller_set_load_at(shared_ptr<Ship> ship_ptr) {
    string island_name;
    cin >> island_name;
    shared_ptr<Island> island_to_load_at = Model::get_instance().get_island_ptr(island_name);
    ship_ptr->set_load_destination(island_to_load_at);
}

void Controller::controller_set_unload_at(shared_ptr<Ship> ship_ptr) {
    string island_name;
    cin >> island_name;
    shared_ptr<Island> island_to_unload_at = Model::get_instance().get_island_ptr(island_name);
    ship_ptr->set_unload_destination(island_to_unload_at);
}

void Controller::controller_dock_at(shared_ptr<Ship> ship_ptr) {
    string island_name;
    cin >> island_name;
    shared_ptr<Island> island_to_dock_at = Model::get_instance().get_island_ptr(island_name);
    ship_ptr->dock(island_to_dock_at);
}

void Controller::controller_attack(shared_ptr<Ship> ship_ptr) {
    string ship_to_attack;
    cin >> ship_to_attack;
    shared_ptr<Ship> ship_to_attack_ptr = Model::get_instance().get_ship_ptr(ship_to_attack);
    ship_ptr->attack(ship_to_attack_ptr);
}

void Controller::controller_refuel(shared_ptr<Ship> ship_ptr) {
    ship_ptr->refuel();
}

void Controller::controller_stop(shared_ptr<Ship> ship_ptr) {
    ship_ptr->stop();
}

void Controller::controller_stop_attack(shared_ptr<Ship> ship_ptr) {
    ship_ptr->stop_attack();
}

// Model-View Commands
void Controller::controller_set_defaults() {
    //view_ptr->set_defaults();
}

void Controller::controller_set_size() {
    int size;
    cin >> size;
    if(cin.fail())
        throw Error("Expected an integer!");
   // view_ptr->set_size(size);
}

void Controller::controller_set_zoom() {
    double zoom;
    cin >> zoom;
    if(cin.fail())
        throw Error(cmdline_double_error_c);
  //  view_ptr->set_scale(zoom);
}

void Controller::controller_set_pan() {
    Point pan_point = read_point_from_cin();
   // view_ptr->set_origin(pan_point);
}

void Controller::controller_show() {
    view_ptr->draw();
}

void Controller::controller_status() {
    Model::get_instance().describe();
}

void Controller::controller_go() {
    Model::get_instance().update();
}

void Controller::controller_create() {
    string input, type;
    double x, y;
    cin >> input >> type >> x >> y;
    if(cin.fail())
        throw Error(cmdline_double_error_c);
    if(input.size() < 2)
        throw Error("Name is too short!");
    if(Model::get_instance().is_name_in_use(input))
        throw Error("Name is already in use!");
    shared_ptr<Ship> new_ship = create_ship(input, type, Point(x, y));
    Model::get_instance().add_ship(new_ship);
}



// output constructor message
Controller::Controller() {
    ship_commands.insert(ship_fn_pair("course", &Controller::controller_set_course_and_speed));
    ship_commands.insert(ship_fn_pair("position", &Controller::controller_set_destination_position_and_speed));
    ship_commands.insert(ship_fn_pair("destination", &Controller::controller_set_island_destination));
    ship_commands.insert(ship_fn_pair("load_at", &Controller::controller_set_load_at));
    ship_commands.insert(ship_fn_pair("unload_at", &Controller::controller_set_unload_at));
    ship_commands.insert(ship_fn_pair("dock_at", &Controller::controller_dock_at));
    ship_commands.insert(ship_fn_pair("attack", &Controller::controller_attack));
    ship_commands.insert(ship_fn_pair("refuel", &Controller::controller_refuel));
    ship_commands.insert(ship_fn_pair("stop", &Controller::controller_stop));
    ship_commands.insert(ship_fn_pair("stop_attack", &Controller::controller_stop_attack));
    
    mv_commands.insert(mv_fn_pair("default", &Controller::controller_set_defaults));
    mv_commands.insert(mv_fn_pair("size", &Controller::controller_set_size));
    mv_commands.insert(mv_fn_pair("zoom", &Controller::controller_set_zoom));
    mv_commands.insert(mv_fn_pair("pan", &Controller::controller_set_pan));
    mv_commands.insert(mv_fn_pair("show", &Controller::controller_show));
    mv_commands.insert(mv_fn_pair("status", &Controller::controller_status));
    mv_commands.insert(mv_fn_pair("go", &Controller::controller_go));
    mv_commands.insert(mv_fn_pair("create", &Controller::controller_create));
    
    cout << "Controller constructed" << endl;
}

// output destructor message
Controller::~Controller() {
    cout << "Controller destructed" << endl;
}

// create View object, run the program by acccepting user commands, then destroy View object
void Controller::run() {
    view_ptr = new MapView;
    Model::get_instance().attach(view_ptr);

    cout << "\nTime " << Model::get_instance().get_time() << ": Enter command: ";
    
    string input;
    cin >> input;
    while(input != "quit") {
        try {
            // First see if we've gotten a Ship command
            if(Model::get_instance().is_ship_present(input)) {
                string command;
                cin >> command;
                shared_ptr<Ship> ship_ptr = Model::get_instance().get_ship_ptr(input);
                auto str_ship_pair = ship_commands.find(command);
                if(str_ship_pair == ship_commands.end())
                    throw Error(cmdline_unrecognized_command_c);
                (this->*str_ship_pair->second)(ship_ptr);
            } // Then look for a Model/View command
                else {
                auto str_fn_pair = mv_commands.find(input);
                if(str_fn_pair == mv_commands.end())
                    throw Error(cmdline_unrecognized_command_c);
                (this->*str_fn_pair->second)();
            }
        } catch(Error& e) {
            cout << e.what() << endl;
            string rest_of_line;
            cin.clear();
            getline(cin, rest_of_line);
        } catch(std::exception& se) {
            cout << se.what() << endl;
            break; // Exit the loop and program
        }
        cout << "\nTime " << Model::get_instance().get_time() << ": Enter command: ";
        cin >> input;
    }
    Model::get_instance().detach(view_ptr);
    delete view_ptr;
    cout <<  "Done" << endl;
    return;

}
