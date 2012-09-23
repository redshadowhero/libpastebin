/*
This program is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#define  __USE_XOPEN
#include <unistd.h>
#undef   __USE_XOPEN
#include <string.h>
#include <ctype.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <getopt.h>

#include <rsh/pastebin.h>
#include <rsh/debug.h>

/*----------------------------------------------------------------------------*\
* Defines
\*----------------------------------------------------------------------------*/
#if ! defined( PB_CLIENT_API_KEY )
#define PB_CLIENT_API_KEY "000000000000000000000000000000000"
#endif 
#define DEFAULT_STDIN_BUFFER_SIZE 1024

#define OPTS "s:n:u:p:k:a:e:litrdh"

#define USAGE \
	"Usage: %s [OPTION...] file1 [file2...]\n" \
	"\t-u, --user=USERNAME\t\tGive a username to authenticate as\n" \
	"\t-p, --pass=PASSWORD\t\tGive a password to authenticate as\n" \
	"\t-k, --key=USERKEY\t\tUse a user key to interact with pastebin\n" \
	"\t-s, --syntax=SYNTAX\t\tUse specified syntax\n" \
	"\t-e, --expire=WHEN\t\tHow long the paste should last.\n" \
	"\t\t\t\t\tAcceptable values are:\n" \
	"\t\t\t\t\tNever: 'N'\n" \
	"\t\t\t\t\t10 minutes: '10M'\n" \
	"\t\t\t\t\t1 hour: '1H'\n" \
	"\t\t\t\t\t1 day: '1D'\n" \
	"\t\t\t\t\t1 month: '1M'\n" \
	"\t-n, --name=NAME\t\t\tGive a name to the paste\n" \
	"\t-l, --list\t\t\tList all supported syntaxes\n" \
	"\t-t, --trending\t\t\tGet trending pastes\n" \
	"\t-i, --stdin\t\t\tUse stdin for input\n" \
	"\t-a, --paste-list=AMOUNT\t\tLists AMOUNT pastes by user. Pass 0 if you want the default amount. Need username or key to use.\n" \
	"\t-r, --retrieve\t\t\tRetrieve the paste and dump it to stdout\n" \
	"\t-d, --delete\t\t\tDelete pastes by ID. Need username or user key to delete.\n" \
	"\t-h, --help\t\t\tPrint this message\n"

/*----------------------------------------------------------------------------*\
* Globals 
\*----------------------------------------------------------------------------*/

pastebin* pb;
bool gb_retrieveFlag;
bool gb_userkeyFlag;
bool gb_passwordFlag;
bool gb_deleteFlag;
bool gb_userlistFlag;
bool gb_stdinFlag;
bool gb_pastepublicFlag; // true means public, false means private
bool gb_pasteunlistedFlag; // true means unlisted, false defers to gb_pastepublicFlag

struct option long_options[] =
{
	{ "syntax",      required_argument,  0,  't' },
	{ "user",        required_argument,  0,  'u' },
	{ "pass",        required_argument,  0,  'p' },
	{ "key",         required_argument,  0,  'k' },
	{ "retrieve",    no_argument,        0,  'r' },
	{ "expire",      required_argument,  0,  'e' },
	{ "trending",    no_argument,        0,  't' },
	{ "paste-list",  required_argument,  0,  'a' },
	{ "delete",      no_argument,        0,  'd' },
	{ "name",        required_argument,  0,  'n' },
	{ "help",        no_argument,        0,  'h' },
	{ "list",        no_argument,        0,  'l' },
	{ 0,             0,                  0,   0  }
};

/*----------------------------------------------------------------------------*\
* Functions
\*----------------------------------------------------------------------------*/

static void printUsage( char* command )
{
	printf( USAGE, command );
}

static char* getIDFromURL( char* url )
/*
* Should parse an id from a pastebin url
* "http://pastebin.com/x9Ddu6mj" should return "x9Ddu6mj"
*/
{
	debugf( "Entering function\n" );
	size_t length = strlen( url );
	for( int i = length-1; i >= 0; i-- )
		if( !isalnum( url[i] ) )
		{
			debugf( "Found ID, returning %s\n", &url[i+1] );
			return (&url[i+1]);
		}
	return NULL;
}

static bool pastestdin()
{
	return false;
}

