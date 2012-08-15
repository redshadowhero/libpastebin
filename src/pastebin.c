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
    "text",          /* SYN_LANG_NONE          */
    "4cs",           /* SYN_LANG_4CS           */
    "6502acme",      /* SYN_LANG_6502ACME      */
    "6502kickass",   /* SYN_LANG_6502KICKASS   */
    "6502tasm",      /* SYN_LANG_6502TASM      */
    "abap",          /* SYN_LANG_ABAP          */
    "actionscript",  /* SYN_LANG_ACTIONSCRIPT  */
    "actionscript3", /* SYN_LANG_ACTIONSCRIPT3 */
    "ada",           /* SYN_LANG_ADA           */
    "algol68",       /* SYN_LANG_ALGOL68       */
    "apache",        /* SYN_LANG_APACHE        */
    "applescript",   /* SYN_LANG_APPLESCRIPT   */
    "apt_sources",   /* SYN_LANG_APT_SOURCES   */
    "asm",           /* SYN_LANG_ASM           */
    "asp",           /* SYN_LANG_ASP           */
    "autoconf",      /* SYN_LANG_AUTOCONF      */
    "autohotkey",    /* SYN_LANG_AUTOHOTKEY    */
    "autoit",        /* SYN_LANG_AUTOIT        */
    "avisynth",      /* SYN_LANG_AVISYNTH      */
    "awk",           /* SYN_LANG_AWK           */
    "basconavr",     /* SYN_LANG_BASCOMAVR     */
    "bash",          /* SYN_LANG_BASH          */
    "basic4gl",      /* SYN_LANG_BASIC4GL      */
    "bibtex",        /* SYN_LANG_BIBTEX        */
    "blitzbasic",    /* SYN_LANG_BLITZBASIC    */
    "bnf",           /* SYN_LANG_BNF           */
    "boo",           /* SYN_LANG_BOO           */
    "bf",            /* SYN_LANG_BF            */
    "c",             /* SYN_LANG_C             */
    "c_mac",         /* SYN_LANG_C_MAC         */
    "cil",           /* SYN_LANG_CIL           */
    "csharp",        /* SYN_LANG_CSHARP        */
    "ccp",           /* SYN_LANG_CPP           */
    "cpp-qt",        /* SYN_LANG_CPP-QT        */
    "c_loadrunner",  /* SYN_LANG_C_LOADRUNNER  */
    "caddcl",        /* SYN_LANG_CADDCL        */
    "cadslip",       /* SYN_LANG_CADLISP       */
    "cfdg",          /* SYN_LANG_CFDG          */
    "chaiscript",    /* SYN_LANG_CHAISCRIPT    */
    "clojure",       /* SYN_LANG_CLOJURE       */
    "klonec",        /* SYN_LANG_KLONEC        */
    "klonecpp",      /* SYN_LANG_KLONECPP      */
    "cmake",         /* SYN_LANG_CMAKE         */
    "cobol",         /* SYN_LANG_COBOL         */
    "coffeescript",  /* SYN_LANG_COFFEESCRIPT  */
    "cfm",           /* SYN_LANG_CFM           */
    "css",           /* SYN_LANG_CSS           */
    "cuesheet",      /* SYN_LANG_CUESHEET      */
    "d",             /* SYN_LANG_D             */
    "dcs",           /* SYN_LANG_DCS           */
    "delphi",        /* SYN_LANG_DELPHI        */
    "oxygene",       /* SYN_LANG_OXYGENE       */
    "diff",          /* SYN_LANG_DIFF          */
    "div",           /* SYN_LANG_DIV           */
    "dos",           /* SYN_LANG_DOS           */
    "dot",           /* SYN_LANG_DOT           */
    "e",             /* SYN_LANG_E             */
    "ecmascript",    /* SYN_LANG_ECMASCRIPT    */
    "eiffel",        /* SYN_LANG_EIFFEL        */
    "email",         /* SYN_LANG_EMAIL         */
    "epc",           /* SYN_LANG_EPC           */
    "erlang",        /* SYN_LANG_ERLANG        */
    "fsharp",        /* SYN_LANG_FSHARP        */
    "falcon",        /* SYN_LANG_FALCON        */
    "fo",            /* SYN_LANG_FO            */
    "f1",            /* SYN_LANG_F1            */
    "fortran",       /* SYN_LANG_FORTRAN       */
    "freebasic",     /* SYN_LANG_FREEBASIC     */
    "freeswitch",    /* SYN_LANG_FREESWITCH    */
    "gambas",        /* SYN_LANG_GAMBAS        */
    "gml",           /* SYN_LANG_GML           */
    "gdb",           /* SYN_LANG_GDB           */
    "genero",        /* SYN_LANG_GENERO        */
    "genie",         /* SYN_LANG_GENIE         */
    "gettext",       /* SYN_LANG_GETTEXT       */
    "go",            /* SYN_LANG_GO            */
    "groovy",        /* SYN_LANG_GROOVY        */
    "gwbasic",       /* SYN_LANG_GWBASIC       */
    "haskell",       /* SYN_LANG_HASKELL       */
    "hicest",        /* SYN_LANG_HICEST        */
    "hq9plus",       /* SYN_LANG_HQ9PLUS       */
    "html4strict",   /* SYN_LANG_HTML4STRICT   */
    "html5",         /* SYN_LANG_HTML5         */
    "icon",          /* SYN_LANG_ICON          */
    "idl",           /* SYN_LANG_IDL           */
    "ini",           /* SYN_LANG_INI           */
    "inno",          /* SYN_LANG_INNO          */
    "intercal",      /* SYN_LANG_INTERCAL      */
    "io",            /* SYN_LANG_IO            */
    "j",             /* SYN_LANG_J             */
    "java",          /* SYN_LANG_JAVA          */
    "java5",         /* SYN_LANG_JAVA5         */
    "javascript",    /* SYN_LANG_JAVASCRIPT    */
    "jquery",        /* SYN_LANG_JQUERY        */
    "kixtart",       /* SYN_LANG_KIXTART       */
    "latex",         /* SYN_LANG_LATEX         */
    "lb",            /* SYN_LANG_LB            */
    "lsl2",          /* SYN_LANG_LSL2          */
    "lisp",          /* SYN_LANG_LISP          */
    "llvm",          /* SYN_LANG_LLVM          */
    "locobasic",     /* SYN_LANG_LOCOBASIC     */
    "logtalk",       /* SYN_LANG_LOGTALK       */
    "lolcode",       /* SYN_LANG_LOLCODE       */
    "lotusformulas", /* SYN_LANG_LOTUSFORMULAS */
    "lotusscript",   /* SYN_LANG_LOTUSSCRIPT   */
    "lscript",       /* SYN_LANG_LSCRIPT       */
    "lua",           /* SYN_LANG_LUA           */
    "m68k",          /* SYN_LANG_M68K          */
    "magiksf",       /* SYN_LANG_MAGIKSF       */
    "make",          /* SYN_LANG_MAKE          */
    "mapbasic",      /* SYN_LANG_MAPBASIC      */
    "matlab",        /* SYN_LANG_MATLAB        */
    "mirc",          /* SYN_LANG_MIRC          */
    "mmix",          /* SYN_LANG_MMIX          */
    "modula2",       /* SYN_LANG_MODULA2       */
    "modula3",       /* SYN_LANG_MODULA3       */
    "68000devpac",   /* SYN_LANG_68000DEVPAC   */
    "mpasm",         /* SYN_LANG_MPASM         */
    "mxml",          /* SYN_LANG_MXML          */
    "mysql",         /* SYN_LANG_MYSQL         */
    "newlisp",       /* SYN_LANG_NEWLISP       */
    "nsis",          /* SYN_LANG_NSIS          */
    "oberon2",       /* SYN_LANG_OBERON2       */
    "objeck",        /* SYN_LANG_OBJECK        */
    "objc",          /* SYN_LANG_OBJC          */
    "ocaml-brief",   /* SYN_LANG_OCAML-BRIEF   */
    "ocaml",         /* SYN_LANG_OCAML         */
    "pf",            /* SYN_LANG_PF            */
    "glsl",          /* SYN_LANG_GLSL          */
    "oobas",         /* SYN_LANG_OOBAS         */
    "oracle11",      /* SYN_LANG_ORACLE11      */
    "oracle8",       /* SYN_LANG_ORACLE8       */
    "oz",            /* SYN_LANG_OZ            */
    "pascal",        /* SYN_LANG_PASCAL        */
    "pawn",          /* SYN_LANG_PAWN          */
    "pcre",          /* SYN_LANG_PCRE          */
    "per",           /* SYN_LANG_PER           */
    "perl",          /* SYN_LANG_PERL          */
    "perl6",         /* SYN_LANG_PERL6         */
    "php",           /* SYN_LANG_PHP           */
    "php-brief",     /* SYN_LANG_PHP-BRIEF     */
    "pic16",         /* SYN_LANG_PIC16         */
    "pike",          /* SYN_LANG_PIKE          */
    "pixelbender",   /* SYN_LANG_PIXELBENDER   */
    "plsql",         /* SYN_LANG_PLSQL         */
    "postgresql",    /* SYN_LANG_POSTGRESQL    */
    "povray",        /* SYN_LANG_POVRAY        */
    "powershell",    /* SYN_LANG_POWERSHELL    */
    "powerbuilder",  /* SYN_LANG_POWERBUILDER  */
    "proftpd",       /* SYN_LANG_PROFTPD       */
    "progress",      /* SYN_LANG_PROGRESS      */
    "prolog",        /* SYN_LANG_PROLOG        */
    "properties",    /* SYN_LANG_PROPERTIES    */
    "providex",      /* SYN_LANG_PROVIDEX      */
    "purebasic",     /* SYN_LANG_PUREBASIC     */
    "pycon",         /* SYN_LANG_PYCON         */
    "python",        /* SYN_LANG_PYTHON        */
    "q",             /* SYN_LANG_Q             */
    "qbasic",        /* SYN_LANG_QBASIC        */
    "rsplus",        /* SYN_LANG_RSPLUS        */
    "rails",         /* SYN_LANG_RAILS         */
    "rebol",         /* SYN_LANG_REBOL         */
    "reg",           /* SYN_LANG_REG           */
    "robots",        /* SYN_LANG_ROBOTS        */
    "rpmspec",       /* SYN_LANG_RPMSPEC       */
    "ruby",          /* SYN_LANG_RUBY          */
    "gnuplot",       /* SYN_LANG_GNUPLOT       */
    "sas",           /* SYN_LANG_SAS           */
    "scala",         /* SYN_LANG_SCALA         */
    "scheme",        /* SYN_LANG_SCHEME        */
    "scilab",        /* SYN_LANG_SCILAB        */
    "sdlbasic",      /* SYN_LANG_SDLBASIC      */
    "smalltalk",     /* SYN_LANG_SMALLTALK     */
    "smarty",        /* SYN_LANG_SMARTY        */
    "sql",          /* SYN_LANG_SQL           */
    "systemverilog", /* SYN_LANG_SYSTEMVERILOG */
    "tsql",          /* SYN_LANG_TSQL          */
    "tcl",           /* SYN_LANG_TCL           */
    "teraterm",      /* SYN_LANG_TERATERM      */
    "thinbasic",     /* SYN_LANG_THINBASIC     */
    "typoscript",    /* SYN_LANG_TYPOSCRIPT    */
    "unicon",        /* SYN_LANG_UNICON        */
    "uscript",       /* SYN_LANG_USCRIPT       */
    "vala",          /* SYN_LANG_VALA          */
    "vbnet",         /* SYN_LANG_VBNET         */
    "verilog",       /* SYN_LANG_VERILOG       */
    "vhdl",          /* SYN_LANG_VHDL          */
    "vim",           /* SYN_LANG_VIM           */
    "visualprolog",  /* SYN_LANG_VISUALPROLOG  */
    "vb",            /* SYN_LANG_VB            */
    "visualfoxpro",  /* SYN_LANG_VISUALFOXPRO  */
    "whitespace",    /* SYN_LANG_WHITESPACE    */
    "whois",         /* SYN_LANG_WHOIS         */
    "winbatch",      /* SYN_LANG_WINBATCH      */
    "xbasic",        /* SYN_LANG_XBASIC        */
    "xml",           /* SYN_LANG_XML           */
    "xorg_conf",     /* SYN_LANG_XORG_CONF     */
    "xpp",           /* SYN_LANG_XPP           */
    "yaml",          /* SYN_LANG_YAML          */
    "z80",           /* SYN_LANG_Z80           */
    "zxbasic"        /* SYN_LANG_ZXBASIC       */
};


