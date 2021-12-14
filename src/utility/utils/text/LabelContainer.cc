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
//LabelContainer.cc

#include "LabelContainer.h"
#include <iostream>

//! @brief Devuelve el valor de la etiqueta de índice i.
const std::string &LabelDictionary::get_etiqueta_indice(const int &i) const
  {
    static std::string retval;
    retval= "";
    bm_type::const_iterator it=boost::multi_index::get<from>(bm).find(i);
    if(it!=bm.end())
      retval= it->second;
    else
      std::cerr << "LabelContainer::get_etiqueta_indice; indice "
                << i << " fuera de rango." << std::endl;
    return retval;
  }

//! @brief Devuelve el valor del indice de la etiqueta que se pasa como
//! parámetro.
int LabelDictionary::get_indice_etiqueta(const std::string &e) const
  {
    int retval= -1;
    bm_type::index_iterator<to>::type it=boost::multi_index::get<to>(bm).find(e);
    if(it!= bm.get<to>().end())
      retval= it->first;
    return retval;
  }

const std::string &LabelDictionary::operator()(const int &i) const
  { return get_etiqueta_indice(i); }

int LabelDictionary::operator()(const std::string &str) const
  { return get_indice_etiqueta(str); }

int LabelDictionary::insert(const std::string &e)
  {
    int id= get_indice_etiqueta(e);
    if(id<0) //la etiqueta no existe.
      {
        id= bm.size();
        bm.insert(bm_type::value_type(id,e));
      }
    assert(id>=0);
    return id;
  }

size_t LabelDictionary::getNumLabels(void) const
  { return bm.size(); }

void LabelDictionary::Print(std::ostream &os) const
  {
    bm_type::index_iterator<to>::type i=boost::multi_index::get<to>(bm).begin();
    for(;i!=boost::multi_index::get<to>(bm).end();i++)
      os << i->first << " " << i->second << std::endl;
  }

std::ostream &operator<<(std::ostream &os,const LabelDictionary &d)
  {
    d.Print(os);
    return os;
  }

LabelDictionary LabelContainer::dic;

//! @brief Constructor.
LabelContainer::LabelContainer(void)
   {}

bool LabelContainer::operator==(const LabelContainer &other) const
  {
    bool retval= false;
    if(this == &other)
      retval= true;
    else if(labels==other.labels)
      retval= true;
    return retval;
  }

//! @brief += operator.
LabelContainer &LabelContainer::operator+=(const LabelContainer &otro)
  {
    extend(otro);
    return *this;
  }

//! @brief -= operator.
LabelContainer &LabelContainer::operator-=(const LabelContainer &otro)
  {
    std::set<int> tmp;
    std::set_difference(labels.begin(),labels.end(),otro.labels.begin(),otro.labels.end(), std::inserter(tmp,tmp.begin()));
    labels= tmp;
    return *this;
  }

//! @brief *= operator (intersection).
LabelContainer &LabelContainer::operator*=(const LabelContainer &otro)
  {
    std::set<int> tmp;
    std::set_intersection(labels.begin(),labels.end(),otro.labels.begin(),otro.labels.end(), std::inserter(tmp,tmp.begin()));
    labels= tmp;
    return *this;
  }

//! @brief Return a reference to the label dictionary.
const LabelDictionary &LabelContainer::getDictionary(void)
  { return dic; }

//! @brief Return the number of labels in this object.
size_t LabelContainer::getNumLabels(void) const
  { return labels.size(); }

//! @brief Return the label identifiers.
const std::set<int> &LabelContainer::getIdsLabels(void) const
  { return labels; }

void LabelContainer::extend(const LabelContainer &otro)
  {
    for(std::set<int>::const_iterator i= otro.labels.begin();i!=otro.labels.end();i++)
      labels.insert(*i);
  }

int LabelContainer::addLabel(const std::string &e)
  {
    int retval= dic.insert(e);
    labels.insert(retval);
    return retval;
  }

int LabelContainer::removeLabel(const std::string &e)
  {
    const int retval= dic.get_indice_etiqueta(e);
    if(retval>=0)
      if(hasLabel(retval))
        { labels.erase(retval); }
    return retval;
  }

bool LabelContainer::hasLabel(const int &id) const
  {
    bool retval= false;
    std::set<int>::iterator it= labels.find(id);
    retval= (it!=labels.end());
    return retval;
  }

bool LabelContainer::hasLabel(const std::string &e) const
  {
    bool retval= false;
    const int id= dic.get_indice_etiqueta(e);
    if(id>=0)
      retval= hasLabel(id);
    return retval;
  }

void LabelContainer::clear(void)
  { labels.clear(); }

void LabelContainer::Print(std::ostream &os) const
  {
    for(std::set<int>::const_iterator i= labels.begin();i!=labels.end();i++)
      os << dic.get_etiqueta_indice(*i) << " ";
  }



std::ostream &operator<<(std::ostream &os,const LabelContainer &lc)
  {
    lc.Print(os);
    return os;
  }

//! @brief Return the union of both containers.
LabelContainer operator+(const LabelContainer &a,const LabelContainer &b)
  {
    LabelContainer retval(a);
    retval+=b;
    return retval;
  }

//! @brief Return the labels in a that are not in b.
LabelContainer operator-(const LabelContainer &s1,const LabelContainer &s2)
  {
    LabelContainer retval(s1);
    retval-=s2;
    return retval;
  }

//! @brief Return the labels in a that are also in b.
LabelContainer operator*(const LabelContainer &s1,const LabelContainer &s2)
  {
    LabelContainer retval(s1);
    retval*=s2;
    return retval;    
  }
