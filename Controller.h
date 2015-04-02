 /* Controller
This class is responsible for controlling the Model and View according to interactions
with the user.
*/
#ifndef CONTROLLER_H
#define CONTROLLER_H
#include <map>
#include <memory>
class Model;
class View;
class Ship;
class Island;
class MapView;
class SailingView;
class BridgeView;

class Controller {
public:	
	// output constructor message
	Controller();
    
	// create View object, run the program by acccepting user commands, then destroy View object
	void run();
    
private:
    std::shared_ptr<MapView> mapview_ptr;
    std::shared_ptr<SailingView> sailview_ptr;
    std::map<std::string, std::shared_ptr<BridgeView>> bridgeview_map;
    std::map<std::string, void(Controller::*)(std::shared_ptr<Ship> ship_ptr)> ship_commands;
    std::map<std::string, void(Controller::*)()> mv_commands;
    
    // Helper Commands
    // Ship Commands
    
    // Set Course and Speed of a Ship
    void set_course_and_speed(std::shared_ptr<Ship> ship_ptr);
    // Set Destination Position and Speed for a Ship
    void set_destination_position_and_speed(std::shared_ptr<Ship> ship_ptr);
    // Set Island Destination for a Ship
    void set_island_destination(std::shared_ptr<Ship> ship_ptr);
    // Set where Ship will load at
    void set_load_at(std::shared_ptr<Ship> ship_ptr);
    // Set where Ship will unload at
    void set_unload_at(std::shared_ptr<Ship> ship_ptr);
    // Set where Ship will dock at
    void dock_at(std::shared_ptr<Ship> ship_ptr);
    // Have a Ship attack another Ship
    void attack(std::shared_ptr<Ship> ship_ptr);
    // Have Ship refuel
    void refuel(std::shared_ptr<Ship> ship_ptr);
    // Have Ship stop
    void stop(std::shared_ptr<Ship> ship_ptr);
    // Have Ship stop attack
    void stop_attack(std::shared_ptr<Ship> ship_ptr);
    
    // Model-View Commands
    
    // Set default Size, Scale, and Origin for MapView
    void set_defaults();
    // Set size for MapView
    void set_size();
    // Set Scale for MapView
    void set_zoom();
    // Set Origin for MapView
    void set_pan();
    // Draw all attached Views
    void show();
    // Output status of all Sim_objects
    void status();
    // Update all Sim_objects
    void go();
    // Create a new Ship
    void create();
    
    // View subclass Commands
    /* - create and open the map view. The Project 4 view commands size, zoom, and
     pan control this view if it is open. Error: map view is already open. */
    void open_map_view();
    /* - close and destroy the map view. Error: no map view is open.
     Note: The other map view commands for default, scale, origin,
     size throw an Error if no map view is open; this check is done first,
     before reading and checking any parameters. */
    void close_map_view();
    /* - create and open the sailing data view. Error: sailing data view is already open. */
    void open_sailing_view();
    /* - close and destroy the sailing data view view. Error: no sailing data
     view is open. */
    void close_sailing_view();
    /* - create and open a bridge view that shows the view from the bridge of ship <shipname>. Errors in
     order of checks: no ship of that name; bridge view is already open for that ship.*/
    void open_bridge_view();
    /* Close a bridge view for ship with name shipname */
    void close_bridge_view();
};

#endif