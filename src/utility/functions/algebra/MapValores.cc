//----------------------------------------------------------------------------
//  xc utils library; general purpose classes and functions.
//
//  Copyright (C)  Luis Claudio Pérez Tato
//
//  XC utils is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or 
//  (at your option) any later version.
//
//  This software is distributed in the hope that it will be useful, but 
//  WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details. 
//
// You should have received a copy of the GNU General Public License 
// along with this program.
// If not, see <http://www.gnu.org/licenses/>.
//----------------------------------------------------------------------------
//MapValores.cc

#include "MapValores.h"
#include "utility/utils/text/text_string.h"

//! @brief Constructor.
MapValores::MapValores(void)
  :map_valores() {}

//! @brief Constructor (receives a string of the form: 'x=4.2,y=x+z,...'.
MapValores::MapValores(const std::string &str)
  :map_valores()
  {
    std::deque<std::string> valores= separa_csv(str);
    std::deque<std::string> par;
    for(std::deque<std::string>::iterator i= valores.begin();i!=valores.end();i++)
      {
        par= split_at_first(*i,'=');
        if(par.size()>1)
          (*this)[par[0]]= ExprAlgebra(par[1]);
        else
	  std::cerr << "MapValores::" << __FUNCTION__
                    << str <<
	            "); assignment expresion was expected, we get: '"
                    << *i << "'\n";
      }
  }
