loggingpriority=10 # because twenty robots will cause a lot of logging messages

frameworkdir=$(ROBOTDIR)/rtb-team-framework
framework_PROGRAMS = rtb-team
noinst_SCRIPTS= createrobot.sh createrobotconf.sh
EXTRA_DIST=$(noinst_SCRIPTS)

brotfrucht_confs=\
Brotfrucht-Robot1-TeamOne.conf\
Brotfrucht-Robot2-TeamOne.conf\
Brotfrucht-Robot3-TeamOne.conf\
Brotfrucht-Robot4-TeamOne.conf\
Brotfrucht-Robot5-TeamOne.conf\
Brotfrucht-Robot1-TeamTwo.conf\
Brotfrucht-Robot2-TeamTwo.conf\
Brotfrucht-Robot3-TeamTwo.conf\
Brotfrucht-Robot4-TeamTwo.conf\
Brotfrucht-Robot5-TeamTwo.conf \
empty-Robot1-TeamThree.conf\
empty-Robot2-TeamThree.conf\
empty-Robot3-TeamThree.conf\
empty-Robot4-TeamThree.conf\
empty-Robot5-TeamThree.conf

CLEANFILES=$(brotfrucht_confs) $(robot_SCRIPTS)

framework_DATA=$(brotfrucht_confs)

robotdir=$(ROBOTDIR)
robot_SCRIPTS=\
Brotfrucht-Robot1-TeamOne.robot\
Brotfrucht-Robot2-TeamOne.robot\
Brotfrucht-Robot3-TeamOne.robot\
Brotfrucht-Robot4-TeamOne.robot\
Brotfrucht-Robot5-TeamOne.robot\
Brotfrucht-Robot1-TeamTwo.robot\
Brotfrucht-Robot2-TeamTwo.robot\
Brotfrucht-Robot3-TeamTwo.robot\
Brotfrucht-Robot4-TeamTwo.robot\
Brotfrucht-Robot5-TeamTwo.robot\
empty-Robot1-TeamThree.robot\
empty-Robot2-TeamThree.robot\
empty-Robot3-TeamThree.robot\
empty-Robot4-TeamThree.robot\
empty-Robot5-TeamThree.robot

%.robot: createrobot.sh
	"$(srcdir)/createrobot.sh" "$@" "$(frameworkdir)"

%.conf: createrobotconf.sh
	"$(srcdir)/createrobotconf.sh" "$@" $(loggingpriority)

rtb_team_LDADD= @ETR_SOCKET_LIBS@

