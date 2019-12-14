
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

class TransactionList {
public:
  Rcpp::StringVector accountId;
  Rcpp::StringVector transactionType;
  Rcpp::DatetimeVector initiated;
  Rcpp::DatetimeVector posted;
  Rcpp::DatetimeVector fundsAvailable;
  Rcpp::NumericVector amount;
  Rcpp::NumericVector units;
  Rcpp::NumericVector oldUnits;
  Rcpp::NumericVector newUnits;
  
  Rcpp::DataFrame toDataFrame();
  TransactionList(); // constructor
};

TransactionList::TransactionList(void) : accountId(0), transactionType(0), 
  initiated(0), posted(0), fundsAvailable(0), amount(0), units(0), oldUnits(0),
  newUnits(0) {
}

Rcpp::DataFrame TransactionList::toDataFrame(){
  // TODO: the datetimes are losing their attributes when getting cast to dataframe.
  return Rcpp::DataFrame::create(
    Rcpp::_["account_id"] = accountId,
    Rcpp::_["transaction_type"] = transactionType,
    Rcpp::_["initiated"] = initiated,
    Rcpp::_["posted"] = posted,
    Rcpp::_["funds_available"] = fundsAvailable,
    Rcpp::_["amount"] = amount,
    Rcpp::_["units"] = units,
    Rcpp::_["old_units"] = oldUnits,
    Rcpp::_["new_units"] = newUnits
  
  );
}

