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
#ifdef __cplusplus
extern "C" {
#endif

#include <rsh/pastebin_syntax.h>
#include <rsh/debug.h>
#include <string.h>

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
	"sql",           /* SYN_LANG_SQL           */
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

char* pb_syntaxstringdesc[SYN_LIST_MAX] = 
{
	"None",                      /* SYN_LANG_NONE          */
	"4CS",                       /* SYN_LANG_4CS           */
	"6502 ACME Cross Assembler", /* SYN_LANG_6502ACME      */
	"6502 Kick Assembler",       /* SYN_LANG_6502KICKASS   */
	"6502 TASM/64TASS",          /* SYN_LANG_6502TASM      */
	"ABAP",                      /* SYN_LANG_ABAP          */
	"ActionScript",              /* SYN_LANG_ACTIONSCRIPT  */
	"ActionScript 3",            /* SYN_LANG_ACTIONSCRIPT3 */
	"Ada",                       /* SYN_LANG_ADA           */
	"ALGOL 68",                  /* SYN_LANG_ALGOL68       */
	"Apache Log",                /* SYN_LANG_APACHE        */
	"AppleScript",               /* SYN_LANG_APPLESCRIPT   */
	"APT Sources",               /* SYN_LANG_APT_SOURCES   */
	"ASM (NASM)",                /* SYN_LANG_ASM           */
	"ASP",                       /* SYN_LANG_ASP           */
	"autoconf",                  /* SYN_LANG_AUTOCONF      */
	"Autohotkey",                /* SYN_LANG_AUTOHOTKEY    */
	"AutoIt",                    /* SYN_LANG_AUTOIT        */
	"Avisynth",                  /* SYN_LANG_AVISYNTH      */
	"Awk",                       /* SYN_LANG_AWK           */
	"BASCOM AVR",                /* SYN_LANG_BASCOMAVR     */
	"Bash",                      /* SYN_LANG_BASH          */
	"Basic4GL",                  /* SYN_LANG_BASIC4GL      */
	"BibTeX",                    /* SYN_LANG_BIBTEX        */
	"Blitz Basic",               /* SYN_LANG_BLITZBASIC    */
	"BNF",                       /* SYN_LANG_BNF           */
	"BOO",                       /* SYN_LANG_BOO           */
	"BrainFuck",                 /* SYN_LANG_BF            */
	"C",                         /* SYN_LANG_C             */
	"C for Macs",                /* SYN_LANG_C_MAC         */
	"C Intermediate Language",   /* SYN_LANG_CIL           */
	"C#",                        /* SYN_LANG_CSHARP        */
	"C++",                       /* SYN_LANG_CPP           */
	"C++ (with QT extensions)",  /* SYN_LANG_CPP-QT        */
	"C: Loadrunner",             /* SYN_LANG_C_LOADRUNNER  */
	"CAD DCL",                   /* SYN_LANG_CADDCL        */
	"CAD Lisp",                  /* SYN_LANG_CADLISP       */
	"CFDG",                      /* SYN_LANG_CFDG          */
	"ChaiScript",                /* SYN_LANG_CHAISCRIPT    */
	"Clojure",                   /* SYN_LANG_CLOJURE       */
	"Clone C",                   /* SYN_LANG_KLONEC        */
	"Clone C++",                 /* SYN_LANG_KLONECPP      */
	"CMake",                     /* SYN_LANG_CMAKE         */
	"COBOL",                     /* SYN_LANG_COBOL         */
	"CoffeeScript",              /* SYN_LANG_COFFEESCRIPT  */
	"ColdFusion",                /* SYN_LANG_CFM           */
	"CSS",                       /* SYN_LANG_CSS           */
	"Cuesheet",                  /* SYN_LANG_CUESHEET      */
	"D",                         /* SYN_LANG_D             */
	"DCS",                       /* SYN_LANG_DCS           */
	"Delphi",                    /* SYN_LANG_DELPHI        */
	"Delphi Prism (Oxygene)",    /* SYN_LANG_OXYGENE       */
	"Diff",                      /* SYN_LANG_DIFF          */
	"DIV",                       /* SYN_LANG_DIV           */
	"DOS",                       /* SYN_LANG_DOS           */
	"DOT",                       /* SYN_LANG_DOT           */
	"E",                         /* SYN_LANG_E             */
	"ECMAScript",                /* SYN_LANG_ECMASCRIPT    */
	"Eiffel",                    /* SYN_LANG_EIFFEL        */
	"Email",                     /* SYN_LANG_EMAIL         */
	"EPC",                       /* SYN_LANG_EPC           */
	"Erlang",                    /* SYN_LANG_ERLANG        */
	"F#",                        /* SYN_LANG_FSHARP        */
	"Falcon",                    /* SYN_LANG_FALCON        */
	"FO Language",               /* SYN_LANG_FO            */
	"Formula One",               /* SYN_LANG_F1            */
	"Fortran",                   /* SYN_LANG_FORTRAN       */
	"FreeBasic",                 /* SYN_LANG_FREEBASIC     */
	"FreeSWITCH",                /* SYN_LANG_FREESWITCH    */
	"GAMBAS",                    /* SYN_LANG_GAMBAS        */
	"Game Maker",                /* SYN_LANG_GML           */
	"GDB",                       /* SYN_LANG_GDB           */
	"Genero",                    /* SYN_LANG_GENERO        */
	"Genie",                     /* SYN_LANG_GENIE         */
	"GetText",                   /* SYN_LANG_GETTEXT       */
	"Go",                        /* SYN_LANG_GO            */
	"Groovy",                    /* SYN_LANG_GROOVY        */
	"GwBasic",                   /* SYN_LANG_GWBASIC       */
	"Haskell",                   /* SYN_LANG_HASKELL       */
	"HicEst",                    /* SYN_LANG_HICEST        */
	"HQ9 Plus",                  /* SYN_LANG_HQ9PLUS       */
	"HTML",                      /* SYN_LANG_HTML4STRICT   */
	"HTML 5",                    /* SYN_LANG_HTML5         */
	"Icon",                      /* SYN_LANG_ICON          */
	"IDL",                       /* SYN_LANG_IDL           */
	"INI file",                  /* SYN_LANG_INI           */
	"Inno Script",               /* SYN_LANG_INNO          */
	"INTERCAL",                  /* SYN_LANG_INTERCAL      */
	"IO",                        /* SYN_LANG_IO            */
	"J",                         /* SYN_LANG_J             */
	"Java",                      /* SYN_LANG_JAVA          */
	"Java 5",                    /* SYN_LANG_JAVA5         */
	"JavaScript",                /* SYN_LANG_JAVASCRIPT    */
	"jQuery",                    /* SYN_LANG_JQUERY        */
	"KiXtart",                   /* SYN_LANG_KIXTART       */
	"Latex",                     /* SYN_LANG_LATEX         */
	"Liberty BASIC",             /* SYN_LANG_LB            */
	"Linden Scripting",          /* SYN_LANG_LSL2          */
	"Lisp",                      /* SYN_LANG_LISP          */
	"LLVM",                      /* SYN_LANG_LLVM          */
	"Loco Basic",                /* SYN_LANG_LOCOBASIC     */
	"Logtalk",                   /* SYN_LANG_LOGTALK       */
	"LOL Code",                  /* SYN_LANG_LOLCODE       */
	"Lotus Formulas",            /* SYN_LANG_LOTUSFORMULAS */
	"Lotus Script",              /* SYN_LANG_LOTUSSCRIPT   */
	"LScript",                   /* SYN_LANG_LSCRIPT       */
	"Lua",                       /* SYN_LANG_LUA           */
	"M68000 Assembler",          /* SYN_LANG_M68K          */
	"MagikSF",                   /* SYN_LANG_MAGIKSF       */
	"Make",                      /* SYN_LANG_MAKE          */
	"MapBasic",                  /* SYN_LANG_MAPBASIC      */
	"MatLab",                    /* SYN_LANG_MATLAB        */
	"mIRC",                      /* SYN_LANG_MIRC          */
	"MIX Assembler",             /* SYN_LANG_MMIX          */
	"Modula 2",                  /* SYN_LANG_MODULA2       */
	"Modula 3",                  /* SYN_LANG_MODULA3       */
	"Motorola 68000 HiSoft Dev", /* SYN_LANG_68000DEVPAC   */
	"MPASM",                     /* SYN_LANG_MPASM         */
	"MXML",                      /* SYN_LANG_MXML          */
	"MySQL",                     /* SYN_LANG_MYSQL         */
	"newLISP",                   /* SYN_LANG_NEWLISP       */
	"NullSoft Installer",        /* SYN_LANG_NSIS          */
	"Oberon 2",                  /* SYN_LANG_OBERON2       */
	"Objeck Programming Langua", /* SYN_LANG_OBJECK        */
	"Objective C",               /* SYN_LANG_OBJC          */
	"OCalm Brief",               /* SYN_LANG_OCAML-BRIEF   */
	"OCaml",                     /* SYN_LANG_OCAML         */
	"OpenBSD PACKET FILTER",     /* SYN_LANG_PF            */
	"OpenGL Shading",            /* SYN_LANG_GLSL          */
	"Openoffice BASIC",          /* SYN_LANG_OOBAS         */
	"Oracle 11",                 /* SYN_LANG_ORACLE11      */
	"Oracle 8",                  /* SYN_LANG_ORACLE8       */
	"Oz",                        /* SYN_LANG_OZ            */
	"Pascal",                    /* SYN_LANG_PASCAL        */
	"PAWN",                      /* SYN_LANG_PAWN          */
	"PCRE",                      /* SYN_LANG_PCRE          */
	"Per",                       /* SYN_LANG_PER           */
	"Perl",                      /* SYN_LANG_PERL          */
	"Perl 6",                    /* SYN_LANG_PERL6         */
	"PHP",                       /* SYN_LANG_PHP           */
	"PHP Brief",                 /* SYN_LANG_PHP-BRIEF     */
	"Pic 16",                    /* SYN_LANG_PIC16         */
	"Pike",                      /* SYN_LANG_PIKE          */
	"Pixel Bender",              /* SYN_LANG_PIXELBENDER   */
	"PL/SQL",                    /* SYN_LANG_PLSQL         */
	"PostgreSQL",                /* SYN_LANG_POSTGRESQL    */
	"POV-Ray",                   /* SYN_LANG_POVRAY        */
	"Power Shell",               /* SYN_LANG_POWERSHELL    */
	"PowerBuilder",              /* SYN_LANG_POWERBUILDER  */
	"ProFTPd",                   /* SYN_LANG_PROFTPD       */
	"Progress",                  /* SYN_LANG_PROGRESS      */
	"Prolog",                    /* SYN_LANG_PROLOG        */
	"Properties",                /* SYN_LANG_PROPERTIES    */
	"ProvideX",                  /* SYN_LANG_PROVIDEX      */
	"PureBasic",                 /* SYN_LANG_PUREBASIC     */
	"PyCon",                     /* SYN_LANG_PYCON         */
	"Python",                    /* SYN_LANG_PYTHON        */
	"q/kdb+",                    /* SYN_LANG_Q             */
	"QBasic",                    /* SYN_LANG_QBASIC        */
	"R",                         /* SYN_LANG_RSPLUS        */
	"Rails",                     /* SYN_LANG_RAILS         */
	"REBOL",                     /* SYN_LANG_REBOL         */
	"REG",                       /* SYN_LANG_REG           */
	"Robots",                    /* SYN_LANG_ROBOTS        */
	"RPM Spec",                  /* SYN_LANG_RPMSPEC       */
	"Ruby",                      /* SYN_LANG_RUBY          */
	"Ruby Gnuplot",              /* SYN_LANG_GNUPLOT       */
	"SAS",                       /* SYN_LANG_SAS           */
	"Scala",                     /* SYN_LANG_SCALA         */
	"Scheme",                    /* SYN_LANG_SCHEME        */
	"Scilab",                    /* SYN_LANG_SCILAB        */
	"SdlBasic",                  /* SYN_LANG_SDLBASIC      */
	"Smalltalk",                 /* SYN_LANG_SMALLTALK     */
	"Smarty",                    /* SYN_LANG_SMARTY        */
	"SQL",                       /* SYN_LANG_SQL           */
	"SystemVerilog",             /* SYN_LANG_SYSTEMVERILOG */
	"T-SQL",                     /* SYN_LANG_TSQL          */
	"TCL",                       /* SYN_LANG_TCL           */
	"Tera Term",                 /* SYN_LANG_TERATERM      */
	"thinBasic",                 /* SYN_LANG_THINBASIC     */
	"TypoScript",                /* SYN_LANG_TYPOSCRIPT    */
	"Unicon",                    /* SYN_LANG_UNICON        */
	"UnrealScript",              /* SYN_LANG_USCRIPT       */
	"Vala",                      /* SYN_LANG_VALA          */
	"VB.NET",                    /* SYN_LANG_VBNET         */
	"VeriLog",                   /* SYN_LANG_VERILOG       */
	"VHDL",                      /* SYN_LANG_VHDL          */
	"VIM",                       /* SYN_LANG_VIM           */
	"Visual Pro Log",            /* SYN_LANG_VISUALPROLOG  */
	"VisualBasic",               /* SYN_LANG_VB            */
	"VisualFoxPro",              /* SYN_LANG_VISUALFOXPRO  */
	"WhiteSpace",                /* SYN_LANG_WHITESPACE    */
	"WHOIS",                     /* SYN_LANG_WHOIS         */
	"Winbatch",                  /* SYN_LANG_WINBATCH      */
	"XBasic",                    /* SYN_LANG_XBASIC        */
	"XML",                       /* SYN_LANG_XML           */
	"Xorg Config",               /* SYN_LANG_XORG_CONF     */
	"XPP",                       /* SYN_LANG_XPP           */
	"YAML",                      /* SYN_LANG_YAML          */
	"Z80 Assembler",             /* SYN_LANG_Z80           */
	"ZXBasic"                    /* SYN_LANG_ZXBASIC       */
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

#ifdef __cplusplus
}
#endif
