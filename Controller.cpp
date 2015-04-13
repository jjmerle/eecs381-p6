#include "Controller.h"
#include "Model.h"
#include "SailingView.h"
#include "MapView.h"
#include "BridgeView.h"
#include "ObjectView.h"
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
using std::make_shared;
using std::shared_ptr;
using std::string;
using std::pair;

const char* const cmdline_double_error_c = "Expected a double!";
const char* const cmdline_unrecognized_command_c = "Unrecognized command!";
const char* const cmdline_negative_speed_error_c = "Negative speed entered!";

// Function pointer types
using ship_fn_pair = pair<string, void(Controller::*)(shared_ptr<Ship> ship_ptr)>;
using mv_fn_pair = pair<string, void(Controller::*)()>;

// File static functions
// Reads a Point from cin, ensuring valid input
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

// Helper functions (commands to be run)

// Set Course and Speed of a Ship
void Controller::set_course_and_speed(shared_ptr<Ship> ship_ptr) {
    double course, speed;
    cin >> course;
    if(cin.fail())
        throw Error(cmdline_double_error_c);
    if(course < 0.0 || course >= 360.0)
        throw Error("Invalid heading entered!");
    cin >> speed;
    if(cin.fail())
        throw Error(cmdline_double_error_c);
    if(speed < 0)
        throw Error(cmdline_negative_speed_error_c);
    ship_ptr->set_course_and_speed(course, speed);
}

// Set Destination Position and Speed for a Ship
void Controller::set_destination_position_and_speed(shared_ptr<Ship> ship_ptr) {
    Point position_point = read_point_from_cin();
    double speed;
    cin >> speed;
    if(cin.fail())
        throw Error(cmdline_double_error_c);
    if(speed < 0)
        throw Error(cmdline_negative_speed_error_c);
    ship_ptr->set_destination_position_and_speed(position_point, speed);
}

