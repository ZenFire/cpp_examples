
/******************************************************************************
 ******************************************************************************/

/** \file account_watcher.cpp
 *  example program to monitor positions and P/L for all user accounts
 */

// L I C E N S E #############################################################//

/*
 * Copyright (C) 2008-2011 BigWells Technology (Zen-Fire). All rights reserved.
 * Confidential and proprietary information of BigWells Technology.
 */

//############################################################################//
// I N C L U D E S ###########################################################//
//############################################################################//

#define BIG_SYS_NO_BOOST

#include <stdlib.h>
#include <zenfire/event/Client.hpp>
#include <zenfire/Account.hpp>
#include <zenfire/Position.hpp>
#include <string>
#include <iostream>
#include "zf_examples_compat.hpp"

using namespace zenfire;

//############################################################################//
// F U N C T I O N S #########################################################//
//############################################################################//

//############################################################################//
//! display usage and exit

void die_usage(zenfire::event::Client* zf)
  {
  if (zf != NULL)
    delete zf;
  std::cout << "usage: [user password]" << std::endl;
  std::exit(0);
  }

//############################################################################//
// C A L L B A C K S #########################################################//
//############################################################################//

// this class implements the event::EventHandler functions by subclassing: see the documentation
// for a full list of functions.
class WatcherHandler : public event::EventHandler
  {
  virtual
  int
  on_status(const Status* status)
    {
    // your login failed; probably a bad username or password.
    if (status->type() == alert::LOGIN_FAILED)
      {
      std::cerr << "Login Failed" << std::endl << std::flush;
      std::exit(1);
      }
    // the version of the API you're using is too old. You can't log in.
    if (status->type() == alert::BAD_VERSION)
      {
      std::cerr << "API version out of date." << std::endl << std::flush;
      std::exit(1);
      }
    // an unknown problem occurred.
    if (status->type() == alert::UNKNOWN)
      {
      std::cerr << "Unknown Problem." << std::endl << std::flush;
      std::exit(1);
      }
    // everything's good! You're logged in.
    if (status->type() == alert::LOGIN_COMPLETE)
      std::cout << "Login OK" << std::endl;

    return 0;
    }

  virtual
  int
  on_account(const Account acct)
    {
    std::cout << "Got Account #" << acct->id() << " \"" << acct->code() << "\"" << std::endl;
    return 0;
    }
  };

