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
//NmbVars.h

#ifndef NMBVARS_H
#define NMBVARS_H

#include "assert.h"
#include <set>
#include <string>
#include "utility/matrices/vectorZ.h"

typedef vectorZ<char> vZ_char;
typedef vectorZ<short unsigned int> vZ_sui;

//! @brief Stores the names of the function variables.
//! Each name occupies one character.
class NmbVars : public vZ_char
  {
  protected:
    void PutStr(const std::string &str);
    void PutChar(const char c)
      { (*this)[0]= c; }
  public:
    NmbVars(const size_t &sz=1,const char &c='x') : vZ_char(sz,'x') {}
    //! @brief Initializes the names of the first n variables from the argument string.
    NmbVars(const std::string &vars) : vZ_char(vars.size(),'x') 
      { PutStr(vars); }
    NmbVars(const char c) : vZ_char(1)
    //Ojo! este es el que utiliza cuando se da un solo parámetro. 
      { PutChar(c); }
    NmbVars( const NmbVars &otro) : vZ_char(otro) {}
    NmbVars( const vZ_char &otro) : vZ_char(otro) {}
    NmbVars &operator=(const NmbVars &otro)
      {
        vZ_char::operator =(otro);
        return *this;
      }
    NmbVars &operator=(const vZ_char &otro)
      {
        vZ_char::operator =(otro);
        return *this;
      }
    //! @brief Return a set of variable names excepth the j-th one.
    NmbVars QuitaVar(short unsigned int j) const
      { return GetMenor(j); }    
    NmbVars Combina(const NmbVars &n2) const;
    vZ_sui Indices(const NmbVars &n) const;
    NmbVars CambioVar(short unsigned int j,const NmbVars &v) const
      {
         NmbVars c= GetMenor(j);
         c= v.Combina(c);
         return c;
      }
    friend NmbVars PutVars(const char var);
    friend NmbVars PutVars(const std::string &var);
    friend NmbVars operator +(const NmbVars &v1,const NmbVars &v2);
    friend std::ostream &operator << (std::ostream &stream, const NmbVars &n)
      { return operator << (stream, (const vZ_char &) n); }
  };



NmbVars PutVars(const char var)
//Devuelve el conjunto de variables
//correspondiente al caracter
//que se le pasa como parámetro.
  {
    NmbVars n(size_t(1));
    n.PutChar(var);
    return n;
  }

NmbVars PutVars(const std::string &vars)
//Devuelve el conjunto de variables
//correspondiente a la cadena de caracteres
//que se le pasa como parámetro.
  {
    NmbVars n(vars.size());
    n.PutStr(vars);
    return n;
  }

inline NmbVars operator +(const NmbVars &v1,const NmbVars &v2)
  { return v1.Combina(v2); }

#endif

