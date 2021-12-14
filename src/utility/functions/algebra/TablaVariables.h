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
//TablaVariables.h

#ifndef TABLAVARIABLES_H
#define TABLAVARIABLES_H

#include "TablaSegnales.h"
#include "Variable.h"

typedef TablaSegnales<Variable> tabla_variables;

class TablaVariables : public tabla_variables
  {
  public:
    TablaVariables(void) : tabla_variables() {}
    inline NamedOperand *Nueva(const Variable &v)
      { return tabla_variables::Nueva(v); }
    inline NamedOperand *Nueva(const std::string &palabra)
      { return tabla_variables::Nueva(Variable(palabra)); }

    inline void Asigna(const std::string &palabra,const double &d)
      {
        Variable *v;
        if((v=CalcDir(palabra))) v->Asigna(d);
      }
    inline void DesAsigna(const std::string &palabra)
      {
        Variable *v;
        if((v= CalcDir(palabra))) v->DesAsigna();
      }
  };

#endif
