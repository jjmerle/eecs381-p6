#include "Sim_object.h"
#include <iostream>
using std::cout;
using std::endl;

// *** define the constructor in Sim_object.cpp to output the supplied message
Sim_object::Sim_object(const std::string& name_) : name(name_) {
    cout << "Sim_object " << name << " constructed" << endl;
}

// *** define the destructor in Sim_object.cpp to output the supplied message
Sim_object::~Sim_object() {
    cout << "Sim_object " << name << " destructed" << endl;
}