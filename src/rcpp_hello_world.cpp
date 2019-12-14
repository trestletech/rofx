
/***************************************************************************
 ofxdump.cpp
 -------------------
 copyright            : (C) 2002 by Benoit Grégoire
 email                : benoitg@coeus.ca
 ***************************************************************************/
/**@file
 * \brief Code for ofxdump utility.  C++ example code
 *
 * ofxdump prints to stdout, in human readable form, everything the library
 understands about a particular ofx response file, and sends errors to
 stderr.  To know exactly what the library understands about of a particular
 ofx response file, just call ofxdump on that file.
 *
 * ofxdump is meant as both a C++ code example and a developper/debugging
 tool.  It uses every function and every structure of the LibOFX API.  By
 default, WARNING, INFO, ERROR and STATUS messages are enabled.  You can
 change these defaults at the top of ofxdump.cpp
 *
 * usage: ofxdump path_to_ofx_file/ofx_filename
 */
/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

// Have to do this after all the other code or we get collisions on WARN variable
// which manifest as a enum syntax parsing
#include <Rcpp.h>
// Disable WARN macro which collides with OfxStatusData
#undef WARN
#undef ERROR

#include <iostream>
#include <iomanip>
#include <cstdlib>
#include <cstring>
#include <string>
#include "libofx/libofx.h"
#include <stdio.h>		/* for printf() */
#ifdef HAVE_CONFIG_H
// #include <config.h>		/* Include config constants, e.g., VERSION TF */
#endif

#include <errno.h>

using namespace std;

int ofx_proc_security_cb(struct OfxSecurityData data, void * security_data)
{
  char dest_string[255];
  cout << "ofx_proc_security():\n";
  if (data.unique_id_valid == true)
  {
    cout << "    Unique ID of the security being traded: " << data.unique_id << "\n";
  }
  if (data.unique_id_type_valid == true)
  {
    cout << "    Format of the Unique ID: " << data.unique_id_type << "\n";
  }
  if (data.secname_valid == true)
  {
    cout << "    Name of the security: " << data.secname << "\n";
  }
  if (data.ticker_valid == true)
  {
    cout << "    Ticker symbol: " << data.ticker << "\n";
  }
  if (data.unitprice_valid == true)
  {
    cout << "    Price of each unit of the security: " << data.unitprice << "\n";
  }
  if (data.date_unitprice_valid == true)
  {
    strftime(dest_string, sizeof(dest_string), "%c %Z", localtime(&(data.date_unitprice)));
    cout << "    Date as of which the unitprice is valid: " << dest_string << "\n";
  }
  if (data.currency_valid == true)
  {
    cout << "    Currency of the unitprice: " << data.currency << "\n";
  }
  if (data.memo_valid == true)
  {
    cout << "    Extra transaction information (memo): " << data.memo << "\n";
  }
  cout << "\n";
  return 0;
}

