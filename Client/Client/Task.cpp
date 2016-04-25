#include "EXider.h"
using namespace EXider;
Task::Task( boost::asio::io_service& io, const PCList& workingPCs, const std::string downloadURL, const std::string executeCommand, const std::string& taskName, int tID, bool autoFree ) :
    m_workingPCs( workingPCs ), m_downloadsFiles( true ), m_result( workingPCs.size(), "-" ),
    m_downloadURL( downloadURL ), m_executeCommand( executeCommand ), m_autoFree( autoFree ), m_io( io ),
    m_thread( boost::bind( &Task::run, this ) ), m_taskName( taskName ), m_tID( tID ) {
    size_t pcID = 0;
    for ( auto pc : m_workingPCs ) {
        pc->setID( pcID++ );
        pc->setCallBackFunction( boost::bind( &Task::handler, this, _1, _2 ) );
    }
}
Task::Task( boost::asio::io_service& io, const PCList& workingPCs, const std::string executeCommand, const std::string& taskName, int tID, bool autoFree ) :
    m_workingPCs( workingPCs ), m_downloadsFiles( false ), m_result( workingPCs.size(), "-" ),
    m_executeCommand( executeCommand ), m_autoFree( autoFree ), m_io( io ),
    m_thread( boost::bind( &Task::run, this ) ), m_taskName( taskName ), m_tID( tID ) {
    size_t pcID = 0;
    for ( auto pc : m_workingPCs ) {
        pc->setID( pcID++ );
        pc->setCallBackFunction( boost::bind( &Task::handler, this, _1, _2 ) );
    }
}
void Task::run() {
    char request[ 256 ];

    for ( auto pc : m_workingPCs ) {
        if ( m_downloadsFiles )
            sprintf( request, "Download %s", m_downloadURL.c_str() );
        else
            sprintf( request, "Run %s -pcid %d", m_executeCommand.c_str(), pc->getID() );
        pc->sendRequest( request );
    }
    m_io.run();
}
void Task::stop() {
    for ( auto pc : m_workingPCs ) {
        pc->sendRequest( "Stop" );
    }
}
const std::string Task::getResult( const std::string& delimeter ) const {
    boost::recursive_mutex::scoped_lock lock( m_mutexForResult );
    std::string sResult = "";
    for ( int i = 0; i < m_result.size(); ++i )
        sResult += m_result[ i ] + ( i != m_result.size() - 1 ? delimeter : "" );
    return sResult;
}
const std::string Task::getInfromation() const {
    std::string sResult = std::string( "Task: " ) + getName() + "\nID: " + boost::lexical_cast<std::string>( getID() ) + "\nResult: " + getResult() + "\nPCs in Task:";
    int countInLine = 3;										// IPs in one line
    int counter = 0;
    for ( auto pc : m_workingPCs ) {
        if ( counter == countInLine ) {
            sResult += "\n";
            counter = 0;
        }
        else if ( counter > 0 ) {
            sResult += "\t";
        }
        sResult += pc->getIP().to_string();
        ++counter;
    }
    return sResult;
}

void Task::handler( boost::shared_ptr<RemotePC> fromPC, const std::string result ) {
    std::istringstream iss( result );
    std::string command;
    iss >> command;
    if ( command == "Reading" ) {
        iss >> command;
        if ( command == "OK" )
            return;
    }
    else if ( command == "Writing" ) {
        iss >> command;
        if ( command == "OK" ) {
            fromPC->readRequest();
        }
    }
    else if ( command == "Connecting" ) {
        iss >> command;
        if ( command == "OK" )
            return;
    }
    else if ( command == "Downloading" ) {
        iss >> command;
        if ( command == "OK" ) {
            char request[ 256 ];
            sprintf( request, "Run %s -pcid %d", m_executeCommand, fromPC->getID() );
            fromPC->sendRequest( request );
        }
    }
    else if ( command == "Result" ) {
        std::getline( iss, command );
        command = command.substr( command.find_first_not_of( ' ' ) );
        boost::recursive_mutex::scoped_lock lock( m_mutexForResult );
        m_result[ fromPC->getID() ] = command;
    }
}
const std::string& Task::getName() const {
    return m_taskName;
}
const size_t Task::getID() const {
    return m_tID;
}
