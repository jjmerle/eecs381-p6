#ifndef UTILITIES_H
#define UTILITIES_H
#include <exception>
struct Point;

class Error : public std::exception {
public:
	Error(const char* msg_) : msg(msg_) {}
    const char* what() const noexcept override
        {return msg;}
private:
	const char* msg;
};

// Error message for reaching 'default' in switch statements
extern const char* const default_switch_error_c;

#endif