class AccountHandler : public account::EventHandler
  {
  virtual
  int
  on_status(const Status& status)
    {
    // your login failed; probably a bad username or password.
    if (status.type() == alert::LOGIN_FAILED)
      {
      std::cerr << "Login Failed for account subscription." << std::endl << std::flush;
      std::exit(1);
      }
    // the version of the API you're using is too old. You can't log in.
    if (status.type() == alert::BAD_VERSION)
      {
      std::cerr << "API version out of date for account subscription." << std::endl << std::flush;
      std::exit(1);
      }
    // an unknown problem occurred.
    if (status.type() == alert::UNKNOWN)
      {
      std::cerr << "Unknown Problem for account subscription." << std::endl << std::flush;
      std::exit(1);
      }
    // everything's good! You're logged in.
    if (status.type() == alert::LOGIN_COMPLETE)
      std::cout << "Login OK for account subscription." << std::endl;

    return 0;
    }


  virtual
  int
  on_position(const zenfire::account::Event &event, const Position pos)
    {
    std::cout << "Got Position type=" << event.type() << " account=" << pos->account_id().id() << " instrument=" << pos->instrument_id().id() << " size=" << pos->size() << " open_pl=" << int64_t(pos->open_pl()) << " closed_pl=" << int64_t(pos->closed_pl()) << std::endl;
    return 0;
    }

  void
  print_orderinfo(std::string naming, const Order& ord, bool detail=false)
    {
    std::cout << naming << " id=" << ord->id() << " acct_id=" << ord->account_id() << " inst_id=" << ord->instrument_id() << " qty=" << ord->qty();

    if (detail)
      std::cout << " qty_filled=" << ord->qty_filled() << " qty_canceled=" << ord->qty_canceled() << " reason=" << uint16_t(ord->reason()) << " status=" << uint16_t(ord->status()) << " error_code=" << ord->error_code() << " message=\"" << ord->message() << "\"";

    std::cout << std::endl;
    }

  // printers for orders
  virtual int
  on_open(const Order ord)
    { print_orderinfo("OPENED", ord); return 0; }
  virtual int
  on_fill(const Order ord)
    { print_orderinfo("FILL", ord, true); return 0; }
  virtual int
  on_bust(const Order ord)
    { print_orderinfo("BUST", ord); return 0; }
  virtual int
  on_fail(const Order ord)
    { print_orderinfo("FAIL", ord, true); return 0; }
  virtual int
  on_modify(const Order ord)
    { print_orderinfo("MODIFY", ord, true); return 0; }
  virtual int
  on_cancel(const Order ord)
    { print_orderinfo("CANCEL", ord, true); return 0; }
  virtual int
  on_cancel_fail(const Order ord)
    { print_orderinfo("CANCEL FAIL", ord, true); return 0; }
  virtual int
  on_modify_fail(const Order ord)
    { print_orderinfo("MODIFY FAIL", ord, true); return 0; }
  virtual int
  on_pending(const Order ord)
    { print_orderinfo("PENDING", ord); return 0; }
  virtual int
  on_modify_pending(const Order ord)
    { print_orderinfo("MODIFY PENDING", ord); return 0; }
  virtual int
  on_invalid(const Order ord)
    { print_orderinfo("OPENED", ord, true); return 0; }
  virtual int
  on_modify_invalid(const Order ord)
    { print_orderinfo("MODIFY INVALID", ord, true); return 0; }
  virtual int
  on_modify_reject(const Order ord)
    { print_orderinfo("MODIFY REJECT", ord, true); return 0; }
  virtual int
  on_reject(const Order ord)
    { print_orderinfo("REJECT", ord, true); return 0; }
  virtual int
  on_trigger(const Order ord)
    { print_orderinfo("TRIGGER", ord); return 0; }
  virtual int
  on_replay(const Order ord)
    { print_orderinfo("REPLAY", ord, true); return 0; }
  };

int main(int argc, char** argv)
  {
  WatcherHandler callback;
  AccountHandler account_callback;
  event::Client *zf = NULL;
  try {
    zf = zenfire::event::Client::create_ini("examples.conf", &callback);
  } catch (zenfire::exception::invalid_config& ic) {
    std::cout << "could not load config, please run from the directory containing examples.conf." << std::endl << std::flush;
    std::exit(1);
  } catch (zenfire::exception::keys_missing& km) {
    std::cout << "zenfire::exception::keys_missing caught, refer to documentation." << std::endl << std::flush;
    std::exit(1);
  }

  if(argc == 1)
    {
    try
      {
      zf->login();
      }
    catch (exception::access_missing& am)
      {
      die_usage(zf);
      }
    }
  else if (argc >= 3)
    {
    std::string user = std::string(argv[1]);
    std::string pass = std::string(argv[2]);

    // wait up to 2000ms for a response to the login message.
    zf->login(user, pass);
    }

  account_id_type aid = 0;
  if (argc >= 4)
    aid = atoi(argv[3]);
  
  if (!zf->sync(2000))
    {
    std::cerr << "Didn't get a response to login fast enough, exiting." << std::endl << std::flush;
    std::exit(1);
    }

  std::vector<account::SubscriptionPtr> acct_subs;

  auto accts = zf->list_accounts();
  for (auto a = accts.begin(); a != accts.end(); a++)
    {
    account_id_type a_id = (*a).id();
    if (aid == 0 || a_id == aid)
      acct_subs.push_back(zf->account_subscribe(a_id, &account_callback));
    }

  for (auto as = acct_subs.begin(); as != acct_subs.end(); as++)
    if (!((*as)->sync(2000)))
      std::cerr << "Waited at least 2 seconds for account subscription for account #" << (*as)->account()->id() << " to start, did not get a response." << std::endl;

  // wait 300 seconds, during which all the events that we've written handlers for will print out results to the screen
  COMPAT_SLEEP(300);

  delete zf;
  }
