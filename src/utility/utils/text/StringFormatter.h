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
//StringFormatter.h

#ifndef STRINGFORMATTER_H
#define STRINGFORMATTER_H

#include <string>

//! @brief Formatea cadenas de caracteres.
class StringFormatter
  {
    static const size_t sz= 1024;
    static char buf[sz];
    std::string fmt;
    size_t num_format_strings(void) const;
    char char_espec_format_string(void) const;
    bool check_format_string(const std::string &,const std::string &) const;
  public:
    StringFormatter(const std::string &format);
    const std::string &format_string(void) const
      { return fmt; }
    std::string &format_string(void)
      { return fmt; }
    std::string format(const std::string &str);
    std::string format(const bool &i);
    std::string format(const int &i);
    std::string format(const long int &i);
    std::string format(const size_t &i);
    std::string format(const double &d);
    std::string format(const float &f);
  };

std::string format(const std::string &format,const std::string &str);
std::string format(const std::string &format,const bool &i);
std::string format(const std::string &format,const int &i);
std::string format(const std::string &format,const long int &i);
std::string format(const std::string &format,const size_t &i);
std::string format(const std::string &format,const double &d);
std::string format(const std::string &format,const float &f);

#endif
