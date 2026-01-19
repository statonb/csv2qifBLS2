#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdarg.h>
#include <string.h>
#include <ctype.h>
#include <getopt.h>
#include <unordered_map>
#include "fileProcessor.h"
#include "stringUtils.h"

const char *SW_VERSION =    "2.08";
const char *SW_DATE =       "2026-01-18";

typedef enum
{
    UNKNOWN_BANK_FORMAT
    , ALLY_FORMAT
    , AMEX_FORMAT
    , BOA_FORMAT
    , CAP1_FORMAT
    , CITI_FORMAT
    , DISCOVER_BANK_FORMAT
    , FIDELITY_FORMAT
    , FNBO_FORMAT
    , SCHWAB_BANK_FORMAT
    , SCHWAB_BROKERAGE_FORMAT
    , TFCU_FORMAT
}   bankFormat_t;


AllyFileProcessor               theAllyFileProcessor;
AmexFileProcessor               theAmexFileProcessor;
BoAFileProcessor                theBoAFileProcessor;
Cap1FileProcessor               theCap1FileProcessor;
CitiFileProcessor               theCitiFileProcessor;
DiscoverBankFileProcessor       theDiscoverBankFileProcessor;
FidelityFileProcessor           theFidelityFileProcessor;
FNBOFileProcessor               theFNBOFileProcessor;
SchwabBankFileProcessor         theSchwabBankFileProcessor;
SchwabBrokerageFileProcessor    theSchwabBrokerageFileProcessor;
TFCUFileProcessor               theTFCUFileProcessor;

std::unordered_map<bankFormat_t, FileProcessor *> bankMap =
{
    {ALLY_FORMAT,               &theAllyFileProcessor}
    ,{AMEX_FORMAT,              &theAmexFileProcessor}
    ,{BOA_FORMAT,               &theBoAFileProcessor}
    ,{CAP1_FORMAT,              &theCap1FileProcessor}
    ,{CITI_FORMAT,              &theCitiFileProcessor}
    ,{DISCOVER_BANK_FORMAT,     &theDiscoverBankFileProcessor}
    ,{FIDELITY_FORMAT,          &theFidelityFileProcessor}
    ,{FNBO_FORMAT,              &theFNBOFileProcessor}
    ,{SCHWAB_BANK_FORMAT,       &theSchwabBankFileProcessor}
    ,{SCHWAB_BROKERAGE_FORMAT,  &theSchwabBrokerageFileProcessor}
    ,{TFCU_FORMAT,              &theTFCUFileProcessor}
};


bankFormat_t string2bankFormat(const char *s)
{
    bankFormat_t    ret = UNKNOWN_BANK_FORMAT;

    if (strcasestr_simple(s, "boa"))
    {
        ret = BOA_FORMAT;
    }
    else if (strcasestr_simple(s, "ally"))
    {
        ret = ALLY_FORMAT;
    }
    else if (strcasestr_simple(s, "amex"))
    {
        ret = AMEX_FORMAT;
    }
    else if (strcasestr_simple(s, "cap1"))
    {
        ret = CAP1_FORMAT;
    }
    else if (strcasestr_simple(s, "citi"))
    {
        ret = CITI_FORMAT;
    }
    else if (strcasestr_simple(s, "disc"))
    {
        ret = DISCOVER_BANK_FORMAT;
    }
    else if (strcasestr_simple(s, "fid"))
    {
        ret = FIDELITY_FORMAT;
    }
    else if (strcasestr_simple(s, "fnbo"))
    {
        ret = FNBO_FORMAT;
    }
    else if (strcasestr_simple(s, "schwabbank"))
    {
        ret = SCHWAB_BANK_FORMAT;
    }
    else if (strcasestr_simple(s, "schwabbrok"))
    {
        ret = SCHWAB_BROKERAGE_FORMAT;
    }
    else if (strcasestr_simple(s, "tfcu"))
    {
        ret = TFCU_FORMAT;
    }
    else
    {
        ret = UNKNOWN_BANK_FORMAT;
    }
    return ret;
}

void usage(const char *prog, const char *extraLine = (const char *)(NULL));

void usage(const char *prog, const char *extraLine)
{
    fprintf(stderr, "%s Ver %s %s\n", prog, SW_VERSION, SW_DATE);
    fprintf(stderr, "usage: %s <options>\n", prog);
    fprintf(stderr, "-i --input filename       input .csv file.\n");
    fprintf(stderr, "                          Extension will be added if not provided.\n");
    fprintf(stderr, "-o --output filename      output .qif file.\n");
    fprintf(stderr, "                          Filename will be generated from input filename\n");
    fprintf(stderr, "                          if not provided.\n");
    fprintf(stderr, "-f --format Bank          Different banks format CSV files differently.\n");
    fprintf(stderr, "                          Possible selections are as follows:\n");
    fprintf(stderr, "                             Ally\n");
    fprintf(stderr, "                             Amex\n");
    fprintf(stderr, "                             BoA\n");
    fprintf(stderr, "                             Cap1\n");
    fprintf(stderr, "                             Citi\n");
    fprintf(stderr, "                             Discover\n");
    fprintf(stderr, "                             Fidelity\n");
    fprintf(stderr, "                             FNBO\n");
    fprintf(stderr, "                             SchwabBank\n");
    fprintf(stderr, "                             SchwabBrokerage\n");
    fprintf(stderr, "                             TFCU\n");
    fprintf(stderr, "-q --quiet                Quiet running (or decrease verbosity).\n");
    fprintf(stderr, "-v --verbose              Increase verbosity\n");
    fprintf(stderr, "-h -?                     Show this usage\n");
    if (extraLine) fprintf(stderr, "\n%s\n", extraLine);
}


