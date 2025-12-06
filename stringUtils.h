#ifndef _STRING_UTILS_H_
#define _STRING_UTILS_H_

void strip_quotes(char *s);
void remove_all_quotes(char *s);
void remove_commas_and_dollars(char *s);
char *strcasestr_simple(const char *hay, const char *needle);
#endif