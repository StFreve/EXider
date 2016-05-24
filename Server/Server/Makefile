BoostDir=/home/stfreve/Desktop/dev/boost_1_59_0
BoostLibraries=-lboost_regex -lboost_date_time -lboost_filesystem -lboost_system -lboost_thread -lboost_serialization -lboost_wserialization
PocoDir=/home/stfreve/Desktop/dev/poco-1.7.3
PocoLibraries=-lCppUnit -lPocoFoundationd -lPocoNet -lPocoUtild -lCppUnitd -lPocoJSON -lPocoNetd  -lPocoXML -lPocoFoundation -lPocoJSONd -lPocoUtil  -lPocoXMLd
Sources=CommandParser.cpp Program.cpp Server.cpp FtpClient.cpp ProgramExecutor.cpp Source.cpp
Exec=exider_server

all: 
	g++ -std=c++11 -pthread -static -I $(BoostDir)/ -I $(PocoDir)/include $(Sources) -L$(BoostDir)/stage/lib  -L$(PocoDir)/lib/Linux/i686 -o $(Exec) $(BoostLibraries)  $(PocoLibraries)
