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
//ActionRelationships.cxx

#include "ActionRelationships.h"
#include "utility/utils/text/text_string.h"
#include "utility/functions/algebra/ExprAlgebra.h"

#include "boost/regex.hpp"
#include "utility/load_combinations/comb_analysis/LoadCombinationVector.h"

//! @brief Elimina el factor que multiplica a la acción en la cadena de
//! la forma "1.35*A" que se pasa como parámetro.
std::string cmb_acc::ActionRelationships::limpia(const std::string &str)
  {
    std::string retval= str;
    if(str.find('*')!=std::string::npos)
      retval=copy_from(str,'*');
    return retval;
  }

//! @brief Return los sumandos de la text string que se pasa como parámetro.
std::deque<std::string> cmb_acc::ActionRelationships::get_combination_addends(const std::string &str)
  {
    std::deque<std::string> retval;
    if((has_char(str,'(')) || (has_char(str,')'))) //Tiene paréntesis
      {
        const ExprAlgebra e(str);
        const std::string s(expand(e).GetString());
        retval= separa_sumandos(s);
      }
    else
      return separa_sumandos(str);
    return retval;
  }

//! @brief Return the names of the actions of the combinations.
std::deque<std::string> cmb_acc::ActionRelationships::get_combination_actions_names(const std::string &str)
  {
    std::deque<std::string> retval= get_combination_addends(str);
    for(std::deque<std::string>::iterator i= retval.begin();i!=retval.end();i++)
      (*i)= q_blancos(limpia(*i)); 
    return retval;
  }

//! @brief Default constructor.
cmb_acc::ActionRelationships::ActionRelationships(void)
  : CommandEntity(), main_actions(0), contiene_incomp(false) {}


//! @brief Añade a las acciones incompatibles la lista de expresiones regulares que se pasa como parámetro.
void cmb_acc::ActionRelationships::concat_incompatibles(const set_string &other)
  {
    for(set_string::const_iterator i= other.begin();i!=other.end();i++)
      appendIncompatible(*i);
  }

//! @brief Añade a las acciones main_actions la lista de expresiones regulares que se pasa como parámetro.
void cmb_acc::ActionRelationships::concat_main_actions(const dq_string &other)
  {
    for(dq_string::const_iterator i= other.begin();i!=other.end();i++)
      appendMain(*i);
  }

//! @brief Return verdadero si alguna de las text strings de combActionsNames
//! verifican la expresión regular "exprReg".
bool cmb_acc::ActionRelationships::match(const std::string &exprReg,const dq_string &combActionsNames) const
  {
    bool retval= false;
    boost::regex expresion(exprReg); //Inicializamos la expresión regular.
    for(dq_string::const_iterator j= combActionsNames.begin();j!=combActionsNames.end();j++)
      {
        const std::string &test= *j;
        retval= regex_match(test, expresion);
        if(retval) break; //No hace falta seguir.
      }
    return retval;
  }

//! @brief Return verdadero si alguna de las text strings que se pasan como parámetro
//! verifica alguna de las expresiones del contenedor de expresiones regulares "exprReg".
bool cmb_acc::ActionRelationships::match_any(const set_string &exprReg,const dq_string &combActionsNames) const
  {
    bool retval= false;
    for(set_string::const_iterator i= exprReg.begin();i!=exprReg.end();i++)
      {
        const std::string str= (*i);
        retval= match(str,combActionsNames);
        if(retval) break; //No hace falta seguir.
      }
    return retval;
  }

//! @brief Return verdadero si para cada una de las expresiones regulares de "exprReg"
//! existe alguna text string de combActionsNames que la verifica.
bool cmb_acc::ActionRelationships::match_all(const set_string &exprReg,const dq_string &combActionsNames) const
  {
    bool retval= false;
    for(set_string::const_iterator i= exprReg.begin();i!=exprReg.end();i++)
      {
        const std::string str= *i;
        retval= match(str,combActionsNames);
        if(!retval) break; //No hace falta seguir.
      }
    return retval;
  }

//! @brief Return verdadero si la text strings que se pasan como parámetro
//! verifica alguna de las expresiones del contenedor "incompatibles".
bool cmb_acc::ActionRelationships::matchIncompatibles(const dq_string &sumandos) const
  { return match_any(incompatibles,sumandos); }

//! @brief Return true if the action which name is being passed as
//! parameter is not compatible with this one, 
//! otherwise said they cannot be present in the same hypothesis.
bool cmb_acc::ActionRelationships::incompatible(const std::string &nmb) const
  {
    bool retval= false;
    const std::deque<std::string> sumandos= get_combination_actions_names(nmb);
    retval= matchIncompatibles(sumandos);
    return retval;
  }

