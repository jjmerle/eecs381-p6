#include "Ship_factory.h"
#include "Utility.h"
#include "Cruise_ship.h"
#include "Cruiser.h"
#include "Tanker.h"
#include <memory>
using std::make_shared;
using std::string;
using std::shared_ptr;

class Ship;
/* This is a very simple form of factory, a function; you supply the information, it creates
 the specified kind of object and returns a pointer to it. The Ship is allocated
 with new, so some other component is resposible for deleting it.
 */

// may throw Error("Trying to create ship of unknown type!")
shared_ptr<Ship> create_ship(const string& name, const string& type, Point initial_position) {
    if(type == "Cruiser") {
        return make_shared<Cruiser>(name, initial_position);
    } else if(type == "Tanker") {
        return make_shared<Tanker>(name, initial_position);
    } else if(type == "Cruise_ship") {
        return make_shared<Cruise_ship>(name, initial_position);
    } else {
        throw Error("Trying to create ship of unknown type!");
    }
}