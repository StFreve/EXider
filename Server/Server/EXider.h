#pragma once
#define _CRT_SECURE_NO_WARNINGS
//Standard
#include <iostream>
#include <sstream>
#include <Fstream>
#include <string>
#include <algorithm>
#include <vector>
#include <queue>
#include <memory>
//Boost
#include <boost\filesystem.hpp>
#include <boost\asio.hpp>
#include <boost\thread.hpp>
#include <boost\timer.hpp>
#include <boost\process.hpp>
#include <boost\filesystem.hpp>
#include <boost\lexical_cast.hpp>
namespace EXider {
	// Variables, Enums and structs
	const size_t PORT = 8008;
	struct EXiderArgument {
		std::string argument;
		std::vector<std::string> parameters;
	};
    typedef std::vector<EXiderArgument> argList;


	// Classes
	class CommandParser;
	class FtpClient;
	class Program;
	class ProgramExecutor;
	class Server;

}
#include "CommandParser.h"
#include "FtpClient.h"
#include "Program.h"
#include "ProgramExecutor.h"
#include "Server.h"