#ifndef PASTEBIN_SYNTAX
#define PASTEBIN_SYNTAX

typedef enum _pb_syntax
{
    SYN_LANG_NONE = 0,
    SYN_LANG_C,
    SYN_LANG_CPP,
    SYN_LANG_JAVA,
    SYN_LANG_BASH,
    // should be the size of the enum list.
    // Everything here should correspond to
    // a string in pb_syntaxstring 
    SYN_LIST_MAX 
} pb_syntax;

extern char* pb_syntaxstring[SYN_LIST_MAX];

char* pb_getSyntaxString( pb_syntax );
// returns something from pb_syntax if found; defaults to "text"
pb_syntax pb_getSyntax( char* );

#endif /* PASTEBIN_SYNTAX */