rtb_team_SOURCES= initcomponents.h \
	common.h\
	stdnamespace.h \
	exceptions/confignotloadedexception.h \
	exceptions/configurationparserexception.h \
	exceptions/exceptionsnamespace.h \
	exceptions/ioexception.h \
	exceptions/keynotfoundexception.h \
	exceptions/parserexception.h \
	exceptions/resourcenotfoundexception.h \
	exceptions/rtbexception.h \
	exceptions/serverispresentexception.h \
	exceptions/strategyexception.h \
	gamecontrol/clientcoordinator.h \
	gamecontrol/clientgamecontroller.h \
	gamecontrol/coordinator.h \
	gamecontrol/coordinatorcomposite.h \
	gamecontrol/coordinatorleaf.h \
	gamecontrol/gamecontroller.h \
	gamecontrol/gamecontrolnamespace.h \
	gamecontrol/gameoptionsrepository.h \
	gamecontrol/servercoordinator.h \
	gamecontrol/servergamecontroller.h \
	io/clientcommunicator.h \
	io/clientconnection.h \
	io/connection.h \
	io/iofactory.h \
	io/ionamespace.h \
	io/localclientconnection.h \
	io/remoteclientconnection.h \
	io/rtbconnection.h \
	io/servercommunicator.h \
	io/serverconnection.h \
	io/unixclientcommunicator.h \
	io/unixinstream.h \
	io/unixinstreambuf.h \
	io/unixiofactory.h \
	io/unixoutstream.h \
	io/unixoutstreambuf.h \
	io/unixremoteclientconnection.h \
	io/unixrtbconnection.h \
	io/unixservercommunicator.h \
	io/unixserverconnection.h \
	log/logdriver.h \
	log/logger.h \
	log/lognamespace.h \
	log/nulllogdriver.h \
	log/rtblogdriver.h \
	log/sysloglogdriver.h \
	log/unixfilelogdriver.h \
	parser/argument_type.h \
	parser/configurationparser.h \
	parser/game_option_type.h \
	parser/message_from_robot_type.h \
	parser/message_to_robot_type.h \
	parser/object_type.h \
	parser/parsernamespace.h \
	parser/robot_option_type.h \
	parser/rtbparser.h \
	parser/rtbparseresult.h \
	parser/warning_type.h \
	rtbglobal/masterresourcecontrol.h \
	rtbglobal/pmap.h \
	rtbglobal/pvector.h \
	rtbglobal/rtbglobalnamespace.h \
	rtbglobal/rtbinit.h \
	strategies/clientspecificrepository.h \
	strategies/clientstate.h \
	strategies/clientstatedata.h \
	strategies/internalserverspecificrepository.h \
	strategies/serverstate.h \
	strategies/serverstatedata.h \
	strategies/sharedserverspecificrepository.h \
	strategies/specificrepository.h \
	strategies/state.h \
	strategies/strategiesnamespace.h \
	strategies/strategyfactory.h \
	bots/brotfrucht/bfclientbasicstate.h \
	bots/brotfrucht/bfclientshoothalfstate.h \
	bots/brotfrucht/bfclientshoottwicestate.h \
	bots/brotfrucht/bfclientspecificrepository.h \
	bots/brotfrucht/bfclientstateenum.h \
	bots/brotfrucht/bffactory.h \
	bots/brotfrucht/bfinternalserverspecificrepository.h \
	bots/brotfrucht/bfserverbasicstate.h \
	bots/brotfrucht/bfservernorotatestate.h \
	bots/brotfrucht/bfserverrotatestate.h \
	bots/brotfrucht/bfserverstateenum.h \
	bots/brotfrucht/bfsharedserverspecificrepository.h \
	bots/brotfrucht/brotfruchtnamespace.h \
	bots/empty/empty_client_basicstate.h \
	bots/empty/empty_client_derivatedstate.h \
	bots/empty/empty_clientspecificrepository.h \
	bots/empty/empty_factory.h \
	bots/empty/empty_internalserverspecificrepository.h \
	bots/empty/empty_namespace.h \
	bots/empty/empty_server_basicstate.h \
	bots/empty/empty_server_derivatedstate.h \
	bots/empty/empty_sharedserverspecificrepository.h \
	initcomponents.cpp \
	initexceptions.cpp \
	main.cpp \
	exceptions/confignotloadedexception.cpp \
	exceptions/configurationparserexception.cpp \
	exceptions/ioexception.cpp \
	exceptions/keynotfoundexception.cpp \
	exceptions/parserexception.cpp \
	exceptions/resourcenotfoundexception.cpp \
	exceptions/rtbexception.cpp \
	exceptions/serverispresentexception.cpp \
	exceptions/strategyexception.cpp \
	gamecontrol/clientcoordinator.cpp \
	gamecontrol/clientgamecontroller.cpp \
	gamecontrol/coordinatorcomposite.cpp \
	gamecontrol/coordinatorleaf.cpp \
	gamecontrol/servercoordinator.cpp \
	gamecontrol/servergamecontroller.cpp \
	io/localclientconnection.cpp \
	io/unixclientcommunicator.cpp \
	io/unixinstream.cpp \
	io/unixinstreambuf.cpp \
	io/unixiofactory.cpp \
	io/unixoutstream.cpp \
	io/unixoutstreambuf.cpp \
	io/unixremoteclientconnection.cpp \
	io/unixrtbconnection.cpp \
	io/unixservercommunicator.cpp \
	io/unixserverconnection.cpp \
	log/logger.cpp \
	log/rtblogdriver.cpp \
	log/sysloglogdriver.cpp \
	log/unixfilelogdriver.cpp \
	parser/configurationparser.cpp \
	parser/rtbparser.cpp \
	rtbglobal/masterresourcecontrol.cpp \
	rtbglobal/rtbinit.cpp \
	strategies/clientstate.cpp \
	strategies/clientstatedata.cpp \
	strategies/serverstate.cpp \
	strategies/serverstatedata.cpp \
	bots/brotfrucht/bfclientbasicstate.cpp \
	bots/brotfrucht/bfclientshoothalfstate.cpp \
	bots/brotfrucht/bfclientshoottwicestate.cpp \
	bots/brotfrucht/bfclientspecificrepository.cpp \
	bots/brotfrucht/bffactory.cpp \
	bots/brotfrucht/bfinternalserverspecificrepository.cpp \
	bots/brotfrucht/bfserverbasicstate.cpp \
	bots/brotfrucht/bfservernorotatestate.cpp \
	bots/brotfrucht/bfserverrotatestate.cpp \
	bots/brotfrucht/bfsharedserverspecificrepository.cpp \
	bots/empty/empty_client_basicstate.cpp \
	bots/empty/empty_client_derivatedstate.cpp \
	bots/empty/empty_clientspecificrepository.cpp \
	bots/empty/empty_factory.cpp \
	bots/empty/empty_internalserverspecificrepository.cpp \
	bots/empty/empty_server_basicstate.cpp \
	bots/empty/empty_server_derivatedstate.cpp \
	bots/empty/empty_sharedserverspecificrepository.cpp 
