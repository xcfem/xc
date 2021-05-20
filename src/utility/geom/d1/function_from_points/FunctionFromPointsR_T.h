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
//FunctionFromPointsR_T.h

#ifndef FUNCTIONFROMPOINTSR_T_H
#define FUNCTIONFROMPOINTSR_T_H

#include <map>
#include <iostream>
#include <vector>
#include <boost/python/list.hpp>

//! @brief function from R to T defined on a set of points.
//!
//! Function that for each point x de la recta real le asigna un
//! valor de clase T. The correspondence is established by points,
//! that is, by pairs (x1,T1), (x2,T2), ...
template <class T>
class FunctionFromPointsR_T
  {
  public:
    typedef std::map<double,T> value_map;
    typedef typename value_map::iterator iterator;
    typedef typename value_map::const_iterator const_iterator;
    typedef typename value_map::const_reverse_iterator const_reverse_iterator;
  private:
    value_map points;
  public:
    T Valor(const double &) const;
    std::vector<T> Valores(const std::vector<double> &) const;
    T operator()(const double &z) const
      { return Valor(z); }
    std::vector<T> operator()(const std::vector<double> &zs) const
      { return Valores(zs); }
    //! @brief Return an iterator to the beginning of the point container.
    const_iterator begin(void) const
      { return points.begin(); }
    //! @brief Return an reverse iterator to the end of the point container.
    const_reverse_iterator rbegin(void) const
      { return points.rbegin(); }
    //! @brief Return an iterator to the end of the point container.
    const_iterator end(void) const
      { return points.end(); }
    //! @brief Return an iterator to the beginning of the point container.
    const_reverse_iterator rend(void) const
      { return points.rend(); }
    //! @brief Return the number of points that define the function.
    size_t size(void) const
      { return points.size(); }
    //! @brief Return true if there are no points.
    size_t empty(void) const
      { return points.empty(); }
    void insert(const double &x,const T &y);
    void clear(void)
      { points.clear(); }
  };

//! @brief Return el valor que corresponde a la abcisa z.
template <class T>
T FunctionFromPointsR_T<T>::Valor(const double &z) const
  {
    T retval= T();
    if(points.empty()) return retval; //If empty return default value.
    const_iterator j= points.upper_bound(z);
    if(j==begin()) //Abscissae z is smaller that all the rest.
      j++;//retval= (*j).second/(*j).first*z;
    if(j==end()) //Abscissae z is greater that all the rest.
      j--;//retval= (*j).second/(*j).first*z;
    const_iterator i= j;
    i--;
    const double x0= (*i).first;
    const T y0= (*i).second;
    const double x1= (*j).first;
    const T y1= (*j).second;
    const T a= (y1-y0)/(x1-x0);
    const T b= y1-a*x1;
    retval= a*z+b;
    return retval;
  }

//! @brief Return los valores que corresponden a las abcisas z.
template <class T>
std::vector<T> FunctionFromPointsR_T<T>::Valores(const std::vector<double> &zs) const
  {
    const size_t sz= zs.size();
    std::vector<T> retval(sz);
    if(points.empty()) return retval; //If empty return default value.
    for(size_t i= 0;i<sz;i++)
      retval[i]= Valor(zs[i]);
    return retval;
  }

//! @brief Inserts the pair (x,y) as a point of the function.
template <class T>
void FunctionFromPointsR_T<T>::insert(const double &x,const T &y)
  {
    points[x]= y;
  }
template <class T>
std::ostream &operator<<(std::ostream &os, const FunctionFromPointsR_T<T> &lt)
  {
    if(!lt.empty())
      {
        typename FunctionFromPointsR_T<T>::const_iterator i= lt.begin();
        os << "x= " << (*i).first
           << " y= " << (*i).second;
        i++;
        for(;i!=lt.end();i++)
          os << std::endl << "x= " << (*i).first
             << " y= " << (*i).second;
      }
    return os;
  }


#endif
