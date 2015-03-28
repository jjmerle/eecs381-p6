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

class Controller {
public:	
	// output constructor message
	Controller();
	// output destructor message
	~Controller();

	// create View object, run the program by acccepting user commands, then destroy View object
	void run();
    
private:
    View* view_ptr;
    std::map<std::string, void(Controller::*)(std::shared_ptr<Ship> ship_ptr)> ship_commands;
    std::map<std::string, void(Controller::*)()> mv_commands;
    
    // Helper Commands
    // Ship Commands
    void controller_set_course_and_speed(std::shared_ptr<Ship> ship_ptr);
    void controller_set_destination_position_and_speed(std::shared_ptr<Ship> ship_ptr);
    void controller_set_island_destination(std::shared_ptr<Ship> ship_ptr);
    void controller_set_load_at(std::shared_ptr<Ship> ship_ptr);
    void controller_set_unload_at(std::shared_ptr<Ship> ship_ptr);
    void controller_dock_at(std::shared_ptr<Ship> ship_ptr);
    void controller_attack(std::shared_ptr<Ship> ship_ptr);
    void controller_refuel(std::shared_ptr<Ship> ship_ptr);
    void controller_stop(std::shared_ptr<Ship> ship_ptr);
    void controller_stop_attack(std::shared_ptr<Ship> ship_ptr);
    
    // Model-View Commands
    void controller_set_defaults();
    void controller_set_size();
    void controller_set_zoom();
    void controller_set_pan();
    void controller_show();
    void controller_status();
    void controller_go();
    void controller_create();
};

#endif