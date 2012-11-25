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
#include <sys/select.h>

#include <rsh/pastebin.h>
#include <rsh/debug.h>

/*----------------------------------------------------------------------------*\
* Defines
\*----------------------------------------------------------------------------*/
#if ! defined( PB_CLIENT_API_KEY )
#define PB_CLIENT_API_KEY "000000000000000000000000000000000"
#endif 
#define DEFAULT_STDIN_BUFFER_SIZE 1048576

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

static int isReady()
{
	fd_set s_read;
	struct timeval timeout;

	FD_ZERO( &s_read );
	FD_SET( fileno(stdin), &s_read );

	timeout.tv_sec = 0;
	timeout.tv_usec = 1;

	return select( fileno(stdin)+1, &s_read, NULL, NULL, &timeout ) == 1 ? true:false;
}

static bool pastestdin()
{
	if( !isReady(stdin) ) return false;

	size_t csz = 1;
	char* string = malloc( sizeof(char)*DEFAULT_STDIN_BUFFER_SIZE );
	*string = '\0';
	char buffer[DEFAULT_STDIN_BUFFER_SIZE];


	while( fgets( buffer, DEFAULT_STDIN_BUFFER_SIZE, stdin ) )
	{
		char* old = string;
		csz += strlen( buffer );
		string = realloc( string, csz );
		if( string == NULL )
		{
			fprintf( stderr, "Can't store stdin in memory!\n" );
			free( old );
			return false;
		}
		strcat( string, buffer );
	}
	if( ferror( stdin ) )
	{
		free( string );
		fprintf( stderr, "Can't read from stdin!\n" );
		return false;
	}

	printf( "%s\n", pb_newPaste( pb, string, strlen(string) ) );

	return true;
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

		if( fsz == 0 ) 
		{
			fclose( file );
			return true; // they wanted an empty file. Doesn't mean we should do anything.
		}

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
		response = pb_deletePaste( pb, id );

	if( !response ) return false;
	else return true;
}

void parseopts( int argc, char** argv )
{
	int c;
	int options_index = 0;
	char* username;
	char* password;
	int user_list_size = 0;
	pb_status returnStatus;

	while( true )
	{
		if( (c = getopt_long( argc, argv, OPTS, long_options, &options_index )) == -1 ) break;

		switch( c )
		{
			case 0:
				if( long_options[options_index].flag != 0 )	
			break;
			case 'h': // ask for help
				printUsage( argv[0] );
			break;

			case 's': // someone set the language
				debugf( "Setting language!\n" );
				pb_setWithOptions( pb, PB_SYNTAX, pb_getSyntax( optarg ) );
			break;

			case 'r':
				if( gb_deleteFlag  )
				{
					fprintf( stderr, "Can't set both the delete and retrieve flags!\n" );
					exit( 4 );
				}

				gb_retrieveFlag = true;
			break;

			case 'd':
				if( gb_retrieveFlag )
				{
					fprintf( stderr, "Can't set both the delete and retrieve flags!\n" );
					exit( 4 );
				}

				gb_deleteFlag = true;
			break;

			case 'e':
				for( int i = 0; i < EXPIRE_LIST_MAX; i++ )
					if( !strcmp( pb_expirestring[i], optarg ) )
						pb_setWithOptions( pb, PB_EXPIRE_DATE, i );

				// TODO: find a way to double-check that they got this entered.
			break;

			case 'n': // naming the paste
				pb_setWithOptions( pb, PB_PASTE_NAME, optarg );
			break;

			case 'l': // list all syntaxes
				printf( "List of supported languages:\n" );
				for( int i = 0; i < SYN_LIST_MAX; i++ )
					printf( "\t%s: %s\n", pb_syntaxstring[i], pb_syntaxstringdesc[i] );
				exit( 0 );
			break;

			case 't': // get trending pastes
				pb_getTrendingPastes( pb );	
				return;
			break;

			case 'u': // give a username
				username = optarg;
				gb_userkeyFlag = true;
			break;

			case 'p':
				password = optarg;
				gb_passwordFlag = true;
			break;

			case 'k':
				pb_setWithOptions( pb, PB_USER_KEY, optarg );
			break;

			case 'a':
				gb_userlistFlag = true;
				user_list_size = atoi( optarg );
			break;

			case '?': // something is wrong; print usage.
				printUsage( argv[0] );
				exit( 2 );
			break;

			default:
				printUsage( argv[0] );
				exit( 2 );
			break;
		}
	}
	
	// Post-main arguments but pre-action cleanup
	// did they, in any way, request a user key?
	if( gb_userkeyFlag && !gb_passwordFlag )
	{
		password = getpass( "Password: " );
		if( password )
			gb_passwordFlag = true;
	}
	if( gb_userkeyFlag && gb_passwordFlag )
	{
		returnStatus = pb_getUserSessionKey( pb, username, password );
		if( returnStatus )
		{
			fprintf( stderr, "%s\n", pb_statusString[returnStatus] );
			exit( returnStatus );
		}
	}

	// handle remaining args
	while( optind <= argc )
	{
		if( optind == argc ) return;
		// they want to retrieve urls
		if( gb_retrieveFlag )
		{
			printf( "Getting paste %s\n", getIDFromURL( argv[optind] ) );
			getPaste( argv[optind] );
			optind++;
			continue;
		}
		
		if( gb_deleteFlag )
		{
			printf( "Deleting paste %s\n", getIDFromURL( argv[optind] ) );
			deletePaste( argv[optind] );
			optind++;
			continue;
		}
		
		pasteFile( argv[optind] );
		optind++;
	}

	if( optind >= argc && gb_userkeyFlag )
	{
		// they just want their key
		if( !gb_userlistFlag )
			printf( "%s\n", pb->userkey );
		else // they want their pastes listed
		{
			// TODO: error checking
			printf( "%s\n", pb_getUserPastes( pb, user_list_size ) );
		}
	}
	else if( optind == argc )
	{
		// well, if we got here, that means there's no options, we didn't 
		// retrieve anything, and we didn't delete anything.
		// I guess we want stdin?
		if( pastestdin() ) return;
		
		// I guess they didn't get stdin.. let's give them help?
		printUsage( argv[0] );
	}
	


}

/*----------------------------------------------------------------------------*\
* main
\*----------------------------------------------------------------------------*/

int main(int argc, char *argv[])
{
	/*
	* init
	*/
	pb                   = pb_newPastebin();
	pb_setWithOptions( pb, PB_DEV_KEY, PB_CLIENT_API_KEY );
	pb_setWithOptions( pb, PB_SYNTAX, SYN_LANG_NONE );
	pb_setWithOptions( pb, PB_USER_KEY, "");
	
	gb_retrieveFlag      = false;
	gb_userkeyFlag       = false;
	gb_passwordFlag      = false;
	gb_deleteFlag        = false;
	gb_userlistFlag      = false;
	gb_stdinFlag         = false;
	gb_pastepublicFlag   = true;
	gb_pasteunlistedFlag = false;

	if( argc==1 )
	{
		printUsage( argv[0] );
		return 0;
	}

	parseopts( argc, argv );

	return 0;
}

#ifdef __cplusplus
}
#endif
