#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <getopt.h>

#include <rsh/pastebin.h>
#include <rsh/pastebin_syntax.h>
#include <rsh/debug.h>

#define OPTS "t:n:u:plrh"

#define USAGE \
    "Usage: %s [OPTION...] file1 [file2...]\n" \
    "\t-u, --user=USERNAME\t\tGive a username to authenticate as\n" \
    "\t-p, --pass=PASSWORD\t\tGive a password to authenticate as\n" \
    "\t-t, --syntax=SYNTAX\t\tUse specified syntax\n" \
    "\t-n, --name=NAME\t\t\tGive a name to the paste\n" \
    "\t-l, --list\t\t\tList all supported syntaxes\n" \
    "\t-r, --retrieve\t\t\tRetrieve the paste and dump it to stdout\n" \
    "\t-h, --help\t\t\tPrint this message\n"

/* globals */

#define no_settings 0x0
#define retrieve_flag 0x1
#define user_key_get 0x2
#define password_set 0x4

pastebin* pb;

typedef uint8_t byte;

/*
* Bits
* [0] retrieve flag
*/
byte settings = no_settings;

#define is_set( bite, set ) ( bite & set )

struct option long_options[] =
{
    { "syntax",   required_argument, 0, 't' },
    { "user",     required_argument, 0, 'u' },
    { "pass",     optional_argument, 0, 'p' },
    { "retrieve", no_argument,       0, 'r' },
    { "name",     required_argument, 0, 'n' },
    { "help",     no_argument,       0, 'h' },
    { "list",     no_argument,       0, 'l' },
    { 0,          0,                 0,  0  }
};

void printusage( char* command )
{
    printf( USAGE, command );
}

void parseOpts( int argc, char** argv )
{
    int c;
    int option_index = 0;
    FILE* file;
    int fsz = 0;
    char* string;
    char* username = NULL;
    char* password = NULL;
    int i = 0;

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

            case 't': // someone set the language
                //printf( "syntax selected: %s\n", optarg );
                pb_setWithOptions( pb, PB_SYNTAX, pb_getSyntax( optarg ) );
            break;

            case 'h': // ask for help
                printusage( argv[0] );
            break;

            case 'r': // retrieve flag set
                settings |= retrieve_flag;
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

            case 'u': // give a username
                debugf( "Username is %s\n", optarg );
                username = optarg; // should probably use strcpy
                settings |= user_key_get;
            break;

            case 'p': // give a password
                if( optarg )
                {
                    settings |= password_set;
                    password = optarg; // should probably use strcpy
                    debugf( "Password is %s\n", optarg );
                }
            break;

            case '?':
                //printf( "Shit son. You done fucked up.\n" );
                printusage( argv[0] );
            break;

            default:
                printf( "Aborting program. You are too stupid to use it.\n" );
                exit( 42 );
            break;
        }
    }
    
    // before we parse the remaining arguments, we need to see if they wanted a session key.
    if( is_set( settings, user_key_get ) )
    {
        if( !is_set( settings, password_set ) ); // need to silently prompt for password somehow. TODO:

        if( pb_getUserSessionKey( pb, username, password ) != STATUS_OKAY )
        {
            // handle problems here.
            debugf( "Couldn't get session key..\n" );
        }
    }

    if( optind < argc )
    {
        //printf( "The kitchen sink:\n" );
        while( optind < argc ) // handle remaining args
        {
            //printf( "\t%s\n", argv[optind++] );
            if( is_set( settings, retrieve_flag ) ) // everything is a retreieve...? TODO: smart check for IDs versus URLs.
                printf( "%s\n", pb_getRawPaste( argv[optind++] ) );
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
                optind++;
            }
        }
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
