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
//PolyPos.h

#ifndef POLYPOS_H
#define POLYPOS_H

#include <deque>
#include "../cgal_types.h"
#include "utility/utils/misc_utils/matem.h"
#include "utility/geom/proximity.h"

//! @ingroup GEOM
//
//! @brief Base class for position lists.
template<class pos>
class PolyPos : public std::deque<pos>
  {
  protected:
    typedef std::deque<pos> deque_pos;
  protected:
    //Auxiliary function.
    static GEOM_FT g(GEOM_FT v1, GEOM_FT v2)
      { return (sqr(v1) + v1 * v2 + sqr(v2)); }
  public:
    typedef typename deque_pos::iterator iterator;
    typedef typename deque_pos::const_iterator const_iterator;
    typedef typename pos::vector vector;

    PolyPos(void): deque_pos() {}
    
    pos getFromPoint(void) const
      { return this->front(); }
    
    pos getToPoint(void) const
      { return this->back(); }
    
    bool operator==(const PolyPos<pos> &other) const
      {
        return ( (const deque_pos &) (*this) == (const deque_pos &) other );
      }
    inline pos *Agrega(const pos &p)
      {
        deque_pos::push_back(p);
        return &(this->back());
      }
    inline void Agrega(const PolyPos<pos> &p)
      { Cat(p); }
    //! @brief Append the vertices between [first,last) to thebefore
    //! end of the list.
    template <class InputIterator>
    void extend(InputIterator first, InputIterator last)
      {
	for(InputIterator i= first;i!=last; i++)
	  Agrega(*i);
      }
    void AgregaSiNuevo(const pos &);
    const_iterator find(const pos &p) const
      { return std::find(this->begin(),this->end(),p); }
    iterator find(const pos &p)
      { return std::find(this->begin(),this->end(),p); }
    bool In(const pos &p) const;
    bool isClosed(const GEOM_FT &tol= 1e-6) const;
    GEOM_FT getLength(void) const;
    GEOM_FT GetMax(unsigned short i) const;
    GEOM_FT GetMin(unsigned short i) const;
    PolyPos GetMayores(unsigned short int i,const GEOM_FT &d) const;
    PolyPos GetMenores(unsigned short int i,const GEOM_FT &d) const;
    pos getCenterOfMass(void) const;
    pos getWeightedCenterOfMass(const std::deque<GEOM_FT> &) const;
    PolyPos<pos> getSwap(void) const;
    void swap(void)
      { *this= this->getSwap(); }
    std::deque<GEOM_FT> &GetSeparaciones(void) const;
    GEOM_FT GetSeparacionMedia(void) const;

    iterator getNearestPoint(const pos &);
    iterator getFarthestPoint(const pos &);
    virtual iterator getFarthestPointFromSegment(iterator it1, iterator it2, GEOM_FT &pMaxDist);
    void simplify(GEOM_FT epsilon, iterator it1, iterator it2);
    void simplify(GEOM_FT epsilon);

    void Cat(const PolyPos<pos> &l);
    template <class inputIterator>
    void Cat(inputIterator begin, inputIterator end);
  };


//! @brief Return true if the points is on en la lista.
template <class pos>
bool PolyPos<pos>::In(const pos &p) const
  { return (find(p)!=this->end()); }

//! @brief Appends the point to the list (if not already there).
template <class pos>
void PolyPos<pos>::AgregaSiNuevo(const pos &p)
  {
    if(!In(p))
      this->push_back(p);
  }

//! @brief True if dist(lastPoint,firstPoint)< tol*length
template <class pos>
bool PolyPos<pos>::isClosed(const GEOM_FT &tol) const
  {
    bool retval= false;
    const GEOM_FT treshold= tol*getLength();
    const pos &first= this->front();
    const pos &last= this->back();
    if(dist(first,last)<treshold)
      retval= true;
    return retval;
  }

//! @brief Return the length of the PolyPos.
template <class pos>
GEOM_FT PolyPos<pos>::getLength(void) const
  {
    if(this->size()<2) return 0.0;
    GEOM_FT temp = 0;
    const_iterator j;
    for(const_iterator i=this->begin(); i != this->end(); i++)
      {
        j= i;j++;
        if (j!=this->end()) temp += dist(*i,*j);
      }
    return temp;
  }

