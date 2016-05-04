#include "EXider.h"
using namespace EXider;
Program::Program( const std::string& fileName, const std::string& path, const std::string& arguments ) :
    m_fileName( fileName ), m_path( path.empty() ? boost::filesystem::current_path().string() + "\\" : path ) {
    deleteWhitespaces( m_fileName );
    deleteWhitespaces( m_path );
    if ( m_path.back() != '/' ) {
        m_path.push_back( '/' );
    }
    parseArguments( arguments );

}
Program::Program( const std::string& fileName, const std::string& path ) : Program( fileName, path, "" ) {

}
Program::Program( const std::string& fullPathWithArguments ) { // TODO

}
const std::string& Program::getFileName() const {
    return m_fileName;
}
const std::string& Program::getPath() const {
    return m_path;
}
const std::vector<std::string>& Program::getArguments() const {
    return m_arguments;

}
const std::string Program::getArgumentsString() const {
    std::string result;
    for ( int i = 0; i < m_arguments.size(); ++i ) {
        result += m_arguments[ i ] + " ";
    }
    return result;
}
const std::string Program::getFullPath() const {
    return getPath() + getFileName();
}
const std::string Program::getFullPathWithArguments() const {

    return getFullPath() + " " + getArgumentsString();

}
void Program::deleteWhitespaces( std::string & str ) {
    if ( !str.empty() )
        str = str.substr( str.find_first_not_of( ' ' ) );
}
void Program::parseArguments( const std::string& argv ) {
    CommandParser parser( argv );
    const argList& arguments( parser.arguments() );
    m_arguments.push_back( getFileName() );                               // Add File Name as a first argument
    for ( int i = 0; i < arguments.size(); ++i ) {
        m_arguments.push_back( "-" );
        m_arguments.back() += arguments[ i ].argument;                    // Add Argument

        for ( int j = 0; j < arguments[ i ].parameters.size(); ++j )
            m_arguments.push_back( arguments[ i ].parameters[ j ] );      // Add Parameters
    }
}
