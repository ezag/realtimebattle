INCLUDES = \
	$(GTK_CFLAGS)\
	 -I$(top_srcdir)/include

realtimebattle_CXXFLAGS =

bin_PROGRAMS = realtimebattle

realtimebattle_SOURCES = \
	RealTimeBattle.cc\
	ArenaBase.cc\
	ArenaController.cc\
	ArenaRealTime.cc\
	ArenaReplay.cc\
	ArenaWindow.cc\
	ControlWindow.cc\
	Dialog.cc\
	Extras.cc\
	GeometricalObjects.cc\
	Gui.cc\
	List.cc\
	MessageWindow.cc\
	Options.cc\
	Robot.cc\
	ScoreWindow.cc\
	Shape.cc\
	Shot.cc\
	StartTournamentWindow.cc\
	StatisticsWindow.cc\
	String.cc\
	Structs.cc\
	Timer.cc\
	Various.cc\
	Vector2D.cc\
	OptionsWindow.cc
if NO_GETOPT_LONG
realtimebattle_SOURCES += getopt.c
endif

realtimebattle_LDFLAGS = 

realtimebattle_LDADD =  \
	$(GTK_LIBS)

