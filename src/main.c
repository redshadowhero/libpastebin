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
#include <rsh/pastebin_syntax.h>
#include <rsh/debug.h>

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

/* globals */

#define  no_settings     0x0  // 00000000
#define  retrieve_flag   0x1  // 00000001
#define  user_key_get    0x2  // 00000010
#define  password_set    0x4  // 00000100
#define  delete_flag     0x8  // 00001000
#define  user_list_get   0x10 // 00010000
#define  stdin_use       0x20 // 00100000

pastebin* pb;

typedef uint8_t byte;

/*
* Bits
* [0] retrieve flag
*/
byte settings = no_settings;

#define is_set( bite, set ) ( bite & set )
#define set( bite, set ) ( bite = (is_set(bite,set) ? bite : bite ^ set) )

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

char* getIDFromURL( char* url )
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

void printusage( char* command )
{
	printf( USAGE, command );
}

void parseOpts( int argc, char** argv )
{
	int c;
	int option_index = 0;
	int user_list_size = 0;
	FILE* file;
	unsigned int fsz = 0;
	char* string;
	char* username = NULL;
	char password[64];
	char* paste_id = NULL;
	int i = 0;
	pb_status retval = 0;

	debugf( "Options with bit values:\n" );
	debugf( "Option %s: %d\n", stringify(no_settings  ), no_settings  );  
	debugf( "Option %s: %d\n", stringify(retrieve_flag), retrieve_flag);
	debugf( "Option %s: %d\n", stringify(user_key_get ), user_key_get );
	debugf( "Option %s: %d\n", stringify(password_set ), password_set );
	debugf( "Option %s: %d\n", stringify(delete_flag  ), delete_flag  );
	debugf( "Option %s: %d\n", stringify(user_list_get), user_list_get);
	debugf( "Option %s: %d\n", stringify(stdin_use    ), stdin_use    );

	while( true )
	{
		c = getopt_long( argc, argv, OPTS, long_options, &option_index );

		if( c == -1 ) break;

		switch( c )
		{
			case 0: // option has set a flag
				if( long_options[option_index].flag != 0 )
					break;
				printf( "Option %s", long_options[option_index].name );
				if( optarg )
					printf( " with arg %s", optarg );
				printf( "\n" );
				break;

			case 's': // someone set the language
				pb_setWithOptions( pb, PB_SYNTAX, pb_getSyntax( optarg ) );
			break;

			case 'h': // ask for help
				printusage( argv[0] );
			break;

			case 'r': // retrieve flag set
				//settings |= retrieve_flag;
				debugf( "Setting retrieve_flag...\n" );
				set( settings, retrieve_flag );
				debugf( "Settings now %c\n", settings );
				if( is_set( settings, delete_flag ) )
				{
					fprintf( stderr, "Both delete and retrieve flags are set!\n" );
					exit( 42 );
				}
			break;

			case 'd': // delete flag set
				//settings |= delete_flag;
				debugf( "Setting delete_flag...\n" );
				set( settings, delete_flag );
				debugf( "Settings now %c\n", settings );
				if( is_set( settings, retrieve_flag ) )
				{
					fprintf( stderr, "Both delete and retrieve flags are set!\n" );
					exit( 42 );
				}
			break;

			case 'e': // set expiration
				for( int i = 0; i < EXPIRE_LIST_MAX; i++ )
					if( !strcmp( pb_expirestring[i], optarg ) )
					{
						pb_setWithOptions( pb, PB_EXPIRE_DATE, i );
					}
			break;

			case 'i': // they want stdin input
				debugf( "Setting stdin_use (%c)... \n", stdin_use );
				//settings |= stdin_use;
				set( settings, stdin_use );
				debugf( "Settings now %c\n", settings );
			break;

			case 'n': // naming the paste
				pb_setWithOptions( pb, PB_PASTE_NAME, optarg );
				debugf( "Set %s to %s\n", stringify( PB_PASTE_NAME ), optarg );
			break;

			case 'l': // list all syntaxes.
				printf( "List of supported languages:\n" );
				for( i = 0; i < SYN_LIST_MAX; i++ )
					printf( "\t%s\n", pb_syntaxstring[i] );

				return;
			break;

			case 't': // get trending pastes..
				
				printf( "Trending pastes:\n%s\n", pb_getTrendingPastes( pb ) );

				return;
			break;

			case 'u': // give a username
				debugf( "Username is %s\n", optarg );
				username = optarg; // should probably use strcpy
				//settings |= user_key_get;
				debugf( "Setting user_key_get\n" );
				set( settings, user_key_get );
				debugf( "Settings now %c\n", settings );
			break;

			case 'p': // give a password
				if( optarg )
				{
					//settings |= password_set;
					debugf( "Setting password_set...\n" );
					set( settings, password_set );
					debugf( "Settings now %c\n", settings );
					strcpy( password, optarg ); // should probably use strcpy
					debugf( "Password is %s\n", optarg );
				}
			break;

			case 'k': // give a user session key
				pb_setWithOptions( pb, PB_USER_KEY, optarg );
			break;

			case 'a': // the user wants to list their pastes
				debugf( "Setting %s to %s\n", stringify( user_list_size ), optarg );
				//settings |= user_list_get;
				debugf( "Setting user_list_get...\n" );
				set( settings, user_list_get );
				debugf( "Settings now %c\n", settings );
				user_list_size = atoi( optarg ) ;
			break;

			case '?':
				printusage( argv[0] );
				exit( 3 );
			break;

			default:
				exit( 42 );
			break;
		}
	}
	
	debugf( "Finished argument parsing; checking for password. %d arguments remain to parse\n", ( argc - optind ) );
	debugf( "Post-parse settings: %c\n", settings );
	
	// before we parse the remaining arguments, we need to see if they wanted a session key.
	if( is_set( settings, user_key_get ) )
	{
		if( !is_set( settings, password_set ) ) // need to silently prompt for password somehow. TODO:
		{
			strcpy( password, getpass( "Password:" ) );
		}

		if( (retval = pb_getUserSessionKey( pb, username, password )) != STATUS_OKAY )
		{
			switch( retval )
			{
				case STATUS_USERNAME_IS_NULL: // shouldn't really get here, but the code needs cleaning up
					fprintf( stderr, "Username is NULL!\n" );
				break;
				case STATUS_PASSWORD_IS_NULL:
					fprintf( stderr, "Password is NULL!\n" );
				break;
				default: // we don't need to handle any of the other status returns
				break;
			}
			debugf( "Couldn't get session key..\n" );
		}
	}

	if( is_set( settings, user_list_get ) )
	{
		printf( "%s\n", pb_getUserPastes( pb, user_list_size ) );
	}

	if( is_set( settings, stdin_use ) ) // they want stdin
	{
		fsz = DEFAULT_STDIN_BUFFER_SIZE;
		size_t csz = 1;
		string = (char*)malloc( sizeof(char)*fsz );
		*string = '\0';
		char buffer[DEFAULT_STDIN_BUFFER_SIZE];

		while( fgets( buffer, DEFAULT_STDIN_BUFFER_SIZE, stdin ) )
		{
			char* old = string;
			csz += strlen( buffer );
			string = realloc( string, csz );
			if( string == NULL )
			{
					fprintf( stderr, "Can't reallocate string to new buffer size!\n" );
					free( old );
					exit( 2 );
			}
			strcat( string, buffer );
		}
		if( ferror( stdin ) )
		{
			free( string );
			fprintf( stderr, "Can't read from stdin!\n" );
			exit( 3 );
		}

		printf( "%s\n", pb_newPaste( pb, string, strlen(string) ) );

		return;
	}

	if( optind >= argc && is_set( settings, user_key_get ) ) // Assume they just want the user key
	{
		if( !is_set( settings, user_key_get ) ) // Assume they just want the user key
		{
			debugf( "Returning only user key\n" );
			printf( "%s\n", pb->userkey );
		}
		else // they want to list their pastes
		{
			debugf( "Returning user pastes\n" );
			printf( "%s\n", pb_getUserPastes( pb, user_list_size ) );
		}

		return;
	}


	while( optind < argc ) // handle remaining args
	{
		if( is_set( settings, retrieve_flag ) ) // they want to retrieve urls
		{
			if( !strncmp( argv[optind], "http:", 5 ) )
			{
				debugf( "URL detected.. extracting ID\n" );
				paste_id = getIDFromURL( argv[optind++] );
				printf( "%s\n", pb_getRawPaste( paste_id ) );
			}
			else
				printf( "%s\n", pb_getRawPaste( argv[optind++] ) );
		}
		else if( is_set( settings, delete_flag ) ) // they want to delete urls
		{
			debugf( "Deleting paste %s\n", argv[optind] );
			if( !strncmp( argv[optind], "http:", 5 ) )
			{
				debugf( "URL detected.. extracting ID\n" );
				paste_id = getIDFromURL( argv[optind++] );
				pb_deletePaste( pb, paste_id );
			}
			else
				pb_deletePaste( pb, argv[optind++] );
		}
		else
		{
			if( (file = fopen( argv[optind], "r" )) != NULL )
			{
				fseek( file, 0, SEEK_END );
				fsz = ftell( file );
				fseek( file, 0, SEEK_SET );
				string = (char*)malloc( sizeof(char)*fsz+1 );
				
				// shouldn't fail... I think
				fread( string, sizeof(char), fsz, file );
				string[fsz] = '\0';

				printf( "%s\n", pb_newPaste( pb, string, fsz+1 ) );
				free( string );
				fclose( file );
			}
			else 
			{
				fprintf( stderr, "Error: Can't open file `%s`\n", argv[optind] );
			}
		}
		optind++;
	}

}

int main( int argc, char** argv )
{ 
	pb = pb_newPastebin();
	pb_setWithOptions( pb, PB_DEV_KEY, PB_CLIENT_API_KEY );
	pb_setWithOptions( pb, PB_SYNTAX, SYN_LANG_NONE );
	pb_setWithOptions( pb, PB_USER_KEY, "");

	if( argc < 2 ) printusage( argv[0] );
	parseOpts( argc, argv );

	pb_freePastebin( pb );

	return 0;
}