char* pb_getSyntaxString( pb_syntax _syntax )
{
	debugf( "Entering function with argument %d\n", _syntax );
	debugf( "Returning with \"%s\"\n", pb_syntaxstring[_syntax] );
	debugf( "Exiting function\n" );
	return pb_syntaxstring[_syntax];
}

pb_syntax pb_getSyntax( const char* _syn )
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

pb_status pb_deletePaste( pastebin* _pb, char* _paste_id )
{
	debugf( "Entering function\n" );
	if( ! _pb->userkey )
		return STATUS_USER_KEY_NOT_SET;
	
	char* argu = (char*)malloc( sizeof(char)*1024 );
	CURL* curl = curl_easy_init();
	struct pb_memblock chunk;

	chunk.memory = malloc(1);
	chunk.size   = 0;

	curl_easy_setopt( curl, CURLOPT_WRITEFUNCTION, pb_memcopy );
	curl_easy_setopt( curl, CURLOPT_WRITEDATA, (void*)&chunk );

	curl_easy_setopt( curl, CURLOPT_URL, PB_API_POST_URL );
	curl_easy_setopt( curl, CURLOPT_POST, 1 ); 

	sprintf( argu, "api_option=delete&api_dev_key=%s&api_user_key=%s&api_paste_key=%s", _pb->devkey, _pb->userkey, _paste_id );
	debugf( "Curl postfields\n%s\n", argu );

	curl_easy_setopt( curl, CURLOPT_POSTFIELDS, argu );
	curl_easy_setopt( curl, CURLOPT_NOBODY, 0 );

	curl_easy_perform( curl );
	debugf( "Response is: %s\n", chunk.memory );

	debugf( "Exiting function\n" );

	return STATUS_OKAY;
}

