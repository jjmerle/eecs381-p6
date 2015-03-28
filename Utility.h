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

/* add any of your own declarations here */
extern const char* const ship_move_error_c;
extern const char* const ship_speed_error_c;
extern const char* const ship_attack_error_c;
extern const char* const tanker_no_cargo_destinations_c;
extern const char* const cmdline_double_error_c;
extern const char* const cmdline_unrecognized_command_c;
extern const char* const cmdline_negative_speed_error_c;

#endif
