//----------------------------------------------------------------------------
//  xc utils library bilioteca de comandos para el intérprete del lenguaje
//  de entrada de datos.
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
//CombinationFactors.cxx

#include "CombinationFactors.h"

//! @brief Return the r-th combination factor.
const float &cmb_acc::CombinationFactors::getCombinationFactor(short int r) const
  {
    switch(r)
      {
      case(0):
        return psi_0;
      case(1):
        return psi_1;
      case(2):
        return psi_2;
      default:
	std::cerr << getClassName() << "::" << __FUNCTION__
	          << " índex: " << r
		  << " out of range (0..2)." << std::endl;
        return psi_0;
      }
  }

//! @brief Print stuff.
void cmb_acc::CombinationFactors::Print(std::ostream &os) const
  {
    os << "psi_0= " << psi_0
       << ", psi_1= " <<  psi_1
       << ", psi_2= " << psi_2;
  }
