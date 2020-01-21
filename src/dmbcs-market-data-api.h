/*
 * Copyright (c) 2020  Dale Mellor
 *
 *
 *  This file is part of the dmbcs-market-data-api package.
 *
 *  The dmbcs-market-data-api package is free software: you can
 *  redistribute it and/or modify it under the terms of the GNU General
 *  Public License as published by the Free Software Foundation, either
 *  version 3 of the License, or (at your option) any later version.
 *
 *  The trader-desk package is distributed in the hope that it will be
 *  useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 *  General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program. If not, see http://www.gnu.org/licenses/.
 */


#ifndef DMBCS_MARKET_DATA_SERVER__H
#define DMBCS_MARKET_DATA_SERVER__H


#include  <chrono>
#include  <stdexcept>
#include  <string>
#include  <vector>


/** \file
 *
 *  Declaration of the \c Market_Data_Api namespace. */


/** A namespace with two functions defined within it, corresponding to the
 *  two functions available at the /trader-desk/ market data server: one
 *  to get the list of markets, and one to get an incremental list of
 *  components in a single market. */

namespace DMBCS  {  namespace  Market_Data_Api
  {
    using  namespace  std;


    /** The functions will throw this if the network stack on the local
     *  machine cannot be made to work, most likely because the interfaces
     *  are down. */
    struct No_Network : runtime_error  { using runtime_error::runtime_error; };

    /** The functions will throw this if the remote server fails in any
     *  way to respond as expected. */
    struct Bad_Communication : runtime_error
                                       { using runtime_error::runtime_error; };
    


    static constexpr const char *const MARKET_SERVER_URL
                                      =  "https://rdmp.org:9443/trader-desk/";



    /** A data vessel holding meta-data about a stock market. */
    struct Market
    {
      /** The marketʼs ticker symbol. */
      string   symbol;

      /** The marketʼs extension to componentsʼ ticker symbols. */
      string   component_extension;

      /** Human-readable, although terse, name of the market. */
      string   name;

      /** The time after midnight at which this market closes. */
      chrono::system_clock::duration  close_time;
    };

    /** Return a list of all the meta-data of all markets available from
     *  the server. */
    /*  Assume curlpp has already been initialized. */
    vector<Market>  get_markets  ();



    /** A data vessel which denotes the addition or removal of a component
     *  of the market. */
    struct Delta
    {
      /** The Yahoo! API ticker symbol for the component, sans any market
       *  extension. */
      string symbol;

      /** A human-readable name of the company. */
      string name;

      /** Has the company recently entered the market, or left it?  Or
       *  perhaps this is a sideways movement: change of name, symbol
       *  and/or market of a market component already being tracked. */
      enum {ADD, REMOVE, SIDEWAYS}  action;
    };

    /** Get a list of market changes since \a last_time. */
    vector<Delta>  get_component_delta  (const string&  market_symbol,
                                         const time_t&  last_time);


    /** We only want to hit the server occasionally (the data will not
     *  change more than once in any 24 hour period).  This helper
     *  function determines whether the time \a t is too recent (short) to
     *  warrant hitting the server now. */
    inline  bool  short_time  (const time_t&  last_update)
    {  return    time (nullptr) - 23 * 60 * 60   <   last_update;  }


} }  /* End of namespace DMBCS::Market_Data_Server. */


#include <curlpp/cURLpp.hpp>  /* This is for the benefit of end-user
                               *   clients. */


#endif  /* Undefined DMBCS_MARKET_DATA_SERVER__H. */