// Set Island Destination for a Ship
void Controller::set_island_destination(shared_ptr<Ship> ship_ptr) {
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

// Set where Ship will load at
void Controller::set_load_at(shared_ptr<Ship> ship_ptr) {
    string island_name;
    cin >> island_name;
    shared_ptr<Island> island_to_load_at = Model::get_instance().get_island_ptr(island_name);
    ship_ptr->set_load_destination(island_to_load_at);
}

// Set where Ship will unload at
void Controller::set_unload_at(shared_ptr<Ship> ship_ptr) {
    string island_name;
    cin >> island_name;
    shared_ptr<Island> island_to_unload_at = Model::get_instance().get_island_ptr(island_name);
    ship_ptr->set_unload_destination(island_to_unload_at);
}

// Set where Ship will dock at
void Controller::dock_at(shared_ptr<Ship> ship_ptr) {
    string island_name;
    cin >> island_name;
    shared_ptr<Island> island_to_dock_at = Model::get_instance().get_island_ptr(island_name);
    ship_ptr->dock(island_to_dock_at);
}

// Have a Ship attack another Ship
void Controller::attack(shared_ptr<Ship> ship_ptr) {
    string ship_to_attack;
    cin >> ship_to_attack;
    shared_ptr<Ship> ship_to_attack_ptr = Model::get_instance().get_ship_ptr(ship_to_attack);
    ship_ptr->attack(ship_to_attack_ptr);
}

// Have Ship refuel
void Controller::refuel(shared_ptr<Ship> ship_ptr) {
    ship_ptr->refuel();
}

// Have Ship stop
void Controller::stop(shared_ptr<Ship> ship_ptr) {
    ship_ptr->stop();
}

// Have Ship stop attack
void Controller::stop_attack(shared_ptr<Ship> ship_ptr) {
    ship_ptr->stop_attack();
}

// Model-View Commands
// Set default Size, Scale, and Origin for MapView
void Controller::set_defaults() {
    if(!mapview_ptr) {
        throw Error("Map view is not open!");
    }
    mapview_ptr->set_defaults();
}

// Set size for MapView
void Controller::set_size() {
    if(!mapview_ptr) {
        throw Error("Map view is not open!");
    }
    int size;
    cin >> size;
    if(cin.fail())
        throw Error("Expected an integer!");
    mapview_ptr->set_size(size);
}

// Set Scale for MapView
void Controller::set_zoom() {
    if(!mapview_ptr) {
        throw Error("Map view is not open!");
    }
    double zoom;
    cin >> zoom;
    if(cin.fail())
        throw Error(cmdline_double_error_c);
    mapview_ptr->set_scale(zoom);
}

// Set Origin for MapView
void Controller::set_pan() {
    if(!mapview_ptr) {
        throw Error("Map view is not open!");
    }
     Point pan_point = read_point_from_cin();
     mapview_ptr->set_origin(pan_point);
}

// Draw all attached Views
void Controller::show() {
    Model::get_instance().draw_views();
}

// Output status of all Sim_objects
void Controller::status() {
    Model::get_instance().describe();
}

// Update all Sim_objects
void Controller::go() {
    Model::get_instance().update();
}

// Create a new Ship
void Controller::create() {
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

/* - create and open the map view. The Project 4 view commands size, zoom, and 
 pan control this view if it is open. Error: map view is already open. */
void Controller::open_map_view() {
    if(mapview_ptr) {
        throw Error("Map view is already open!");
    } else {
        mapview_ptr = make_shared<MapView>();
        Model::get_instance().attach(mapview_ptr);
    }
}

/* - close and destroy the map view. Error: no map view is open.
 Note: The other map view commands for default, scale, origin, 
 size throw an Error if no map view is open; this check is done first,
 before reading and checking any parameters. */
void Controller::close_map_view() {
    if(!mapview_ptr) {
        throw Error("Map view is not open!");
    } else {
        Model::get_instance().detach(mapview_ptr);
        mapview_ptr.reset();
    }
}

/* - create and open the sailing data view. Error: sailing data view is already open. */
void Controller::open_sailing_view() {
    if(sailview_ptr) {
        throw Error("Sailing data view is already open!");
    } else {
        sailview_ptr = make_shared<SailingView>();
        Model::get_instance().attach(sailview_ptr);
    }
    
}

/* - close and destroy the sailing data view view. Error: no sailing data
 view is open. */
void Controller::close_sailing_view() {
    if(!sailview_ptr) {
        throw Error("Sailing data view is not open!");
    } else {
        Model::get_instance().detach(sailview_ptr);
        sailview_ptr.reset();
    }
}

/* - create and open a bridge view that shows the view from the bridge of ship <shipname>. Errors in
 order of checks: no ship of that name; bridge view is already open for that ship.*/
void Controller::open_bridge_view() {
    string shipname;
    cin >> shipname;
    if(!Model::get_instance().is_name_in_use(shipname)) {
        throw Error("Ship not found!");
    }
    auto bridgeview_it = bridgeview_map.find(shipname);
    if(bridgeview_it != bridgeview_map.end()) {
        throw Error("Bridge view is already open for that ship!");
    } else {
        shared_ptr<BridgeView> bridgeview_ptr = make_shared<BridgeView>(shipname);
        bridgeview_map[shipname] = bridgeview_ptr;
        Model::get_instance().attach(bridgeview_ptr);
    }
}

/* Close a bridge view for ship with name shipname */
void Controller::close_bridge_view() {
    string shipname;
    cin >> shipname;
    auto bridgeview_it = bridgeview_map.find(shipname);
    if(bridgeview_it == bridgeview_map.end()) {
        throw Error("Bridge view for that ship is not open!");
    } else {
        Model::get_instance().detach(bridgeview_it->second);
        bridgeview_map.erase(bridgeview_it);
    }
}

/* - create and open a object view that shows the view from birds-eye of object <objectname>. Errors in
 order of checks: no object of that name; objectview is already open for that object.*/
void Controller::open_object_view() {
    string objectname;
    cin >> objectname;
    if(!(Model::get_instance().is_ship_present(objectname) || Model::get_instance().is_island_present(objectname))) {
        throw Error("Object not found!");
    }
    auto objectview_it = objectview_map.find(objectname);
    if(objectview_it != objectview_map.end()) {
        throw Error("Object view is already open for that object!");
    } else {
        shared_ptr<ObjectView> objectview_ptr = make_shared<ObjectView>(objectname);
        objectview_map[objectname] = objectview_ptr;
        Model::get_instance().attach(objectview_ptr);
    }
}

/* Close a objectview for object with name objectname */
void Controller::close_object_view() {
    string objectname;
    cin >> objectname;
    auto objectview_it = objectview_map.find(objectname);
    if(objectview_it == objectview_map.end()) {
        throw Error("Object view for that object is not open!");
    } else {
        Model::get_instance().detach(objectview_it->second);
        objectview_map.erase(objectview_it);
    }
}

// output constructor message
Controller::Controller() {
    ship_commands.insert(ship_fn_pair("course", &Controller::set_course_and_speed));
    ship_commands.insert(ship_fn_pair("position", &Controller::set_destination_position_and_speed));
    ship_commands.insert(ship_fn_pair("destination", &Controller::set_island_destination));
    ship_commands.insert(ship_fn_pair("load_at", &Controller::set_load_at));
    ship_commands.insert(ship_fn_pair("unload_at", &Controller::set_unload_at));
    ship_commands.insert(ship_fn_pair("dock_at", &Controller::dock_at));
    ship_commands.insert(ship_fn_pair("attack", &Controller::attack));
    ship_commands.insert(ship_fn_pair("refuel", &Controller::refuel));
    ship_commands.insert(ship_fn_pair("stop", &Controller::stop));
    ship_commands.insert(ship_fn_pair("stop_attack", &Controller::stop_attack));
    
    mv_commands.insert(mv_fn_pair("default", &Controller::set_defaults));
    mv_commands.insert(mv_fn_pair("size", &Controller::set_size));
    mv_commands.insert(mv_fn_pair("zoom", &Controller::set_zoom));
    mv_commands.insert(mv_fn_pair("pan", &Controller::set_pan));
    mv_commands.insert(mv_fn_pair("show", &Controller::show));
    mv_commands.insert(mv_fn_pair("status", &Controller::status));
    mv_commands.insert(mv_fn_pair("go", &Controller::go));
    mv_commands.insert(mv_fn_pair("create", &Controller::create));
    
    mv_commands.insert(mv_fn_pair("open_map_view", &Controller::open_map_view));
    mv_commands.insert(mv_fn_pair("close_map_view", &Controller::close_map_view));
    mv_commands.insert(mv_fn_pair("open_sailing_view", &Controller::open_sailing_view));
    mv_commands.insert(mv_fn_pair("close_sailing_view", &Controller::close_sailing_view));
    mv_commands.insert(mv_fn_pair("open_bridge_view", &Controller::open_bridge_view));
    mv_commands.insert(mv_fn_pair("close_bridge_view", &Controller::close_bridge_view));
    mv_commands.insert(mv_fn_pair("open_object_view", &Controller::open_object_view));
    mv_commands.insert(mv_fn_pair("close_object_view", &Controller::close_object_view));
}

// create View object, run the program by acccepting user commands, then destroy View object
void Controller::run() {
    Model::get_instance(); // instantiate the Model

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
    cout <<  "Done" << endl;
    return;

}