static bool pasteFile( char* _fn )
{
	FILE* file;
	unsigned int fsz;
	char* string;
	char* response;

	if( (file = fopen( _fn, "r" )) )  // be optimistic that this will work
	{
		fseek( file, 0, SEEK_END );
		fsz = ftell( file );
		fseek( file, 0, SEEK_SET );

		if( fsz == 0 ) true; // they wanted an empty file. Doesn't mean we should do anything.

		string = malloc( sizeof(char)*fsz+1 );
		fread( string, sizeof(char), fsz, file );
		string[fsz] = '\0';

		// paste the string and then check the status
		response = pb_newPaste( pb, string, fsz+1 );
			
		switch( pb->lastStatus )
		{
			case STATUS_OKAY:
			default:
				printf( "%s \n", response );
				if( string ) free( string );
				if( response ) free( response );
				return true;
			break;
			case STATUS_INVALID_API_OPTION:
				fprintf( stderr, "Warning, an error that never should have occurred has occured\n" );
			break;
			case STATUS_INVALID_API_DEV_KEY:
				fprintf( stderr, "Warning, dev key is invalid!\n" );
			break;
			case STATUS_IP_BLOCKED:
				fprintf( stderr, "Warning, ip is blocked!\n" );
			break;
			case STATUS_MAX_UNLISTED_PASTES:
				fprintf( stderr, "You can't create anymore unlisted pastes!\n" );
			break;
			case STATUS_MAX_PRIVATE_PASTES:
				fprintf( stderr, "You can't create anymore private pastes!\n" );
			break;
			case STATUS_EMPTY_PASTE:
				fprintf( stderr, "Your paste was empty!\n" );
			break;
			case STATUS_MAX_PASTE_FILE_SIZE:
				fprintf( stderr, "Your paste was too large!\n" );
			break;
			case STATUS_INVALID_EXPIRE_DATE:
				 // should never actually get here, because of how the API handles expires
				fprintf( stderr, "Invalid expire date!\n" );
			break;
			case STATUS_INVALID_PASTE_PRIVATE: // ditto
				fprintf( stderr, "Invalid paste private state!\n" );
			break;
			case STATUS_MAX_PASTE_PER_DAY:
				fprintf( stderr, "Max pastes per day reached!\n" );
			break;
			case STATUS_INVALID_PASTE_FORMAT: // also ditto again
				fprintf( stderr, "Invalid paste format! Use --list to see valid paste formats!\n" );
			break;
		}
		if( string ) free( string );
		if( response ) free( response );

		return false; // didn't get STATUS_OKAY
	}
	else // be sad
	{
		fprintf(stderr, "Error: can't open file `%s'\n", _fn );
		return false;
	}
}

static bool getPaste( char* id )
{
	char* response;
	if( !strncmp( id, "http:", 5 ) || !strncmp( id, "https:", 6 ) )
	{
		// don't free this; it's just a pointer to the id in the old string
		char* newid = getIDFromURL( id );
		response = pb_getRawPaste( newid );
	}
	else // id provided, or so we hope.
		response = pb_getRawPaste( id );
	
	if( response == NULL )
		return false;
	else
	{
		printf( "%s \n", response );
		if( response ) free( response );
		return true;
	}

	return false;
}

static bool deletePaste( char* id )
{
	pb_status response;
	if( !strncmp( id, "http:", 5 ) || !strncmp( id, "https:", 6 ) )
	{
		char* newid = getIDFromURL( id );
		response = pb_deletePaste( pb, newid );
	}
	else // id provided
		response = pb_deletePaste( pb, newid );

	if( !response ) return false;
	else return true;
}

void parseopts( int argc, const char** argv )
{
	int c;
	int options_index = 0;

	while( true )
	{
		if( (c = getopt_long( argc, argv, OPTS, long_options, &options_index )) == -1 ) break;

		switch( c )
		{

		}
	}
}

/*----------------------------------------------------------------------------*\
* main
\*----------------------------------------------------------------------------*/

int main(int argc, const char *argv[])
{
	/*
	* init
	*/
	pb                   = pb_newPastebin();
	gb_retrieveFlag      = false;
	gb_userkeyFlag       = false;
	gb_passwordFlag      = false;
	gb_deleteFlag        = false;
	gb_userlistFlag      = false;
	gb_stdinFlag         = false;
	gb_pastepublicFlag   = true;
	gb_pasteunlistedFlag = false;

	return 0;
}

#ifdef __cplusplus
}
#endif