int main(int argc, char *argv[])
{
    int                 opt;
    char                inFileName[MAX_LINE];
    char                outFileName[MAX_LINE];
    bool                usageError = false;
    char                *cp;
    FILE                *fpIn;
    FILE                *fpOut;
    char                line[MAX_LINE];
    int                 numTransactions = 0;
    int                 verbosity = 2;
    bankFormat_t        bankFormat = UNKNOWN_BANK_FORMAT;
    FileProcessor       *theFileProcessor;

    // inFileName[0] = '\0';
    strcpy(inFileName, "stmt"); // Default input file name
    outFileName[0] = '\0';

    struct option longOptions[] =
    {
        {"input",       required_argument,  0,      'i'}
        ,{"output",     required_argument,  0,      'o'}
        ,{"format",     required_argument,  0,      'f'}
        ,{"quiet",      no_argument,        0,      'q'}
        ,{"verbose",    no_argument,        0,      'v'}
        ,{0,0,0,0}
    };

    while (1)
    {
        int optionIndex = 0;
        opt = getopt_long(argc, argv, "i:o:f:qvh?", longOptions, &optionIndex);

        if (-1 == opt) break;

        switch (opt)
        {
        case 'i':
            strcpy(inFileName, optarg);
            break;
        case 'o':
            strcpy(outFileName, optarg);
            break;
        case 'f':
            bankFormat = string2bankFormat(optarg);
            break;
        case 'q':
            --verbosity;
            break;
        case 'v':
            ++verbosity;
            break;
        case 'h':
        case '?':
        default:
            usageError = true;
            break;
        }
    }

    if (usageError)
    {
        usage(basename(argv[0]));
        return -1;
    }

    if (UNKNOWN_BANK_FORMAT == bankFormat)
    {
        usage(basename(argv[0]), "Unknown Bank Format");
        return -6;
    }

    // strcpy(inFileName, "/home/bruno/Downloads/schwab.csv");
    if ('\0' == inFileName[0])
    {
        usage(basename(argv[0]), "Input filename required");
        return -2;
    }

    cp = strchr(inFileName, '.');
    if ((char *)(NULL) == cp)
    {
        // No extension provided.  Add .csv
        strcat(inFileName, ".csv");
    }

    if ('\0' == outFileName[0])
    {
        // Create output file name from input file name
        strcpy(outFileName, inFileName);
        cp = strrchr(outFileName, '.');
        if ((char *)(NULL) == cp)
        {
            // Something went wrong because there should
            // definately be a '.' in the filename
            usage(basename(argv[0]), "Internal error with file names");
            return -3;
        }
        else
        {
            *cp = '\0';
            strcat(outFileName, ".qif");
        }
    }
    else
    {
        // An output file name was provided.
        // See if it has an extension
        cp = strchr(outFileName, '.');
        if ((char *)(NULL) == cp)
        {
            // No extension provided.  Add .qif
            strcat(outFileName, ".qif");
        }
    }

    fpIn = fopen(inFileName, "r");
    if ((FILE *)(NULL) == fpIn)
    {
        usage(basename(argv[0]), "Error opening input file");
        return -4;
    }

    fpOut = fopen(outFileName, "w");
    if ((FILE *)(NULL) == fpOut)
    {
        usage(basename(argv[0]), "Error opening output file");
        fclose(fpIn);
        return -5;
    }

    std::unordered_map<bankFormat_t, FileProcessor *>::const_iterator got = bankMap.find(bankFormat);
    if (got == bankMap.end())
    {
        usage(basename(argv[0]), "Internal error with bank format");
        fclose(fpIn);
        fclose(fpOut);
        return -8;
    }
    else
    {
        theFileProcessor = got->second;
    }

    if (verbosity >= 1)
    {
        printf("%s Ver %s %s\n", basename(argv[0]), SW_VERSION, SW_DATE);
        printf("Bank Format: %d\n", (int)bankFormat);
    }

    fprintf(fpOut, "!Type:Bank\n");

    while (fgets(line, sizeof(line), fpIn))
    {
        if (theFileProcessor->processLine(line))
        {
            if  (verbosity >= 2)
            {
                printf("%s\t%.16s\t$%.2lf\n", theFileProcessor->getDate(), theFileProcessor->getDesc(), theFileProcessor->getAmtd());
            }

            fprintf(fpOut, "D%s\n", theFileProcessor->getDate());
            fprintf(fpOut, "P%s\n", theFileProcessor->getDesc());
            fprintf(fpOut, "T%.2lf\n", theFileProcessor->getAmtd());
            fprintf(fpOut, "C*\n");
            fprintf(fpOut, "^\n");
            ++numTransactions;
        }
    }

    fclose(fpIn);
    fclose(fpOut);

    if (verbosity >= 1)
    {
        printf("Input File            : %s\n", inFileName);
        printf("Output File           : %s\n", outFileName);
        printf("Number of Transactions: %d\n", numTransactions);
    }


    return 0;

}
