#pragma once
#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <set>
#include <list>
#include <exception>

// Boost
#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <boost/regex.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/format.hpp>
#include <boost/filesystem.hpp>
// Poco
#include <Poco/Net/FTPClientSession.h>
#include <Poco/StreamCopier.h>
// LibSSH
#define LIBSSH_STATIC
#include <libssh\libssh.h>

namespace EXider {
    const size_t EXIDER_PORT = 8008;
    enum rpcStatus {	// RemotePC Statuses
        NotConencted = 1,
        Available = 2,
        Busy = 4,
        ConnectionError = 8
    };

    struct EXiderArgument {
        std::string argument;
        std::vector<std::string> parameters;
    };



    class CommandParser;
    class Logger;
    class Information;	// Different information printer
    class FtpClient;	// FTP Client to upload and download files
    class RemotePC;		// Remote PC controller (Server controller) 
    class Task;			// Task controller
    class Client;		// Main class producing all activity


    struct RPComparator;
    typedef std::set<boost::shared_ptr<RemotePC>, RPComparator> PCList;
    typedef std::vector<EXiderArgument> argList;
}
#include "CommandParser.h"
#include "Logger.h"
#include "FtpClient.h"
#include "Information.h"
#include "RemotePC.h"
#include "RPComparator.h"
#include "Task.h"
#include "Client.h"
#include "SSH.h"