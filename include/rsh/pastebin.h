#ifndef PASTEBIN_H
#define PASTEBIN_H

#include <stdint.h>
#include <stdbool.h>
#include <stdarg.h>

#include "pastebin_syntax.h"

/* pastebin settings shortcuts */
#define   PB_NO_SETTINGS 0x0
// privacy settings
#define   PB_PASTE_PUBLIC     0x0 /* binary 00000000 */
#define   PB_PASTE_PRIVATE    0x1 /* binary 00000001 */
#define   PB_PASTE_UNLISTED   0x2 /* binary 00000010 */
// for use with pb_setWithOptions
#define   PB_DEV_KEY    256
#define   PB_SYNTAX     257
#define   PB_USER_KEY   258
#define   PB_PASTE_NAME 259

/* API URLs */
#define   PB_API_POST_URL    "http://pastebin.com/api/api_post.php"
#define   PB_API_LOGIN_URL   "http://pastebin.com/api/api_login.php"
#define   PB_API_RAW_URL     "http://pastebin.com/raw.php?i="

typedef uint16_t pb_settings;

typedef enum _pb_status
{
	STATUS_OKAY = 0,
	/* paste failure codes */
	STATUS_INVALID_API_OPTION,    /* "Bad API request, invalid api_option"                                         */
	STATUS_INVALID_API_DEV_KEY,   /* "Bad API request, invalid api_dev_key"                                        */
	STATUS_IP_BLOCKED,            /* "Bad API request, IP blocked"                                                 */
	STATUS_MAX_UNLISTED_PASTES,   /* "Bad API request, maximum number of 25 unlisted pastes for your free account" */
	STATUS_MAX_PRIVATE_PASTES,    /* "Bad API request, maximum number of 10 private pastes for your free account"  */
	STATUS_EMPTY_PASTE,           /* "Bad API request, api_paste_code was empty"                                   */
	STATUS_MAX_PASTE_FILE_SIZE,   /* "Bad API request, maximum paste file size exceeded"                           */
	STATUS_INVALID_EXPIRE_DATE,   /* "Bad API request, invalid api_expire_date"                                    */
	STATUS_INVALID_PASTE_PRIVATE, /* "Bad API request, invalid api_paste_private"                                  */
	STATUS_INVALID_PASTE_FORMAT,  /* "Bad API request, invalid api_paste_format"                                   */
	/* function failure codes */
	STATUS_USERNAME_IS_NULL,
	STATUS_PASSWORD_IS_NULL,
	/* size of this enum */
	STATUS_LIST_MAX
} pb_status;

typedef struct _pastebin
{
	char* devkey;
	char* userkey;
	char* pastename;
	pb_status lastStatus;
	pb_settings settings;
	pb_syntax syntax;
} pastebin;

// creation/destruction
pastebin* pb_newPastebin();
void	  pb_freePastebin( pastebin* );

// settings
void pb_set( pastebin*, pb_settings );
void* pb_get( pastebin*, pb_settings );
void pb_setWithOptions( pastebin*, pb_settings, ... );
void pb_unset( pastebin*, pb_settings );
bool pb_isSet( pastebin*, pb_settings );

// api
/**
* @param[in] _pb The pastebin struct.
* @param[in] _str The string to paste.
* @param[in] _sz The size of the string.
* @return A url with the paste if successful; NULL on error
*/
char* pb_newPaste( pastebin*, char*, int );
/** Gets a session key for the user
* @param[in] _pb The pastebin struct
* @param[in] _username The username
* @param[in] _password The password
* @return The session key; NULL otherwise.
*/
pb_status pb_getUserSessionKey( pastebin* /* pb */, char* /* username */, char* /* password */ );
/** Gets a list of all user pastest, delimited by a given limit size.
* @param[in] _pb The pastebin object.
* @param[in] _size The number of entries to return
* @return A list of pastes
*/
// TODO:
char* pb_getUserPastes( pastebin*, int /* list size */ );
/**
* @param[in] _pb The pastebin struct.
* @return A list of trending pastes; NULL on failure.
*/
// TODO:
char* pb_getTrendingPastes( pastebin* );
/** Delete a paste.
* @param[in] _key The user key. Get with pb_getUserSessionKey
* @param[in] _pastekey the paste key to delete.
*/
// TODO:
pb_status pb_deletePaste( pastebin*, char* /* key of the paste to delete */ );
/**
* @param[in] _key The id to get the raw for.
* @return A string with the paste content; NULL otherwise.
*/
char* pb_getRawPaste( char* /* paste key */ );

#endif /* PASTEBIN_H */
