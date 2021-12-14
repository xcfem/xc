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
//latex.cc

#include "latex.h"
#include <boost/algorithm/string.hpp>

//! @brief Devuelve el código equivalente el LaTeX
//! para los caracteres con significado especial
std::string ascii2latex(const std::string &s)
  {
    std::string tmp(s);
    if(tmp.find('\\')) //Tiene escape.
      boost::replace_all(tmp,"\\(","(");
    if(tmp.find('\\'))
      boost::replace_all(tmp,"\\)",")");
    if(tmp.find('\\'))
      boost::replace_all(tmp,"\\[","[");
    if(tmp.find('\\'))
      boost::replace_all(tmp,"\\]","]");
    std::string retval;
    for(size_t i=0;i<tmp.length();i++)
      {
        if(tmp[i]=='_') retval+= "\\";
        if(tmp[i]=='%') retval+= "\\";
        if(tmp[i]=='$') retval+= "\\";
        if(tmp[i]=='&') retval+= "\\";
        if(tmp[i]=='>')
	  {
            retval+= "$>$";
            continue;
          }
        retval+= tmp[i];
      }
    return retval;
  }
