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
    "4CS", /* SYN_LANG_4CS */
    "6502acme", /* SYN_LANG_6502acme */
    "6502kickass", /* SYN_LANG_6502kickass */
    "6502tasm", /* SYN_LANG_6502tasm */
    "ABAP" /* SYN_LANG_abap */
    "ACTIONSCRIPT", /* SYN_LANG_actionscript */
    "ACTIONSCRIPT3", /* SYN_LANG_actionscript3 */
    "ADA", /* SYN_LANG_ada */
    "algol68", /* SYN_LANG_algol68 */
    "APACHE", /* SYN_LANG_apache */
    "APPLESCRIPT", /* SYN_LANG_applescript */
    "APT_SOURCES", /* SYN_LANG_apt_sources */
    "ASM", /* SYN_LANG_asm */
    "ASP", /*SYN_LANG_asp */
    "AUTOCONF", /* SYN_LANG_autoconf */
    "AUTOHOTKEY", /* SYN_LANG_autohotkey */
    "AUTOIT", /* SYN_LANG_autoit */
    "AVISYNTH", /* SYN_LANG_avisynth */
    "AWK", /* SYN_LANG_awk */
    "BASCONAVR", /* SYN_LANG_bascomavr */
    "BASH", /* SYN_LANG_bash */
    "BASIC4GL", /* SYN_LANG_basic4gl */
    "BIBTEX", /* SYN_LANG_bibtex */
    "BLITZBASIC", /* SYN_LANG_blitzbasic */
    "BNF", /* SYN_LANG_bnf */
    "BOO", /* SYN_LANG_boo */
    "BF", /* SYN_LANG_bf */
    "C", /* SYN_LANG_c */
    "C_MAC", /* SYN_LANG_c_mac */
    "CIL" /* SYN_LANG_cil */
    "CSHARP", /* SYN_LANG_csharp */
    "CCP", /* SYN_LANG_cpp */
    "CPP-QT", /* SYN_LANG_cpp-qt */
    "C_LOADRUNNER", /*SYN_LANG_c_loadrunner */
    "CADDCL", /* SYN_LANG_caddcl */
    "CADSLIP", /* SYN_LANG_cadlisp */
    "cfdg" /*SYN_LANG_cfdg */
    "chaiscript", /* SYN_LANG_chaiscript */
    "clojure", /* SYN_LANG_clojure */
    "klonec", /* SYN_LANG_klonec */
    "klonecpp", /* SYN_LANG_klonecpp */
    "cmake", /* SYN_LANG_cmake */
    "cobol", /* SYN_LANG_cobol */
    "coffeescript" /* SYN_LANG_coffeescript */
    "cfm", /*SYN_LANG_cfm */
    "css", /* SYN_LANG_css */
    "cuesheet", /* SYN_LANG_cuesheet */
    "d", /* SYN_LANG_d */
    "dcs", /* SYN_LANG_dcs */
    "delphi", /* SYN_LANG_delphi */
    "oxygene", /* SYN_LANG_oxygene */
    "DIFF", /* SYN_LANG_diff */
    "div", /* SYN_LANG_div */
    "dos", /* SYN_LANG_dos */
    "dot", /* SYN_LANG_dot */
    "e", /* SYN_LANG_e */
    "ecmascript", /* SYN_LANG_ecmascript */
    "eiffel", /* SYN_LANG_eiffel */
    "email" /* SYN_LANG_email */
    "epc", /* SYN_LANG_epc */
    "erlang", /* SYN_LANG_erlang */
    "fsharp", /* SYN_LANG_fsharp */
    "falcon", /* SYN_LANG_falcon */
    "fo", /* SYN_LANG_fo */
    "f1", /* SYN_LANG_f1 */
    "fortran", /* SYN_LANG_fortran */
    "FREEBASIC", /* SYN_LANG_freebasic */
    "FREESWITCH", /* SYN_LANG_freeswitch */
    "GAMBAS", /* SYN_LANG_gambas */
    "GML", /* SYN_LANG_gml */
    "gdb", /* SYN_LANG_gdb */
    "genero", /* SYN_LANG_genero */
    "genie", /* SYN_LANG_genie */
    "gettext", /* SYN_LANG_gettext */
    "go", /* SYN_LANG_go */
    "groovy", /* SYN_LANG_groovy */
    "gwbasic", /* SYN_LANG_gwbasic */
    "haskell", /* SYN_LANG_haskell */
    "hicest", /* SYN_LANG_hicest */
    "hq9plus", /* SYN_LANG_hq9plus */
    "html4strict", /* SYN_LANG_html4strict */
    "html5", /* SYN_LANG_html5 */
    "icon", /* SYN_LANG_icon */
    "idl", /* SYN_LANG_idl */
    "ini", /* SYN_LANG_ini */
    "inno", /* SYN_LANG_inno */
    "intercal", /* SYN_LANG_intercal */
    "io", /* SYN_LANG_io */
    "j", /* SYN_LANG_j */
    "java", /* SYN_LANG_java */
    "java5", /* SYN_LANG_java5 */
    "javascript", /* SYN_LANG_javascript */
    "jquery", /* SYN_LANG_jquery */
    "kixtart", /* SYN_LANG_kixtart */
    "latex", /* SYN_LANG_latex */
    "lb", /* SYN_LANG_lb */
    "lsl2", /* SYN_LANG_lsl2 */
    "lisp", /* SYN_LANG_lisp */
    "llvm", /* SYN_LANG_llvm */
    "locobasic", /* SYN_LANG_locobasic */
    "logtalk" /* SYN_LANG_logtalk */
    "lolcode", /* SYN_LANG_lolcode */
    "lotusformulas", /* SYN_LANG_lotusformulas */
    "lotusscript", /* SYN_LANG_lotusscript */
    "lscript", /* SYN_LANG_lscript */
    "lua", /* SYN_LANG_lua */
    "m68k", /* SYN_LANG_m68k */
    "magiksf", /* SYN_LANG_magiksf */
    "make", /* SYN_LANG_make */
    "mapbasic", /* SYN_LANG_mapbasic */
    "matlab", /* SYN_LANG_matlab */
    "mirc", /* SYN_LANG_mirc */
    "mmix", /* SYN_LANG_mmix */
    "modula2", /* SYN_LANG_modula2 */
    "modula3", /* SYN_LANG_modula3 */
    "68000devpac", /* SYN_LANG_68000devpac */
    "mpasm", /* SYN_LANG_mpasm */
    "mxml", /* SYN_LANG_mxml */
    "mysql", /* SYN_LANG_mysql */
    "newlisp", /* SYN_LANG_newlisp */
    "nsis", /* SYN_LANG_nsis */
    "oberon2", /* SYN_LANG_oberon2 */
    "objeck", /* SYN_LANG_objeck */
    "objc", /* SYN_LANG_objc */
    "ocaml-brief", /* SYN_LANG_ocaml-brief */
    "ocaml", /* SYN_LANG_ocaml */
    "pf", /* SYN_LANG_pf */
    "glsl", /* SYN_LANG_glsl */
    "oobas", /* SYN_LANG_oobas */
    "oracle11", /* SYN_LANG_oracle11 */
    "oracle8", /* SYN_LANG_oracle8 */
    "oz", /* SYN_LANG_oz */
    "pascal", /* SYN_LANG_pascal */
    "pawn", /* SYN_LANG_pawn */
    "pcre", /* SYN_LANG_pcre */
    "per", /* SYN_LANG_per */
    "perl", /* SYN_LANG_perl */
    "perl6", /* SYN_LANG_perl6 */
    "php", /* SYN_LANG_php */
    "php-brief", /* SYN_LANG_php-brief */
    "pic16", /* SYN_LANG_pic16 */
    "pike", /* SYN_LANG_pike */
    "pixelbender", /* SYN_LANG_pixelbender */
    "plsql", /* SYN_LANG_plsql */
    "postgresql", /* SYN_LANG_postgresql */
    "povray", /* SYN_LANG_povray */
    "powershell", /* SYN_LANG_powershell */
    "powerbuilder", /* SYN_LANG_powerbuilder */
    "proftpd", /* SYN_LANG_proftpd */
    "progress", /* SYN_LANG_progress */
    "prolog", /* SYN_LANG_prolog */
    "properties", /* SYN_LANG_properties */
    "providex", /* SYN_LANG_providex */
    "purebasic", /* SYN_LANG_purebasic */
    "pycon", /* SYN_LANG_pycon */
    "python", /* SYN_LANG_python */
    "q", /* SYN_LANG_q */
    "qbasic", /* SYN_LANG_qbasic */
    "rsplus", /* SYN_LANG_rsplus */
    "rails", /* SYN_LANG_rails */
    "rebol", /* SYN_LANG_rebol */
    "reg", /* SYN_LANG_reg */
    "robots", /* SYN_LANG_robots */
    "rpmspec", /* SYN_LANG_rpmspec */
    "ruby", /* SYN_LANG_ruby */
    "gnuplot", /* SYN_LANG_gnuplot */
    "sas", /* SYN_LANG_sas */
    "scala", /* SYN_LANG_scala */
    "scheme", /* SYN_LANG_scheme */
    "scilab", /* SYN_LANG_scilab */
    "sdlbasic", /* SYN_LANG_sdlbasic */
    "smalltalk", /* SYN_LANG_smalltalk */
    "smarty", /* SYN_LANG_smarty */
    "squl", /* SYN_LANG_sql */
    "systemverilog", /* SYN_LANG_systemverilog */
    "tsql", /* SYN_LANG_tsql */
    "tcl", /* SYN_LANG_tcl */
    "teraterm", /* SYN_LANG_teraterm */
    "thinbasic", /* SYN_LANG_thinbasic */
    "typoscript", /* SYN_LANG_typoscript */
    "unicon", /* SYN_LANG_unicon */
    "uscript", /* SYN_LANG_uscript */
    "vala", /* SYN_LANG_vala */
    "vbnet", /* SYN_LANG_vbnet */
    "verilog", /* SYN_LANG_verilog */
    "vhdl", /* SYN_LANG_vhdl */
    "vim", /* SYN_LANG_vim */
    "visualprolog", /* SYN_LANG_visualprolog */
    "vb", /* SYN_LANG_vb */
    "visualfoxpro", /* SYN_LANG_visualfoxpro */
    "whitespace", /* SYN_LANG_whitespace */
    "whois", /* SYN_LANG_whois */
    "winbatch", /* SYN_LANG_winbatch */
    "xbasic", /* SYN_LANG_xbasic */
    "xml", /* SYN_LANG_xml */
    "xorg_conf", /* SYN_LANG_xorg_conf */
    "xpp", /* SYN_LANG_xpp */
    "yaml", /* SYN_LANG_yaml */
    "z80", /* SYN_LANG_z80 */
    "zxbasic", /* SYN_LANG_zxbasic */
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
