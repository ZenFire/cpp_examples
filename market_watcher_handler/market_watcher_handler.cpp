
/******************************************************************************
 ******************************************************************************/

/** \file market_watcher_handler.cpp
 *  watch market data for an instrument example program using low level handler.
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

#include <string>
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

void die_usage(zenfire::event::Client* zf)
  {
  if (zf != NULL)
    delete zf;
  std::cout << "usage: symbol [user password]" << std::endl;
  std::exit(0);
  }

//############################################################################//
// C A L L B A C K S #########################################################//
//############################################################################//

using namespace zenfire;

// this class implements the event::EventHandler functions by subclassing: see the documentation
// for a full list of functions.
class WatcherHandler : public event::EventHandler
  {
protected:
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
public:
  Instrument inst;
protected:
  virtual
  int
  on_status(const Status& status)
    {
    if(status.type() == alert::UP)
      std::cout << "STATUS: Feed Up" << std::endl;
    if(status.type() == alert::UNKNOWN)
      std::cerr << "ERROR: Feed Unknown Problem" << std::endl;
    if(status.type() == alert::DOWN)
      std::cout << "ERROR: Feed Down" << std::endl;
    if(status.type() == alert::BAD_VERSION)
      std::cerr << "ERROR: Feed API Version Bad" << std::endl;
    return 0;
    }

  void
  display_tick(const char* tt, const Tick& t)
    {
    std::cout << tt << " @" << t.timestamp() << ":" << t.micros() << " " << t.instrument_id().id() << " price=";

    if (inst)
      std::cout << inst->format_price(t);
    else
      std::cout << t.price();

    std::cout << " size=" << t.size() << " flags=" << t.flags() << std::endl;
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
  WatcherHandler callback;
  TickDisplayer tickback;

  event::Client *zf = NULL;
  try {
    zf = zenfire::event::Client::create_ini("examples.conf", &callback);
  } catch (zenfire::exception::invalid_config&) {
    std::cout << "could not load config, please run from the directory containing examples.conf." << std::endl << std::flush;
    std::exit(1);
  } catch (zenfire::exception::keys_missing&) {
    std::cout << "zenfire::exception::keys_missing caught, refer to documentation." << std::endl << std::flush;
    std::exit(1);
  }
  
  if (argc < 2)
    {
    die_usage(zf);
    }
  else if(argc != 4)
    {
    try
      {
      zf->login();
      }
    catch (exception::access_missing&)
      {
      die_usage(zf);
      }
    }
  else
    {
    std::string user = std::string(argv[2]);
    std::string pass = std::string(argv[3]);

    // wait up to 2000ms for a response to the login message.
    zf->login(user, pass);
    }

  std::string symbol(argv[1]);

  if (!zf->sync(2000))
    {
    std::cerr << "Didn't get a response to login fast enough, exiting." << std::endl << std::flush;
    std::exit(1);
    }

  if(callback.acct_ids.size() < 1)
    {
    std::cerr << "No accounts received, credentials probably bad." << std::endl << std::flush;
    std::exit(1);
    }

  zf->find_instrument(symbol);
  zf->sync(3000);

  Instrument inst = zf->get_instrument(symbol);
  tickback.inst = inst;

  if(!inst)
    {
    std::cerr << "did not get the instrument." << std::endl << std::flush;
    std::exit(1);
    }

  std::cout << "Found instrument " << inst->symbol() << " on exchange " << int(inst->exchange()) << std::endl;

  feed::SubscriptionPtr sub = zf->feed_subscribe(inst, &tickback);

  COMPAT_SLEEP(45);

  zf->logout();
  delete zf;
  return 0;
  }

/******************************************************************************
 ******************************************************************************/

