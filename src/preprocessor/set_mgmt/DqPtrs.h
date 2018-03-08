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
//DqPtrs.h
//deque de pointers (se emplear en la clase Set).


#ifndef DQPTRS_H
#define DQPTRS_H

#include "xc_utils/src/nucleo/EntCmd.h"
#include "xc_utils/src/nucleo/EntPropSorter.h"
#include <deque>
#include <set>
#include "utility/actor/actor/MovableID.h"
#include <boost/iterator/indirect_iterator.hpp>



namespace XC {

//!  @ingroup Set
//! 
//!  @brief Pointer to (nodes, elements, points, lines,...) container.
//! 
//!  This obnect contains 0 o more pointers to objects of ONE of this types:
//!  - Nodde.
//!  - Finite elements.
//!  - Point.
//!  - Line.
//!  - Suprface.
//!  - Body.
template <class T>
class DqPtrs: public EntCmd, protected std::deque<T *>
  {
  public:
    typedef typename std::deque<T *> lst_ptr;
    typedef typename lst_ptr::const_iterator const_iterator;
    typedef typename lst_ptr::iterator iterator;
    typedef typename lst_ptr::reference reference;
    typedef typename lst_ptr::const_reference const_reference;
    typedef typename lst_ptr::size_type size_type;
    typedef boost::indirect_iterator<iterator> indIterator;
  public:
    DqPtrs(EntCmd *owr= nullptr);
    DqPtrs(const DqPtrs &otro);
    explicit DqPtrs(const std::deque<T *> &ts);
    explicit DqPtrs(const std::set<const T *> &ts);
    DqPtrs &operator=(const DqPtrs &);
    DqPtrs &operator+=(const DqPtrs &);
    void extend(const DqPtrs &);
    //void extend_cond(const DqPtrs &otro,const std::string &cond);
    bool push_back(T *);
    bool push_front(T *);
    inline bool empty(void) const
      { return lst_ptr::empty(); }
    inline iterator begin(void)
      { return lst_ptr::begin(); }
    const_iterator begin(void) const
      { return lst_ptr::begin(); }
    iterator end(void)
      { return lst_ptr::end(); }
    const_iterator end(void) const
      { return lst_ptr::end(); }
    inline indIterator indBegin(void)
      { return indIterator(lst_ptr::begin()); }
    inline indIterator indEnd(void)
      { return indIterator(lst_ptr::end()); }
    const T &get(const size_t &i) const;
    void clear(void);
    void clearAll(void);
    inline size_type size(void) const
      { return lst_ptr::size(); }
    bool in(const T *) const;
    //void sort_on_prop(const std::string &cod,const bool &ascending= true);

