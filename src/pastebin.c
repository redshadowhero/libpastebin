#include <rsh/pastebin.h>
#include <rsh/pastebin_syntax.h>
#include <rsh/debug.h>

#include <stdlib.h>
#include <stdbool.h>
#include <stdarg.h>
#include <string.h>

#include <curl/curl.h>

/*
* private stuff
*/

struct pb_memblock 
{
    char*  memory;
    size_t size;
};

static size_t pb_memcopy( void* ptr, size_t size, size_t nmemb, void* data )
{
    debugf( "Entering function\n" );

    size_t realsize = size*nmemb;
    struct pb_memblock* mem = (struct pb_memblock*)data;

    mem->memory = realloc( mem->memory, mem->size + realsize+1 );
    if( mem->memory == NULL )
    {
        debugf( "%s is NULL!\n", stringify( mem->memory ) );
        return 0;
    }
    memcpy( &(mem->memory[mem->size]), ptr, realsize );
    mem->size += realsize;
    mem->memory[mem->size] = 0;

    debugf( "Exiting function\n" );

    return realsize;
}

/*
* End static/local items
*/

/* 
* pastebin_syntax.h implemenatation 
*/

char* pb_syntaxstring[SYN_LIST_MAX] =
{
    "text",  /* SYN_LANG_NONE */
    "c",     /* SYN_LANG_C    */
    "cpp",   /* SYN_LANG_CPP  */
    "java",  /* SYN_LANG_JAVA */
    "bash",  /* SYN_LANG_BASH */
};


char* pb_getSyntaxString( pb_syntax _syntax )
{
    debugf( "Entering function with argument %d\n", _syntax );
    debugf( "Returning with \"%s\"\n", pb_syntaxstring[_syntax] );
    debugf( "Exiting function\n" );
    return pb_syntaxstring[_syntax];
}

pb_syntax pb_getSyntax( char* _syn )
{
    debugf( "Entering function with argument \"%s\"\n", _syn );

    for( unsigned int i = 0; i < SYN_LIST_MAX; i++ )
        if( !strcmp( pb_syntaxstring[i], _syn ) )
        {
            debugf( "Matching syntax found at index %i\n", i );
            debugf( "Exiting function\n" );
            return (pb_syntax)i;
        }


    debugf( "No matching syntax found; returning %s\n", stringify( SYN_LANG_NONE ) );
    debugf( "Exiting function\n" );
    return SYN_LANG_NONE;
}

/*
* End pastebin_syntax.h
*/

/* 
* pastebin.h implementation 
*/

pastebin* pb_newPastebin()
{
    debugf( "Entering function\n" );

    pastebin* _rv = (pastebin*)malloc( sizeof(pastebin) );
    _rv->settings = PB_NO_SETTINGS;
    _rv->syntax   = SYN_LANG_NONE;

    debugf( "Exiting function\n" );

    return _rv;
}

void pb_freePastebin( pastebin* _pb )
{
    debugf( "Entering function\n" );
    free( _pb );
    debugf( "Exiting function\n" );
}

void pb_set( pastebin* _pb, pb_settings _settings )
{
    debugf( "Entering function with setting argument %d\n", _settings );
    _pb->settings |= _settings;
}

void pb_setWithOptions( pastebin* _pb, pb_settings _settings, ... )
{
    debugf( "Entering function\n" );
    // TODO:
    va_list ap;

    va_start( ap, _settings );

    switch( _settings )
    {
        case PB_DEV_KEY:
            _pb->devkey = va_arg( ap, char* );
            debugf( "%s set to \"%s\"\n", stringify( _pb->devkey ), _pb->devkey );
        break;
        case PB_SYNTAX:
            _pb->syntax = va_arg( ap, pb_syntax );
            debugf( "%s set to %d\n", stringify( _pb->syntax ), (int)_pb->syntax );
        break;
        case PB_USER_KEY:
            _pb->userkey = va_arg( ap, char* );
            debugf( "%s set to \"%s\"\n", stringify( _pb->userkey ), _pb->userkey );
        break;
        case PB_PASTE_NAME:
            _pb->pastename = va_arg( ap, char* );
            debugf( "%s set to \"%s\"\n", stringify( _pb->pastename ), _pb->pastename );
        break;
    }

    va_end( ap );
    debugf( "Exiting function\n" );
}

void pb_unset( pastebin* _pb, pb_settings _settings )
{
    debugf( "Entering function with argument %d\n", _settings );

    if( pb_isSet( _pb, _settings ) ) 
    {
        debugf( "Item set; unsetting...\n" );
        _pb->settings ^= _settings;
    }

    debugf( "Exiting function" );
}

bool pb_isSet( pastebin* _pb, pb_settings _setting )
{
    debugf( "Entering function looking for setting %d\n", _setting );
    if( (_pb->settings & _setting) ) 
    {
        debugf( "Item is set; exiting function\n" );
        return true;
    }
    else
    {
        debugf( "Item isn't set; exiting function\n" );
        return false;
    }
}

