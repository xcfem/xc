// -*-c++-*-
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
//SlidingVectorsSystem2d.h
//Sliding vectors system.

#ifndef SLIDINGVECTORSSYSTEM2D_H
#define SLIDINGVECTORSSYSTEM2D_H

#include "../ProtoGeom.h"
#include "SlidingVector2d.h"
#include "Pos2d.h"

class SlidingVector2d;
class Line2d;
class Ref2d2d;

//! @ingroup GEOM
//
//! @brief Sliding vectors system.
class SlidingVectorsSystem2d: public SlidingVector2d
  {
    GEOM_FT mom; //!< Moment with respect to org;
  public:
    SlidingVectorsSystem2d(const Pos2d &O= Pos2d(),const Vector2d &R= Vector2d(),const GEOM_FT &Mo= 0.0);
    explicit SlidingVectorsSystem2d(const SlidingVector2d &v);
    //Resultant and moment.
    Vector2d getResultant(void) const
      { return getVector(); }
    Vector2d getResultant(const Ref2d2d &ref) const;
    GEOM_FT getMoment(void) const
      { return mom; }
    GEOM_FT getMoment(const Pos2d &P) const;
    bool Nulo(void) const;
    void Neg(void);
    SlidingVectorsSystem2d reduceTo(const Pos2d &Q) const;
    Line2d getZeroMomentLine(void) const;
    std::vector<SlidingVector2d> distribute(const std::vector<Pos2d> &) const;
    boost::python::list distributePyNoWeights(const boost::python::list &) const;
    std::vector<SlidingVector2d> distribute(const std::vector<Pos2d> &, const std::vector<double> &) const;
    boost::python::list distributePy(const boost::python::list &, const boost::python::list &) const;

    SlidingVectorsSystem2d &operator+=(const SlidingVector2d &v);
    SlidingVectorsSystem2d &operator-=(const SlidingVector2d &v);
    SlidingVectorsSystem2d &operator+=(const SlidingVectorsSystem2d &s);
    SlidingVectorsSystem2d &operator-=(const SlidingVectorsSystem2d &s);
    SlidingVectorsSystem2d &operator*=(const GEOM_FT &d);
    friend SlidingVectorsSystem2d operator+(const SlidingVectorsSystem2d &s1,const SlidingVectorsSystem2d &s2);
    friend SlidingVectorsSystem2d operator-(const SlidingVectorsSystem2d &s1,const SlidingVectorsSystem2d &s2);
    friend SlidingVectorsSystem2d operator*(const GEOM_FT &d, const SlidingVectorsSystem2d &s);
    friend SlidingVectorsSystem2d operator*(const SlidingVectorsSystem2d &s,const GEOM_FT &d);
    void Print(std::ostream &os) const;
    void PrintLtx(std::ostream &,const std::string &ud_long= " m",const GEOM_FT &f_long=1.0, const std::string &ud_f= " N",const GEOM_FT &f_f=1.0) const;
    friend std::ostream &operator<<(std::ostream &os, const SlidingVectorsSystem2d &svd2d);
  };

SlidingVectorsSystem2d operator+(const SlidingVectorsSystem2d &s1,const SlidingVectorsSystem2d &s2);
SlidingVectorsSystem2d operator-(const SlidingVectorsSystem2d &s1,const SlidingVectorsSystem2d &s2);
SlidingVectorsSystem2d operator*(const GEOM_FT &d, const SlidingVectorsSystem2d &s);
SlidingVectorsSystem2d operator*(const SlidingVectorsSystem2d &s,const GEOM_FT &d);
SlidingVectorsSystem2d operator+(const SlidingVector2d &v1,const SlidingVector2d &v2);
SlidingVectorsSystem2d operator+(const SlidingVectorsSystem2d &s,const SlidingVector2d &v);
SlidingVectorsSystem2d operator+(const SlidingVector2d &v,const SlidingVectorsSystem2d &s);
SlidingVectorsSystem2d operator-(const SlidingVectorsSystem2d &svd2d);
std::ostream &operator<<(std::ostream &os, const SlidingVectorsSystem2d &svd2d);


#endif