int ofx_proc_transaction_cb(struct OfxTransactionData data, void * transaction_data)
{
  
  TransactionList* tl{static_cast<TransactionList*>(transaction_data)};
  
  if (data.account_id_valid == true)
  {
    tl->accountId.push_back(data.account_id);
  } else {
    tl->accountId.push_back(NA_STRING);
  }
  
  // TODO: move to a new function
  Rcpp::String type = NA_STRING;
  if (data.transactiontype_valid == true)
  {
    if (data.transactiontype == OFX_CREDIT)
      type = "CREDIT: Generic credit";
    else if (data.transactiontype == OFX_DEBIT)
      type = "DEBIT: Generic debit";
    else if (data.transactiontype == OFX_INT)
      type = "INT: Interest earned or paid (Note: Depends on signage of amount)";
    else if (data.transactiontype == OFX_DIV)
      type = "DIV: Dividend";
    else if (data.transactiontype == OFX_FEE)
      type = "FEE: FI fee";
    else if (data.transactiontype == OFX_SRVCHG)
      type = "SRVCHG: Service charge";
    else if (data.transactiontype == OFX_DEP)
      type = "DEP: Deposit";
    else if (data.transactiontype == OFX_ATM)
      type = "ATM: ATM debit or credit (Note: Depends on signage of amount)";
    else if (data.transactiontype == OFX_POS)
      type = "POS: Point of sale debit or credit (Note: Depends on signage of amount)";
    else if (data.transactiontype == OFX_XFER)
      type = "XFER: Transfer";
    else if (data.transactiontype == OFX_CHECK)
      type = "CHECK: Check";
    else if (data.transactiontype == OFX_PAYMENT)
      type = "PAYMENT: Electronic payment";
    else if (data.transactiontype == OFX_CASH)
      type = "CASH: Cash withdrawal";
    else if (data.transactiontype == OFX_DIRECTDEP)
      type = "DIRECTDEP: Direct deposit";
    else if (data.transactiontype == OFX_DIRECTDEBIT)
      type = "DIRECTDEBIT: Merchant initiated debit";
    else if (data.transactiontype == OFX_REPEATPMT)
      type = "REPEATPMT: Repeating payment/standing order";
    else if (data.transactiontype == OFX_OTHER)
      type = "OTHER: Other";
    else
      type = "Unknown transaction type";
  }
  tl->transactionType.push_back(type);
  
  if (data.date_initiated_valid == true)
  {
    tl->initiated.push_back(Rcpp::Datetime(data.date_initiated));
  } else {
    tl->initiated.push_back(NA_REAL);
  }
  
  if (data.date_posted_valid == true)
  {
    cout << data.date_posted << "\n";
    tl->posted.push_back(Rcpp::Datetime(data.date_posted));
  } else {
    tl->posted.push_back(NA_REAL);
  }
  
  if (data.date_funds_available_valid == true)
  {
    tl->fundsAvailable.push_back(Rcpp::Datetime(data.date_funds_available));
  } else {
    tl->fundsAvailable.push_back(NA_REAL);
  }
  
  if (data.amount_valid == true)
  {
    tl->amount.push_back(data.amount);
  } else {
    tl->amount.push_back(NA_REAL);
  }
  
  if (data.units_valid == true)
  {
    tl->units.push_back(data.units);
  } else {
    tl->units.push_back(NA_REAL);
  }
  
  if (data.oldunits_valid == true)
  {
    tl->oldUnits.push_back(data.oldunits);
  } else {
    tl->oldUnits.push_back(NA_REAL);
  }
  
  if (data.newunits_valid == true)
  {
    tl->newUnits.push_back(data.newunits);
  } else {
    tl->newUnits.push_back(NA_REAL);
  }
  
  
  return 0;
  /*

  
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
      type = "BUYDEBT (Buy debt security)";
    else if (data.invtransactiontype == OFX_BUYMF)
      type = "BUYMF (Buy mutual fund)";
    else if (data.invtransactiontype == OFX_BUYOPT)
      type = "BUYOPT (Buy option)";
    else if (data.invtransactiontype == OFX_BUYOTHER)
      type = "BUYOTHER (Buy other security type)";
    else if (data.invtransactiontype == OFX_BUYSTOCK)
      type = "BUYSTOCK (Buy stock))";
    else if (data.invtransactiontype == OFX_CLOSUREOPT)
      type = "CLOSUREOPT (Close a position for an option)";
    else if (data.invtransactiontype == OFX_INCOME)
      type = "INCOME (Investment income is realized as cash into the investment account)";
    else if (data.invtransactiontype == OFX_INVEXPENSE)
      type = "INVEXPENSE (Misc investment expense that is associated with a specific security)";
    else if (data.invtransactiontype == OFX_JRNLFUND)
      type = "JRNLFUND (Journaling cash holdings between subaccounts within the same investment account)";
    else if (data.invtransactiontype == OFX_MARGININTEREST)
      type = "MARGININTEREST (Margin interest expense)";
    else if (data.invtransactiontype == OFX_REINVEST)
      type = "REINVEST (Reinvestment of income)";
    else if (data.invtransactiontype == OFX_RETOFCAP)
      type = "RETOFCAP (Return of capital)";
    else if (data.invtransactiontype == OFX_SELLDEBT)
      type = "SELLDEBT (Sell debt security.  Used when debt is sold, called, or reached maturity)";
    else if (data.invtransactiontype == OFX_SELLMF)
      type = "SELLMF (Sell mutual fund)";
    else if (data.invtransactiontype == OFX_SELLOPT)
      type = "SELLOPT (Sell option)";
    else if (data.invtransactiontype == OFX_SELLOTHER)
      type = "SELLOTHER (Sell other type of security)";
    else if (data.invtransactiontype == OFX_SELLSTOCK)
      type = "SELLSTOCK (Sell stock)";
    else if (data.invtransactiontype == OFX_SPLIT)
      type = "SPLIT (Stock or mutial fund split)";
    else if (data.invtransactiontype == OFX_TRANSFER)
      type = "TRANSFER (Transfer holdings in and out of the investment account)";
    else
      type = "ERROR, this investment transaction type is unknown.  This is a bug in ofxdump";
    
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
   */
}//end ofx_proc_transaction()


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
    stmt["available_balance_date"] = Rcpp::Datetime(data.available_balance_date);
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
  
  TransactionList tl = TransactionList();
  
  ofx_set_statement_cb(libofx_context, ofx_proc_statement_cb, &inf);
  ofx_set_account_cb(libofx_context, ofx_proc_account_cb, &inf);
  ofx_set_transaction_cb(libofx_context, ofx_proc_transaction_cb,&tl);
  ofx_set_security_cb(libofx_context, ofx_proc_security_cb, &inf);
  ofx_set_status_cb(libofx_context, ofx_proc_status_cb, &inf);
  
  cout << filename << "\n";
  
  enum LibofxFileFormat file_format = libofx_get_file_format_from_str(LibofxImportFormatList, "AUTODETECT");

  cout << file_format << "\n";
  
  libofx_proc_file(libofx_context, filename.c_str(), file_format);
  
  // Bring the accumulated transactions onto the list
  inf["transactions"] = tl.toDataFrame();
  
  return inf;
}
