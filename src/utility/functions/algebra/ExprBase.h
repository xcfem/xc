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
//ExprBase.h

#ifndef EXPRBASE_H
#define EXPRBASE_H

#include "ProtoExpr.h"

class ExprBase: public ProtoExpresion
  {
  protected:
    bool err_traduc; //!< Verdadero si se produce un error en la traducción.
  public:
    //! @brief Constructor por defecto.
    ExprBase(const bool &ET)
      : ProtoExpresion(), err_traduc(ET) {}
    virtual bool operator==(const ExprBase &) const;
    inline bool operator!=(const ExprBase &other) const
      { return !operator==(other); }
    //! @brief Devuelve verdadero si ha habido errores en la traducción.
    inline bool ErrorTraduccion(void) const
      { return err_traduc; }
  };

#endif













