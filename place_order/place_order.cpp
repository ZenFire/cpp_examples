
/******************************************************************************
 ******************************************************************************/

/** \file place_order.cpp
 *  place 1 market order example program
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
#include "zf_examples_compat.hpp"

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
  std::cout << "usage: user password" << std::endl;
  std::exit(0);
  }

//############################################################################//
// C A L L B A C K S #########################################################//
//############################################################################//

using namespace zenfire;

class PlacerHandler : public event::EventHandler
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
    acct_ids.push_back(acct->id());
    return 0;
    }

  virtual
  int
  on_open(const Order ord)
    {
    std::cout << "opened" << std::endl;
    return 0;
    }

  virtual
  int
  on_instrument(event::request_id_type id, const Instrument inst)
    {
    if (inst->id() == 0)
      std::cout << "Could not find instrument." << std::endl; // API-72 this does not actually happen, bug in API
    else
      std::cout << "Got " << inst->symbol() << std::endl;  
    return 0;
    }

public:
  std::vector<uint32_t> acct_ids;
};

//############################################################################//
// M A I N ###################################################################//
//############################################################################//

price_type get_price_interactive(std::string prompt)
  {
  price_type price = 0;

  while(price == 0)
    {
    std::cout << "Enter " << prompt << ":";

    std::cin >> price;
    std::cin.clear();
    std::cin.ignore(255,'\n');
    }

  return price;
  }

int main(int argc, char **argv)
  {
  if(argc != 3)
    die_usage();

  PlacerHandler callback;
  event::Client *zf = zenfire::event::Client::create("examples.conf", &callback);

  std::string user = std::string(argv[1]);
  std::string pass = std::string(argv[2]);

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

  std::cout << "Enter account number:" << std::endl;

  for(auto ai = callback.acct_ids.begin(); ai != callback.acct_ids.end(); ai++)
    std::cout << " * " << *ai << " " << zf->get_account(*ai)->code() << std::endl;

  uint32_t acctno = 0;

  while (acctno == 0)
    {
    std::cin >> acctno;
    std::cin.clear();
    std::cin.ignore(255,'\n');
    }

  std::cout << "Using account " << acctno << std::endl;

  std::string prod;

  std::cout << "Enter symbol.exchange: ";
  std::cin >> prod;

  zf->find_instrument(prod);

  COMPAT_SLEEP(3);

  Instrument product = zf->get_instrument(prod);

  if (!product)
    {
    std::cerr << "did not get the product." << std::endl;
    std::exit(1);
    }

  Exchange product_exchange = zf->get_exchange(product->exchange());

  if (!product_exchange)
    {
    std::cerr << "did not get an exchange matching the product." << std::endl;
    std::exit(1);
    }

  std::cout << "Got product " << product->symbol() << " on exchange " << product_exchange->code() << std::endl;

  zenfire::order::side::Type side = zenfire::order::side::NO_SIDE;

  while(side == zenfire::order::side::NO_SIDE)
    {
    int act;
    
    std::cout << "possible actions:" << std::endl;
    std::cout << int(zenfire::order::side::BUY) << ": Buy" << std::endl;
    std::cout << int(zenfire::order::side::SELL) << ": Sell" << std::endl;
    std::cout << "please select an action to place market order: ";

    std::cin >> act;
    std::cin.clear();
    std::cin.ignore(255,'\n');

    switch(act)
      {
    case zenfire::order::side::BUY:
      side = zenfire::order::side::BUY;
      break;
    case zenfire::order::side::SELL:
      side = zenfire::order::side::SELL;
      break;
      }
    }

  zenfire::order::type::Type order_type = zenfire::order::type::NO_TYPE;
  while(order_type == zenfire::order::type::NO_TYPE)
    {
    int act;
    
    std::cout << "possible order types:" << std::endl;
    std::cout << int(zenfire::order::type::MARKET) << ": Market" << std::endl;
    std::cout << int(zenfire::order::type::LIMIT) << ": Limit" << std::endl;
    std::cout << int(zenfire::order::type::STOP_MARKET) << ": Stop Market" << std::endl;
    std::cout << int(zenfire::order::type::STOP_LIMIT) << ": Stop Limit" << std::endl;

    std::cout << "please select an action to continue (Market orders will place immediately): ";

    std::cin >> act;
    std::cin.clear();
    std::cin.ignore(255,'\n');

    switch(act)
      {
    case zenfire::order::type::MARKET:
      order_type = zenfire::order::type::MARKET;
      break;
    case zenfire::order::type::LIMIT:
      order_type = zenfire::order::type::LIMIT;
      break;
    case zenfire::order::type::STOP_MARKET:
      order_type = zenfire::order::type::STOP_MARKET;
      break;
    case zenfire::order::type::STOP_LIMIT:
      order_type = zenfire::order::type::STOP_LIMIT;
      break;
      }
    }

  zenfire::OrderSpecs specs = OrderSpecs::create();

  specs->set_account_id(acctno);
  specs->set_instrument_id(product->id());
  specs->set_type(order_type);
  specs->set_side(side);

  if (order_type == zenfire::order::type::LIMIT || order_type == zenfire::order::type::STOP_LIMIT)
    {
    specs->set_limit_price(get_price_interactive("Limit price"));
    }

  if (order_type == zenfire::order::type::STOP_LIMIT || order_type == zenfire::order::type::STOP_MARKET)
    {
    specs->set_trigger_price(get_price_interactive("Trigger price"));
    }

  specs->set_duration(zenfire::order::duration::DAY);
  specs->set_qty(1);
  specs->set_zentag(std::string("hello this is a test"));
  specs->set_tag(std::string("test"));

  zenfire::Order placed = zf->place_order(specs);

  for (int i = 0; i < 10; i++)
    {
    COMPAT_SLEEP(1);
    if (placed->qty_filled() == 1)
      {
      std::cout << "filled" << std::endl;
      return 0;
      }
    }
  std::cout << "did not fill within 10 seconds, but may later. goodbye." << std::endl;
  
  zf->logout();
  delete zf;
  }

/******************************************************************************
 ******************************************************************************/

