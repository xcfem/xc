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
//Action.cxx

#include "Action.h"
#include "boost/lexical_cast.hpp"
#include "utility/utils/text/text_string.h"
#include "utility/utils/text/en_letra.h"
#include "utility/utils/text/latex.h"
#include "utility/kernel/python_utils.h"
#include "ActionWrapperList.h"
#include "utility/functions/algebra/ExprAlgebra.h"

const double cmb_acc::Action::zero= 1e-6;

//! @brief Default constructor.
//! @param n: action name.
//! @param descrip: action description.
cmb_acc::Action::Action(const std::string &n, const std::string &descrip)
  : NamedEntity(n), description(descrip), relaciones(), nodet(false), f_pond(1.0) {}

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

//! @brief When the actions is a combination return its decomposition.
cmb_acc::Action::map_descomp cmb_acc::Action::getComponents(void) const
  {
    map_descomp descomp;
    typedef std::deque<std::string> dq_string;
    const std::string name= getName();
    if((name.find('*')==std::string::npos) and (name.find('+')==std::string::npos)) //No product or sum.
      descomp[name]= 1.0;
    else
      {
	dq_string str_sumandos= ActionRelationships::get_combination_addends(name);
	for(dq_string::iterator i= str_sumandos.begin();i!=str_sumandos.end();i++)
	  {
	    const std::string &str_sum_i= *i;
	    if(str_sum_i.find('*')==std::string::npos) //No product.
              {
		descomp[str_sum_i]= 1.0;
	      }
	    else
	      {
		dq_string str_prod= separa_cadena(str_sum_i,"*");
		const size_t sz2= str_prod.size();
		if(sz2!=2)
		  std::cerr << "The " << str_sum_i
			    << " term of the sum is wrong." << std::endl;
		else
		  {
		    const float factor= boost::lexical_cast<float>(q_blancos(str_prod[0]));
		    const std::string nmb_acc= q_blancos(str_prod[1]);
		    descomp[nmb_acc]= factor;
		  }
	      }
	  }
      }
    return descomp;
  }

//! @brief Return the action components in a Python dictionary.
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

//! @brief Return a Python dictionary with the object members values.
boost::python::dict cmb_acc::Action::getPyDict(void) const
  {
    boost::python::dict retval= NamedEntity::getPyDict();
    retval["description"]= description;
    retval["relations"]= relaciones.getPyDict();
    retval["nodet"]= nodet;
    retval["f_pond"]= f_pond;
    return retval;
  }

//! @brief Set the values of the object members from a Python dictionary.
void cmb_acc::Action::setPyDict(const boost::python::dict &d)
  {
    NamedEntity::setPyDict(d);
    description= boost::python::extract<std::string>(d["description"]);
    boost::python::dict tmp= boost::python::extract<boost::python::dict>(d["relations"]);
    relaciones.setPyDict(tmp);
    nodet= boost::python::extract<bool>(d["nodet"]);
    f_pond= boost::python::extract<double>(d["f_pond"]);
  }


//! @brief When it's a combination, it returns the factors that multiply
//! each of the actions in the argument.
std::vector<double> cmb_acc::Action::getCoefficients(const std::vector<std::string> &base) const
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

//! @brief When it's a combination, it returns the factors that multiply
//! each of the actions in the argument.
boost::python::list cmb_acc::Action::getCoefficientsPy(const boost::python::list &base) const
  {
    std::vector<std::string> v_string;
    const size_t sz= boost::python::len(base);
    for(size_t i= 0;i<sz;++i)
      v_string.push_back(boost::python::extract<std::string>(base[i]));
    const std::vector<double> tmp= this->getCoefficients(v_string);
    boost::python::list retval= vector_double_to_py_list(tmp);
    return retval;
  }

//! \fn cmb_acc::Action::clean_names(void)
//! @brief ??
void cmb_acc::Action::clean_names(void)
  {
    NamedEntity::Name()= ActionRelationships::limpia(getName());
    description= ActionRelationships::limpia(description);
  }

//! \fn cmb_acc::Action::multiplica(const double &d)
//! @brief Multiplica la acción por el escalar que se pasa como parámetro.
void cmb_acc::Action::multiplica(const double &d)
  {
    f_pond*= d;
    clean_names();
    const std::string strnum= num2str(f_pond,2);
    map_descomp components= getComponents();
    const size_t sz= components.size();
    if(getName().empty())
      std::cerr << getClassName() << "::" << __FUNCTION__
	        << "; action with empty and f_pond= " << f_pond << std::endl;
    if(sz==1)
      {
        NamedEntity::Name()= strnum + "*" + getName();
        description= strnum + "*" + description;
      }
    else
      {
	std::string str_product= "";
	for(map_descomp::const_iterator i= components.begin(); i!= components.end(); i++)
	  {
	    const std::string key= (*i).first;
	    const double factor= (*i).second*d;
	    const std::string str_factor= num2str(factor,2);
	    str_product+= str_factor + "*" + key + " + ";
	  }
	str_product.resize(str_product.size () - 3); // Remove the last " + "
	NamedEntity::Name()= str_product;
	description= str_product;
      }
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
        description+= " + " + f.description;
      }
    else
      {
	NamedEntity::Name()= f.getName();
        description= f.description;
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

//! @brief Return true si la acción que se pasa como parámetro es incompatible con esta,
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
//! @brief Return true if this actions is not compatible with
//! the argument one, so they cannot be present both in the same combination.
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

//! @brief Print stuff.
void cmb_acc::Action::Print(std::ostream &os) const
  {
    os << getName() << "; '" << description << "'; ";
    relaciones.Print(os);
  }

//! @brief Return true if actions are incompatible.
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
