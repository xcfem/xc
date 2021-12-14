//----------------------------------------------------------------------------
//  xc utils library; general purpose classes and functions.
//
//  Copyright (C)  Luis C. Pérez Tato
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
//StrTok.cc

#include "StrTok.h"
#include <cstdlib>

std::string StrTok::get_token(char c)
  //Si no encuentra el caracter c devuelve la cadena
  //desde pos hasta el final.
  {
    std::string retval("");
    if(empty()) return retval;
    if(final()) return retval;
    size_t nueva_pos= find(c,pos);
    if(nueva_pos<length()) //Encontró c.
      retval= substr(pos,nueva_pos-pos);
    else //Devuelve hasta el final
      {
        retval= substr(pos,length());
        nueva_pos= length();
      }
    pos= std::min(nueva_pos+1,length());
    return retval;
  }
std::string StrTok::resto(void)
  {
    std::string retval("");
    if(final())
      return retval;
    else
      return substr(pos,length()-pos);
  }
StrTok::dq_campos StrTok::campos(char sep)
  {
    dq_campos retval;
    std::string campo;
    do
      {
        campo= get_token(sep);
        retval.push_back(campo);
      }
    while(pos<size());
    return retval;
  }

double StrTok::get_float(char c)
  { return atof(get_token(c).c_str()); }
long int StrTok::get_int(char c)
  { return atol(get_token(c).c_str()); }
