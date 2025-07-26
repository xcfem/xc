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
//ActionRelationships.hxx

#ifndef RELACCIONES_H
#define RELACCIONES_H

#include "utility/kernel/CommandEntity.h"
#include <set>

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
    typedef std::set<std::string> set_string;
    
    typedef std::deque<std::string> dq_string;

    static std::string limpia(const std::string &str);
    static std::deque<std::string> get_combination_addends(const std::string &str);
    static std::deque<std::string> get_combination_actions_names(const std::string &str);
  private:
    set_string incompatibles; //!< Expresiones regulares verdaderas para acciones incompatibles con ésta.
    dq_string main_actions; //!< Expresiones regulares verdaderas para acciones de las que ésta es esclava.
    bool contiene_incomp; //!< True if the combination contains incompatible actions.
    
    template<typename InputIterator>
    std::string names(InputIterator, InputIterator) const;
    void concat_incompatibles(const set_string &);
    void concat_main_actions(const dq_string &);
    bool match(const std::string &,const dq_string &) const;
    bool match_any(const set_string &,const dq_string &) const;
    bool match_all(const set_string &,const dq_string &) const;

  public:
    ActionRelationships(void);

    //! @brief Append the regular expression argument to the list of incompatible actions.
    inline void appendIncompatible(const std::string &str)
      { incompatibles.insert(str); }
    //! @brief Append the regular expression argument to the list of main actions.
    inline void appendMain(const std::string &str)
      { main_actions.push_back(str); }

    bool matchIncompatibles(const dq_string &) const;
    bool incompatible(const std::string &) const;

    bool esclavaDe(const std::string &) const;
    bool esEsclava(void) const
      { return !main_actions.empty(); }
    bool tieneHuerfanas(void) const
      { return !main_actions.empty(); }
    void updateMainActions(const std::string &nmb);

    inline void setContieneIncomp(bool b)
      { contiene_incomp= b; }
    //! @brief Return true if this combination contains incompatible
    //! actions.
    inline bool contieneIncomp(void) const
      { return contiene_incomp; }

    void concat(const ActionRelationships &);

    boost::python::dict getPyDict(void) const;
    void setPyDict(const boost::python::dict &);
    
    std::string incompatibleNames(void) const;
    std::string masterNames(void) const;
    void Print(std::ostream &os) const;
  };

std::ostream &operator<<(std::ostream &os,const ActionRelationships &acc);

const LoadCombinationVector &get_compatibles(const LoadCombinationVector &);
const LoadCombinationVector &filtraCombsEsclavasHuerfanas(const LoadCombinationVector &);
  
//! @brief Return a string with the names of the list
//! separated by commas.
template<typename InputIterator>
std::string cmb_acc::ActionRelationships::names(InputIterator begin, InputIterator end) const
  {
    std::string retval;
    if(begin!=end)
      {
	InputIterator i= begin;
        retval= (*i); i++;
        for(;i!= end;i++)
          retval+= "," + (*i);
      }
    return retval;
  }

} //fin namespace nmb_acc.

#endif