//! @brief Return the maximum value of j-th coordinate.
template <class pos>
GEOM_FT PolyPos<pos>::GetMax(unsigned short j) const
  {
    GEOM_FT retval(0.0);
    if(!this->empty())
      {
        const_iterator i=this->begin();
        retval= (*i)(j);
        for(; i != this->end(); i++)
          retval= std::max(retval,(*i)(j));
      }
    return retval;
  }

//! @brief Return the minimum value of j-th coordinate.
template <class pos>
GEOM_FT PolyPos<pos>::GetMin(unsigned short j) const
  {
    GEOM_FT retval(0.0);
    if(!this->empty())
      {
        const_iterator i=this->begin();
        retval= (*i)(j);
        for(; i != this->end(); i++)
          retval= std::min(retval,(*i)(j));
      }
    return retval;
  }

template <class pos>
//! @brief Return the center of mass del polígono.
pos PolyPos<pos>::getCenterOfMass(void) const
  {
    const size_t sz= this->size();
    const pos org= pos();
    if(sz<1) return org;
    if(sz<2) return *(this->begin());
    const_iterator i= this->begin();
    vector vpos_center_of_mass((*i).VectorPos());
    i++;
    for(; i != this->end(); i++) 
      vpos_center_of_mass= vpos_center_of_mass + (*i).VectorPos();
    vpos_center_of_mass= vpos_center_of_mass * (1.0/sz);
    return org+ vpos_center_of_mass;
  }

template <class pos>
//! @brief Return the center of mass del polígono.
pos PolyPos<pos>::getWeightedCenterOfMass(const std::deque<GEOM_FT> &areas) const
  {
    pos retval;
    const size_t sz= this->size();
    if(sz>0)
      {
        assert(sz==areas.size());
        if(sz<2)
          retval= *(this->begin());
        else
          {
            std::deque<GEOM_FT>::const_iterator iArea= areas.begin();
            GEOM_FT areaTot(0.0);
            const_iterator i= this->begin();
            vector vpos_center_of_mass((*i).VectorPos()*(*iArea));
            areaTot+= *iArea;
            i++; iArea++;
            for(; i != this->end(); i++)
              {
                vpos_center_of_mass= vpos_center_of_mass + (*i).VectorPos()*(*iArea);
                areaTot+= *iArea;
              }
            if(areaTot!=0.0)
              {
                vpos_center_of_mass= vpos_center_of_mass * (1.0/areaTot);
                retval= pos()+vpos_center_of_mass;
              }
          }
      }
    return retval;
  }

//! @brief Return a list with the elements in reverse order.
template <class pos>
PolyPos<pos> PolyPos<pos>::getSwap(void) const
  {
    PolyPos<pos> retval(*this);
    const size_t sz= this->size();
    if (sz<2) return retval;
    for(size_t i=0; i<sz; i++) 
      retval[sz-i-1]= (*this)[i];
    return retval;
  }

//! @brief Compute the distance from each point to the nearest one.
template <class pos>
std::deque<GEOM_FT> &PolyPos<pos>::GetSeparaciones(void) const
  {
    const size_t sz= this->size();
    static std::deque<GEOM_FT> retval;
    retval.resize(sz);
    const double grande= 10.0*getBnd((*this)).Diagonal().GetModulus();
    for(size_t i= 0;i<sz;i++)
      retval[i]= grande;
    if(sz>1)
      {
        double d;
        for(size_t i= 0;i<sz;i++)
          {
            const pos &pi= (*this)[i];
            for(size_t j= 0;j<sz;j++)
              if(i!=j)
                {
                  const pos &pj= (*this)[j];
                  d= dist(pi,pj);
                  if(d<retval[i] && d>0.0)
                    retval[i]= d;
                }
          }
      }
    return retval;
  }
//! @brief Return the average distance between points.
template <class pos>
GEOM_FT PolyPos<pos>::GetSeparacionMedia(void) const
  {
    GEOM_FT retval(0);
    const size_t sz= this->size();
    if(sz>0)
      {
        const std::deque<GEOM_FT> &seps= this->GetSeparaciones();
        for(size_t i= 0;i<sz;i++)
          retval+= seps[i];
        retval/=sz;
      }
    return retval;
  }

