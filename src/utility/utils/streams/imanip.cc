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
//imanip.cc

#include "imanip.h"

void vuelca_stream(std::istream &is,std::ostream &os)
  {
    while(!is.eof()) os << is.get();
  }

std::istream &ignorar_hasta(std::istream &is,char delim)
  {
    if(is.eof())
      return is;
    else
      while(!is.eof())
        if(char(is.get())==delim) break;
    return is;
  }

int busquedabruta(std::istream &a,const std::string &p)
//Coloca el puntero de lectura del stream despues de la
//primera ocurrencia de la cadena p.
//Esta rutina se puede mejorar (Ver Algoritmos en C++).
//Devuelve verdadero si encontro la cadena.
  {
    int M= p.length();
    int encontrada= 0;
    char c;
    if(!a.get(c)) return encontrada;
    int j;
    for(j= 0;j<M;j++)
      {
        if(c != p[j])
          {
            a.seekg(a.tellg()-std::streampos(j));
            j= -1;
          }
        if(!a.get(c))
          {
            a.seekg(a.tellg()-std::streampos(j));
            a.clear();
            break;
          }
      }
    if(j>=M-1)
      {
        if(j==M) a.putback(c);
        encontrada= 1;
      }
    return encontrada;
  }

std::istream &find_alnum(std::istream &is)
  {
    if(is.eof()) return is;
    char c;
    while(!is.eof())
      {
        c= is.peek();
        if(isalnum(c))
          break;
        else
          c= is.get();
      }
    return is;
  }

std::istream &find_notspace(std::istream &is)
  {
    if(is.eof()) return is;
    char c;
    while(!is.eof())
      {
        c= is.peek();
        if(!isspace(c))
          break;
        else
          c= is.get();
      }
    return is;
  }

std::istream &get_tok(std::istream &is,token_pair2<std::string> pair)
  {
    if(is.eof()) return is;
    char c;
    c= is.peek();
    pair.t= "";
    while(!is.eof())
      if(pair.delim.find(c) < pair.delim.length())
        break;
      else
        {
          c= is.get();
          pair.t+=c;
        }
    return is;
  }