//! @brief Return true if the action which name is being passed as parameter
//! is a master of this one, otherwise said this one can only be present 
//! in a hypothesis if the master does it too.
bool cmb_acc::ActionRelationships::esclavaDe(const std::string &nmb) const
  { 
    bool retval= false;
    for(dq_string::const_iterator i= main_actions.begin();i!=main_actions.end();i++)
      {
        boost::regex expresion(*i); //Inicializamos la expresión regular.
        retval= regex_match(nmb,expresion);
        if(!retval) break; //No hace falta seguir.
      }
    return retval;
  }

//! @brief Remove from the masters lists those which names match with
//! the argument name.
void cmb_acc::ActionRelationships::updateMainActions(const std::string &nmb)
  {
    if(!main_actions.empty())
      {
        const dq_string combActionsNames= get_combination_actions_names(nmb); //Names of the actions in this combination.
        dq_string nuevas;
        for(dq_string::const_iterator i= main_actions.begin();i!=main_actions.end();i++)
          if(!match(*i,combActionsNames)) // main action not found.
            nuevas.push_back(*i);
        main_actions= nuevas;
      }
  }

void cmb_acc::ActionRelationships::concat(const ActionRelationships &other)
  {
    concat_incompatibles(other.incompatibles);
    concat_main_actions(other.main_actions);
  }

std::string cmb_acc::ActionRelationships::incompatibleNames(void) const
  { return this->names(incompatibles.begin(), incompatibles.end()); }

std::string cmb_acc::ActionRelationships::masterNames(void) const
  { return this->names(main_actions.begin(), main_actions.end()); }

void cmb_acc::ActionRelationships::Print(std::ostream &os) const
  {
    os << "incompatibles: {" << incompatibleNames() << "}; main_actions: {"
       << masterNames() << "}";
    if(contiene_incomp) os << " contains incompatible actions." << std::endl;
  }

//! @brief Return a Python dictionary with the object members values.
boost::python::dict cmb_acc::ActionRelationships::getPyDict(void) const
  {
    boost::python::dict retval= CommandEntity::getPyDict();
    boost::python::list lstIncomp;
    for(set_string::const_iterator i= incompatibles.begin();i!=incompatibles.end(); i++)
      lstIncomp.append(*i);
    retval["incompatibles"]= lstIncomp;
    boost::python::list lstMainAct;
    for(dq_string::const_iterator i= main_actions.begin();i!=main_actions.end(); i++)
      lstMainAct.append(*i);
    retval["main_actions"]= lstMainAct;
    retval["contiene_incomp"]= contiene_incomp;
    return retval;
  }

//! @brief Set the values of the object members from a Python dictionary.
void cmb_acc::ActionRelationships::setPyDict(const boost::python::dict &d)
  {
    CommandEntity::setPyDict(d);
    boost::python::list lstIncomp= boost::python::extract<boost::python::list>(d["incompatibles"]);
    const size_t szi= len(lstIncomp);
    for(size_t i=0; i<szi; i++)
      {
	const std::string &str= boost::python::extract<std::string>(lstIncomp[i]);
	incompatibles.insert(str);
      }
    boost::python::list lstMainAct= boost::python::extract<boost::python::list>(d["main_actions"]);
    const size_t szm= len(lstMainAct);
    for(size_t i=0; i<szm; i++)
      main_actions.push_back(boost::python::extract<std::string>(lstMainAct[i]));
    contiene_incomp= boost::python::extract<bool>(d["contiene_incomp"]);
  }

std::ostream &cmb_acc::operator<<(std::ostream &os,const ActionRelationships &a)
  {
    a.Print(os);
    return os;
  }

//! @brief Return the combinations filtering those that contain incompatible
//! actions.
const cmb_acc::LoadCombinationVector &cmb_acc::get_compatibles(const LoadCombinationVector &comb)
  {
    static LoadCombinationVector retval;
    const size_t sz= comb.size();
    if(sz<1)
      {
        retval.resize(0);
        return retval;
      }
    LoadCombinationVector tmp(sz);
    size_t cont=0;
    for(size_t i=0;i<sz;i++)
      if(!(comb[i].getRelaciones().contieneIncomp())) // has no incompatible actions.
       {
         tmp[cont]= comb[i]; // append it.
         cont++;
       }
    retval.resize(cont);
    for(size_t i=0;i<cont;i++)
      retval[i]= tmp[i];
    return retval;
  }

//! @brief Filters the combinations where an slave actions appears without
//! its master.
const cmb_acc::LoadCombinationVector &cmb_acc::filtraCombsEsclavasHuerfanas(const cmb_acc::LoadCombinationVector &comb)
  {
    static LoadCombinationVector retval;
    const size_t sz= comb.size();
    if(sz<1)
      {
        retval.resize(0);
        return retval;
      }
    LoadCombinationVector tmp(sz);
    size_t cont=0;
    for(size_t i=0;i<sz;i++)
      if(!(comb[i].getRelaciones().tieneHuerfanas())) // Has not orphan secondaries.
        {
          tmp[cont]= comb[i]; // append it.
          cont++;
        }
    retval.resize(cont);
    for(size_t i=0;i<cont;i++)
      retval[i]= tmp[i];
    return retval;
  }

