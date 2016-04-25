#pragma once
#include "EXider.h"
namespace EXider {
    class CommandParser {
        std::string m_command;
        argList m_arguments;
    public:
        CommandParser( const std::string& command );
        CommandParser();
        void parseCommand( const std::string& command );
        const std::string& command() const;
        const argList& arguments() const;
    };
}