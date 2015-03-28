/* This is a very simple form of factory, a function; you supply the information, it creates
the specified kind of object and returns a pointer to it. The Ship is allocated
with new, so some other component is resposible for deleting it.
*/
#ifndef SHIP_FACTORY_H
#define SHIP_FACTORY_H
#include <memory>
#include <string>
struct Point;
class Ship;


// may throw Error("Trying to create ship of unknown type!")
std::shared_ptr<Ship> create_ship(const std::string& name, const std::string& type, Point initial_position);

#endif
