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

class ActionRValueList;

//! @defgroup CMBACC Routines to generate combinations of actions.
//
//! @ingroup CMBACC
//
//! @brief Action or linear combination of actions.
class Action: public NamedEntity
  {
    std::string descripcion; //!< Descripcion de la acción p. ej. "Viento derecho".
    ActionRelationships relaciones; //!< Relaciones de una acción con el resto.
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
    //! @brief Return la descripción de la acción.
    inline const std::string &GetDescripcion(void) const
      { return descripcion; }
    //! @brief Asigna a la acción la descripción que se pasa como parámetro.
    inline void SetDescripcion(const std::string &desc)
      { descripcion= desc; }
    const ActionRelationships &getRelaciones(void) const
      { return relaciones; }
    inline double getWeightingFactor(void) const
      { return f_pond; }
    inline void setWeightingFactor(const double &f)
      { f_pond= f; }
    typedef std::map<std::string,float> map_descomp;
    map_descomp getComponents(void) const;
    boost::python::dict getComponentsPy(void) const;
    

    //! @brief Return verdadero si esta acción es incompatible con la que se pasa como parámetro.
    bool Incompatible(const Action &f) const;
    //! @brief Return verdadero si esta acción es compatible con la que se pasa como parámetro.
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

    std::string incompatibleStringList(ActionRValueList *af) const;
    std::vector<double> getCoeficientes(const std::vector<std::string> &) const;
    virtual void Print(std::ostream &os) const;
  };

std::ostream &operator<<(std::ostream &os,const Action &acc);

bool incompatibles(const Action &acc_i,const Action &acc_j);

//! @brief Return the list of actions incompatible with the one 
//! being passed as parameter.
template <class InputIterator>
std::deque<const Action *> listaIncompatibles(const Action *acc,InputIterator begin,InputIterator end)
  {
    std::deque<const Action *> retval;
    for(InputIterator i=begin;i!=end;i++)
      {
        const Action &data= *i;
        if(acc->Incompatible(data)) retval.push_back(&data);
      }
    return retval;
  }

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
