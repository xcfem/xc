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
//LoadCombinations.h

#ifndef LOADCOMBINATIONS_H
#define LOADCOMBINATIONS_H

#include "LoadCombinationVector.h"

namespace cmb_acc{
class ActionContainer;
class ActionWeightingMap;

//! @ingroup CMBACC
//
//! @brief Almacena todas las Load Combinations que deben formarse para las acciones.
class LoadCombinations: public CommandEntity
  {
  private:
    //LoadCombinations para estados límite últimos.
    LoadCombinationVector comb_uls_transient; //!< LoadCombinations para estados límite últimos en situaciones persistentes o transitorias.
    LoadCombinationVector comb_uls_accid; //!< LoadCombinations para estados límite últimos en situaciones accidentales.
    LoadCombinationVector comb_uls_sism; //!< LoadCombinations para estados límite últimos en situaciones sísmicas.

    //LoadCombinations para estados límite de servicio.
    LoadCombinationVector comb_sls_poco_frec; //!< LoadCombinations para estados límite de servicio en situaciones poco frecuentes.
    LoadCombinationVector comb_sls_frec; //!< LoadCombinations para estados límite de servicio en situaciones frecuentes.
    LoadCombinationVector comb_sls_cuasi_perm; //!< LoadCombinations para estados límite de servicio en situaciones cuasi - permanentes.

  public:
    LoadCombinations(void);
    LoadCombinations(const ActionContainer &acc);
    virtual ~LoadCombinations(void);
    void Concat(const LoadCombinations &others);

    inline LoadCombinationVector getULSTransientCombinations(void)
      { return comb_uls_transient;}
    inline LoadCombinationVector getULSAccidentalCombinations(void)
      { return comb_uls_accid; }
    inline LoadCombinationVector getULSSeismicCombinations(void)
      { return comb_uls_sism; }

    inline LoadCombinationVector getSLSCharacteristicCombinations(void)
      { return comb_sls_poco_frec; }
    inline LoadCombinationVector getSLSFrequentCombinations(void)
      { return comb_sls_frec; }
    inline LoadCombinationVector getSLSQuasiPermanentCombinations(void)
      { return comb_sls_cuasi_perm; }
  };
} //fin namespace nmb_acc.


#endif