int ofx_proc_transaction_cb(struct OfxTransactionData data, void * transaction_data)
{
  char dest_string[255];
  cout << "ofx_proc_transaction():\n";
  
  if (data.account_id_valid == true)
  {
    cout << "    Account ID : " << data.account_id << "\n";
  }
  
  if (data.transactiontype_valid == true)
  {
    if (data.transactiontype == OFX_CREDIT)
      strncpy(dest_string, "CREDIT: Generic credit", sizeof(dest_string));
    else if (data.transactiontype == OFX_DEBIT)
      strncpy(dest_string, "DEBIT: Generic debit", sizeof(dest_string));
    else if (data.transactiontype == OFX_INT)
      strncpy(dest_string, "INT: Interest earned or paid (Note: Depends on signage of amount)", sizeof(dest_string));
    else if (data.transactiontype == OFX_DIV)
      strncpy(dest_string, "DIV: Dividend", sizeof(dest_string));
    else if (data.transactiontype == OFX_FEE)
      strncpy(dest_string, "FEE: FI fee", sizeof(dest_string));
    else if (data.transactiontype == OFX_SRVCHG)
      strncpy(dest_string, "SRVCHG: Service charge", sizeof(dest_string));
    else if (data.transactiontype == OFX_DEP)
      strncpy(dest_string, "DEP: Deposit", sizeof(dest_string));
    else if (data.transactiontype == OFX_ATM)
      strncpy(dest_string, "ATM: ATM debit or credit (Note: Depends on signage of amount)", sizeof(dest_string));
    else if (data.transactiontype == OFX_POS)
      strncpy(dest_string, "POS: Point of sale debit or credit (Note: Depends on signage of amount)", sizeof(dest_string));
    else if (data.transactiontype == OFX_XFER)
      strncpy(dest_string, "XFER: Transfer", sizeof(dest_string));
    else if (data.transactiontype == OFX_CHECK)
      strncpy(dest_string, "CHECK: Check", sizeof(dest_string));
    else if (data.transactiontype == OFX_PAYMENT)
      strncpy(dest_string, "PAYMENT: Electronic payment", sizeof(dest_string));
    else if (data.transactiontype == OFX_CASH)
      strncpy(dest_string, "CASH: Cash withdrawal", sizeof(dest_string));
    else if (data.transactiontype == OFX_DIRECTDEP)
      strncpy(dest_string, "DIRECTDEP: Direct deposit", sizeof(dest_string));
    else if (data.transactiontype == OFX_DIRECTDEBIT)
      strncpy(dest_string, "DIRECTDEBIT: Merchant initiated debit", sizeof(dest_string));
    else if (data.transactiontype == OFX_REPEATPMT)
      strncpy(dest_string, "REPEATPMT: Repeating payment/standing order", sizeof(dest_string));
    else if (data.transactiontype == OFX_OTHER)
      strncpy(dest_string, "OTHER: Other", sizeof(dest_string));
    else
      strncpy(dest_string, "Unknown transaction type", sizeof(dest_string));
    cout << "    Transaction type: " << dest_string << "\n";
  }
  
  
  if (data.date_initiated_valid == true)
  {
    strftime(dest_string, sizeof(dest_string), "%c %Z", localtime(&(data.date_initiated)));
    cout << "    Date initiated: " << dest_string << "\n";
  }
  if (data.date_posted_valid == true)
  {
    strftime(dest_string, sizeof(dest_string), "%c %Z", localtime(&(data.date_posted)));
    cout << "    Date posted: " << dest_string << "\n";
  }
  if (data.date_funds_available_valid == true)
  {
    strftime(dest_string, sizeof(dest_string), "%c %Z", localtime(&(data.date_funds_available)));
    cout << "    Date funds are available: " << dest_string << "\n";
  }
  if (data.amount_valid == true)
  {
    cout << "    Total money amount: " << setiosflags(ios::fixed) << setiosflags(ios::showpoint) << setprecision(2) << data.amount << "\n";
  }
  if (data.units_valid == true)
  {
    cout << "    # of units: " << setiosflags(ios::fixed) << setiosflags(ios::showpoint) << setprecision(2) << data.units << "\n";
  }
  if (data.oldunits_valid == true)
  {
    cout << "    # of units before split: " << setiosflags(ios::fixed) << setiosflags(ios::showpoint) << setprecision(2) << data.oldunits << "\n";
  }
  if (data.newunits_valid == true)
  {
    cout << "    # of units after split: " << setiosflags(ios::fixed) << setiosflags(ios::showpoint) << setprecision(2) << data.newunits << "\n";
  }
  if (data.unitprice_valid == true)
  {
    cout << "    Unit price: " << setiosflags(ios::fixed) << setiosflags(ios::showpoint) << setprecision(2) << data.unitprice << "\n";
  }
  if (data.fees_valid == true)
  {
    cout << "    Fees: " << setiosflags(ios::fixed) << setiosflags(ios::showpoint) << setprecision(2) << data.fees << "\n";
  }
  if (data.commission_valid == true)
  {
    cout << "    Commission: " << setiosflags(ios::fixed) << setiosflags(ios::showpoint) << setprecision(2) << data.commission << "\n";
  }
  if (data.fi_id_valid == true)
  {
    cout << "    Financial institution's ID for this transaction: " << data.fi_id << "\n";
  }
  if (data.fi_id_corrected_valid == true)
  {
    cout << "    Financial institution ID replaced or corrected by this transaction: " << data.fi_id_corrected << "\n";
  }
  if (data.fi_id_correction_action_valid == true)
  {
    cout << "    Action to take on the corrected transaction: ";
    if (data.fi_id_correction_action == DELETE)
      cout << "DELETE\n";
    else if (data.fi_id_correction_action == REPLACE)
      cout << "REPLACE\n";
    else
      cout << "ofx_proc_transaction(): This should not happen!\n";
  }
  if (data.invtransactiontype_valid == true)
  {
    cout << "    Investment transaction type: ";
    if (data.invtransactiontype == OFX_BUYDEBT)
      strncpy(dest_string, "BUYDEBT (Buy debt security)", sizeof(dest_string));
    else if (data.invtransactiontype == OFX_BUYMF)
      strncpy(dest_string, "BUYMF (Buy mutual fund)", sizeof(dest_string));
    else if (data.invtransactiontype == OFX_BUYOPT)
      strncpy(dest_string, "BUYOPT (Buy option)", sizeof(dest_string));
    else if (data.invtransactiontype == OFX_BUYOTHER)
      strncpy(dest_string, "BUYOTHER (Buy other security type)", sizeof(dest_string));
    else if (data.invtransactiontype == OFX_BUYSTOCK)
      strncpy(dest_string, "BUYSTOCK (Buy stock))", sizeof(dest_string));
    else if (data.invtransactiontype == OFX_CLOSUREOPT)
      strncpy(dest_string, "CLOSUREOPT (Close a position for an option)", sizeof(dest_string));
    else if (data.invtransactiontype == OFX_INCOME)
      strncpy(dest_string, "INCOME (Investment income is realized as cash into the investment account)", sizeof(dest_string));
    else if (data.invtransactiontype == OFX_INVEXPENSE)
      strncpy(dest_string, "INVEXPENSE (Misc investment expense that is associated with a specific security)", sizeof(dest_string));
    else if (data.invtransactiontype == OFX_JRNLFUND)
      strncpy(dest_string, "JRNLFUND (Journaling cash holdings between subaccounts within the same investment account)", sizeof(dest_string));
    else if (data.invtransactiontype == OFX_MARGININTEREST)
      strncpy(dest_string, "MARGININTEREST (Margin interest expense)", sizeof(dest_string));
    else if (data.invtransactiontype == OFX_REINVEST)
      strncpy(dest_string, "REINVEST (Reinvestment of income)", sizeof(dest_string));
    else if (data.invtransactiontype == OFX_RETOFCAP)
      strncpy(dest_string, "RETOFCAP (Return of capital)", sizeof(dest_string));
    else if (data.invtransactiontype == OFX_SELLDEBT)
      strncpy(dest_string, "SELLDEBT (Sell debt security.  Used when debt is sold, called, or reached maturity)", sizeof(dest_string));
    else if (data.invtransactiontype == OFX_SELLMF)
      strncpy(dest_string, "SELLMF (Sell mutual fund)", sizeof(dest_string));
    else if (data.invtransactiontype == OFX_SELLOPT)
      strncpy(dest_string, "SELLOPT (Sell option)", sizeof(dest_string));
    else if (data.invtransactiontype == OFX_SELLOTHER)
      strncpy(dest_string, "SELLOTHER (Sell other type of security)", sizeof(dest_string));
    else if (data.invtransactiontype == OFX_SELLSTOCK)
      strncpy(dest_string, "SELLSTOCK (Sell stock)", sizeof(dest_string));
    else if (data.invtransactiontype == OFX_SPLIT)
      strncpy(dest_string, "SPLIT (Stock or mutial fund split)", sizeof(dest_string));
    else if (data.invtransactiontype == OFX_TRANSFER)
      strncpy(dest_string, "TRANSFER (Transfer holdings in and out of the investment account)", sizeof(dest_string));
    else
      strncpy(dest_string, "ERROR, this investment transaction type is unknown.  This is a bug in ofxdump", sizeof(dest_string));
    
    cout << dest_string << "\n";
  }
  if (data.unique_id_valid == true)
  {
    cout << "    Unique ID of the security being traded: " << data.unique_id << "\n";
  }
  if (data.unique_id_type_valid == true)
  {
    cout << "    Format of the Unique ID: " << data.unique_id_type << "\n";
  }
  if (data.security_data_valid == true)
  {
    ofx_proc_security_cb(*(data.security_data_ptr), NULL );
  }
  
  if (data.server_transaction_id_valid == true)
  {
    cout << "    Server's transaction ID (confirmation number): " << data.server_transaction_id << "\n";
  }
  if (data.check_number_valid == true)
  {
    cout << "    Check number: " << data.check_number << "\n";
  }
  if (data.reference_number_valid == true)
  {
    cout << "    Reference number: " << data.reference_number << "\n";
  }
  if (data.standard_industrial_code_valid == true)
  {
    cout << "    Standard Industrial Code: " << data.standard_industrial_code << "\n";
  }
  if (data.payee_id_valid == true)
  {
    cout << "    Payee_id: " << data.payee_id << "\n";
  }
  if (data.name_valid == true)
  {
    cout << "    Name of payee or transaction description: " << data.name << "\n";
  }
  if (data.memo_valid == true)
  {
    cout << "    Extra transaction information (memo): " << data.memo << "\n";
  }
  cout << "\n";
  return 0;
}//end ofx_proc_transaction()