char* pb_getRawPaste( char* _paste_id )
{
    debugf( "Entering function\n" );
    CURL* curl = curl_easy_init();
    // CURLcode response; // TODO: later, use this for error checking.
    struct pb_memblock chunk;
    char url[128];
    sprintf( url, PB_API_RAW_URL "%s", _paste_id );

    chunk.memory = malloc(1);
    chunk.size = 0;

    if( curl ) // init went through smoothly
    {
        curl_easy_setopt( curl, CURLOPT_WRITEFUNCTION, pb_memcopy );
        curl_easy_setopt( curl, CURLOPT_WRITEDATA, (void*)&chunk );

        curl_easy_setopt( curl, CURLOPT_URL, url );
        /*response =*/ curl_easy_perform( curl );

        curl_easy_cleanup( curl );
        
        debugf( "Exiting function\n" );
        return chunk.memory;
    }

    debugf( "curl didn't initialize correctly! Exiting function\n" );
    // if curl was NULL
    return NULL;
}

char* pb_newPaste( pastebin* _pb, char* _str, int _sz )
{
    debugf( "Entering function\n" );

    char* pastestr = (char*)malloc( sizeof(char)*(_sz*3)+1 );
    char* argu     = (char*)malloc( sizeof(char)*(_sz*4)+256 );
    CURL* curl     = curl_easy_init();
    //CURLcode res;
    struct pb_memblock chunk;

    chunk.memory = malloc(1);
    chunk.size = 0;

    curl_easy_setopt( curl, CURLOPT_WRITEFUNCTION, pb_memcopy );
    curl_easy_setopt( curl, CURLOPT_WRITEDATA, (void*)&chunk );

    curl_easy_setopt( curl, CURLOPT_URL, PB_API_POST_URL );
    curl_easy_setopt( curl, CURLOPT_POST, 1 );

    pastestr = curl_easy_escape( curl, _str, _sz );
    if( _pb->pastename )
        _pb->pastename = curl_easy_escape( curl, _pb->pastename, strlen( _pb->pastename ) ); // TODO: too many lookups. Replace with local variable.

    sprintf( argu, "api_option=paste&api_user_key=%s&api_paste_private=%d&api_paste_name=%s&api_paste_expire_date=N&api_paste_format=%s&api_dev_key=%s&api_paste_code=%s",
             _pb->userkey, (pb_isSet(_pb, PB_PASTE_UNLISTED)?1:(pb_isSet( _pb, PB_PASTE_PRIVATE)?2:0)), (_pb->pastename)?_pb->pastename:"", 
             pb_getSyntaxString( _pb->syntax ), _pb->devkey, pastestr );

    debugf( "Curl postfields:\n%s\n", argu );

    curl_easy_setopt( curl, CURLOPT_POSTFIELDS, argu );
    curl_easy_setopt( curl, CURLOPT_NOBODY, 0 );

    /*res =*/ curl_easy_perform( curl );
    curl_easy_cleanup( curl );

    // TODO:
    _pb->lastStatus = STATUS_OKAY;
    debugf( "Exiting function\n" );
    return chunk.memory;
}

pb_status pb_getUserSessionKey( pastebin* _pb, char* _username, char* _password )
{
    // note that we don't store the username or password; just the key after it completes successfully. 
    debugf( "Entering function with argument %s: %s, %s: %s\n", stringify( _username ), _username, stringify( _password ), _password );

    if( !_username )
    {
        debugf( "username is NULL!\n" );
        return STATUS_USERNAME_IS_NULL;
    }
    if( !_password )
    {
        debugf( "password is NULL!\n" );
        return STATUS_PASSWORD_IS_NULL;
    }

    
    char* argu = (char*)malloc( sizeof(char)*1024 ); // 1kb should suffice. If it doesn't, your password is too long, bro
    char* user = (char*)malloc( sizeof(char)*strlen(_username)*3 );
    char* pass = (char*)malloc( sizeof(char)*strlen(_password)*3 );
    CURL* curl = curl_easy_init();
    user = curl_easy_escape( curl, _username, strlen( _username ) );
    pass = curl_easy_escape( curl, _password, strlen( _password ) );
    struct pb_memblock chunk;

    chunk.memory = malloc(1);
    chunk.size   = 0;

    curl_easy_setopt( curl, CURLOPT_WRITEFUNCTION, pb_memcopy );
    curl_easy_setopt( curl, CURLOPT_WRITEDATA, (void*)&chunk );

    curl_easy_setopt( curl, CURLOPT_URL, PB_API_LOGIN_URL );
    curl_easy_setopt( curl, CURLOPT_POST, 1 );

    sprintf( argu, "api_dev_key=%s&api_user_name=%s&api_user_password=%s", _pb->devkey, user, pass );
    debugf( "Curl postfields:\n%s\n", argu );

    curl_easy_setopt( curl, CURLOPT_POSTFIELDS, argu );
    curl_easy_setopt( curl, CURLOPT_NOBODY, 0 );

    curl_easy_perform( curl );
    debugf( "user key is:\n%s\n", chunk.memory );
    _pb->userkey = chunk.memory;

    debugf( "Exiting function\n" );

    return STATUS_OKAY;
}
