#include "fileProcessor.h"
#include "stringUtils.h"

FileProcessor::FileProcessor()
    : amtd(0.0)
    , inTransactionSection(false)
    , withdrawModifier(1.0)
{
}

bool FileProcessor::isInTransactions(const char *line)
{
    return (strncmp(line, inTransactionKey.c_str(), inTransactionKey.length()) == 0);
}

bool FileProcessor::extractStandard(int dateField, int descField, int amtField)
{
    strcpy(date, fields[dateField]);
    strip_quotes(date);
    strcpy(desc, fields[descField]);
    strip_quotes(desc);
    modifyDescription(desc);        // Perform custom description modifications on per-bank type basis
    strcpy(amt, fields[amtField]);
    remove_commas_dollars_and_quotes(amt);
    return true;
}

void FileProcessor::modifyDescription(char *desc)
{
    (void)desc;
}

bool FileProcessor::processLine(char *line)
{
    bool ret = false;

    // Remove newline
    line[strcspn(line, "\r\n")] = '\0';

    if (line[0] == '\0') return false;

    if (false == inTransactionSection)
    {
        remove_all_quotes(line);
        inTransactionSection = isInTransactions(line);
        return false;
    }

    parse_csv_line(line, fields, MAX_FIELDS);

    ret = extractData();

    if (ret)
    {
        if (amt[0] == '\0') ret = false;
        amtd = strtod(amt, NULL) * withdrawModifier;
    }

    return ret;
}

DebitCreditFileProcessor::DebitCreditFileProcessor() : FileProcessor()
{

}

bool DebitCreditFileProcessor::extractDebitCredit(int dateField, int descField, int debitField, int creditField)
{
    strcpy(date, fields[dateField]);
    strip_quotes(date);

    strcpy(desc, fields[descField]);
    strip_quotes(desc);
    modifyDescription(desc);        // Perform custom description modifications on per-bank type basis

    strcpy(amt, fields[debitField]);
    remove_commas_dollars_and_quotes(amt);
    // The debit field might be empty or zero
    if  (   (amt[0] == '\0')
         || (0 == strcmp(amt, "0"))
        )
    {
        strcpy(amt, fields[creditField]);     // Try the Credit field instead
        remove_commas_dollars_and_quotes(amt);
        withdrawModifier = 1.0;
    }
    else
    {
        // Debit (or withdraw) field had an entry.
        // This is a positive number, but
        // QIF needs it to be negative.
        withdrawModifier = -1.0;
    }

    return true;
}

bool DebitCreditFileProcessor::extractDebitCreditByType(int dateField, int descField, int amtField, int debitCreditField)
{
    char debitCredit[MAX_LINE];
    bool ret = true;

    strcpy(date, fields[dateField]);
    strip_quotes(date);

    strcpy(desc, fields[descField]);
    strip_quotes(desc);
    modifyDescription(desc);        // Perform custom description modifications on per-bank type basis

    strcpy(amt, fields[amtField]);
    remove_commas_dollars_and_quotes(amt);

    strcpy(debitCredit, fields[debitCreditField]);
    strip_quotes(debitCredit);

    // The debit/credit field should contain "Debit" or "Credit"
    // If it's "Debit", make it negative
    if (strcasestr_simple(debitCredit, "debit"))
    {
        withdrawModifier = -1.0;
    }
    else if(strcasestr_simple(debitCredit, "credit"))
    {
        withdrawModifier = 1.0;
    }
    else
    {
        ret = false;
    }

    return ret;
}

AmexFileProcessor::AmexFileProcessor() : FileProcessor()
{
    inTransactionKey = "<UNUSED>";
    inTransactionSection = true;
}

bool AmexFileProcessor::extractData(void)
{
    extractStandard(0, 1, 2);
    // Fix date format
    return (iso_to_qif_date(date, date, sizeof(date))) ? true : false;
}

