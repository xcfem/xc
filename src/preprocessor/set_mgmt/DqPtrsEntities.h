// -*-c++-*-
//----------------------------------------------------------------------------
//  XC program; finite element analysis code
//  for structural analysis and design.
//
//  Copyright (C)  Luis C. Pérez Tato
//
//  This program derives from OpenSees <http://opensees.berkeley.edu>
//  developed by the  «Pacific earthquake engineering research center».
//
//  Except for the restrictions that may arise from the copyright
//  of the original program (see copyright_opensees.txt)
//  XC is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or 
//  (at your option) any later version.
//
//  This software is distributed in the hope that it will be useful, but 
//  WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details. 
//
//
// You should have received a copy of the GNU General Public License 
// along with this program.
// If not, see <http://www.gnu.org/licenses/>.
//----------------------------------------------------------------------------
//DqPtrsEntities.h
//deque de pointers (se emplear en la clase Set).


#ifndef DQPTRSENTITIES_H
#define DQPTRSENTITIES_H

#include "DqPtrs.h"
#include "utility/geom/pos_vec/Pos3d.h"
#include "utility/geom/pos_vec/Vector3d.h"
#include "utility/geom/d3/BND3d.h"

class GeomObj3d;
class BND3d;

namespace XC {

//! @brief Container for preprocessor entities (points, lines, surfaces,...)
template <class T>
class DqPtrsEntities: public DqPtrs<T>
  {
  public:
    typedef DqPtrs<T> dq_ptr;
    typedef typename dq_ptr::const_iterator const_iterator;
    typedef typename dq_ptr::iterator iterator;

    DqPtrsEntities(CommandEntity *owr= nullptr)
      : DqPtrs<T>(owr) {}
    DqPtrsEntities(const DqPtrs<T> &other)
      : DqPtrs<T>(other) {}
    explicit DqPtrsEntities(const std::deque<T *> &ts)
      : DqPtrs<T>(ts) {}
    explicit DqPtrsEntities(const std::set<const T *> &ts)
      : DqPtrs<T>(ts) {}

    DqPtrsEntities &operator-=(const DqPtrsEntities &);
    DqPtrsEntities &operator*=(const DqPtrsEntities &);

    void remove(const DqPtrsEntities<T> &other);
    void intersect(const DqPtrsEntities<T> &other);

