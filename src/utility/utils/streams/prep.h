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
//prep.h
//Preprocessing functions.

#ifndef PREP_H
#define PREP_H

#include <iostream>
#include <string>

void quita_comentarios(std::istream &,std::ostream &);
int find_and_replace1(std::istream &,const std::string &,std::ostream &,const std::string &);
int find_and_replace(std::istream &,const std::string &,std::ostream &,const std::string &);

std::string get_bloque_comillas(std::istream &, bool &error);
std::string get_bloque_parentesis(std::istream &,bool &error);
std::string get_bloque_llaves(std::istream &is,bool &error,const bool &ignora_esp);

#endif
