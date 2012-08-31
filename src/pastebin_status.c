#ifdef __cplusplus
extern "C" {
#endif

#include <rsh/pastebin_status.h>
#include <rsh/debug.h>

char* pb_statusString[STATUS_INVALID_PASTE_FORMAT+1] =
{
	"",                                                                            /* STATUS_OKAY                  */
	"Bad API request, invalid api_option",                                         /* STATUS_INVALID_API_OPTION    */
	"Bad API request, invalid api_dev_key",                                        /* STATUS_INVALID_API_DEV_KEY   */
	"Bad API request, IP blocked",                                                 /* STATUS_IP_BLOCKED            */
	"Bad API request, maximum number of 25 unlisted pastes for your free account", /* STATUS_MAX_UNLISTED_PASTES   */
	"Bad API request, maximum number of 10 private pastes for your free account",  /* STATUS_MAX_PRIVATE_PASTES    */
	"Bad API request, api_paste_code was empty",                                   /* STATUS_EMPTY_PASTE           */
	"Bad API request, maximum paste file size exceeded",                           /* STATUS_MAX_PASTE_FILE_SIZE   */
	"Bad API request, invalid api_expire_date",                                    /* STATUS_INVALID_EXPIRE_DATE   */
	"Bad API request, invalid api_paste_private",                                  /* STATUS_INVALID_PASTE_PRIVATE */
	"Post limit, maximum pastes per 24h reached",                                  /* STATUS_MAX_PASTE_PER_DAY     */
	"Bad API request, invalid api_paste_format",                                   /* STATUS_INVALID_PASTE_FORMAT  */
};

#ifdef __cplusplus
}
#endif
