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
    "4CS", /* SYN_LANG_4cs */
    "6502 ACME Cross Assembler", /* SYN_LANG_6502acme */
    "6502 Kick Assembler", /* SYN_LANG_6502kickass */6502kickass */
    "6502 TASM/64TASS", /* SYN_LANG_6502tasm */
    "ABAP", /* SYN_LANG_abap */
    "ActionScript", /* SYN_LANG_actionscript */
    "ActionScript 3", /* SYN_LANG_actionscript3 */
    "Ada", /* SYN_LANG_ada */
    "ALGOL 68", /* SYN_LANG_algol68 */
    "Apache Log", /* SYN_LANG_apache */
    "AppleScript", /* SYN_LANG_applescript */
    "APT Sources", /* SYN_LANG_apt_sources */
    "ASM (NASM)", /* SYN_LANG_asm */
    "ASP", /* SYN_LANG_asp */
    "autoconf", /* SYN_LANG_autoconf */
    "Autohotkey", /* SYN_LANG_autohotkey */
    "AutoIt", /* SYN_LANG_autoit */
    "Avisynth", /* SYN_LANG_avisynth */
    "Awk", /* SYN_LANG_awk */
    "BASCOM AVR", /* SYN_LANG_bascomavr */
    "Bash", /* SYN_LANG_bash */
    "Basic4GL", /* SYN_LANG_basic4gl */
    "BibTeX", /* SYN_LANG_bibtex */
    "Blitz Basic", /* SYN_LANG_blitzbasic */
    "BNF", /* SYN_LANG_bnf */
    "BOO", /* SYN_LANG_boo */
    "BrainFuck", /* SYN_LANG_bf */
    "C", /* SYN_LANG_c */
    "C for Macs", /* SYN_LANG_c_mac */
    "C Intermediate Language", /* SYN_LANG_cil */
    "C#", /* SYN_LANG_csharp */
    "C++", /* SYN_LANG_cpp */
    "C++ (with QT extensions)", /* SYN_LANG_cpp-qt */
    "C: Loadrunner", /* SYN_LANG_c_loadrunner */
    "CAD DCL", /* SYN_LANG_caddcl */
    "CAD Lisp", /* SYN_LANG_cadlisp */
    "CFDG", /* SYN_LANG_cfdg */
    "ChaiScript", /* SYN_LANG_chaiscript */
    "Clojure", /* SYN_LANG_clojure */
    "Clone C", /* SYN_LANG_klonec */
    "Clone C++", /* SYN_LANG_klonecpp */
    "CMake", /* SYN_LANG_cmake */
    "COBOL", /* SYN_LANG_cobol */
    "CoffeeScript", /* SYN_LANG_coffeescript */
    "ColdFusion", /* SYN_LANG_cfm */
    "CSS", /* SYN_LANG_css */
    "Cuesheet", /* SYN_LANG_cuesheet */
    "D", /* SYN_LANG_d */
    "DCS", /* SYN_LANG_dcs */
    "Delphi", /* SYN_LANG_delphi */
    "Delphi Prism (Oxygene)", /* SYN_LANG_oxygene */
    "Diff", /* SYN_LANG_diff */
    "DIV", /* SYN_LANG_div */
    "DOS", /* SYN_LANG_dos */
    "DOT", /* SYN_LANG_dot */
    "E", /* SYN_LANG_e */
    "ECMAScript", /* SYN_LANG_ecmascript */
    "Eiffel", /* SYN_LANG_eiffel */
    "Email", /* SYN_LANG_email */
    "EPC", /* SYN_LANG_epc */
    "Erlang", /* SYN_LANG_erlang */
    "F#", /* SYN_LANG_fsharp */
    "Falcon", /* SYN_LANG_falcon */
    "FO Language", /* SYN_LANG_fo */
    "Formula One", /* SYN_LANG_f1 */
    "Fortran", /* SYN_LANG_fortran */
    "FreeBasic", /* SYN_LANG_freebasic */
    "FreeSWITCH", /* SYN_LANG_freeswitch */
    "GAMBAS", /* SYN_LANG_gambas */
    "Game Maker", /* SYN_LANG_gml */
    "GDB", /* SYN_LANG_gdb */
    "Genero", /* SYN_LANG_genero */
    "Genie", /* SYN_LANG_genie */
    "GetText", /* SYN_LANG_gettext */
    "Go", /* SYN_LANG_go */
    "Groovy", /* SYN_LANG_groovy */
    "GwBasic", /* SYN_LANG_gwbasic */
    "Haskell", /* SYN_LANG_haskell */
    "HicEst", /* SYN_LANG_hicest */
    "HQ9 Plus", /* SYN_LANG_hq9plus */
    "HTML", /* SYN_LANG_html4strict */
    "HTML 5", /* SYN_LANG_html5 */
    "Icon", /* SYN_LANG_icon */
    "IDL", /* SYN_LANG_idl */
    "INI file", /* SYN_LANG_ini */
    "Inno Script", /* SYN_LANG_inno */
    "INTERCAL", /* SYN_LANG_intercal */
    "IO", /* SYN_LANG_io */
    "J", /* SYN_LANG_j */
    "Java", /* SYN_LANG_java */
    "Java 5", /* SYN_LANG_java5 */
    "JavaScript", /* SYN_LANG_javascript */
    "jQuery", /* SYN_LANG_jquery */
    "KiXtart", /* SYN_LANG_kixtart */
    "Latex", /* SYN_LANG_latex */
    "Liberty BASIC", /* SYN_LANG_lb */
    "Linden Scripting", /* SYN_LANG_lsl2 */
    "Lisp", /* SYN_LANG_lisp */
    "LLVM", /* SYN_LANG_llvm */
    "Loco Basic", /* SYN_LANG_locobasic */
    "Logtalk", /* SYN_LANG_logtalk*/
    "LOL Code", /* SYN_LANG_lolcode */
    "Lotus Formulas", /* SYN_LANG_lotusformulas */
    "Lotus Script", /* SYN_LANG_lotusscript */
    "LScript", /* SYN_LANG_lscript */
    "Lua", /* SYN_LANG_lua */
    "M68000 Assembler", /* SYN_LANG_m68k */
    "MagikSF", /* SYN_LANG_magiksf */
    "Make", /* SYN_LANG_make */
    "MapBasic", /* SYN_LANG_mapbasic */
    "MatLab", /* SYN_LANG_matlab */
    "mIRC", /* SYN_LANG_mirc */
    "MIX Assembler", /* SYN_LANG_mmix */
    "Modula 2", /* SYN_LANG_modula2 */
    "Modula 3", /* SYN_LANG_modula3 */
    "Motorola 68000 HiSoft Dev", /* SYN_LANG_68000devpac */
    "MPASM", /* SYN_LANG_mpasm */
    "MXML", /* SYN_LANG_mxml */
    "MySQL", /* SYN_LANG_mysql */
    "newLISP", /* SYN_LANG_newlisp */
    "NullSoft Installer", /* SYN_LANG_nsis */
    "Oberon 2", /* SYN_LANG_oberon2 */
    "Objeck Programming Langua", /* SYN_LANG_objeck */
    "Objective C", /* SYN_LANG_objc */
    "OCalm Brief", /* SYN_LANG_ocaml-brief */
    "OCaml", /* SYN_LANG_ocaml*/
    "OpenBSD PACKET FILTER", /* SYN_LANG_pf */
    "OpenGL Shading", /* SYN_LANG_glsl */
    "Openoffice BASIC", /* SYN_LANG_oobas */
    "Oracle 11", /* SYN_LANG_oracle11 */
    "Oracle 8", /* SYN_LANG_oracle8 */
    "Oz", /* SYN_LANG_oz */
    "Pascal", /* SYN_LANG_pascal */
    "PAWN", /* SYN_LANG_pawn */
    "PCRE", /* SYN_LANG_pcre */
    "Per", /* SYN_LANG_per */
    "Perl", /* SYN_LANG_perl */
    "Perl 6", /* SYN_LANG_perl6 */
    "PHP", /* SYN_LANG_php */
    "PHP Brief", /* SYN_LANG_php-brief */
    "Pic 16", /* SYN_LANG_pic16 */
    "Pike", /* SYN_LANG_pike */
    "Pixel Bender", /* SYN_LANG_pixelbender */
    "PL/SQL", /* SYN_LANG_plsql */
    "PostgreSQL", /* SYN_LANG_postgresql */
    "POV-Ray", /* SYN_LANG_povray */
    "Power Shell", /* SYN_LANG_powershell */
    "PowerBuilder", /* SYN_LANG_powerbuilder */
    "ProFTPd", /* SYN_LANG_proftpd */
    "Progress", /* SYN_LANG_progress */
    "Prolog", /* SYN_LANG_prolog */
    "Properties", /* SYN_LANG_properties */
    "ProvideX", /* SYN_LANG_providex */
    "PureBasic", /* SYN_LANG_purebasic */
    "PyCon", /* SYN_LANG_pycon */
    "Python", /* SYN_LANG_python */
    "q/kdb+", /* SYN_LANG_q */
    "QBasic", /* SYN_LANG_qbasic */
    "R", /* SYN_LANG_rsplus */
    "Rails*/", /* SYN_LANG_rails */
    "REBOL", /* SYN_LANG_rebol */
    "REG", /* SYN_LANG_reg */
    "Robots", /* SYN_LANG_robots */
    "RPM Spec", /* SYN_LANG_rpmspec */
    "Ruby", /* SYN_LANG_ruby */
    "Ruby Gnuplot", /* SYN_LANG_gnuplot */
    "SAS", /* SYN_LANG_sas */
    "Scala", /* SYN_LANG_scala */
    "Scheme", /* SYN_LANG_scheme */
    "Scilab", /* SYN_LANG_scilab */
    "SdlBasic", /* SYN_LANG_sdlbasic */
    "Smalltalk", /* SYN_LANG_smalltalk */
    "Smarty", /* SYN_LANG_smarty */
    "SQL", /* SYN_LANG_sql */
    "SystemVerilog", /* SYN_LANG_systemverilog */
    "T-SQL", /* SYN_LANG_tsql */
    "TCL", /* SYN_LANG_tcl */
    "Tera Term", /* SYN_LANG_teraterm */
    "thinBasic", /* SYN_LANG_thinbasic */
    "TypoScript", /* SYN_LANG_typoscript */
    "Unicon", /* SYN_LANG_unicon */
    "UnrealScript", /* SYN_LANG_uscript */
    "Vala", /* SYN_LANG_vala */
    "VB.NET", /* SYN_LANG_vbnet */
    "VeriLog",/* SYN_LANG_verilog */
    "VHDL", /* SYN_LANG_vhdl */
    "VIM", /* SYN_LANG_vim */
    "Visual Pro Log", /* SYN_LANG_visualprolog */
    "VisualBasic", /* SYN_LANG_vb */
    "VisualFoxPro", /* SYN_LANG_visualfoxpro */
    "WhiteSpace", /* SYN_LANG_whitespace */
    "WHOIS", /* SYN_LANG_whois */
    "Winbatch", /* SYN_LANG_winbatch */
    "XBasic", /* SYN_LANG_xbasic */
    "XML", /* SYN_LANG_xml */
    "Xorg Config", /* SYN_LANG_xorg_conf */
    "XPP", /* SYN_LANG_xpp */
    "YAML", /* SYN_LANG_yaml */
    "Z80 Assembler", /* SYN_LANG_z80 */
    "ZXBasic", /* SYN_LANG_zxbasic */
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
