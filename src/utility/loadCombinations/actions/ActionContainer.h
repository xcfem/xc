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
//ActionContainer.h
//Almacena todas las familias de acciones para la estructura o el elemento estructural.

#ifndef ACTIONCONTAINER_H
#define ACTIONCONTAINER_H

#include "ActionsFamily.h"

namespace cmb_acc{
//! @ingroup CMBACC
//
//! @brief Almacena todas las familias de acciones.
class ActionContainer: public CommandEntity
  {
  protected:
    ActionsFamily G; //!< Permanent actions.
    ActionsFamily G_aster; //!< Non-constant permanent actions.
    ActionsFamily Q; //!< Variable actions.
    ActionsFamily A; //!< Accidental actions.
    ActionsFamily AS; //!< Earthquake actions.

    LoadCombinationVector GetPermanentes(const bool &uls,const bool &sit_accidental) const;
    LoadCombinationVector GetVariables(const LoadCombinationVector &permanentes,const bool &uls,const bool &sit_accidental,const short int &v) const;
    LoadCombinationVector GetAccSis(const LoadCombinationVector &previas,const ActionsFamily &Acc) const;

  protected:
    friend class ActionWeightingMap;
  public:
    ActionContainer(void);
    inline virtual ~ActionContainer(void) {}

    ActionRValue &insert(const std::string &,const Action &,const std::string &combination_factors_name="",const std::string &partial_safety_factors_name="");

    const ActionsFamily &getPermanentActions(void) const;
    void setPermanentActions(const ActionsFamily &);
    const ActionsFamily &getPermanentActionsNC(void) const;
    void setPermanentActionsNC(const ActionsFamily &);
    const ActionsFamily &getVariableActions(void) const;
    void setVariableActions(const ActionsFamily &);
    const ActionsFamily &getAccidentalActions(void) const;
    void setAccidentalActions(const ActionsFamily &);
    const ActionsFamily &getSeismicActions(void) const;
    void setSeismicActions(const ActionsFamily &);
    
    //Estados límite últimos.
    LoadCombinationVector GetPersistentesOTransit(void) const;
    LoadCombinationVector GetAccidentales(void) const;
    LoadCombinationVector GetSismicas(void) const;
    LoadCombinationVector GetCombULS(void) const;
    //Estados límite de servicio.
    LoadCombinationVector GetPocoFrecuentes(void) const;
    LoadCombinationVector GetFrecuentes(void) const;
    LoadCombinationVector GetCuasiPermanentes(void) const;
    LoadCombinationVector GetCombSLS(void) const;

  };
} //fin namespace nmb_acc.

#endif
