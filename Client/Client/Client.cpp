#include "Client.h"
using namespace EXider;
Client::Client( boost::asio::io_service & io ) :
    m_io( io ), m_info( this ), m_nextTaskID( 0 ) {

}

void Client::run() {
    std::string comLine;
    CommandParser parser;
    boost::system::error_code er;
    while ( true ) {
        std::cout << "EXider >> ";
        std::getline( std::cin, comLine );
        parser.parseCommand( comLine );
        if ( parser.command() == "pc" ) {
            auto arg = parser.arguments();
            if ( arg.empty() ) {
                std::cout << "No arguments of command \"pc\"" << std::endl;
            }
            else if ( arg[ 0 ].argument == "list" ) {
                size_t Status = 0;
                bool badStatus = false;
                if ( !arg[ 0 ].parameters.empty() ) {
                    m_info.wrongParameter( arg[ 0 ].parameters[ 0 ] );
                    continue;
                }
                bool params = false;
                for ( int i = 1; i < arg.size(); ++i ) {
                    if ( !arg[ i ].parameters.empty() ) {
                        m_info.wrongParameter( arg[ i ].parameters[ 0 ] );
                        params = true;
                        break;
                    }
                    if ( arg[ i ].argument == "available" )
                        Status |= Available;
                    else if ( arg[ i ].argument == "busy" )
                        Status |= Busy;
                    else if ( arg[ i ].argument == "not-connected" )
                        Status |= NotConencted;
                    else {
                        m_info.wrongArgument( arg[ i ].argument );
                        badStatus = true;
                        break;
                    }
                }
                if ( !badStatus && !params ) {
                    m_info.pcList( Status == 0 ? Available : Status );
                }

            }
            else if ( arg[ 0 ].argument == "add" ) {
                std::vector<boost::asio::ip::address> pc_ip;
                bool badIP = false;
                for ( int i = 1; i < arg.size(); ++i ) {
                    if ( arg[ i ].argument == "ip" ) {
                        for ( int j = 0; j < arg[ i ].parameters.size(); ++j ) {
                            auto ip = boost::asio::ip::address::from_string( arg[ i ].parameters[ j ], er );
                            if ( er ) {
                                badIP = true;
                            }
                            else
                                pc_ip.push_back( ip );
                        }
                    }
                    else if ( arg[ i ].argument == "f" ) {
                        for ( int j = 0; j < arg[ i ].parameters.size(); ++j ) {
                            std::ifstream is( arg[ i ].parameters[ j ] );
                            if ( is.bad() ) {
                                badIP = true;
                                continue;
                            }
                            std::string strIP;
                            while ( is >> strIP ) {
                                auto ip = boost::asio::ip::address::from_string( strIP, er );
                                if ( er ) {
                                    badIP = true;
                                }
                                else
                                    pc_ip.push_back( ip );
                            }
                        }
                    }
                }
                if ( badIP ) {
                    m_info.warning( "Not all IPs will be processed. Some of them don't look like IP Addresses." );
                }
                addRemotePCs( pc_ip );
            }
            else if ( arg[ 0 ].argument == "delete" ) {
                std::vector<boost::asio::ip::address> pc_ip;
                bool badIP = false;
                for ( int i = 1; i < arg.size(); ++i ) {
                    if ( arg[ i ].argument == "ip" ) {
                        for ( int j = 0; j < arg[ i ].parameters.size(); ++j ) {
                            auto ip = boost::asio::ip::address::from_string( arg[ i ].parameters[ j ], er );
                            if ( er ) {
                                badIP = true;
                            }
                            else
                                pc_ip.push_back( ip );
                        }
                    }
                    else if ( arg[ i ].argument == "f" ) {
                        for ( int j = 0; j < arg[ i ].parameters.size(); ++j ) {
                            std::ifstream is( arg[ i ].parameters[ j ] );
                            if ( is.bad() ) {
                                badIP = true;
                                continue;
                            }
                            std::string strIP;
                            while ( is >> strIP ) {
                                auto ip = boost::asio::ip::address::from_string( strIP, er );
                                if ( er ) {
                                    badIP = true;
                                }
                                else
                                    pc_ip.push_back( ip );
                            }
                        }
                    }
                }
                if ( badIP ) {
                    m_info.warning( "Not all IPs will be processed. Some of them don't look like IP Addresses." );
                }
                deleteRemotePCs( pc_ip );
            }
            else if ( arg[ 0 ].argument == "save" ) {
                if ( arg.size() != 1 ) {
                    m_info.wrongArgument( arg[ 1 ].argument );
                }
                else if ( arg[ 0 ].parameters.size() != 1 ) {
                    m_info.print( "No file name found!" );
                }
                else
                    saveRemotePCs( arg[ 0 ].parameters[ 0 ] );
            }
            else if ( arg[ 0 ].argument == "help" ) {
                m_info.help( Information::CommandType::PC );
            }
            else {
                m_info.wrongArgument( arg[ 0 ].argument );
            }
        }
        else if ( parser.command() == "task" ) {
            auto arg = parser.arguments();
            if ( arg.empty() ) {
                std::cout << "No arguments of command \"task\"" << std::endl;
            }
            else if ( arg[ 0 ].argument == "status" ) {
                if ( arg.size() != 1 ) {
                    std::cout << "Invalid arguments." << std::endl;
                }
                if ( arg[ 0 ].parameters.empty() ) {
                    m_info.taskList();
                }
                else {
                    bool notAllProcessed = false;
                    std::vector<size_t> tIDs;
                    for ( int i = 0; i < arg[ 0 ].parameters.size(); ++i ) {
                        bool badNumber = false;
                        for ( int j = 0; j < arg[ 0 ].parameters[ i ].size(); ++j ) {
                            if ( arg[ 0 ].parameters[ i ][ j ] > '9' || arg[ 0 ].parameters[ i ][ j ] < '0' ) {
                                notAllProcessed = badNumber = true;
                                break;
                            }
                        }
                        if ( badNumber )
                            continue;
                        std::istringstream iss( arg[ 0 ].parameters[ i ] );
                        tIDs.push_back( 0 );
                        iss >> tIDs.back();
                    }
                    if ( notAllProcessed ) {
                        std::cout << "Warning! Not all Tasks will be processed." << std::endl;
                    }
                    m_info.taskInformation( tIDs );
                }
            }
            else if ( arg[ 0 ].argument == "start" ) {
                bool badArgument = false;
                bool moreThanOneName = false;
                int computersWillBeUsed = 1;
                std::string programArguments = "";
                std::string programPath = "";
                std::string taskName = "";
                bool withoutSending = false;
                if ( !arg[ 0 ].parameters.empty() )
                    programPath = arg[ 0 ].parameters[ 0 ];
                for ( int i = 1; i < arg.size(); ++i ) {
                    if ( ( arg[ i ].argument == "c" || arg[ i ].argument == "counter" ) && arg[ i ].parameters.size() == 1 ) {
                        computersWillBeUsed = std::atoi( arg[ i ].parameters[ 0 ].c_str() );
                    }
                    else if ( ( arg[ i ].argument == "p" || arg[ i ].argument == "path" ) && arg[ i ].parameters.size() == 1 ) {
                        programPath = arg[ i ].parameters[ 0 ];
                    }
                    else if ( ( arg[ i ].argument == "a" || arg[ i ].argument == "arg" ) && arg[ i ].parameters.size() == 1 ) {
                        programArguments = arg[ i ].parameters[ 0 ];
                    }
                    else if ( ( arg[ i ].argument == "ws" || arg[ i ].argument == "without-sending" ) && arg[ i ].parameters.empty() ) {
                        withoutSending = true;
                    }
                    else if ( ( arg[ i ].argument == "n" || arg[ i ].argument == "name" ) && arg[ i ].parameters.size() == 1 ) {
                        moreThanOneName |= !taskName.empty();
                        taskName = arg[ i ].parameters[ 0 ];
                    }
                    else {
                        m_info.wrongArgument( arg[ i ].argument );
                        badArgument = true;
                        break;
                    }
                }
                if ( badArgument )
                    continue;
                startTask( ( taskName.empty() ? std::string( "Task " ) + boost::lexical_cast<std::string>( m_nextTaskID ) : taskName ), m_nextTaskID, programPath, programArguments, computersWillBeUsed, withoutSending );
                ++m_nextTaskID;
            }
            else if ( arg[ 0 ].argument == "stop" ) {

            }
            else if ( arg[ 0 ].argument == "discard" ) {

            }
            else if ( arg[ 0 ].argument == "help" ) {
                m_info.help( Information::CommandType::Task );
            }
            else {
                m_info.wrongArgument( arg[ 0 ].argument );
            }
        }
        else if ( parser.command() == "help" ) {
            m_info.help();
        }
        else if ( parser.command() == "exit" ) {
            break;
        }
        else {
            m_info.wrongArgument( parser.command() );
        }
    }


}
void Client::addRemotePCs( const std::vector<boost::asio::ip::address>& IPs ) {
    for ( auto ip : IPs ) {
        boost::shared_ptr<RemotePC> pc( new RemotePC( m_io, ip ) );
        if ( m_notConnectedPC.find( pc ) != m_notConnectedPC.end() ||
             m_freePC.find( pc ) != m_freePC.end() ||
             m_busyPC.find( pc ) != m_busyPC.end() ) {	// Check, if the IP Address is already in one of the containers.
            m_info.warning( std::string( "Remote PC with IP Address: " ) + ip.to_string() + " has been already added." );
            continue;
        }
        if ( pc->connect() ) {
            m_freePC.insert( pc );
            m_info.print( std::string( "Remote PC with IP " ) + ip.to_string() + " was connected." );
        }
        else {
            m_notConnectedPC.insert( pc );
            m_info.print( std::string( "Remote PC with IP " ) + ip.to_string() + " wasn't connected." );
        }
    }
}
void Client::deleteRemotePCs( const std::vector<boost::asio::ip::address>& IPs ) {
    for ( size_t i = 0; i < IPs.size(); ++i ) {
        m_freePC.erase( boost::shared_ptr<RemotePC>( new RemotePC( m_io, IPs[ i ] ) ) );
        m_busyPC.erase( boost::shared_ptr<RemotePC>( new RemotePC( m_io, IPs[ i ] ) ) );
        m_notConnectedPC.erase( boost::shared_ptr<RemotePC>( new RemotePC( m_io, IPs[ i ] ) ) );
        std::cout << "Remote PC with IP " << IPs[ i ].to_string() << " was deleted.\n";
    }
}

