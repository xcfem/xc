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
//LoadCombinationVector.hxx
//Almacena las sumas correspondientes a cada una de las variaciones
//de una familia de acciones.

#ifndef LOADCOMBINATIONVECTOR_HXX
#define LOADCOMBINATIONVECTOR_HXX

#include "utility/loadCombinations/actions/Action.h"
#include "utility/matrices/m_double.h"

namespace cmb_acc{
//! @ingroup CMBACC
//
//! @brief Combination container. Each component of the vector is a
//! combination of actions.
class LoadCombinationVector: public std::vector<Action>, public CommandEntity
  {
  private:
    bool Existe(const Action &f) const;
    bool Nula(const double &tol) const;
    size_t CuentaNulas(const double &tol) const;
    size_t CuentaDistintas(const LoadCombinationVector &s2) const;
    const LoadCombinationVector &GetDistintas(const LoadCombinationVector &s2) const;
  public:
    //! @brief Constructor.
    LoadCombinationVector(const size_t &sz= 0);
    static LoadCombinationVector ProdCartesiano(const LoadCombinationVector &f1,const LoadCombinationVector &f2,const double &tol);
    static LoadCombinationVector Concat(const LoadCombinationVector &f1,const LoadCombinationVector &f2,const double &tol);
    void Numera(const std::string &prefix= "H");
    const LoadCombinationVector &GetDistintas(void) const;
    const LoadCombinationVector &GetNoNulas(const double &tol) const;
    m_double getCoeficientes(const std::vector<std::string> &) const;
    void Print(std::ostream &os) const;
  };

std::ostream &operator<<(std::ostream &os,const LoadCombinationVector &vc);

} //fin namespace nmb_acc.

#endif