int ofx_proc_statement_cb(struct OfxStatementData data, void * statement_data)
{
  Rcpp::List* inf{static_cast<Rcpp::List*>(statement_data)};
  if (inf->containsElementNamed("statement")){
    // Already have an statement
    Rcpp::stop("Found multiple statement in the OFX file; not currently supported");
    return -1;
  }
  
  Rcpp::List stmt = Rcpp::List::create();
  
  if (data.currency_valid == true)
  {
    stmt["currency"] = data.currency;
  }
  if (data.account_id_valid == true)
  {
    stmt["account_id"] = data.account_id;
  }
  //char dest_string[255];
  if (data.date_start_valid == true)
  {
    stmt["date_start"] = Rcpp::Datetime(data.date_start);
  }
  if (data.date_end_valid == true)
  {
    stmt["date_end"] = Rcpp::Datetime(data.date_end);
  }
  if (data.ledger_balance_valid == true)
  {
    stmt["ledger_balance"] = data.ledger_balance;
  }
  if (data.ledger_balance_date_valid == true)
  {
    stmt["ledger_balance_date"] = Rcpp::Datetime(data.ledger_balance_date);
  }
  if (data.available_balance_valid == true)
  {
    stmt["available_balance"] = data.available_balance;
    //cout << "    Available balance: " << setiosflags(ios::fixed) << setiosflags(ios::showpoint) << setprecision(2) << data.available_balance << "\n";
  }
  if (data.available_balance_date_valid == true)
  {
    // TODO: all times should be interpreted as UTC, currently parsed in localtime
    stmt["available_balance_date"] = Rcpp::Datetime(data.available_balance_date);
    /*Rcpp::Datetime dt = Rcpp::Datetime(data.available_balance_date);
     dt.attr("tzone") = "UTC";
     stmt["available_balance_date"] = dt;*/
  }
  if (data.marketing_info_valid == true)
  {
    stmt["marketing_info"] = data.marketing_info;
  }
  
  (*inf)["statement"] = stmt;
  return 0;
}//end ofx_proc_statement()

