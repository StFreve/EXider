#pragma once
#define _CRT_SECURE_NO_WARNINGS
//Standard
#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <algorithm>
#include <vector>
#include <queue>
#include <memory>
// Boost
#include <boost/filesystem.hpp>
#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <boost/timer.hpp>
#include <boost/process.hpp>
#include <boost/filesystem.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/format.hpp>

// Poco
#include <Poco/Net/FTPClientSession.h>
#include <Poco/StreamCopier.h>

namespace EXider {
	// Variables, Enums and structs
	const size_t PORT = 8008;
	struct EXiderArgument {
		std::string argument;
		std::vector<std::string> parameters;
	};
    typedef std::vector<EXiderArgument> argList;
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
    static const char slash = '\\';
#else
    static const char slash = '/';
#endif

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