void Client::saveRemotePCs( const std::string& fileToSave ) {
    std::ofstream os( fileToSave );
    if ( os.bad() ) {
        std::cout << "Can't open file " << fileToSave << std::endl;
    }
    else {
        for ( auto it : m_freePC )
            os << it->getIP().to_string() << " ";
        for ( auto it : m_busyPC )
            os << it->getIP().to_string() << " ";
    }
}

void Client::startTask( const std::string& taskName, size_t taskID, const std::string & filePath, const std::string & arguments, int computersToUse, bool withoutSendingProgram ) {
    if ( computersToUse > m_freePC.size() ) {
        m_info.error( std::string( "Not enough computers to process current task. " ) + boost::lexical_cast<std::string>( m_freePC.size() ) + " available." );
        return;
    }
    PCList listForTask;
    for ( int i = 0; i < computersToUse; ++i ) {
        listForTask.insert( *m_freePC.begin() );
        m_busyPC.insert( *m_freePC.begin() );
        m_freePC.erase( m_freePC.begin() );
    }
    std::vector<std::string> commands;

    //if ( !withoutSendingProgram ) {
    //    commands.push_back( boost::str( boost::format( "Download %1" ) % 1) % URL )
    //}

    commands.push_back( boost::str( boost::format( "Run %1% %2%" ) % filePath % arguments ) );
   
    m_tasks.push_back( boost::shared_ptr<Task>( new Task( m_io, listForTask, commands, taskName, taskID ) ) );

    m_info.print( "Task started." );
    // TODO
}
void Client::stopTask( size_t tID ) {
    for ( auto task : m_tasks ) {
        if ( task->getID() == tID ) {
            task->stop();
            m_info.print( "Task was stopped." );
        }
    }
}
void Client::discardTask( size_t tID ) {
    for ( int i = 0; i < m_tasks.size(); ++i ) {
        if ( m_tasks[ i ]->getID() == tID ) {
            m_tasks.erase( m_tasks.begin() + i );
            m_info.print( "Task was discarded." );
            break;
        }
    }
}
