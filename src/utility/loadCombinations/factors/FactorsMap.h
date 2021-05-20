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
//FactorsMap.h
//Contenedor de coeficientes de simultaneidad de acciones.

#ifndef FACTORSMAP_H
#define FACTORSMAP_H

#include "utility/kernel/CommandEntity.h"
#include <map>


namespace cmb_acc {

class LoadCombinationVector;

//! @ingroup CMBACC
//
//! @brief Contenedor de coeficientes de simultaneidad de acciones.
template <class Factors>
class FactorsMap: public CommandEntity, public std::map<std::string,Factors>
  {
  public:
    typedef typename std::map<std::string,Factors>::const_iterator const_iterator;
    typedef typename std::map<std::string,Factors>::iterator iterator;
  private:
    static Factors default_factors; //!< Default values (all =1.0).
    bool exists(const std::string &) const;
    Factors *getPtrCoefs(const std::string &);
    Factors *crea_coefs(const std::string &);
    void print_err_not_found(const std::string &, const std::string &) const;
  public:
    FactorsMap(void);
    const Factors *getPtrCoefs(const std::string &) const;
    const Factors &BuscaCoefs(const std::string &) const;
    void insert(const std::string &,const Factors &);
    std::deque<std::string> getNames(void) const;
  };

template <class Factors>
Factors FactorsMap<Factors>::default_factors;

//! @brief Return true if the name exists.
template <class Factors>
bool FactorsMap<Factors>::exists(const std::string &name) const
  { return (this->find(name)!=this->end()); }

//! @brief Error message when name not found.
template <class Factors>
void FactorsMap<Factors>::print_err_not_found(const std::string &functionName, const std::string &name) const
   {
      std::cerr << getClassName() << "::" << functionName
		<< "; factors with name: '"
		<< name << "' not found." << std::endl
		<< " candidates are: ";
      const std::deque<std::string> candidates= this->getNames();
      if(!candidates.empty())
	{
	  std::deque<std::string>::const_iterator i= candidates.begin();
	  std::cerr << *i;
	  i++;
          for(;i!=candidates.end();i++)
	    std::cerr << ", " << *i;
	  std::cerr << std::endl;
	}
   }
   
//! @brief Return the names of the factor families.
template <class Factors>
std::deque<std::string> FactorsMap<Factors>::getNames(void) const
  {
    std::deque<std::string> retval;
    for(const_iterator i= this->begin();i!= this->end();i++)
      retval.push_back(i->first);
    return retval;
  }

 
//! @brief Return a pointer to the factors named with the string
//! argument.
template <class Factors>
Factors *FactorsMap<Factors>::getPtrCoefs(const std::string &name)
  {
    Factors *retval= nullptr;
    if(this->exists(name))
      retval= &((*this)[name]);
    else
      this->print_err_not_found(__FUNCTION__,name);
    return retval;
  }

//! @brief Return the factors identified by name.
template <class Factors>
const Factors &FactorsMap<Factors>::BuscaCoefs(const std::string &name) const
  {
    if(this->exists(name))
      return this->find(name)->second;
    else
      {
        this->print_err_not_found(__FUNCTION__,name);
	return default_factors;
      }
  }

//! @brief Return a pointer to the factors identified by name.
template <class Factors>
const Factors *FactorsMap<Factors>::getPtrCoefs(const std::string &name) const
  {
    const Factors *retval= nullptr;
    if(this->exists(name))
      retval= &(this->find(name)->second);
    else
      this->print_err_not_found(__FUNCTION__,name);
    return retval;
  }

//! @brief Default constructor.
template <class Factors>
FactorsMap<Factors>::FactorsMap(void)
  : CommandEntity() {}

//! @brief Creates the factors with the name being passed as parameter.
template <class Factors>
Factors *FactorsMap<Factors>::crea_coefs(const std::string &name)
  {
    Factors *retval= nullptr;
    if(this->exists(name))
      retval= &(this->find(name)->second);
    else //los coeficientes son nuevos.
      {
        Factors tmp;
        (*this)[name]= tmp;
        retval= getPtrCoefs(name);
      }
    return retval;
  }

//! @brief Inserts the coefficients.
template <class Factors>
void FactorsMap<Factors>::insert(const std::string &name,const Factors &c)
  { (*this)[name]= c; }

} // fin namespace cmb_acc

#endif
