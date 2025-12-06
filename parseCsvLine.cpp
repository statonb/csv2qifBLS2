#include <string.h>
#include "parseCsvLine.h"

// Parse a CSV line into fields handling quoted fields and empty fields.
// Returns number of fields parsed (up to max_fields).
int parse_csv_line(const char *line, char fields[][MAX_LINE], int max_fields) {
    int fi = 0;
    const char *p = line;

    while (*p != '\0' && fi < max_fields) {
        char *out = fields[fi];
        int o = 0;

        if (*p == '"') {
            // Quoted field
            p++; // skip opening quote
            while (*p != '\0') {
                if (*p == '"') {
                    // Handle escaped double quotes
                    if (*(p+1) == '"') {
                        out[o++] = '"';
                        p += 2;
                        continue;
                    }
                    p++; // closing quote
                    break;
                }
                out[o++] = *p++;
            }
            // Skip until comma or end
            while (*p != '\0' && *p != ',') p++;
            if (*p == ',') p++;
        } else {
            // Unquoted field
            while (*p != '\0' && *p != ',') {
                out[o++] = *p++;
            }
            if (*p == ',') p++;
        }

        out[o] = '\0';
        fi++;
    }

    // Handle trailing commas meaning empty fields
    int len = strlen(line);
    for (int i = len - 1; i >= 0; i--) {
        if (line[i] == ',') {
            if (fi < max_fields) {
                fields[fi][0] = '\0';
                fi++;
            }
        } else break;
    }

    return fi;
}
