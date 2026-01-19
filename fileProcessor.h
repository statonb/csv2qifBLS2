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
        bool        processLine(char *line);

    protected:

        virtual bool    extractData(void) = 0;
        bool            extractStandard(int dateField, int descField, int amtField);
        virtual bool    isInTransactions(const char *line);
        virtual void    modifyDescription(char *desc);

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
        bool    extractDebitCreditByType(int dateField, int descField, int amtField, int debitCreditField);
};

class AllyFileProcessor: public FileProcessor
{
    public:
        AllyFileProcessor();

    protected:
        virtual bool    extractData(void);
};

class AmexFileProcessor: public FileProcessor
{
    public:
        AmexFileProcessor();

    protected:
        virtual bool    extractData(void);
        virtual bool    isInTransactions(const char *line);
        virtual void    modifyDescription(char *dest);
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

class Cap1FileProcessor: public DebitCreditFileProcessor
{
    public:
        Cap1FileProcessor();

    protected:
        virtual bool    extractData(void);
        virtual void    modifyDescription(char *dest);
};

class DiscoverBankFileProcessor: public DebitCreditFileProcessor
{
    public:
        DiscoverBankFileProcessor();

    protected:
        virtual bool    extractData(void);
        virtual void    modifyDescription(char *dest);
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

class FNBOFileProcessor: public FileProcessor
{
    public:
        FNBOFileProcessor();

    protected:
        virtual bool    extractData(void);
        virtual bool    isInTransactions(const char *line);
        virtual void    modifyDescription(char *dest);
};

class SchwabFileProcessor: virtual public FileProcessor
{
    public:
        SchwabFileProcessor();
};

class SchwabBankFileProcessor: public DebitCreditFileProcessor, public SchwabFileProcessor
{
    public:
        SchwabBankFileProcessor();

    protected:
        virtual bool    extractData(void);
        virtual void    modifyDescription(char *dest);
};

class SchwabBrokerageFileProcessor: public BrokerageFileProcessor, public SchwabFileProcessor
{
    public:
        SchwabBrokerageFileProcessor();

    protected:
        virtual bool    extractData(void);
};

class TFCUFileProcessor: public FileProcessor
{
    public:
        TFCUFileProcessor();

    protected:
        virtual bool    extractData(void);
        virtual void    modifyDescription(char *dest);
};

#endif