template <class pos>
PolyPos<pos> PolyPos<pos>::GetMayores(unsigned short int i,const GEOM_FT &d) const
  {
    PolyPos<pos> retval;
    for(const_iterator j=this->begin();j != this->end();j++)
      if ((*j)(i) > d) retval.push_back(*j);
    return retval;
  }

template <class pos>
PolyPos<pos> PolyPos<pos>::GetMenores(unsigned short int i,const GEOM_FT &d) const
  {
    PolyPos<pos> retval;
    for(const_iterator j=this->begin();j != this->end();j++)
      if ((*j)(i) < d) retval.push_back(*j);
    return retval;
  }

//! @brief Returns the farthest point from those of the list.
template <class pos>
typename PolyPos<pos>::iterator PolyPos<pos>::getNearestPoint(const pos &p)
  { return nearest(this->begin(), this->end(),p); }

//! @brief Returns the farthest point from those of the list.
template <class pos>
typename PolyPos<pos>::iterator PolyPos<pos>::getFarthestPoint(const pos &p)
  {
    iterator i= this->begin();
    iterator retval= i; i++;
    GEOM_FT maxDist= dist(p,*retval);
    GEOM_FT d= 0.0;
    for(;i!= this->end();i++)
      {
    	d= dist(p,*i);
    	if(d>maxDist)
    	  {
    	    maxDist= d;
    	    retval= i;
    	  }
      }
    return retval;
  }

/**
   * @param i1 iterator to the first point.
   * @param i2 iterator to the second point.
   * @param pMaxDist pointer to the maximum distance of _line[return index].
   * @return the index of the point farthest fromthe segment (t1,t2).
   */
template <class pos>
typename PolyPos<pos>::iterator PolyPos<pos>::getFarthestPointFromSegment(iterator , iterator , GEOM_FT &)
  {
    std::cerr << "PolyPos<>::" << __FUNCTION__
	      << ";must be redefined in derived classes." << std::endl;
    return this->begin();
  }

/**
   * Douglas Peucker algorithm implementation. 
   * Recursively delete points that are within epsilon.
   * @param epsilon the higher the more aggressive.
   * @param iterator of the first point in a segment.
   * @param iterator of the last point in a segment.
   */
template <class pos>
void PolyPos<pos>::simplify(GEOM_FT epsilon, iterator it1, iterator it2)
  {
    if (distance(it1, it2) <= 1) return;

    // Acquire the farthest point from the segment it1, it2
    GEOM_FT dist= -1.0;
    iterator index= getFarthestPointFromSegment(it1, it2, dist);

    // If the farthest point exceeds epsilon, recurse, with index as pivot.
    if (dist > epsilon)
      {
        simplify(epsilon, it1, index);
        simplify(epsilon, index, it2);
      }
    else
      {
        // Delete everything except it1 and it2.
        auto it = it1;
        it++;
        for (; it != it2; )
	  { this->erase(it++); }
      }
  }

/**
 * @param epsilon The higher, the more points gotten rid of.
 */
template <class pos>
void PolyPos<pos>::simplify(GEOM_FT epsilon)
  {
    const bool closed= this->isClosed();
    if(closed)
      {
        iterator i= this->begin();
	iterator j= this->getFarthestPoint(*i);
	simplify(epsilon,i,j);
	i= j;
        j= this->end(); --j; //Last point.
        simplify(epsilon,i,j);
      }
    else
      {
        iterator i= this->begin();
        iterator j= this->end(); --j; //Last point.
        simplify(epsilon,i,j);
      }
  }

template <class pos>
void PolyPos<pos>::Cat(const PolyPos<pos> &l)
  {
    for(typename PolyPos<pos>::const_iterator k= l.begin();k!=l.end();k++)
      this->push_back(*k);
  }

//!@brief Insert the objects between [first,last).
template <class pos> template<class inputIterator>
void PolyPos<pos>::Cat(inputIterator begin, inputIterator end)
  {
    for(inputIterator i= begin;i!=end;i++)
      this->push_back(*i);
  }

template <class pos>
std::ostream &operator<<(std::ostream &os,const PolyPos<pos> &l)
  {
    if(l.size()<1) return os;
    typedef typename PolyPos<pos>::const_iterator c_iterator;
    c_iterator i= l.begin();
    os << *i; i++;
    for(;i!= l.end();i++)
      os << " , " << *i;
    return os;
  }


#endif






