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
#ifndef PASTEBIN_STATUS_H
#define PASTEBIN_STATUS_H

#ifdef __cplusplus
extern "C" {
#endif

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
	STATUS_MAX_PASTE_PER_DAY,     /* "Post limit, maximum pastes per 24h reached"                                  */
	STATUS_INVALID_PASTE_FORMAT,  /* "Bad API request, invalid api_paste_format"                                   */
	/* function failure codes */
	/* pb_setWithOptions()    */
	STATUS_NOTHING_TO_SET,        /* Something went wrong with pb_setWithOptions                                   */
	/* pb_getUserSessionKey() */
	STATUS_USERNAME_IS_NULL,      /* Trying to call the function with username being NULL                          */
	STATUS_PASSWORD_IS_NULL,      /* Trying to call the function with the password being NULL                      */
	STATUS_USER_KEY_NOT_SET,
	/* size of this enum */
	STATUS_LIST_MAX
} pb_status;

extern char* pb_statusString[STATUS_INVALID_PASTE_FORMAT+1];

#ifdef __cplusplus
}
#endif

#endif /* PASTEBIN_STATUS_H */
