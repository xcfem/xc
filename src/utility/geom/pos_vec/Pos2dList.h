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
//Pos2dList.

#ifndef LISTAPOS2D_H
#define LISTAPOS2D_H

#include "../d2/GeomObj2d.h"

class Polygon2d;

//! @ingroup GEOM
//
//! @brief List of 2D positions.
class Pos2dList: public GeomObj2d
  {
  public:
    typedef GeomObj::list_Pos2d list_Pos2d;
    typedef GeomObj::list_Pos2d::iterator point_iterator;
    typedef GeomObj::list_Pos2d::const_iterator point_const_iterator;
  protected:
    list_Pos2d lista_ptos;
    Pos2d &operator[](const size_t &i);
  public:
    Pos2dList(void);
    explicit Pos2dList(const GeomObj::list_Pos2d &l);
    explicit Pos2dList(const boost::python::list &);
    virtual GeomObj *clon(void) const
      { return new Pos2dList(*this); }
    inline size_t getNumberOfPoints(void) const
      { return lista_ptos.size(); }
    inline point_const_iterator points_begin(void) const
      { return lista_ptos.begin(); }
    const point_const_iterator points_end(void) const
      { return lista_ptos.end(); }
    inline short unsigned int Dimension(void) const
      { return 0; }

    const GeomObj::list_Pos2d &getPoints(void) const
      { return lista_ptos; }
    const Pos2d &operator[](const size_t &i) const;

    const Pos2d *appendPoint(const Pos2d &p);
    void appendPointPy(const Pos2d &p);
    template <class InputIterator>
    //! Insert the vertices between [first,last).
    inline void assign(InputIterator first, InputIterator last)
      { lista_ptos.assign(first,last); }
    virtual bool In(const Pos2d &p, const double &tol= 0.0) const;

    virtual GEOM_FT Ix(void) const;
    virtual GEOM_FT Iy(void) const;
    virtual GEOM_FT Iz(void) const;

    //! @brief Product of inertia.
    inline virtual GEOM_FT Pxy(void) const
      { return NAN; }
    //! @brief Return the length of the object.
    virtual GEOM_FT getLength(void) const
      { return 0.0; }
    //! @brief Return the area of the object.
    virtual GEOM_FT getArea(void) const
      { return 0.0; }
    virtual GEOM_FT GetMax(unsigned short int i) const;
    virtual GEOM_FT GetMin(unsigned short int i) const;
    Pos2dList GetMayores(unsigned short int i,const GEOM_FT &d) const;
    Pos2dList GetMenores(unsigned short int i,const GEOM_FT &d) const;
    const Pos2d &Point(const size_t &i) const;
    Pos2d getCenterOfMass(void) const
      { return lista_ptos.getCenterOfMass(); }

    void Transform(const Trf2d &trf2d);
    Pos2dList Offset(const GEOM_FT &d) const;

    std::deque<GEOM_FT> &GetSeparaciones(void) const;
    std::deque<GEOM_FT> &getCovers(const Polygon2d &) const;
    double GetSeparacionMedia(void) const;

    void Print(std::ostream &stream) const;
    void Plot(Plotter &) const;
  };

#endif






