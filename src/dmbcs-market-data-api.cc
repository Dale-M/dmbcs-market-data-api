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


#include "dmbcs-market-data-api.h"
#include <curlpp/Options.hpp>
#include <curlpp/Easy.hpp>
#include <regex>


namespace DMBCS { namespace Market_Data_Api {


  namespace Curl  =  cURLpp;
  namespace Opt   =  Curl::Options;
    


static  string  spacify  (string&&  x)
  {
    transform (begin (x), end (x),
               begin (x),
               []  (const unsigned char  a)  { return isspace (a) ? ' ' : a; });

    return x;
  }



static  string::const_iterator  pass_version  (const string&  x)
  {
     static const regex  head  {"^ *'\\( *\\( *1( [^)]*)?\\)"};
     smatch  m;

     if  (! regex_search (x, m, head))
       {
         if  (x.substr (0, 2)  ==  "'(")
           throw Bad_Communication
                       {gettext ("please update this application to a later "
                                 "version; new data communication protocols "
                                 "have been seen")};
         else
           throw Bad_Communication
                       {gettext ("service failed; probably you are trying "
                                 "to make too many requests in a short time")};
       }

     return  begin (x)  +  m [0].length ();
  }



static  string  get_curl_response  (const string&  query)
  {
    string      response;
    Curl::Easy  request;
            
    request.setOpt (Opt::Url {query});

    request.setOpt (Opt::WriteFunction 
                       {[&response] (char* buffer,  size_t size,  size_t n) 
                                    { response  +=  string {buffer, 
                                                            buffer + size * n};
                                      return  size * n; }});

    request.setOpt (Opt::SslVerifyPeer {0});

    request.perform ();
            
    if (response.empty ())
           throw No_Network {gettext ("got no CURL response")};

    return  spacify (move (response));
  }



static  vector<Market>  get_markets  (const string&  url_endpoint)
    try
      {
        const string  response  {get_curl_response (MARKET_SERVER_URL
                                                               + url_endpoint)};
    
        vector <Market>  ret;

        auto  cursor  {pass_version (response)};

        static const regex  re
          { " *\\( *\"([^\"']+)\" *\"([^\"']*)\" *\"([^\"]+)\""
                                  " *([0-9]+(\\.[0-9]*)?)( [^)]*)?\\)" };

        for (smatch match; ; )
          {
            if (! regex_search (cursor, end (response), match, re,
                                regex_constants::match_continuous))
                return ret;

            ret.push_back
              ({.symbol  =  match [1].str (),
                .component_extension  =  match [2].str (),
                .name  =  match [3].str (),
                .close_time  =  chrono::minutes
                                    (int (strtod (match [4].str ().data (),
                                                  nullptr)
                                             * 60.0))});

            cursor  +=  match [0].length ();
          }
      }
  catch  (No_Network&)
    {   throw No_Network  {  gettext ("CURL got no markets meta-data")};  }



vector<Market>  get_markets  ()
                {   return  get_markets  ("markets");   }
    


vector<Delta>  get_component_delta  (const string&  market_symbol,
                                     const time_t&  last_time)
    try
      {
        ostringstream hold;
        hold << MARKET_SERVER_URL 
             << "components?m=" << market_symbol
             << "&t=" << last_time;

        const string  response  {get_curl_response (hold.str ())};

        vector<Delta> ret;
        ret.reserve (500);

        auto  cursor  {pass_version (response)};

        for (smatch m; ; )
          {
            /* Remember that these fields are injected into a database, so
             * be wary of unsanitary inputs. */
            static const regex  part 
                       {" *\\( *([^\"' ]+) *\"([^\"]+)\" *([-+*])( [^)]*)?\\)"};

            if (! regex_search (cursor, end (response), m, part,
                                regex_constants::match_continuous))
              return ret;

            ret.push_back ({.symbol  =  m [1].str (),
                            .name    =  m [2].str (),
                            .action  =  m [3].str () == "+" ? Delta::ADD
                                      : m [3].str () == "-" ? Delta::REMOVE
                                                  /* "*" */ : Delta::SIDEWAYS});

            cursor  +=  m [0].length ();
          }
      }
    catch (No_Network &)
      {   throw  No_Network {gettext ("CURL got no market component data")};   }
    catch (Curl::RuntimeError &)
      {   return  vector<Delta> {};   }


} }  /* End of namespace DMBCS::Trader_Desk. */
