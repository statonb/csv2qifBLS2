#ifndef _STRING_UTILS_H_
#define _STRING_UTILS_H_

void strip_quotes(char *s);
void remove_all_quotes(char *s);
void remove_commas_dollars_and_quotes(char *s);
char *strcasestr_simple(const char *hay, const char *needle);
int iso_to_qif_date(const char *iso, char *out, size_t outsz);
#endif