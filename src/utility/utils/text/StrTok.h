// -*-c++-*-
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
//StrTok.h

#ifndef STR_TOK_HXX
#define STR_TOK_HXX

#include <string>
#include <deque>

class StrTok: public std::string
  {
    size_t pos;
  public:

    typedef std::deque<std::string> dq_campos;

    StrTok(const std::string &str)
      : std::string(str), pos(0) {}
    inline bool final(void) const
      { return (pos>=length()); } 
    std::string get_token(char c);
      //Si no encuentra el caracter c devuelve la cadena
      //desde pos hasta el final.
    double get_float(char c);
    long int get_int(char c);
    std::string resto(void);
    dq_campos campos(char sep);
  };

#endif
