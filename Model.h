#ifndef MODEL_H
#define MODEL_H

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

#include "Sim_object.h"

#include <map>
#include <set>
#include <memory>
#include <vector>

struct Point;
class Ship;
class Island;
class View;


class Model {
public:
    
    static Model & get_instance();
    
    
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
    
    //Notify views of this objects speed
    void notify_speed(const std::string& name, double speed);
    
    //Notify views of this objects fuel
    void notify_fuel(const std::string& name, double fuel);
    
    //Notify views of this objects course
    void notify_course(const std::string& name, double course);
    
    // notify the views about an object's location
    void notify_location(const std::string& name, Point location);
    
    // notify the views that an object is now gone
    void notify_gone(const std::string& name);
    
    // remove the Ship from the containers.
    void remove_ship(std::shared_ptr<Ship> ship_ptr);
    
    //Will return a pointer to the island if that position is an island. nullptr otherwise
    std::shared_ptr<Island> get_island_at(Point p);
    
    // disallow copy/move construction or assignment
    Model(const Model &) = delete;
    Model(Model &&) = delete;
    
    const std::map<std::string, std::shared_ptr<Island>> & get_Islands() const;
    
    const std::map<std::string, std::shared_ptr<Ship>> & get_Ships() const;
    
    
    friend class Model_destroyer;
private:
    
    struct Sim_object_name_sort {
        bool operator () (std::shared_ptr<const Sim_object> ptr_1, std::shared_ptr<const Sim_object> ptr_2){
            return ptr_1->get_name() < ptr_2->get_name();
        }
    };
    
    
    Model();
    ~Model(){}
    //MITCH: Maybe make a unique ptr
    static Model * singleton_ptr;
    int time;		// the simulated time
    
    std::vector<std::shared_ptr<View>> view_ptrs;
    std::set<std::shared_ptr<Sim_object>, Sim_object_name_sort> sim_objects;
    std::map<std::string, std::shared_ptr<Island>> islands;
    std::map<std::string, std::shared_ptr<Ship>> ships;
    
};

class Model_destroyer{
public:
    ~Model_destroyer(){
        delete Model::singleton_ptr;
        Model::singleton_ptr = nullptr;
    }
};

#endif