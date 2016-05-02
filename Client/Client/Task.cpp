#include "EXider.h"
using namespace EXider;
Task::Task( boost::asio::io_service& io, const PCList& workingPCs, const std::vector<std::string>& commands, const std::string& taskName, int tID, bool autoFree )
    : m_io( io )
    , m_mutexForResult()
    , m_taskName( taskName )
    , m_tID( tID )
    , m_workingPCs( workingPCs )
    , m_result( workingPCs.size(), "-" )
    , m_workingStep( workingPCs.size(), 0 )
    , m_commands( commands )
    , m_autoFree( autoFree )
    , m_thread() {
    size_t pcID = 0;
    for ( auto pc : m_workingPCs ) {
        pc->setID( pcID++ );
        pc->setCallBackFunction( boost::bind( &Task::handler, this, _1, _2 ) );
    }
}

void Task::run() {
    char request[ 256 ];

    for ( auto pc : m_workingPCs ) {
        sendNextCommand( pc );
    }
    m_io.run();
}
void Task::start() {
    if ( m_thread.get() == nullptr ) {  // Creating thread, if it wasn't already created.
        m_thread.reset( new  boost::thread( boost::bind( &Task::run, this ) ) );
    }
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
    if ( command == "Writing" ) {
        iss >> command;
        if ( command == "OK" ) {

        }
    }
    if ( command == "Downloading" ) {                // Here commands, which only say, that they worked correctly
        iss >> command;
        if ( command == "OK" )
            ++m_workingStep[ fromPC->getID() ];
        else if ( command == "Error" ) {
            // TODO
        }
    }
    else if ( command == "Result" ) {
        ++m_workingStep[ fromPC->getID() ];

        std::getline( iss, command );
        command = command.substr( command.find_first_not_of( ' ' ) );
        boost::recursive_mutex::scoped_lock lock( m_mutexForResult );
        m_result[ fromPC->getID() ] = command;
    }
    sendNextCommand( fromPC );
}
const std::string& Task::getName() const {
    return m_taskName;
}
const size_t Task::getID() const {
    return m_tID;
}
const std::string Task::getFullName() const {
    return str( boost::format( "[%1%] %2%" ) % m_tID % m_taskName );
}
const PCList Task::getPCList() const {
    return m_workingPCs;
}
void Task::sendNextCommand( const boost::shared_ptr<RemotePC>& pc ) {
    const size_t pcID = pc->getID();

    assert( pcID < m_workingPCs.size() );
    assert( m_workingStep[ pcID ] <= m_commands.size() );
    if ( m_workingStep[ pcID ] < m_commands.size() ) {
        pc->sendRequest( m_commands[ m_workingStep[ pc->getID() ] ] );
        pc->readRequest();
    }
    else if ( m_autoFree ) {
        m_workingPCs.erase( pc );
    }
}