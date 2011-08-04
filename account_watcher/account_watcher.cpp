
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

#include <zenfire/event/Client.hpp>
#include <zenfire/Account.hpp>
#include <zenfire/Position.hpp>
#include <iostream>
#include "zf_examples_compat.hpp"

using namespace zenfire;

//############################################################################//
// F U N C T I O N S #########################################################//
//############################################################################//

//############################################################################//
//! display usage and exit

void die_usage()
  {
  std::cout << "usage: user password" << std::endl;
  std::exit(0);
  }

//############################################################################//
// C A L L B A C K S #########################################################//
//############################################################################//

class WatcherHandler : public event::EventHandler
  {
  virtual
  int
  on_status(const Status *status)
    {
    if (status->type() == alert::LOGIN_FAILED)
      {
      std::cerr << "Login Failed" << std::endl;
      std::exit(1);
      }
    if (status->type() == alert::BAD_VERSION)
      {
      std::cerr << "API version out of date." << std::endl;
      std::exit(1);
      }
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

  virtual
  int
  on_position(const zenfire::account::Event &event, const Position pos)
    {
    std::cout << "Got Position type=" << event.type() << " acct=" << pos->account_id().id() << " inst=" << pos->instrument_id().id() << " size=" << pos->size() << " open_pl=" << int64_t(pos->open_pl()) << " closed_pl=" << int64_t(pos->closed_pl()) << std::endl;
    return 0;
    }
  };

int main(int argc, char** argv)
  {
  if(argc != 3)
    die_usage();

  WatcherHandler callback;
  event::Client *zf = zenfire::event::Client::create_ini("examples.conf", &callback);

  std::string user = std::string(argv[1]);
  std::string pass = std::string(argv[2]);

  zf->login(user, pass);
  if (!zf->sync(2000))
    {
    std::cerr << "Didn't get a response to login fast enough, exiting." << std::endl;
    std::exit(1);
    }

  COMPAT_SLEEP(300);

  zf->logout();
  delete zf;
  }
