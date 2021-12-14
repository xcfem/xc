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
//imanip.h

#ifndef IMANIP_H
#define IMANIP_H

#include<iostream>
#include<iomanip>
#include<string>
#include<cctype>
#include<cstdlib>

template <class TP>
class imanip
  {
  public:
    std::istream& (*_f)(std::istream&, TP);
    TP _a;
    imanip(std::istream& (*f)(std::istream&, TP), TP a) : _f(f), _a(a) {}
    //friend std::istream& operator>> <>(std::istream& i, const imanip<TP>& m);
  };

template <class TP>
inline std::istream& operator>>(std::istream& i, const imanip<TP>& m)
  { return (*m._f)( i, m._a); }

void vuelca_stream(std::istream &is,std::ostream &os);

std::istream &ignorar_hasta(std::istream &is,char delim);


int busquedabruta(std::istream &a,const std::string &p);
//Coloca el puntero de lectura del stream después de la
//primera ocurrencia de la cadena p.
//Esta rutina se puede mejorar (Ver Algoritmos en C++).
//Devuelve verdadero si encontro la cadena.

inline int find_start(std::istream &is,const std::string &str)
//Devuelve la posici'on del primer car'acter de la cadena str
//en el stream is.
  {
    int encontrada= 0;
    if((encontrada = busquedabruta(is,str)))
      is.seekg(-(str.length()+1),std::ios::cur);
    return encontrada;
  }

//! @brief Devuelve la posicióon del primer caráacter de la cadena str
//! en el stream is.
inline std::streampos find_end(std::istream &is,const std::string &str)
  { return busquedabruta(is,str); }

inline std::istream &ignorar_hasta(std::istream &is,const std::string &p)
  {
    busquedabruta(is,p);
    return is;
  }

inline imanip<char> ignore_to(char delim)
  { return imanip<char>(&ignorar_hasta,delim); }

inline imanip<const std::string &> ignore_to(const std::string &delim)
  { return imanip<const std::string &>(&ignorar_hasta,delim); }

std::istream &find_alnum(std::istream &is);

std::istream &find_notspace(std::istream &is);

template <class T>
struct token_pair
  {    
    T &t;
    const char delim;
    token_pair(T &s,const char d): t(s),delim(d){}
  };

inline std::istream &get_token(std::istream &is,token_pair<std::string> pair)
  {
    const size_t sz= 256;
    char c[sz];
    is.getline(c,sz,pair.delim);
    pair.t= c;
    return is;
  }

inline std::istream &get_token(std::istream &is,token_pair<int> pair)
  {
    std::string str;
    token_pair<std::string> pair_str(str,pair.delim);
    get_token(is,pair_str);
    pair.t= atoi(pair_str.t.data());
    return is;
  }

inline std::istream &get_token(std::istream &is,token_pair<bool> pair)
  {
    int i;
    token_pair<int> pair_i(i,pair.delim);
    get_token(is,pair_i);
    pair.t= (pair_i.t == 1);
    return is;
  }

inline std::istream &get_token(std::istream &is,token_pair<float> pair)
  {
    std::string str;
    token_pair<std::string> pair_str(str,pair.delim);
    get_token(is,pair_str);
    pair.t= atof(pair_str.t.data());
    return is;
  }

inline std::istream &get_token(std::istream &is,token_pair<double> pair)
  {
    std::string str;
    token_pair<std::string> pair_str(str,pair.delim);
    get_token(is,pair_str);
    pair.t= atof(pair_str.t.data());
    return is;
  }

template<class T> 
inline imanip<token_pair<T> > get_token(T &str,const char delim= ',')
  {
    token_pair<T> pair(str,delim);
    return imanip<token_pair<T> >(&get_token,pair);
  }

template <class T>
struct token_pair2
  {    
    T &t;
    const std::string delim;
    token_pair2(T &s,const std::string d): t(s),delim(d){}
  };

std::istream &get_tok(std::istream &is,token_pair2<std::string> pair);

template<class T>
inline imanip<token_pair2<T> > get_tok(T &str,const std::string delim= ",")
  {
    token_pair2<T> pair(str,delim);
    return imanip<token_pair2<T> >(&get_tok,pair);
  }

#endif

