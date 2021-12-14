// -*-c++-*-
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
//tab_cod.hxx
//Utilidades para tablas de códigos.
#ifndef TAB_COD_H
#define TAB_COD_H

#include <string>

char latin1TOpc850ML(int c);

std::string latin1TOpc850ML(const std::string &s);

unsigned char pc8TOlatin1(int c);

std::string pc8TOlatin1(const std::string &);

std::string iso8859_1TOlatex(int c);
//Devuelve el codigo de las vocales con acento y la egne
//en LaTex a partir de su c'odigo en ASCII iso8859/1 Latin 1.

std::string iso8859_1TO_utf8(const std::string &);

#endif
