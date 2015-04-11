/*
Model is part of a simplified Model-View-Controller pattern.
Model keeps track of the Sim_objects in our little world. It is the only
component that knows how many Islands and Ships there are, but it does not
know about any of their derived classes, nor which Ships are of what kind of Ship. 
It has facilities for looking up objects by name, and removing Ships.  When
created, it creates an initial group of Islands and Ships using the Ship_factory.
Finally, it keeps the system's time.

Controller tells Model what to do; Model in turn tells the objects what do, and
when asked to do so by an object, tells all the Views whenever anything changes that might be relevant.
Model also provides facilities for looking up objects given their name.
*/
#ifndef MODEL_H
#define MODEL_H
#include <map>
#include <memory>
#include <set>
#include <list>
#include <cstring>
#include <vector>
class Sim_object;
class Island;
class Ship;
class View;
struct Point;

class Model {
public:
    // static method to get the instance of Model
    static Model& get_instance();
    
    // disallow copy/move construction or assignment
    Model(Model& other)=delete;
    Model(Model&& other)=delete;
    Model& operator=(Model& rhs)=delete;
    Model& operator=(Model&& rhs)=delete;

	// return the current time
	int get_time() {return time;}

	// is name already in use for either ship or island?
    // either the identical name, or identical in first two characters counts as in-use
	bool is_name_in_use(const std::string& name) const;

	// is there such an island?
	bool is_island_present(const std::string& name) const;
	// will throw Error("Island not found!") if no island of that name
    std::shared_ptr<Island> get_island_ptr(const std::string& name) const;

	// is there such an ship?
	bool is_ship_present(const std::string& name) const;
	// add a new ship to the list, and update the view
    void add_ship(std::shared_ptr<Ship>);
	// will throw Error("Ship not found!") if no ship of that name
    std::shared_ptr<Ship> get_ship_ptr(const std::string& name) const;
	
	// tell all objects to describe themselves
	void describe() const;
	// increment the time, and tell all objects to update themselves
	void update();	
    
	/* View services */
	// Attaching a View adds it to the container and causes it to be updated
    // with all current objects'location (or other state information.
    void attach(std::shared_ptr<View>);
	// Detach the View by discarding the supplied pointer from the container of Views
    // - no updates sent to it thereafter.
    void detach(std::shared_ptr<View>);
    // Iterates over all View* and calls draw()
    void draw_views();
	
    // notify the views about an object's location
	void notify_location(const std::string& name, Point location);
	// notify the views that an object is now gone
	void notify_gone(const std::string& name);
    // Update ship fuel
    void notify_fuel(const std::string& name, double fuel);
    // Update ship speed
    void notify_course_and_speed(const std::string& name, double course, double speed);
    
    // remove the Ship from the containers.
    void remove_ship(std::shared_ptr<Ship> ship_ptr);
    // Return a set of Island location Points
    std::vector<std::shared_ptr<Island>> get_islands();
private:
    // create the initial objects, output constructor message
    Model();
    ~Model() {};
    
    struct Name_Comparator {
        bool operator() (std::shared_ptr<Sim_object> s1, std::shared_ptr<Sim_object> s2);
    };
    
	int time;		// the simulated time
    std::set<std::shared_ptr<Sim_object>, Name_Comparator> all_objects;
    std::map<std::string, std::shared_ptr<Ship>> ships;
    std::map<std::string, std::shared_ptr<Island>> islands;
    std::list<std::shared_ptr<View>> view_list;
    
    void create_and_insert_island(const std::string& name_, Point position_,
                              double fuel_ = 0., double production_rate_ = 0.);
    void create_and_insert_ship(const std::string& name, const std::string& type,
                                Point initial_position);
};

#endif
