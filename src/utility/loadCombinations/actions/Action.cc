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
//Action.cxx

#include "Action.h"
#include "boost/lexical_cast.hpp"
#include "utility/utils/text/text_string.h"
#include "utility/utils/text/en_letra.h"
#include "utility/utils/text/latex.h"


#include "ActionRValueList.h"
#include "utility/functions/algebra/ExprAlgebra.h"

const double cmb_acc::Action::zero= 1e-6;

//! @brief Default constructor.
cmb_acc::Action::Action(const std::string &n, const std::string &descrip)
  : NamedEntity(n), descripcion(descrip),relaciones(), f_pond(1.0) {}

//! \fn cmb_acc::Action::NULA(void)
//! @brief Return una acción nula.
cmb_acc::Action cmb_acc::Action::NULA(void)
  {
    Action retval;
    retval.f_pond= 0.0;
    return retval;
  }

//! @brief Return true if the action is not a combination of simpler ones.
bool cmb_acc::Action::Simple(void) const
  { return (getName().find('+')==std::string::npos); }

//! @brief Return the expandend name by applying the distributive property
//! of the multiplication so, if the combination name is '1.5*G1+1.35*(G2+F2)'
//! it returns '1.5*G1+1.35*G2+1.35*F2'.
const std::string cmb_acc::Action::getExpandedName(void) const
  {
    std::string retval= getName();
    if(has_char(retval,'('))
      {
        const ExprAlgebra tmp(retval);
        retval= expand(tmp).GetString();
      }
    return retval;
  }

//! @brief Return la decomposition when the action is a combination.
cmb_acc::Action::map_descomp cmb_acc::Action::getComponents(void) const
  {
    map_descomp descomp;
    typedef std::deque<std::string> dq_string;
    dq_string str_sumandos= ActionRelationships::get_combination_addends(getName());
    for(dq_string::iterator i= str_sumandos.begin();i!=str_sumandos.end();i++)
      {
        const std::string &str_sum_i= *i;
        dq_string str_prod= separa_cadena(str_sum_i,"*");
        const size_t sz2= str_prod.size();
        if(sz2!=2)
	  std::cerr << "El sumando: " << str_sum_i << " está mal expresado." << std::endl;
        else
          {
            const float factor= boost::lexical_cast<float>(q_blancos(str_prod[0]));
            const std::string nmb_acc= q_blancos(str_prod[1]);
            descomp[nmb_acc]= factor;
          } 
      }
    return descomp;
  }

//! @brief Return la decomposition when the action is a combination.
boost::python::dict cmb_acc::Action::getComponentsPy(void) const
  {
    boost::python::dict retval; 
    const map_descomp descomp= getComponents();
    for(map_descomp::const_iterator i= descomp.begin(); i!= descomp.end(); i++)
      {
	const std::string key= (*i).first;
	const double factor= (*i).second;
	retval[key]= factor;
      }
    return retval;
  }

//! @brief When it's a combination, it returns the factors that multiply
//! each of the actions in the argument.
std::vector<double> cmb_acc::Action::getCoeficientes(const std::vector<std::string> &base) const
  {
    const size_t sz= base.size();
    std::vector<double> retval(sz,0.0);
    const map_descomp descomp= getComponents();
    for(size_t i= 0;i<sz;i++)
      {
        const std::string &nmb_accion= base[i];
	map_descomp::const_iterator j= descomp.find(nmb_accion);
        if(j!=descomp.end())
          retval[i]= j->second;
      }
    return retval;
  }

//! \fn cmb_acc::Action::clean_names(void)
//! @brief ??
void cmb_acc::Action::clean_names(void)
  {
    NamedEntity::Name()= ActionRelationships::limpia(getName());
    descripcion= ActionRelationships::limpia(descripcion);
  }

