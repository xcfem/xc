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
//ActionRelationships.hxx

#ifndef RELACCIONES_H
#define RELACCIONES_H

#include "utility/kernel/CommandEntity.h"

//! \namespace<cmb_acc>
//! Routines that generate combinations of actions.
namespace cmb_acc{

class LoadCombinationVector;

//! @ingroup CMBACC
//
//! @brief Relationship of an action with the other ones.
class ActionRelationships: public CommandEntity
  {
  public:
    typedef std::deque<std::string> dq_string;
    typedef dq_string::iterator iterator;
    typedef dq_string::const_iterator const_iterator;

    static std::string limpia(const std::string &str);
    static std::deque<std::string> get_combination_addends(const std::string &str);
    static std::deque<std::string> get_combination_actions_names(const std::string &str);
  private:
    dq_string incompatibles; //!< Expresiones regulares verdaderas para acciones incompatibles con ésta.
    dq_string maestras; //!< Expresiones regulares verdaderas para acciones de las que ésta es esclava.
    bool nodet; //!< Verdadero si la acción no puede ser determinante.
    bool contiene_incomp; //!< True if the combination contains incompatible actions.

    std::string names(const dq_string &) const;
    void concat_incompatibles(const dq_string &);
    void concat_maestras(const dq_string &);
    bool match(const std::string &,const dq_string &) const;
    bool match_any(const dq_string &,const dq_string &) const;
    bool match_all(const dq_string &,const dq_string &) const;

  public:
    ActionRelationships(void);

    //! @brief Agrega a la lista de incompatibles la expresión regular que se pasa como parámetro.
    inline void AgregaIncompatible(const std::string &str)
      { incompatibles.push_back(str); }
    //! @brief Agrega a la lista de maestras la expresión regular que se pasa como parámetro.
    inline void AgregaMaestra(const std::string &str)
      { maestras.push_back(str); }
    inline bool NoDeterminante(void) const
      { return nodet; }
    void setNoDeterminante(const bool &b)
      { nodet= b; }

    bool matchIncompatibles(const dq_string &) const;
    bool incompatible(const std::string &) const;

    bool esclavaDe(const std::string &) const;
    bool esEsclava(void) const
      { return !maestras.empty(); }
    bool tieneHuerfanas(void) const
      { return !maestras.empty(); }
    void updateMaestras(const std::string &nmb);

    inline void setContieneIncomp(bool b)
      { contiene_incomp= b; }
    //! @brief Return true if this combination contains incompatible
    //! actions.
    inline bool contieneIncomp(void) const
      { return contiene_incomp; }

    void concat(const ActionRelationships &);

    std::string incompatibleNames(void) const;
    std::string masterNames(void) const;
    void Print(std::ostream &os) const;
  };

std::ostream &operator<<(std::ostream &os,const ActionRelationships &acc);

const LoadCombinationVector &get_compatibles(const LoadCombinationVector &);
const LoadCombinationVector &filtraCombsEsclavasHuerfanas(const LoadCombinationVector &);

} //fin namespace nmb_acc.

#endif