void AmexFileProcessor::modifyDescription(char *desc)
{
    if (strstr(desc, "Interest Payment"))
    {
        strcpy(desc, "Interest Earned");
    }
    return;
}

bool AmexFileProcessor::isInTransactions(const char *line)
{
    (void)line;
    return true;    // There is no header in Ally files.  The first line is a transaction
}

AllyFileProcessor::AllyFileProcessor() : FileProcessor()
{
    inTransactionKey = "Date,";
}

bool AllyFileProcessor::extractData(void)
{
    extractStandard(0, 4, 2);
    // Fix date format
    return (iso_to_qif_date(date, date, sizeof(date))) ? true : false;
}

BoAFileProcessor::BoAFileProcessor() : FileProcessor()
{
    inTransactionKey = "Date,";
}

bool BoAFileProcessor::extractData(void)
{
    return extractStandard(0, 1, 2);
}

BrokerageFileProcessor::BrokerageFileProcessor() : FileProcessor()
{

}

void BrokerageFileProcessor::modifyCDDescription(void)
{
    if (strncasecmp(desc, "INTEREST", 8) == 0)
    {
        strcpy(desc, cusip2bank.getBankNameC(symbol));
        strcat(desc, " - Interest");
    }
    else if ((strncasecmp(desc, "REDEMPTION", 10) == 0))
    {
        strcpy(desc, cusip2bank.getBankNameC(symbol));
        strcat(desc, " - Redemption");
    }
}

void BrokerageFileProcessor::modifyMMDescription(void)
{
    if  (   (strncasecmp(desc, "DIVIDEND", 8) == 0)
         || (strncasecmp(desc, "Reinvest Dividend", 17) == 0)
         || (strncasecmp(desc, "Cash Dividend", 13) == 0)
        )
    {
        strcpy(desc, symbol);
        strcat(desc, " Dividend");
    }
    else if (   (strncasecmp(desc, "REINVESTMENT", 12) == 0)
             || (strncasecmp(desc, "YOU BOUGHT", 10) == 0)
             || (strncasecmp(desc, "Reinvest Shares", 15) == 0)
             || (strncasecmp(desc, "Buy", 3) == 0)
            )
    {
        strcpy(desc, symbol);
        strcat(desc, " Purchase");
    }
    else if (   (strncasecmp(desc, "YOU SOLD", 8) == 0)
             || (strncasecmp(desc, "Sell", 4) == 0)
            )
    {
        strcpy(desc, symbol);
        strcat(desc, " Sale");
    }
}

void BrokerageFileProcessor::modifyTBillDescription(void)
{
    if (strncasecmp(desc, "YOU BOUGHT", 10) == 0)
    {
        strcpy(desc, "T-Bill Purchase");
    }
    else if ((strncasecmp(desc, "REDEMPTION", 10) == 0))
    {
        strcpy(desc, "T-Bill Redemption");
    }
}

CitiFileProcessor::CitiFileProcessor() : DebitCreditFileProcessor()
{
    inTransactionKey = "Status";
}

bool CitiFileProcessor::extractData(void)
{
    return extractDebitCredit(1, 2, 3, 4);
}

Cap1FileProcessor::Cap1FileProcessor() : DebitCreditFileProcessor()
{
    inTransactionKey = "Account Number,";
}

bool Cap1FileProcessor::extractData(void)
{
    bool ret = extractDebitCreditByType(2, 1, 4, 3);
    modifyDescription(desc);
    return ret;
}

void Cap1FileProcessor::modifyDescription(char *desc)
{
    if (strstr(desc, "Monthly Interest Paid"))
    {
        strcpy(desc, "Interest Earned");
    }
    return;
}

DiscoverBankFileProcessor::DiscoverBankFileProcessor() : DebitCreditFileProcessor()
{
    inTransactionKey = "Transaction Date,";
}

bool DiscoverBankFileProcessor::extractData(void)
{
    return extractDebitCredit(0, 1, 3, 4);
}

