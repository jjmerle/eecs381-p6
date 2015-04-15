#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <memory>
#include <map>
#include <iosfwd>
#include <vector>

class View;
class Ship;
class MapView;
class BridgeView;
class SailingView;
class ObjectView;
/* Controller
 This class is responsible for controlling the Model and View according to interactions
 with the user.
 */


class Controller {
public:
    
    // create View object, run the program by acccepting user commands, then destroy View object
    void run();
    
private:
    std::vector<std::shared_ptr<View>> all_views;
    std::shared_ptr<MapView> map_view_ptr;
    std::shared_ptr<SailingView> sailing_view_ptr;
    std::map<std::string, std::shared_ptr<BridgeView>> bridge_views;
    std::map<std::string, std::shared_ptr<ObjectView>> object_views;
    
    // TODO - comments before declarations?
    // Ship commands
    void course(std::shared_ptr<Ship> ship_ptr);
    void position(std::shared_ptr<Ship> ship_ptr);
    void destination(std::shared_ptr<Ship> ship_ptr);
    void load_at(std::shared_ptr<Ship> ship_ptr);
    void unload_at(std::shared_ptr<Ship> ship_ptr);
    void dock_at(std::shared_ptr<Ship> ship_ptr);
    void attack(std::shared_ptr<Ship> ship_ptr);
    void refuel(std::shared_ptr<Ship> ship_ptr);
    void stop(std::shared_ptr<Ship> ship_ptr);
    void stop_attack(std::shared_ptr<Ship> ship_ptr);
    //View commands
    void reset_map_to_default();
    void size();
    void zoom();
    void pan();
    void show();
    void open_map_view();
    void close_map_view();
    void open_sailing_view();
    void close_sailing_view();
    void open_bridge_view();
    void close_bridge_view();
    //Model commands
    void status();
    void go();
    void create();
    /* - create and open a object view that shows the view from birds-eye of object <objectname>. Errors in
     order of checks: no object of that name; objectview is already open for that object.*/
    void open_object_view();
    /* Close a objectview for object with name objectname */
    void close_object_view();
};

#endif