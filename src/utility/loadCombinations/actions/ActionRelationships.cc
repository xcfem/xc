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
//ActionRelationships.cxx

#include "ActionRelationships.h"
#include "utility/utils/text/text_string.h"
#include "utility/functions/algebra/ExprAlgebra.h"

#include "boost/regex.hpp"
#include "utility/loadCombinations/comb_analysis/LoadCombinationVector.h"

//! @brief Elimina el factor que multiplica a la acción en la cadena de
//! la forma "1.35*A" que se pasa como parámetro.
std::string cmb_acc::ActionRelationships::limpia(const std::string &str)
  {
    std::string retval= str;
    if(str.find('*')!=std::string::npos)
      retval=copia_desde(str,'*');
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
  : CommandEntity(), incompatibles(0), maestras(0), nodet(false), contiene_incomp(false) {}

//! @brief Return a string with the names of the list
//! separated by commas.
std::string cmb_acc::ActionRelationships::names(const dq_string &l) const
  {
    std::string retval;
    if(!l.empty())
      {
        const_iterator i=l.begin();
        retval= (*i); i++;
        for(;i!=l.end();i++)
          retval+= "," + (*i);
      }
    return retval;
  }

//! @brief Añade a las acciones incompatibles la lista de expresiones regulares que se pasa como parámetro.
void cmb_acc::ActionRelationships::concat_incompatibles(const dq_string &other)
  {
    for(const_iterator i= other.begin();i!=other.end();i++)
      AgregaIncompatible(*i);
  }

//! @brief Añade a las acciones maestras la lista de expresiones regulares que se pasa como parámetro.
void cmb_acc::ActionRelationships::concat_maestras(const dq_string &other)
  {
    for(const_iterator i= other.begin();i!=other.end();i++)
      AgregaMaestra(*i);
  }

//! @brief Return verdadero si alguna de las text strings de combActionsNames
//! verifican la expresión regular "exprReg".
bool cmb_acc::ActionRelationships::match(const std::string &exprReg,const dq_string &combActionsNames) const
  {
    bool retval= false;
    boost::regex expresion(exprReg); //Inicializamos la expresión regular.
    for(const_iterator j= combActionsNames.begin();j!=combActionsNames.end();j++)
      {
        const std::string &test= *j;
        retval= regex_match(test, expresion);
        if(retval) break; //No hace falta seguir.
      }
    return retval;
  }

//! @brief Return verdadero si alguna de las text strings que se pasan como parámetro
//! verifica alguna de las expresiones del contenedor de expresiones regulares "exprReg".
bool cmb_acc::ActionRelationships::match_any(const dq_string &exprReg,const dq_string &combActionsNames) const
  {
    bool retval= false;
    for(const_iterator i= exprReg.begin();i!=exprReg.end();i++)
      {
        const std::string str= *i;
        retval= match(str,combActionsNames);
        if(retval) break; //No hace falta seguir.
      }
    return retval;
  }

//! @brief Return verdadero si para cada una de las expresiones regulares de "exprReg"
//! existe alguna text string de combActionsNames que la verifica.
bool cmb_acc::ActionRelationships::match_all(const dq_string &exprReg,const dq_string &combActionsNames) const
  {
    bool retval= false;
    for(const_iterator i= exprReg.begin();i!=exprReg.end();i++)
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
    for(const_iterator i= maestras.begin();i!=maestras.end();i++)
      {
        boost::regex expresion(*i); //Inicializamos la expresión regular.
        retval= regex_match(nmb,expresion);
        if(!retval) break; //No hace falta seguir.
      }
    return retval;
  }

//! @brief Remove from the masters lists those which names match with
//! the argument name.
void cmb_acc::ActionRelationships::updateMaestras(const std::string &nmb)
  {
    if(!maestras.empty())
      {
        const dq_string combActionsNames= get_combination_actions_names(nmb); //Names of the actions in this combination.
        dq_string nuevas;
        for(const_iterator i= maestras.begin();i!=maestras.end();i++)
          if(!match(*i,combActionsNames)) //No se encontró la maestra.
            nuevas.push_back(*i);
        maestras= nuevas;
      }
  }

void cmb_acc::ActionRelationships::concat(const ActionRelationships &other)
  {
    concat_incompatibles(other.incompatibles);
    concat_maestras(other.maestras);
  }

std::string cmb_acc::ActionRelationships::incompatibleNames(void) const
  { return names(incompatibles); }
std::string cmb_acc::ActionRelationships::masterNames(void) const
  { return names(maestras); }

void cmb_acc::ActionRelationships::Print(std::ostream &os) const
  {
    os << "incompatibles: {" << incompatibleNames() << "}; maestras: {"
       << masterNames() << "}";
    if(contiene_incomp) os << " contiente incompatibles." << std::endl;
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
      if(!(comb[i].getRelaciones().contieneIncomp())) //Si no contiene acciones incompatibles.
       {
         tmp[cont]= comb[i]; //La agregamos.
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
      if(!(comb[i].getRelaciones().tieneHuerfanas())) //No contiene esclavas huerfanas.
        {
          tmp[cont]= comb[i]; //La agregamos.
          cont++;
        }
    retval.resize(cont);
    for(size_t i=0;i<cont;i++)
      retval[i]= tmp[i];
    return retval;
  }