//! \fn cmb_acc::Action::multiplica(const double &d)
//! @brief Multiplica la acción por el escalar que se pasa como parámetro.
void cmb_acc::Action::multiplica(const double &d)
  {
    f_pond*= d;
    clean_names();
    const std::string strnum= num2str(f_pond,2);
    NamedEntity::Name()= strnum + "*" + getName();
    descripcion= strnum + "*" + descripcion;
  }

//! \fn cmb_acc::Action::suma(const Action &f)
//! @brief Suma a ésta la acción que se pasa como parámetro (sólo en el caso de que sean compatibles).
void cmb_acc::Action::suma(const Action &f)
  {
    if(f.Nula(zero)) return;

    //Mark the incompatible combinations.
    if((Incompatible(f))) 
      relaciones.setContieneIncomp(true);
    if(f.relaciones.contieneIncomp())
      relaciones.setContieneIncomp(true);

    if(getName().size()>0)
      {
        NamedEntity::Name()+= " + " + f.getName();
        descripcion+= " + " + f.descripcion;
      }
    else
      {
	NamedEntity::Name()= f.getName();
        descripcion= f.descripcion;
      }
    relaciones.concat(f.relaciones);
    relaciones.updateMainActions(getName());
    if(Nula(zero) && f.Nula(zero)) //Si ambas son nulas la suma es nula.
      f_pond= 0.0;
    else //Otherwise we don't know.
      f_pond= 1.0;
    if(!Owner())
      set_owner(const_cast<CommandEntity *>(f.Owner()));
  }

//! @brief Return verdadero si la acción que se pasa como parámetro es incompatible con esta,
//! es decir que ambas no pueden estar presentes en la misma hipótesis.
//!
//! Action will be incompatible if its name verifies one or more of the 
//! regular expressions contained in the incompatibles member.
//! La acción no podrá ser incompatible consigo misma.
bool cmb_acc::Action::incompatible(const Action &f) const
  {
    bool retval= false;
    if(this != &f) //La carga no puede ser incompatible consigo misma.
      retval= relaciones.incompatible(f.getName());
    return retval;
  }

//! \fn cmb_acc::Action::Incompatible(const Action &f) const
//! @brief Return verdadero si la acción que se pasa como parámetro es incompatible con esta,
//! es decir que ambas no pueden estar presentes en la misma hipótesis.
//!
//! @param f: Action cuya incompatibilidad con ésta se comprueba.
//! Action will be incompatible if its name verifies one or more of the 
//! regular expressions contained in the incompatibles member.
//! La acción no podrá ser incompatible consigo misma.
bool cmb_acc::Action::Incompatible(const Action &f) const
  {
    bool retval= false;
    if(this != &f) //La carga no puede ser incompatible consigo misma.
      {
        retval= relaciones.incompatible(f.getName());
        if(!retval) retval= f.relaciones.incompatible(getName());
      }
    return retval;
  }

std::string cmb_acc::Action::incompatibleStringList(ActionRValueList *af) const
  {
    std::string retval;
    if(af)
      {
        std::deque<const cmb_acc::Action *> incomp= listaIncompatibles(this,af->begin(),af->end());
        if(!incomp.empty())
          retval= actionPtrsNames(incomp.begin(),incomp.end());
      }
    else
      std::cerr << "El puntero a la familia de acciones es nulo." << std::endl;
    return retval; 
  }

//! @brief Print stuff.
void cmb_acc::Action::Print(std::ostream &os) const
  {
    os << getName() << "; '" << descripcion << "'; ";
    relaciones.Print(os);
  }

//! @brief Return verdadero si las acciones de índices i y j son incompatibles.
bool cmb_acc::incompatibles(const Action &acc_i,const Action &acc_j)
  {
    bool retval= false;
    if(&acc_i!=&acc_j) //Una acción nunca es incompatible consigo misma.
      {
        if(acc_i.Incompatible(acc_j))
          retval= true;
        else
          if(acc_j.Incompatible(acc_i)) retval= true;
      }
    return retval;
  }

std::ostream &cmb_acc::operator<<(std::ostream &os,const Action &a)
  {
    a.Print(os);
    return os;
  }
