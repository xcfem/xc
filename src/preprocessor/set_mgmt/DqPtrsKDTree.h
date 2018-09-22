//----------------------------------------------------------------------------
//  XC program; finite element analysis code
//  for structural analysis and design.
//
//  Copyright (C)  Luis Claudio Pérez Tato
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
//DqPtrsKDTree.h


#ifndef DQPTRSKDTREE_H
#define DQPTRSKDTREE_H

#include "DqPtrs.h"
#include <set>

class Pos3d;
class Vector3d;

namespace XC {
class TrfGeom;

//!  @ingroup Set
//! 
//!  @brief Container with a KDTree.
//!
template <class T,class KDTree>
class DqPtrsKDTree: public DqPtrs<T>
  {
    KDTree kdtree; //!< space-partitioning data structure for organizing objects.
  protected:
    void create_tree(void);
  public:
    typedef typename DqPtrs<T>::const_iterator const_iterator;
    typedef typename DqPtrs<T>::iterator iterator;
    typedef typename DqPtrs<T>::reference reference;
    typedef typename DqPtrs<T>::const_reference const_reference;
    typedef typename DqPtrs<T>::size_type size_type;

    DqPtrsKDTree(CommandEntity *owr= nullptr);
    DqPtrsKDTree(const DqPtrsKDTree &);
    explicit DqPtrsKDTree(const std::deque<T *> &);
    explicit DqPtrsKDTree(const std::set<const T *> &);
    DqPtrsKDTree &operator=(const DqPtrsKDTree &);
    DqPtrsKDTree &operator+=(const DqPtrsKDTree &);
    void extend(const DqPtrsKDTree &);
    //void extend_cond(const DqPtrsKDTree &,const std::string &cond);
    bool push_back(T *);
    bool push_front(T *);
    void clearAll(void);

    T *getNearest(const Pos3d &p);
    const T *getNearest(const Pos3d &p) const;
  };

//! @brief Creates the KD tree.
template <class T,class KDTree>
void DqPtrsKDTree<T,KDTree>::create_tree(void)
  {
    kdtree.clear();
    for(iterator i= this->begin();i!=this->end();i++)
      {
        T *tPtr= *i;
        assert(tPtr);
        kdtree.insert(*tPtr);
      }
  }

//! @brief Constructor.
template <class T,class KDTree>
DqPtrsKDTree<T,KDTree>::DqPtrsKDTree(CommandEntity *owr)
  : DqPtrs<T>(owr) {}

//! @brief Copy constructor.
template <class T,class KDTree>
DqPtrsKDTree<T,KDTree>::DqPtrsKDTree(const DqPtrsKDTree &other)
  : DqPtrs<T>(other)
  { create_tree(); }

//! @brief Copy constructor.
template <class T,class KDTree>
DqPtrsKDTree<T,KDTree>::DqPtrsKDTree(const std::deque<T *> &ts)
  : DqPtrs<T>(ts)
  { create_tree(); }

//! @brief Copy constructor.
template <class T,class KDTree>
DqPtrsKDTree<T,KDTree>::DqPtrsKDTree(const std::set<const T *> &st)
  : DqPtrs<T>()
  {
    typename std::set<const T *>::const_iterator k;
    k= st.begin();
    for(;k!=st.end();k++)
      push_back(const_cast<T *>(*k));
  }

//! @brief Assignment operator.
template <class T,class KDTree>
DqPtrsKDTree<T,KDTree> &DqPtrsKDTree<T,KDTree>::operator=(const DqPtrsKDTree &other)
  {
    DqPtrs<T>::operator=(other);
    kdtree= other.kdtree;
    return *this;
  }

//! @brief Extend this container with the elements of
//! the container being passed as parameter.
template <class T,class KDTree>
void DqPtrsKDTree<T,KDTree>::extend(const DqPtrsKDTree &other)
  {
    for(register DqPtrsKDTree<T,KDTree>::const_iterator i= other.begin();i!=other.end();i++)
      push_back(*i);
  }
//! @brief += operator.
template <class T,class KDTree>
DqPtrsKDTree<T,KDTree> &DqPtrsKDTree<T,KDTree>::operator+=(const DqPtrsKDTree &other)
  {
    extend(other);
    return *this;
  }

//! @brief Inserts an object at the end of the container.
template <class T,class KDTree>
bool XC::DqPtrsKDTree<T,KDTree>::push_back(T *t)
  {
    bool retval= DqPtrs<T>::push_back(t);
    if(retval)
      kdtree.insert(*t);
    return retval;
  }

//! @brief Inserts an object at the begining of the container.
template <class T,class KDTree>
bool XC::DqPtrsKDTree<T,KDTree>::push_front(T *t)
  {
    bool retval= DqPtrs<T>::push_front(t);
    if(retval)
      kdtree.insert(*t);
    return retval;
}

//! @brief Clears out the list of pointers and erases the properties of the object (if any).
template <class T,class KDTree>
void DqPtrsKDTree<T,KDTree>::clearAll(void)
  {
    DqPtrs<T>::clear();
    kdtree.clear();
  }
//! @brief Returns the object closest to the point being passed as parameter.
template <class T,class KDTree>
T *DqPtrsKDTree<T,KDTree>::getNearest(const Pos3d &p)
  {
    T *retval= const_cast<T *>(kdtree.getNearest(p));
    return retval;
  }

//! @brief Returns the object closest to the point being passed as parameter.
template <class T,class KDTree>
const T *DqPtrsKDTree<T,KDTree>::getNearest(const Pos3d &p) const
  {
    DqPtrsKDTree<T,KDTree> *this_no_const= const_cast<DqPtrsKDTree *>(this);
    return this_no_const->getNearest(p);
  }

//! @brief Return the union of both containers.
template <class T,class KDTree>
DqPtrsKDTree<T,KDTree> operator+(const DqPtrsKDTree<T,KDTree> &a,const DqPtrsKDTree<T,KDTree> &b)
  {
    DqPtrsKDTree<T,KDTree> retval(a);
    retval+=b;
    return retval;
  }

//! @brief Return the objects in a that are not in b.
template <class T,class KDTree>
DqPtrsKDTree<T,KDTree> operator-(const DqPtrsKDTree<T,KDTree> &a,const DqPtrsKDTree<T,KDTree> &b)
  {
    DqPtrsKDTree<T,KDTree> retval;
    for(typename DqPtrsKDTree<T,KDTree>::const_iterator i= a.begin();i!=a.end();i++)
      {
        T *n= (*i);
        assert(n);
	if(!b.in(n)) //If not in b.
	  retval.push_back(n);
      }
    return retval;    
  }

//! @brief Return the object in a that are also in b.
template <class T,class KDTree>
DqPtrsKDTree<T,KDTree> operator*(const DqPtrsKDTree<T,KDTree> &a,const DqPtrsKDTree<T,KDTree> &b)
  {
    DqPtrsKDTree<T,KDTree> retval;
    for(typename DqPtrsKDTree<T,KDTree>::const_iterator i= a.begin();i!=a.end();i++)
      {
        T *n= (*i);
        assert(n);
	if(b.in(n)) //If also in b.
	  retval.push_back(n);
      }
    return retval;    
  }

} //end of XC namespace
#endif