char* pb_getTrendingPastes( pastebin* _pb )
{
	char* argu = (char*)malloc( sizeof(char)*1024 );
	CURL* curl = curl_easy_init();
	struct pb_memblock chunk;

	chunk.memory = malloc(1);
	chunk.size = 0;

	curl_easy_setopt( curl, CURLOPT_WRITEFUNCTION, pb_memcopy );
	curl_easy_setopt( curl, CURLOPT_WRITEDATA, (void*)&chunk );

	curl_easy_setopt( curl, CURLOPT_URL, PB_API_POST_URL );
	curl_easy_setopt( curl, CURLOPT_POST, 1 );

	sprintf( argu, "api_dev_key=%s&api_option=trends", _pb->devkey );
	debugf( "Curl postfields\n%s\n", argu );

	curl_easy_setopt( curl, CURLOPT_POSTFIELDS, argu );
	curl_easy_setopt( curl, CURLOPT_NOBODY, 0 );

	curl_easy_perform( curl );
	debugf( "Response from server:\n%s\n", chunk.memory );

	debugf( "Exiting function\n" );

	return chunk.memory;
}

char* pb_getUserPastes( pastebin* _pb, int _size )
{
	debugf( "Entering function\n" );
	if( _size < 0 || _size > 1000 )
		return NULL;
	if( _size == 0 )
		_size = 50;

	char* argu = (char*)malloc( sizeof(char)*1024 );
	CURL* curl = curl_easy_init();
	struct pb_memblock chunk;

	chunk.memory = malloc(1);
	chunk.size = 0;
	
	curl_easy_setopt( curl, CURLOPT_WRITEFUNCTION, pb_memcopy );
	curl_easy_setopt( curl, CURLOPT_WRITEDATA, (void*)&chunk );

	curl_easy_setopt( curl, CURLOPT_URL, PB_API_POST_URL );
	curl_easy_setopt( curl, CURLOPT_POST, 1 );

	sprintf( argu, "api_dev_key=%s&api_option=list&api_user_key=%s&api_results_limit=%d", _pb->devkey, _pb->userkey, _size );
	debugf( "Curl postfields\n%s\n", argu );

	curl_easy_setopt( curl, CURLOPT_POSTFIELDS, argu );
	curl_easy_setopt( curl, CURLOPT_NOBODY, 0 );

	curl_easy_perform( curl );
	debugf( "Response from the server:\n%s\n", chunk.memory );

	debugf( "Exiting function\n" );

	return chunk.memory;
}
