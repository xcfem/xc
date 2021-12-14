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
//Function.h

#ifndef FUNCION_H
#define FUNCION_H

#include <cassert>
#include <algorithm>
#include "NmbVars.h"

class Function
  {
  protected:
    NmbVars Vars; //!< Independent variable names.
  public:
    Function(void) {}
    Function(const NmbVars &vars)
      { Vars= vars; }      
    Function(const char *vars)
      { Vars= PutVars(vars); }
    Function(const char var)
      { Vars= PutVars(var); }
    Function(const Function &other) 
      { Vars= other.Vars; }
    Function &operator=(const Function &other)
      {
        Vars= other.Vars;
        return *this;
      }
    short unsigned int GetDim(void) const
      { return Vars.size(); }
    inline char GetNmbVar(short unsigned int i) const
    //Las variables se numeran de 1 a n;
      { return Vars.at(i-1); }
    void PutNmbVar(short unsigned int i,char nmb)
      { Vars[i-1]= nmb; }
    void PutStr(const char *vars)
      { Vars= PutVars(vars); }
    void PutChar(const char var)
      { Vars= PutVars(var); }
    void CambioVar(short unsigned int j,const NmbVars &vars)
      { Vars= Vars.CambioVar(j,vars); }
    const NmbVars &GetVars(void) const
      { return Vars; }
    friend Function operator +(const Function &f1,const Function &f2)
      { 
        Function f(f1.Vars.Combina(f2.Vars));
        return f;
      }
    friend Function operator -(const Function &f1,const Function &f2)
      {
        Function f(f1.Vars.Combina(f2.Vars));
        return f;
      }
    friend Function operator *(const Function &f1,const Function &f2)
      {
        Function f(f1.Vars.Combina(f2.Vars));
        return f;
      }      
    inline friend bool operator==(const Function &f1,const Function &f2)
      {
        if (f1.Vars != f2.Vars) return 0;
        return 1;
      }
    inline friend bool operator!=(const Function &f1,const Function &f2)
      { return !(f1==f2); }
  };



#endif

