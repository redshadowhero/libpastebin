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

/*
* TODO:
* + pb_getUserInformation (http://pastebin.com/api#9)
*/

#ifndef PASTEBIN_H
#define PASTEBIN_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>
#include <stdarg.h>

#include "pastebin_syntax.h"
#include "pastebin_status.h"

/* pastebin settings shortcuts */
#define   PB_NO_SETTINGS 0x0
// privacy settings
#define   PB_PASTE_PUBLIC     0x0 /* binary 00000000 */
#define   PB_PASTE_PRIVATE    0x1 /* binary 00000001 */
#define   PB_PASTE_UNLISTED   0x2 /* binary 00000010 */
// for use with pb_setWithOptions
#define   PB_DEV_KEY     256
#define   PB_SYNTAX      257
#define   PB_USER_KEY    258
#define   PB_PASTE_NAME  259
#define   PB_EXPIRE_DATE 260

/* API URLs */
#define   PB_API_POST_URL    "http://pastebin.com/api/api_post.php"
#define   PB_API_LOGIN_URL   "http://pastebin.com/api/api_login.php"
#define   PB_API_RAW_URL     "http://pastebin.com/raw.php?i="

typedef uint16_t pb_settings;

typedef enum _pb_expire
{
	EXPIRE_NEVER = 0,
	EXPIRE_10M,
	EXPIRE_1H,
	EXPIRE_1D,
	EXPIRE_1M,
	/* size of the enum  */
	EXPIRE_LIST_MAX
} pb_expire;

extern char* pb_expirestring[ EXPIRE_LIST_MAX ];

typedef struct _pastebin
{
	char* devkey;
	char* userkey;
	char* pastename;
	pb_status lastStatus;
	pb_expire expiration;
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
/** Gets a list of all user pastes, delimited by a given limit size.
* @param[in] _pb The pastebin object.
* @param[in] _size The number of entries to return
* @return A list of pastes
*/
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
pb_status pb_deletePaste( pastebin*, char* /* key of the paste to delete */ );
/**
* @param[in] _key The id to get the raw for.
* @return A string with the paste content; NULL otherwise.
*/
char* pb_getRawPaste( char* /* paste key */ );

#ifdef __cplusplus
}
#endif

#endif /* PASTEBIN_H */
