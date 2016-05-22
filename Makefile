BoostDir=/home/stfreve/Desktop/dev/boost_1_59_0/
BoostLibraries=-lboost_regex
Sources=example.cpp
ServerSources=CommandParser.cpp Program.cpp Server.cpp FtpClient.cpp ProgramExecutor.cpp Source.cpp
ServerDir=Server/Server
Exec=example

all: 
	g++ -I $(BoostDir) $(Sources) -L$(BoostDir)/stage/lib $(BoostLibraries) -o $(Exec)
