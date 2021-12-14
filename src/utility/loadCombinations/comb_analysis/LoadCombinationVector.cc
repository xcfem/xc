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
//LoadCombinationVector.cxx

#include "LoadCombinationVector.h"
#include "utility/utils/text/en_letra.h"


//! @brief Constructor.
cmb_acc::LoadCombinationVector::LoadCombinationVector(const size_t &sz)
  : std::vector<Action>(sz), CommandEntity()
  {}

        
//! @brief Return true if all the combinations in the vector are null.
bool cmb_acc::LoadCombinationVector::Nula(const double &tol) const
  {
    for(size_t i=0;i<size();i++)
      if(!(*this)[i].Nula(tol)) return false;
    return true;
  }

//! @brief Return el number of non-zero combinations in the container.
size_t cmb_acc::LoadCombinationVector::CuentaNulas(const double &tol) const
  {
    size_t retval=0;
    for(size_t i=0;i<size();i++)
      if((*this)[i].Nula(tol)) retval++;
    return retval;
  }

        
//! @brief Return the non-zero combinations.
const cmb_acc::LoadCombinationVector &cmb_acc::LoadCombinationVector::GetNoNulas(const double &tol) const
  {
    const size_t num_nulas= CuentaNulas(tol);
    if(num_nulas==0) return *this;
    const size_t sznn= size()-num_nulas;
    static LoadCombinationVector retval;
    retval.resize(sznn);
    size_t cont=0;
    for(size_t i=0;i<size();i++)
      if(!(*this)[i].Nula(tol))
        {
          retval[cont]= (*this)[i];
          cont++;
        }
    return retval;
  }

//! @brief Return true if the combination argument is found on this container.
bool cmb_acc::LoadCombinationVector::Existe(const Action &f) const
  {
    bool retval= false;
    const std::string nmb_f= f.getName();
    for(size_t i=0;i<size();i++)
      if((*this)[i].getName()==nmb_f)
        {
          retval= true;
          break;
        }
    return retval;
  }
        
//! @brief Return the combinations filtering the repeated ones.
const cmb_acc::LoadCombinationVector &cmb_acc::LoadCombinationVector::GetDistintas(void) const
  {
    static LoadCombinationVector retval;
    const size_t sz= size();
    if(sz<1)
      {
        retval.resize(0);
        return retval;
      }
    LoadCombinationVector tmp(sz);
    tmp[0]= (*this)[0]; // append the first-one.
    size_t cont=1;
    for(size_t i=1;i<sz;i++)
      if(!tmp.Existe((*this)[i])) // if different from the previous.
       {
         tmp[cont]= (*this)[i]; // append it.
         cont++;
       }
    retval.resize(cont);
    for(size_t i=0;i<cont;i++)
      retval[i]= tmp[i];
    return retval;
  }
        
//! @brief Counts the combinations from s2 that are not in this container.
size_t cmb_acc::LoadCombinationVector::CuentaDistintas(const LoadCombinationVector &s2) const
  {
    const size_t sz_s2= s2.size();
    size_t retval=0;
    for(size_t i=0;i<sz_s2;i++)
      if(!this->Existe(s2[i])) retval++;
    return retval;
  }
        
//! @brief Return the combinations from s2 that are not in this container.
const cmb_acc::LoadCombinationVector &cmb_acc::LoadCombinationVector::GetDistintas(const LoadCombinationVector &s2) const
  {
    const size_t num_distintas= CuentaDistintas(s2);
    const size_t sz_s2= s2.size();
    if(num_distintas==sz_s2) return s2; //Todas son distintas.
    const size_t sznn= num_distintas;
    static LoadCombinationVector retval;
    retval.resize(sznn);
    size_t cont=0;
    for(size_t i=0;i<sz_s2;i++)
      if(!this->Existe(s2[i]))
        {
          retval[cont]= s2[i];
          cont++;
        }
    return retval;
  }

        
//! \fn cmb_acc::LoadCombinationVector cmb_acc::LoadCombinationVector::ProdCartesiano(const LoadCombinationVector &f1,const LoadCombinationVector &f2,const double &tol)
//! @brief Builds the cartesian product of the combinations from both
//! vectors.
//!
//! For example, if:
//! LoadCombinations f1: f11,f12,f13
//! LoadCombinations f2: f21,f22
//! Return: f11+f21,f12+f21,f13+f21,f11+f22,f12+f22,f13+f22
cmb_acc::LoadCombinationVector cmb_acc::LoadCombinationVector::ProdCartesiano(const LoadCombinationVector &f1,const LoadCombinationVector &f2,const double &tol)
  //
  {
    const LoadCombinationVector &ann= f1;
    const LoadCombinationVector &bnn= f2;
    const size_t sz_ann= ann.size();
    if(sz_ann==0) return bnn;
    const size_t sz_bnn= bnn.size();
    if(sz_bnn==0) return ann;
    const size_t sz= sz_ann*sz_bnn;
    LoadCombinationVector retval(sz);
    for(size_t i=0;i<sz_ann;i++)
      for(size_t j=0;j<sz_bnn;j++)
        {
          size_t ind_retval= i*sz_bnn+j;
          retval[ind_retval]= ann[i]+bnn[j];
        }
    return retval;
  }

//! @brief Concatenate the combination of the vector arguments.
cmb_acc::LoadCombinationVector cmb_acc::LoadCombinationVector::Concat(const LoadCombinationVector &f1,const LoadCombinationVector &f2,const double &tol)
  {
    const LoadCombinationVector &ann= f1;
    const LoadCombinationVector bnn= f1.GetDistintas(f2); //Las de f2 que no esten en f1.
    const size_t sz_ann= ann.size();
    if(sz_ann==0) return bnn;
    const size_t sz_bnn= bnn.size();
    if(sz_bnn==0) return ann;
    const size_t sz= sz_ann+sz_bnn;
    LoadCombinationVector retval(sz);
    for(size_t i=0;i<sz_ann;i++)
      retval[i]= ann[i];
    for(size_t i=0;i<sz_bnn;i++)
      retval[i+sz_ann]= bnn[i];
    return retval;
  }
        
//! \fn void cmb_acc::LoadCombinationVector::Numera(const std::string &prefix)
//! @brief Appends the prefix text 'HNNN=' to the name of each of the vector
//! components, where NNN is the hypothesis number.
void cmb_acc::LoadCombinationVector::Numera(const std::string &prefix)
  {
    size_t cont= 1;
    for(iterator i= begin(); i!=end();i++,cont++)
      (*i).setName(prefix + num2str(cont) + "= " + (*i).getName());
  }

//! @brief Return the factors that correspond to the actions in the argument
//! in each of the combinations of this vector.
m_double cmb_acc::LoadCombinationVector::getCoeficientes(const std::vector<std::string> &base) const
  {
    const size_t n_rows= size();
    const size_t n_cols= base.size();
    m_double retval(n_rows,n_cols);
    for(size_t i= 0;i<n_rows;i++)
      {
        const std::vector<double> coefs_i= (*this)[i].getCoeficientes(base);
        for(size_t j= 0;j<n_cols;j++)
          retval(i+1,j+1)= coefs_i[j];
      }
    return retval;
  }

//! @brief Print stuff.
void cmb_acc::LoadCombinationVector::Print(std::ostream &os) const
  {
    for(const_iterator i= begin(); i!=end();i++)
      os << (*i) << std::endl;
  }

std::ostream &cmb_acc::operator<<(std::ostream &os,const LoadCombinationVector &vc)
  {
    vc.Print(os);
    return os;
  }