    T *searchName(const std::string &nmb);
    T *findTag(const size_t &);
    T *getNearest(const Pos3d &p);
    Pos3d getCentroid(void) const;
    const T *getNearest(const Pos3d &p) const;
    DqPtrsEntities<T> pickEntitiesInside(const GeomObj3d &, const double &tol= 0.0) const;
    BND3d Bnd(void) const;
  };

 
//! @brief Returns a pointer to the object identified by the name.
template <class T>
T *DqPtrsEntities<T>::searchName(const std::string &nmb)
  {
    for(const_iterator i= this->begin();i!=this->end();i++)
      if((*i)->getName()==nmb) return *i;
    return nullptr;
  }
  
//! @brief Returns a pointer to the object identified by the tag argument.
template <class T>
T *DqPtrsEntities<T>::findTag(const size_t &tag)
  {
    for(const_iterator i= this->begin();i!=this->end();i++)
      if((*i)->getTag()==tag) return *i;
    return nullptr;
  }
  
//! @brief Returns the centroid of the entities.
template <class T>
Pos3d DqPtrsEntities<T>::getCentroid(void) const
  {
    Pos3d retval;
    if(!this->empty())
      {
	const size_t sz= this->size();
	const_iterator bg= this->begin();
	if(sz<2)
	  {
	    const Pos3d pos= (*bg)->getCentroid();
	    retval= pos;
	  }
        else
	  {
	    const_iterator i= bg;
	    const Pos3d pos= (*i)->getCentroid();
            Vector3d vpos_center_of_mass(pos.VectorPos());
            i++;
            for(; i != this->end(); i++)
	      {
   	        const Pos3d pos= (*i)->getCentroid();
                vpos_center_of_mass= vpos_center_of_mass + pos.VectorPos();
	      }
            vpos_center_of_mass= vpos_center_of_mass * (1.0/sz);
            retval+= vpos_center_of_mass;
	  }
      }
    else
      std::cerr << this->getClassName() << "::" << __FUNCTION__
		<< "; point set is empty." << std::endl;
    return retval;
  }
  
//! @brief Returns the object closest to the position being passed as parameter.
template <class T>
T *DqPtrsEntities<T>::getNearest(const Pos3d &p)
  {
    T *retval= nullptr;
    if(!this->empty())
      {
        const_iterator i= this->begin();
        double d2= (*i)->getSquaredDistanceTo(p);
        retval= *i; i++;
        double tmp;
        for(;i!=this->end();i++)
          {
            tmp= (*i)->getSquaredDistanceTo(p);
            if(tmp<d2)
              {
                d2= tmp;
                retval= *i;
              }
          }
      }
    return retval;
  }

//! @brief Returns the object closest to the position being passed as parameter.
template <class T>
const T *DqPtrsEntities<T>::getNearest(const Pos3d &p) const
  {
    const T *retval= nullptr;
    if(!this->empty())
      {
        const_iterator i= this->begin();
        double d2= (*i)->getSquaredDistanceTo(p);
        retval= *i; i++;
        double tmp;
        for(;i!=this->end();i++)
          {
            tmp= (*i)->getSquaredDistanceTo(p);
            if(tmp<d2)
              {
                d2= tmp;
                retval= *i;
              }
          }
      }
    return retval;
  }

//! @brief Return a container with the entities that lie inside the
//! geometric object.
//!
//! @param geomObj: geometric object that must contain the nodes.
//! @param tol: tolerance for "In" function.
template <class T>
DqPtrsEntities<T> DqPtrsEntities<T>::pickEntitiesInside(const GeomObj3d &geomObj, const double &tol) const
  {
    DqPtrsEntities<T> retval;
    for(const_iterator i= this->begin();i!= this->end();i++)
      {
        T *t= (*i);
        assert(t);
	if(t->In(geomObj,tol))
	  retval.push_back(t);
      }
    return retval;
  }

//! @brief Return the entities boundary.
//!
template <class T>
BND3d DqPtrsEntities<T>::Bnd(void) const
  {
    BND3d retval;
    if(!this->empty())
      {
	const_iterator i= this->begin();
        const T *t= (*i);
        assert(t);
        retval= t->Bnd();
	i++;
	for(;i!= this->end();i++)
	  {
	    const T *t= (*i);
	    assert(t);
	    retval+= t->Bnd();
	  }
      }
    return retval;
  }

//! @brief Removes the objects that belongs also to the parameter.
template <class T>
void DqPtrsEntities<T>::remove(const DqPtrsEntities<T> &other)
  {
    for(const_iterator i= other.begin();i!= other.end();i++)
      {
        const T *t= (*i);
	iterator j= find(this->begin(),this->end(),t);
	if(j!=this->end()) //Found.
	  this->erase(j);
      }
  }

//! @brief Removes the objects that belongs also to the parameter.
template <class T>
void DqPtrsEntities<T>::intersect(const DqPtrsEntities<T> &other)
  {
    for(const_iterator i= other.begin();i!= other.end();i++)
      {
        const T *t= (*i);
	iterator j= find(this->begin(),this->end(),t);
	if(j==this->end()) //Not found
	  this->erase(j);
      }
  }

//! @brief -= (difference) operator.
template <class T>
DqPtrsEntities<T> &DqPtrsEntities<T>::operator-=(const DqPtrsEntities &other)
  {
    remove(other);
    return *this;
  }

//! @brief *= (intersection) operator.
template <class T>
DqPtrsEntities<T> &DqPtrsEntities<T>::operator*=(const DqPtrsEntities &other)
  {
    intersect(other);
    return *this;
  }

 
//! @brief Return the union of both containers.
template <class T>
DqPtrsEntities<T> operator+(const DqPtrsEntities<T> &a,const DqPtrsEntities<T> &b)
  {
    DqPtrsEntities<T> retval(a);
    retval+=b;
    return retval;
  }

//! @brief Return the entities in a that are not in b (set difference).
template <class T>
DqPtrsEntities<T> operator-(const DqPtrsEntities<T> &a,const DqPtrsEntities<T> &b)
  {
    DqPtrsEntities<T> retval;
    for(typename DqPtrsEntities<T>::const_iterator i= a.begin();i!= a.end();i++)
      {
        const T *t= (*i);
	const typename DqPtrsEntities<T>::const_iterator j= find(b.begin(),b.end(),t);
	if(j==b.end()) //Not found in b.
	  retval.push_back(t);
      }
    return retval;
  }

//! @brief Return the entities in a that are also in b (set intersection).
template <class T>
DqPtrsEntities<T> operator*(const DqPtrsEntities<T> &a,const DqPtrsEntities<T> &b)
  {
    DqPtrsEntities<T> retval;
    for(typename DqPtrsEntities<T>::const_iterator i= a.begin();i!= a.end();i++)
      {
        const T *t= (*i);
	const typename DqPtrsEntities<T>::const_iterator j= find(b.begin(),b.end(),t);
	if(j!=b.end()) //Found also in b.
	  retval.push_back(t);
      }
    return retval;
  }

} //end of XC namespace

#endif

