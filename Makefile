CC = g++
LD = g++

CFLAGS = -c -pedantic-errors -std=c++11 -Wall -Werror
LFLAGS = -pedantic -Wall

OBJS = p5_main.o Model.o Controller.o Ship_factory.o Tanker.o Cruiser.o Warship.o Ship.o Island.o Utility.o Geometry.o Navigation.o Track_base.o Sim_object.o View.o Cruise_ship.o Views.o
PROG = p5exe
SDEMO3 = sdemo5exe

default: $(PROG)
sdemo3: $(SDEMO3)


$(PROG): $(OBJS)
	$(LD) $(LFLAGS) $(OBJS) -o $(PROG)

p5_main.o: p5_main.cpp Model.h Controller.h
	$(CC) $(CFLAGS) p5_main.cpp

Controller.o: Controller.cpp Controller.h Model.h Utility.h View.h Ship_factory.h Ship.h Island.h
	$(CC) $(CFLAGS) Controller.cpp

Model.o: Model.cpp Model.h Island.h Cruiser.h Utility.h Ship_factory.h View.h
	$(CC) $(CFLAGS) Model.cpp

Ship_factory.o: Ship_factory.cpp Ship_factory.h Cruiser.h Tanker.h Utility.h
	$(CC) $(CFLAGS) Ship_factory.cpp

Tanker.o: Tanker.cpp Tanker.h Island.h Utility.h
	$(CC) $(CFLAGS) Tanker.cpp

Cruiser.o: Cruiser.cpp Cruiser.h
	$(CC) $(CFLAGS) Cruiser.cpp

Warship.o: Warship.cpp Warship.h Utility.h
	$(CC) $(CFLAGS) Warship.cpp

Ship.o: Ship.cpp Geometry.h Island.h Utility.h Model.h
	$(CC) $(CFLAGS) Ship.cpp

Island.o: Island.cpp Island.h Sim_object.h Geometry.h Model.h
	$(CC) $(CFLAGS) Island.cpp

Utility.o: Utility.cpp Utility.h
	$(CC) $(CFLAGS) Utility.cpp

Geometry.o: Geometry.cpp Controller.h Model.h Utility.h View.h Ship_factory.h Geometry.h Ship.h Island.h
	$(CC) $(CFLAGS) Geometry.cpp

Navigation.o: Navigation.cpp Navigation.h 
	$(CC) $(CFLAGS) Navigation.cpp

Track_base.o: Track_base.cpp Navigation.h
	$(CC) $(CFLAGS) Track_base.cpp

Sim_object.o: Sim_object.cpp Sim_object.h
	$(CC) $(CFLAGS) Sim_object.cpp

View.o: View.cpp View.h Utility.h
	$(CC) $(CFLAGS) View.cpp

Views.o: Views.cpp Views.h Utility.h Model.h Ship.h Geometry.h View.h
	$(CC) $(CFLAGS) Views.cpp

Cruise_ship.o: Cruise_ship.cpp Cruise_ship.h Ship.h Model.h Geometry.h Island.h Utility.h
	$(CC) $(CFLAGS) Cruise_ship.cpp

clean:
	rm -f *.o

real_clean:
	rm -f *.o
	rm -f *exe