void DiscoverBankFileProcessor::modifyDescription(char *desc)
{
    if (strstr(desc, "Interest Paid"))
    {
        strcpy(desc, "Interest Earned");
    }
    return;
}

FidelityFileProcessor::FidelityFileProcessor() : BrokerageFileProcessor()
{
    inTransactionKey = "Run Date,";
}

bool FidelityFileProcessor::extractData(void)
{
    strcpy(cashBal, fields[15]);
    strip_quotes(cashBal);
    if (strncasecmp(cashBal, "Processing", 10) == 0) {
        // Skip transactions that are still in process
        return false;
    }

    // Don't use the standard extract for Fidelity.
    // Check to be sure the date is valid because
    // Fidelity puts long text lines at the end of the file.
    strcpy(date, fields[0]);
    strip_quotes(date);
    if (isdigit(date[0]) == 0) {
        // Skip lines without a valid date
        return false;
    }
    strcpy(desc, fields[1]);
    strcpy(symbol, fields[2]);
    strcpy(amt, fields[14]);
    remove_commas_dollars_and_quotes(amt);


    // Determine if the description needs to be modified
    strip_quotes(desc);
    strip_quotes(symbol);
    if (mmSymbols.contains(symbol)) {
        modifyMMDescription();
    }
    else if (strncasecmp(symbol, "912797", 6) == 0) {
        modifyTBillDescription();
    }
    else if (cusip2bank.contains(symbol)) {
        modifyCDDescription();
    }
    return true;
}

FNBOFileProcessor::FNBOFileProcessor() : FileProcessor()
{
    inTransactionKey = "<UNUSED>";
    inTransactionSection = true;
}

bool FNBOFileProcessor::extractData(void)
{
    return extractStandard(0, 3, 1);
}

void FNBOFileProcessor::modifyDescription(char *desc)
{
    if (strstr(desc, "INTEREST PAYMENT"))
    {
        strcpy(desc, "Interest Earned");
    }
    return;
}

bool FNBOFileProcessor::isInTransactions(const char *line)
{
    (void)line;
    return true;    // There is no header in Ally files.  The first line is a transaction
}

SchwabFileProcessor::SchwabFileProcessor(): FileProcessor()
{
    inTransactionKey = "Date,";
}

SchwabBankFileProcessor::SchwabBankFileProcessor(): DebitCreditFileProcessor(), SchwabFileProcessor()
{
}

bool SchwabBankFileProcessor::extractData(void)
{
    return extractDebitCredit(0, 4, 5, 6);
}

void SchwabBankFileProcessor::modifyDescription(char *desc)
{
    if (strstr(desc, "Interest Paid"))
    {
        strcpy(desc, "Interest Earned");
    }
    return;
}

SchwabBrokerageFileProcessor::SchwabBrokerageFileProcessor(): BrokerageFileProcessor(), SchwabFileProcessor()
{
}

bool SchwabBrokerageFileProcessor::extractData(void)
{
    char            *cp;

    extractStandard(0, 3, 7);
    // Remove any "as of ..." portion of the field
    cp = strstr(date, " as of");
    if (cp) *cp = '\0';

    strcpy(symbol, fields[2]);
    strip_quotes(symbol);

    // Determine if the description needs to be modified
    if (mmSymbols.contains(symbol)) {
        // Replace the description with the action
        strcpy(desc, fields[1]);
        strip_quotes(desc);
        modifyMMDescription();
    }
    return true;
}

TFCUFileProcessor::TFCUFileProcessor() : FileProcessor()
{
    inTransactionKey = "Transaction ID,";
}

bool TFCUFileProcessor::extractData(void)
{
    return extractStandard(1, 7, 4);
}

void TFCUFileProcessor::modifyDescription(char *desc)
{
    if (strstr(desc, "Dividend"))
    {
        strcpy(desc, "TFCU Dividend");
    }
    return;
}
