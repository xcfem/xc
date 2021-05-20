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
//SlidingVectorsSystem3d.h
//Sliding vectors system.

#ifndef SLIDINGVECTORSSYSTEM3D_H
#define SLIDINGVECTORSSYSTEM3D_H

#include "../ProtoGeom.h"
#include "SlidingVector3d.h"
#include "Pos3d.h"

class Line3d;
class Plane;
class Ref3d3d;


//! @ingroup GEOM
//
//! @brief Sliding vectors system en un espacio tridimensional.
class SlidingVectorsSystem3d: public SlidingVector3d
  {
    Vector3d mom; //!< Moment with respect to org;
  public:
    SlidingVectorsSystem3d(const Pos3d &O= Pos3d(),const Vector3d &R= Vector3d(),const Vector3d &Mo= Vector3d());
    explicit SlidingVectorsSystem3d(const SlidingVector3d &v);
    Vector3d getResultant(void) const
      { return getVector(); }
    Vector3d getResultant(const Ref3d3d &ref) const;
    const Vector3d &getMoment(void) const
      { return mom; }
    SlidingVector3d getMoment(const Pos3d &P) const;
    GEOM_FT getMoment(const Line3d &e) const;
    Vector3d getMoment(const Ref3d3d &ref) const;
    bool Nulo(void) const;
    void Neg(void);
    Line3d centralAxis(void) const;
    bool existsZeroMomentLine(const double &tol= sqrt_mchne_eps_dbl) const;
    Line3d getZeroMomentLine(const double &tol= sqrt_mchne_eps_dbl) const;
    Pos3d PointOfApplication(const Plane &p) const;

    SlidingVectorsSystem3d reduceTo(const Pos3d &Q) const;
    std::vector<SlidingVector3d> distribute(const std::vector<Pos3d> &) const;
    boost::python::list distributePyNoWeights(const boost::python::list &) const;
    std::vector<SlidingVector3d> distribute(const std::vector<Pos3d> &, const std::vector<double> &) const;
    boost::python::list distributePy(const boost::python::list &, const boost::python::list &) const;
    

    SlidingVectorsSystem3d &operator+=(const SlidingVector3d &v);
    SlidingVectorsSystem3d &operator-=(const SlidingVector3d &v);
    SlidingVectorsSystem3d &operator+=(const SlidingVectorsSystem3d &s);
    SlidingVectorsSystem3d &operator-=(const SlidingVectorsSystem3d &s);
    SlidingVectorsSystem3d &operator*=(const GEOM_FT &d);
    friend SlidingVectorsSystem3d operator+(const SlidingVectorsSystem3d &s1,const SlidingVectorsSystem3d &s2);
    friend SlidingVectorsSystem3d operator-(const SlidingVectorsSystem3d &s1,const SlidingVectorsSystem3d &s2);
    friend SlidingVectorsSystem3d operator*(const GEOM_FT &d, const SlidingVectorsSystem3d &s);
    friend SlidingVectorsSystem3d operator*(const SlidingVectorsSystem3d &s,const GEOM_FT &d);
    void Print(std::ostream &os) const;
    void PrintLtx(std::ostream &os,const std::string &ud_long= " m",const GEOM_FT &f_long=1.0, const std::string &ud_f= " N",const GEOM_FT &f_f=1.0) const;
    friend std::ostream &operator<<(std::ostream &os, const SlidingVectorsSystem3d &svd3d);
  };

SlidingVectorsSystem3d operator+(const SlidingVectorsSystem3d &s1,const SlidingVectorsSystem3d &s2);
SlidingVectorsSystem3d operator-(const SlidingVectorsSystem3d &s1,const SlidingVectorsSystem3d &s2);
SlidingVectorsSystem3d operator*(const GEOM_FT &d, const SlidingVectorsSystem3d &s);
SlidingVectorsSystem3d operator*(const SlidingVectorsSystem3d &s,const GEOM_FT &d);
SlidingVectorsSystem3d operator+(const SlidingVector3d &v1,const SlidingVector3d &v2);
SlidingVectorsSystem3d operator+(const SlidingVectorsSystem3d &s,const SlidingVector3d &v);
SlidingVectorsSystem3d operator+(const SlidingVector3d &v,const SlidingVectorsSystem3d &s);
SlidingVectorsSystem3d operator-(const SlidingVectorsSystem3d &svd3d);
std::ostream &operator<<(std::ostream &os, const SlidingVectorsSystem3d &svd3d);

#endif
