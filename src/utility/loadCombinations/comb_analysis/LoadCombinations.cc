//----------------------------------------------------------------------------
//  xc utils library bilioteca de comandos para el intérprete del lenguaje
//  de entrada de datos.
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
//LoadCombinations.cc

#include "LoadCombinations.h"
#include "utility/loadCombinations/actions/ActionContainer.h"


//! @brief Default constructor.
cmb_acc::LoadCombinations::LoadCombinations(void)
 {}

//! @brief Destructor.
cmb_acc::LoadCombinations::~LoadCombinations(void)
  {}

//! @brief Constructor.
cmb_acc::LoadCombinations::LoadCombinations(const ActionContainer &acc)
  : comb_uls_transient(acc.GetPersistentesOTransit()), comb_uls_accid(acc.GetAccidentales()),
    comb_uls_sism(acc.GetSismicas()), comb_sls_poco_frec(acc.GetPocoFrecuentes()),
    comb_sls_frec(acc.GetFrecuentes()), comb_sls_cuasi_perm(acc.GetCuasiPermanentes())
  {}


//! @brief Concatenate the combinations from both objects.
void cmb_acc::LoadCombinations::Concat(const LoadCombinations &others)
  {
    comb_uls_transient= LoadCombinationVector::Concat(comb_uls_transient,others.comb_uls_transient,0.0);
    comb_uls_accid= LoadCombinationVector::Concat(comb_uls_accid,others.comb_uls_accid,0.0);
    comb_uls_sism= LoadCombinationVector::Concat(comb_uls_sism,others.comb_uls_sism,0.0);
    comb_sls_poco_frec= LoadCombinationVector::Concat(comb_sls_poco_frec,others.comb_sls_poco_frec,0.0);
    comb_sls_frec= LoadCombinationVector::Concat(comb_sls_frec,others.comb_sls_frec,0.0);
    comb_sls_cuasi_perm= LoadCombinationVector::Concat(comb_sls_cuasi_perm,others.comb_sls_cuasi_perm,0.0);
  }
