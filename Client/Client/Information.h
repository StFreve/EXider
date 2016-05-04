#pragma once
#include "EXider.h"
namespace EXider {
	class Information {
		const Client* m_client;

	public:
		enum CommandType {
			PC = 1,
			Task = 2,
			Usage = 4
		};
		Information( const Client* client );
		
		void taskList();                                            // List of tasks with results
		void taskInformation(const std::set<boost::shared_ptr<EXider::Task> >& taskList );          // Full information about tasks

		void pcList( size_t Status = Available);                    // List of RemotePCs

		void help(size_t CommandTypes = (PC | Task | Usage) );      // Exider's commands instructions

		void error( const std::string& message ) const;	            // Error message
		void warning( const std::string& message ) const;           // Warning message
		void wrongArgument( const std::string& arg ) const;	        // Message of incorrect argument
		void wrongParameter( const std::string& param ) const;      // Message of incorrect parameter
		void wrongCommand( const std::string& command ) const;      // Message of incorrect command
		void print( const std::string& message ) const;             // Print message
	};
}