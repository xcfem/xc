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
//Action.hxx

#ifndef ACTION_H
#define ACTION_H

#include <string>
#include <vector>
#include <deque>
#include <cmath>
#include "utility/kernel/NamedEntity.h"
#include "ActionRelationships.h"

//! \namespace<cmb_acc>
//! Routines to generate combinations of actions.
namespace cmb_acc{

class ActionWrapperList;

//! @defgroup CMBACC Routines to generate combinations of actions.
//
//
//! @brief Action or linear combination of actions.
//! @ingroup CMBACC
class Action: public NamedEntity
  {
    std::string description; //!< Description de la acción p. ej. "Viento derecho".
    ActionRelationships relaciones; //!< Relations of this action with the rest of them.
    bool nodet; //!< True if the action cannot be determinant.
    double f_pond; //!< Factor que pondera a la acción.

    void clean_names(void);
    bool incompatible(const Action &f) const;
    void multiplica(const double &d);
    void suma(const Action &f);
  public:
    static const double zero; //!< Treshold to consider the action as zero.
    Action(const std::string &n="", const std::string &descrip="");

    static Action NULA(void);
    //! @brief Sets the name to the action.
    inline void setName(const std::string &nmb)
      { NamedEntity::Name()= nmb; }
    const std::string getExpandedName(void) const;
    //! @brief Return the description of the action.
    inline const std::string &getDescription(void) const
      { return description; }
    //! @brief Set the description of the action.
    inline void setDescription(const std::string &desc)
      { description= desc; }
    const ActionRelationships &getRelaciones(void) const
      { return relaciones; }
    inline double getWeightingFactor(void) const
      { return f_pond; }
    inline void setWeightingFactor(const double &f)
      { f_pond= f; }

    inline bool notDeterminant(void) const
      { return nodet; }
    void setNotDeterminant(const bool &b)
      { nodet= b; }

    typedef std::map<std::string,float> map_descomp;
    map_descomp getComponents(void) const;
    boost::python::dict getComponentsPy(void) const;
    boost::python::dict getPyDict(void) const;
    void setPyDict(const boost::python::dict &);    

    bool Incompatible(const Action &) const;
    //! @brief Return true if this actions is compatible with
    //! the argument one.
    inline bool Compatible(const Action &f) const
      { return !Incompatible(f); }

    bool Simple(void) const;
    //! @brief Return verdadero si la acción es nula.
    inline bool Nula(const double &tol= zero) const
      { return (fabs(f_pond)<tol); }
     //! @brief Return la acción multiplicada por un escalar.
    Action GetMult(const double &d) const
      {
        Action retval(*this);
        retval*=(d);
        return retval;
      }
    //! @brief Producto por un escalar.
    inline Action &operator*=(const double &d)
      {
        multiplica(d);
        return *this;
      }
    //! @brief Addition
    inline Action &operator+=(const Action &f)
      {
        suma(f);
        return *this;
      }
    //! @brief Product by a scalar.
    inline friend Action operator*(const double &d,const Action &f)
      { return f.GetMult(d); }
    //! @brief Product by a scalar.
    inline friend Action operator*(const Action &f,const double &d)
      { return d*f; }
    //! @brief Addition.
    inline friend Action operator+(const Action &f1,const Action &f2)
      {
        Action retval(f1);
        retval+=(f2);
        return retval;
      }

    std::vector<double> getCoeficientes(const std::vector<std::string> &) const;
    virtual void Print(std::ostream &os) const;
  };

std::ostream &operator<<(std::ostream &os,const Action &acc);

bool incompatibles(const Action &acc_i,const Action &acc_j);

//! @brief Returns a list with the action names.
template <class InputIterator>
std::string actionsNames(InputIterator begin,InputIterator end)
  {
    std::string retval;
    InputIterator i=begin;
    retval= (*i).getName(); i++;
    for(;i!=end;i++)
      retval+= "," + (*i).getName();
    return retval;
  }

//! @brief Returns a list with the action names.
template <class InputIterator>
std::string actionPtrsNames(InputIterator begin,InputIterator end)
  {
    std::string retval;
    InputIterator i=begin;
    retval= (*i)->getName(); i++;
    for(;i!=end;i++)
      retval+= "," + (*i)->getName();
    return retval;
  }
} //fin namespace nmb_acc.

#endif
