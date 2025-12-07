#include <stdio.h>
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

// Remove all characters from a number field
void remove_commas_dollars_and_quotes(char *s) {
    char *dst = s, *src = s;
    while (*src) {
        if  (   (*src != ',')
             && (*src != '$')
             && (*src != '"')
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

int iso_to_qif_date(const char *iso, char *out, size_t outsz)
{
    // Input:  "2025-11-28"
    // Output: "11/28/2025"
    // Returns 1 on success, 0 on invalid input.
    int year, month, day;

    if (!iso || strlen(iso) < 10)
        return 0;

    // Expect YYYY-MM-DD exactly in fixed positions.
    if (!isdigit(iso[0]) || !isdigit(iso[1]) ||
        !isdigit(iso[2]) || !isdigit(iso[3]) ||
        iso[4] != '-' ||
        !isdigit(iso[5]) || !isdigit(iso[6]) ||
        iso[7] != '-' ||
        !isdigit(iso[8]) || !isdigit(iso[9]))
        return 0;

    year  = (iso[0]-'0')*1000 + (iso[1]-'0')*100 + (iso[2]-'0')*10 + (iso[3]-'0');
    month = (iso[5]-'0')*10   + (iso[6]-'0');
    day   = (iso[8]-'0')*10   + (iso[9]-'0');

    if (month < 1 || month > 12 || day < 1 || day > 31)
        return 0;

    // Output in MM/DD/YYYY format.
    // Make sure out has space for at least 11 bytes including NUL.
    if (outsz < 11)
        return 0;

    snprintf(out, outsz, "%02d/%02d/%04d", month, day, year);
    return 1;
}