    const ID &getTags(void) const;
    template <class InputIterator>
    void insert(iterator pos, InputIterator f, InputIterator l)
      { lst_ptr::insert(pos,f,l); }

    
    int sendTags(int posSz,int posDbTag,DbTagData &dt,CommParameters &cp);
    const ID &receiveTags(int posSz,int pDbTg,DbTagData &dt,const CommParameters &cp);

  };

//! @brief Constructor.
template <class T>
DqPtrs<T>::DqPtrs(EntCmd *owr)
  : EntCmd(owr),lst_ptr() {}

//! @brief Copy constructor.
template <class T>
DqPtrs<T>::DqPtrs(const DqPtrs<T> &otro)
  : EntCmd(otro), lst_ptr(otro)
  {}

//! @brief Copy from deque container.
template <class T>
DqPtrs<T>::DqPtrs(const std::deque<T *> &ts)
  : EntCmd(), lst_ptr(ts)
  {}

//! @brief Copy from set container.
template <class T>
DqPtrs<T>::DqPtrs(const std::set<const T *> &st)
  : EntCmd(), lst_ptr()
  {
    typename std::set<const T *>::const_iterator k;
    k= st.begin();
    for(;k!=st.end();k++)
      lst_ptr::push_back(const_cast<T *>(*k));
  }

//! @brief Assignment operator.
template <class T>
DqPtrs<T> &DqPtrs<T>::operator=(const DqPtrs &otro)
  {
    EntCmd::operator=(otro);
    lst_ptr::operator=(otro);
    return *this;
  }

//! @brief += (union) operator.
template <class T>
DqPtrs<T> &DqPtrs<T>::operator+=(const DqPtrs &otro)
  {
    extend(otro);
    return *this;
  }


//! @brief Extend this container with the pointers from the container
//! being passed as parameter.
template <class T>
void DqPtrs<T>::extend(const DqPtrs &otro)
  {
    for(register const_iterator i= otro.begin();i!=otro.end();i++)
      push_back(*i);
  }

//! @brief Clears out the list of pointers.
template<class T>
void DqPtrs<T>::clear(void)
  { lst_ptr::clear(); }

//! @brief Clears out the list of pointers and erases the properties of the object (if any).
template<class T>
void DqPtrs<T>::clearAll(void)
  {
    clear();
    EntCmd::clearPyProps();
  }

//! @brief Access specified node with bounds checking.
template<class T>
const T &DqPtrs<T>::get(const size_t &i) const
  { 
    const T *ptr= lst_ptr::at(i);
    return *ptr; 
  }

//! @brief Returns true if the pointer is in the container.
template<class T>
bool DqPtrs<T>::in(const T *ptr) const
  {
    bool retval= false;
    for(const_iterator i= begin();i!= end();i++)
      if(*i==ptr)
        {
          retval= true;
          break;
        }
    return retval;
  }


template <class T>
bool DqPtrs<T>::push_back(T *t)
  {
    bool retval= false;
    if(t)
      {
        if(find(begin(),end(),t) == end()) //It's a new element.
          {
            lst_ptr::push_back(t);
            retval= true;
          }
      }
    else
      std::cerr << getClassName() << "::" << __FUNCTION__
	        << "; attempt to insert a null pointer." << std::endl;
    return retval;
  }

template <class T>
bool DqPtrs<T>::push_front(T *t)
  {
    bool retval= false;
    if(t)
      {
        if(find(begin(),end(),t) == end()) //New element.
          {
            lst_ptr::push_front(t);
            retval= true;
          }
      }
    else
      std::cerr << getClassName() << "::" << __FUNCTION__
	        << "; attempt to insert a null pointer." << std::endl;
    return retval;
  }

//! @brief Returns the tags of the objects.
template <class T>
const ID &DqPtrs<T>::getTags(void) const
  {
    static ID retval;
    const int sz= size();
    if(sz>0)
      {
        retval.resize(sz);
        int loc =0;
        // loop over objs in deque adding their dbTag to the ID
        for(const_iterator i= begin();i!=end();i++)
          {
            retval(loc)= (*i)->getTag();
            loc++;
          }
      }
    return retval;
  }

//! @brief Sends the dbTags of the sets trough the channel being passed as parameter.
template <class T>
int DqPtrs<T>::sendTags(int posSz,int posDbTag,DbTagData &dt,CommParameters &cp)
  {
    const int sz= size();
    int res= cp.sendInt(sz,dt,CommMetaData(posSz));
    if(sz>0)
      {
        const ID &tags= getTags();
        res+= cp.sendID(tags,dt,CommMetaData(posDbTag));
      }
    if(res<0)
      std::cerr << "DqPtrs<T>::sendDbTags - failed to send the IDs.\n";
    return res;
  }

//! @brief Sends the dbTags of the sets through the channel being passed as parameter.
template <class T>
const ID &DqPtrs<T>::receiveTags(int posSz,int posDbTag,DbTagData &dt,const CommParameters &cp)
  {
    static ID retval;
    int sz= 0;
    int res= cp.receiveInt(sz,dt,CommMetaData(posSz));
    if(sz>0)
      {
        retval.resize(sz);
        res= cp.receiveID(retval,dt,CommMetaData(posDbTag));
      }
    if(res<0)
      std::cerr << "DqPtrs<T>::receiveTags - failed to receive the IDs.\n";
    return retval;
  }

} //end of XC namespace


#endif