int ofx_proc_account_cb(struct OfxAccountData data, void * account_data)
{
  Rcpp::List* inf{static_cast<Rcpp::List*>(account_data)};
  if (inf->containsElementNamed("account")){
    // Already have an account
    Rcpp::stop("Found multiple accounts in the OFX file; not currently supported");
    return -1;
  }
  
  Rcpp::List account = Rcpp::List::create();
  
  if (data.account_id_valid == true)
  {
    account["id"] = data.account_id;
    account["name"] = data.account_name;
  }
  if (data.account_type_valid == true)
  {
    std::string typestr;
    switch (data.account_type)
    {
    case OfxAccountData::OFX_CHECKING :
      typestr = "checking";
      break;
    case OfxAccountData::OFX_SAVINGS :
      typestr = "savings";
      break;
    case OfxAccountData::OFX_MONEYMRKT :
      typestr = "moneymarket";
      break;
    case OfxAccountData::OFX_CREDITLINE :
      typestr = "creditline";
      break;
    case OfxAccountData::OFX_CMA :
      typestr = "cma";
      break;
    case OfxAccountData::OFX_CREDITCARD :
      typestr = "creditcard";
      break;
    case OfxAccountData::OFX_INVESTMENT :
      typestr = "investment";
      break;
    default:
      typestr = "unknown";
    }
    account["type"] = typestr;
  }
  if (data.currency_valid == true)
  {
    account["currency"] = data.currency;
  }
  
  if (data.bank_id_valid)
    account["bank_id"] = data.bank_id;
  
  if (data.branch_id_valid)
    account["branch_id"] = data.branch_id;
  
  if (data.account_number_valid)
    account["number"] = data.account_number;
  
  (*inf)["account"] = account;
  
  return 0;
}//end ofx_proc_account()



