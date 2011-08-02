
/******************************************************************************
 ******************************************************************************/

/** \file market_watcher_handler.cpp
 *  watch market data for an instrument example program using low level handler
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
#include <zenfire/feed/FunctionHandler.hpp>
#include <zenfire/Tick.hpp>
#include "zf_examples_compat.hpp"

#include <map>
#include <iostream>
#include <sstream>
#include <ctime>
#include <cstdlib>

//############################################################################//
// F U N C T I O N S #########################################################//
//############################################################################//

//############################################################################//
//! display usage and exit

void die_usage()
  {
  std::cout << "usage: user password symbol" << std::endl;
  std::exit(0);
  }

//############################################################################//
// C A L L B A C K S #########################################################//
//############################################################################//

using namespace zenfire;

class WatcherHandler : public event::EventHandler
  {
protected:
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
    std::cout << " got account" << std::endl;
    acct_ids.push_back(acct->id());
    return 0;
    }

  virtual
  int
  on_instrument(event::request_id_type id, const Instrument inst)
    {
    if(inst->id() == 0)
      {
      std::cout << "Could not find instrument." << std::endl;
      return 0;
      }

    std::cout << "Got " << inst->symbol() << std::endl;
    return 0;
    }

public:
  std::vector<uint32_t> acct_ids;
  };

class TickDisplayer : public feed::Handler
  {
protected:
  virtual
  int
  on_status(const Status& status)
    {
    if(status.type() == alert::UNKNOWN)
      std::cerr << "ERROR: Feed Unknown Problem" << std::endl;
    if(status.type() == alert::UP)
      std::cout << "ERROR: Feed Up" << std::endl;
    if(status.type() == alert::DOWN)
      std::cout << "ERROR: Feed Down" << std::endl;
    if(status.type() == alert::BAD_VERSION)
      std::cerr << "ERROR: Feed API Version Bad" << std::endl;
    return 0;
    }

  void
  display_tick(const char* tt, const Tick& t)
    {
    std::cout << "TICK::" << tt << " @" << t.timestamp() << ":" << t.micros() << " " << t.instrument_id().id() << " price=" << t.price() << std::endl;
    }

  virtual
  int
  on_best_ask(const Tick& t)
    {
    display_tick("best_ask  ", t);
    return 0;
    }
  virtual
  int
  on_ask(const Tick& t)
    {
    display_tick("ask       ", t);
    return 0;
    }
  virtual
  int
  on_best_bid(const Tick& t)
    {
    display_tick("best_bid  ", t);
    return 0;
    }
  virtual
  int
  on_bid(const Tick& t)
    {
    display_tick("bid       ", t);
    return 0;
    }
  virtual
  int
  on_trade(const Tick& t)
    {
    display_tick("trade     ", t);
    return 0;
    }
  virtual
  int
  on_low(const Tick& t)
    {
    display_tick("low       ", t);
    return 0;
    }
  virtual
  int
  on_high(const Tick& t)
    {
    display_tick("high      ", t);
    return 0;
    }
  virtual
  int
  on_settlement(const Tick& t)
    {
    display_tick("settlement", t);
    return 0;
    }
  virtual
  int
  on_mode(const Tick& t)
    {
    display_tick("mode      ", t);
    return 0;
    }
  };

//############################################################################//
// M A I N ###################################################################//
//############################################################################//

int main(int argc, char **argv)
  {
  if(argc != 4)
    die_usage();

  WatcherHandler callback;
  TickDisplayer tickback;
  event::Client *zf = zenfire::event::Client::create("examples.conf", &callback);
  
  // get auth info from the command line
  std::string user(argv[1]);
  std::string pass(argv[2]);
  std::string symbol(argv[3]);

  zf->login(user, pass);
  if (!zf->sync(2000))
    {
    std::cerr << "Didn't get a response to login fast enough, exiting." << std::endl;
    std::exit(1);
    }

  if(callback.acct_ids.size() < 1)
    {
    std::cerr << "No accounts received, credentials probably bad." << std::endl;
    std::exit(1);
    }

  zf->find_instrument(symbol);

  COMPAT_SLEEP(3);

  Instrument inst = zf->get_instrument(symbol);

  if(!inst)
    {
    std::cerr << "did not get the instrument." << std::endl;
    std::exit(1);
    }

  Exchange exch = zf->get_exchange(inst->exchange());

  std::cout << "Found instrument " << inst->symbol() << ":" << exch->code() << std::endl;

  feed::SubscriptionPtr sub = zf->feed_subscribe(inst, &tickback);

  COMPAT_SLEEP(45);

  zf->logout();
  delete zf;
  return 0;
  }

/******************************************************************************
 ******************************************************************************/

