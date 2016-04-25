#include "EXider.h"
using namespace EXider;
Information::Information( const Client* client ) :
    m_client( client ) {

}

void Information::taskList() {
    for ( auto task : m_client->m_tasks ) {
        print( std::string( "[" ) + boost::lexical_cast<std::string>( task->getID() ) + "] " + task->getName() + " result: " + task->getResult() );
    }
}
void Information::taskInformation( std::vector<size_t> tIDs ) {
    if ( tIDs.empty() ) {
        return;
    }
    std::sort( tIDs.begin(), tIDs.end() );
    size_t tIDi = 0;
    for ( auto task : m_client->m_tasks ) {
        if ( task->getID() == tIDs[ tIDi ] ) {
            print( task->getInfromation() );
            if ( ++tIDi >= tIDs.size() )
                break;
        }
        else if ( task->getID() > tIDs[ tIDi ] ) {
            warning( std::string( "No task with ID " ) + boost::lexical_cast<std::string>( tIDs[ tIDi ] ) + " was found." );
            if ( ++tIDi >= tIDs.size() )
                break;
        }
    }
}

void Information::pcList( size_t Status ) {
    const int countInLine = 3;
    if ( Status & Available ) {
        std::cout << "Available PCs: " << std::endl;
        int counter = 0;
        for ( auto pc : m_client->m_freePC ) {
            if ( counter == countInLine ) {
                std::cout << std::endl;
                counter = 0;
            }
            else if ( counter > 0 ) {
                std::cout << "\t";
            }
            std::cout << pc->getIP().to_string();
            ++counter;
        }
        std::cout << std::endl;
    }
    if ( Status & Busy ) {
        std::cout << "Busy PCs: " << std::endl;
        int counter = 0;
        for ( auto pc : m_client->m_busyPC ) {
            if ( counter == countInLine ) {
                std::cout << std::endl;
                counter = 0;
            }
            else if ( counter > 0 ) {
                std::cout << "\t";
            }
            std::cout << pc->getIP().to_string();
            ++counter;
        }
        std::cout << std::endl;
    }
    if ( Status & NotConencted ) {
        std::cout << "Not connected PCs: " << std::endl;
        int counter = 0;
        for ( auto pc : m_client->m_notConnectedPC ) {
            if ( counter == countInLine ) {
                std::cout << std::endl;
                counter = 0;
            }
            else if ( counter > 0 ) {
                std::cout << "\t";
            }
            std::cout << pc->getIP().to_string();
            ++counter;
        }
        std::cout << std::endl;
    }
}

void Information::help( size_t CommandTypes ) {
    if ( CommandTypes & Usage ) {
        print(
            "EXider Information\n"
            "Usage (pc/task/help) (arguments) [parameters]\n"
            );
    }
    if ( CommandTypes & PC ) {
        print(
            "pc -(add/delete/save/list) [arguments]\n"
            "This command controls RemotePCs.\n"
            "\n"
            "-add                     - Adds new RemotePCs.\n"
            "   -ip IP_List           - Adds new RemotePCs by IP Address.\n"
            "   -f FileName_List      - Adds new RemotePCs by IPs from files.\n"
            "\n"
            "-delete                  - Deletes RemotePCs.\n"
            "   -ip IP_List           - Delete RemotePCs by their IP Address.\n"
            "   -f FileName_List      - Delete RemotePCs by IPs from files.\n"
            "\n"
            "-save FileName           - Saves RemotePCs to file.\n"
            "\n"
            "-list                    - List of RemotePCs. If used without arguments will print only Available RemotePCs.\n"
            "   -available            - Prints available RemotePC's IP Addresses\n"
            "   -busy                 - Prints busy RemotePC's IP Addresses\n"
            "   -not-connected        - Prints not connected RemotePC's IP Addresses\n"
            "\n"
            "-help                    - Information about this command\n"
            );
    }
}
void Information::error( const std::string& message ) const {
    print( std::string( "Error! " ) + message );
}
void Information::warning( const std::string& message ) const {
    print( std::string( "Warning! " ) + message );
}
void Information::wrongArgument( const std::string& arg ) const {
    print( std::string( "Wrong argument name \"" ) + arg + "\"." );
}
void Information::wrongParameter( const std::string& param ) const {
    print( std::string( "Wrong parameter \"" ) + param + "\"." );
}
void Information::wrongCommand( const std::string& command ) const {
    print( std::string( "Wrong command \"" ) + command + "\"." );
}
void Information::print( const std::string& message ) const {
    std::cout << message << std::endl;
}