int ofx_proc_status_cb(struct OfxStatusData data, void * status_data)
{
  cout << "ofx_proc_status():\n";
  if (data.ofx_element_name_valid == true)
  {
    cout << "    Ofx entity this status is relevant to: " << data.ofx_element_name << " \n";
  }
  if (data.severity_valid == true)
  {
    cout << "    Severity: ";
    switch (data.severity)
    {
    case OfxStatusData::INFO :
      cout << "INFO\n";
      break;
    case OfxStatusData::WARN :
      cout << "WARN\n";
      break;
    case OfxStatusData::ERROR :
      cout << "ERROR\n";
      break;
    default:
      cout << "WRITEME: Unknown status severity!\n";
    }
  }
  if (data.code_valid == true)
  {
    cout << "    Code: " << data.code << ", name: " << data.name << "\n    Description: " << data.description << "\n";
  }
  if (data.server_message_valid == true)
  {
    cout << "    Server Message: " << data.server_message << "\n";
  }
  cout << "\n";
  return 0;
}

// [[Rcpp::export]]
SEXP ofx_info(SEXP path)
{
  /*ofx_PARSER_msg = false;
  ofx_DEBUG_msg = false;
  ofx_WARNING_msg = false;
  ofx_ERROR_msg = false;
  ofx_INFO_msg = false;*/
  
  Rcpp::List inf = Rcpp::List::create();
  LibofxContextPtr libofx_context = libofx_get_new_context();
  
  //char **inputs ; /* unamed options */
  //unsigned inputs_num ; /* unamed options number */
  //const char* filename = "/Users/jeff/Downloads/test.ofx";
  
  string filename = Rcpp::as<string>(path);
  
  ofx_set_statement_cb(libofx_context, ofx_proc_statement_cb, &inf);
  ofx_set_account_cb(libofx_context, ofx_proc_account_cb, &inf);
  ofx_set_transaction_cb(libofx_context, ofx_proc_transaction_cb,&inf);
  ofx_set_security_cb(libofx_context, ofx_proc_security_cb, &inf);
  ofx_set_status_cb(libofx_context, ofx_proc_status_cb, &inf);
  
  cout << filename << "\n";
  
  enum LibofxFileFormat file_format = libofx_get_file_format_from_str(LibofxImportFormatList, "AUTODETECT");

  cout << file_format << "\n";
  
  libofx_proc_file(libofx_context, filename.c_str(), file_format);
  
  return inf;
}
