#ifndef _FILE_PROCESSOR_H_
#define _FILE_PROCESSOR_H_
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdarg.h>
#include <string.h>
#include <ctype.h>
#include <string>
#include "parseCsvLine.h"
#include "cusipBankMap.h"
#include "mmSymbols.h"

class FileProcessor
{
    public:
        FileProcessor();

        double      getAmtd(void)   {return amtd;}
        const char *getDate(void)   {return date;}
        const char *getDesc(void)   {return desc;}
        bool        isInTransactions(const char *line);
        bool        processLine(char *line);

    protected:

        virtual bool    extractData(void) = 0;
        bool            extractStandard(int dateField, int descField, int amtField);

        char            amt[MAX_LINE];
        double          amtd;
        char            date[MAX_LINE];
        char            desc[MAX_LINE];
        char            fields[MAX_FIELDS][MAX_LINE];
        std::string     inTransactionKey;
        bool            inTransactionSection;
        double          withdrawModifier;
};

class DebitCreditFileProcessor: virtual public FileProcessor
{
    public:
        DebitCreditFileProcessor();

    protected:
        bool    extractDebitCredit(int dateField, int descField, int debitField, int creditField);
};

class AllyFileProcessor: public FileProcessor
{
    public:
        AllyFileProcessor();

    protected:
        virtual bool    extractData(void);
};

class BoAFileProcessor: public FileProcessor
{
    public:
        BoAFileProcessor();

    protected:
        virtual bool    extractData(void);
};

class CitiFileProcessor: public DebitCreditFileProcessor
{
    public:
        CitiFileProcessor();

    protected:
        virtual bool    extractData(void);
};

class BrokerageFileProcessor: virtual public FileProcessor
{
    public:
        BrokerageFileProcessor();

    protected:
        CUSIPBankMap        cusip2bank;
        MoneyMarketSymbols  mmSymbols;
        char                symbol[MAX_LINE];

        void                modifyCDDescription(void);
        void                modifyMMDescription(void);
        void                modifyTBillDescription(void);
};

class FidelityFileProcessor: public BrokerageFileProcessor
{
    public:
        FidelityFileProcessor();

    protected:
        char                cashBal[MAX_LINE];

        virtual bool        extractData(void);
};

class SchwabFileProcessor: virtual public FileProcessor
{
    public:
        SchwabFileProcessor();
};

class SchwabBankFileProcessor: virtual public DebitCreditFileProcessor, public SchwabFileProcessor
{
    public:
        SchwabBankFileProcessor();

    protected:
        virtual bool    extractData(void);
};

class SchwabBrokerageFileProcessor: public BrokerageFileProcessor, public SchwabFileProcessor
{
    public:
        SchwabBrokerageFileProcessor();

    protected:
        virtual bool    extractData(void);
};

#endif