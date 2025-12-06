#include <string.h>
#include <ctype.h>

// Remove surrounding quotes from a field, if present
void strip_quotes(char *s) {
    size_t len = strlen(s);
    if (len >= 2 && s[0] == '"' && s[len-1] == '"') {
        memmove(s, s+1, len-2);
        s[len-2] = '\0';
    }
}

// Remove all quotes from a line.
// This will remove quotes from within a field
// so only do this when trying to find the
// column header line 
void remove_all_quotes(char *s) {
    char *dst = s, *src = s;
    while (*src) {
        if  (*src != '"')
        {
            *dst++ = *src;
        }
        src++;
    }
    *dst = '\0';
}

// Remove all commas from a number field
void remove_commas_and_dollars(char *s) {
    char *dst = s, *src = s;
    while (*src) {
        if  (   (*src != ',')
             && (*src != '$')
            )
        {
            *dst++ = *src;
        }
        src++;
    }
    *dst = '\0';
}

char *strcasestr_simple(const char *hay, const char *needle) {
    size_t nlen = strlen(needle);
    if (nlen == 0) return (char *)hay;
    for (; *hay; hay++) {
        if (tolower((unsigned char)*hay) == tolower((unsigned char)*needle)) {
            if (strncasecmp(hay, needle, nlen) == 0) return (char *)hay;
        }
    }
    return NULL;
}

