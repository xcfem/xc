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
//Segment3d.h

#ifndef SEGMENT3D_H
#define SEGMENT3D_H

#include "Linear3d.h"
#include "Line3d.h"
#include "Ray3d.h"
#include "../pos_vec/Pos3d.h"

class Dir3d;
class VectorPos3d;

typedef std::pair<int,int> int_pair;
typedef std::deque<int_pair> int_pair_deque;

//! @ingroup GEOM
//
//! @brief Segment en tres dimensiones.
class Segment3d : public Linear3d
  {
    CGSegment_3 cgseg;
  public:
    Segment3d(void);
    Segment3d(const CGSegment_3 &r);
    Segment3d(const Pos3d &p1,const Pos3d &p2);
    virtual bool operator==(const Segment3d &) const;

    const CGSegment_3 &ToCGAL(void) const
      { return cgseg; }
    virtual GeomObj *clon(void) const;
    void swap(void);
    void TwoPoints(const Pos3d &p1,const Pos3d &p2);
    virtual GEOM_FT GetMax(unsigned short int i) const;
    virtual GEOM_FT GetMin(unsigned short int i) const;
    Line3d getSupportLine(void) const;
    Pos3d getFromPoint(void) const;
    Pos3d getToPoint(void) const;
    GEOM_FT getSlope(void) const;
    const Pos3d Point(const int &i) const;
    Pos3d PtoParametricas(const GEOM_FT &lambda) const;
    double getParamNaturalCoord(const GEOM_FT &) const;
    Pos3d getPointNaturalCoord(const GEOM_FT &chi) const;
    GEOM_FT getLambda(const Pos3d &p) const;
    bool isDegenerated(void) const;
    virtual bool In(const Pos3d &p, const double &tol= 0.0) const;
    virtual GEOM_FT dist2(const Pos3d &p) const;
    GEOM_FT dist(const Pos3d &p) const;
    void Put(const Pos3d &p1,const Pos3d &p2);

    bool isParallel(const Line3d &r) const;
    bool isParallel(const Ray3d &sr) const;
    bool isParallel(const Segment3d &r) const;

    GeomObj3d::list_Pos3d getIntersection(unsigned short int, const double &) const;
    GeomObj3d::list_Pos3d getIntersection(const Line3d &r) const;
    GeomObj3d::list_Pos3d getIntersection(const Ray3d &sr) const;
    GeomObj3d::list_Pos3d getIntersection(const Segment3d &sg) const;
    bool connected(const Pos3d &, const GEOM_FT &tol= 0.0) const;

    Pos3d Projection(const Pos3d &) const;
    Vector3d Projection(const Vector3d &) const;
    
    virtual GEOM_FT getLength(void) const;
    virtual Pos3d getCenterOfMass(void) const;
    Dir3d GetDir(void) const;
    Vector3d VDir(void) const;
    virtual Vector3d getIVector(void) const;
    virtual Vector3d getJVector(void) const;
    virtual Vector3d getKVector(void) const;
    GEOM_FT getAngle(const Vector3d &v) const;
    GEOM_FT getAngle(const Segment3d &v) const;
    friend GEOM_FT angle(const Segment3d &r,const Vector3d &v);
    virtual GEOM_FT Ix(void) const;
    virtual GEOM_FT Iy(void) const;
    virtual GEOM_FT Iz(void) const;
    
    VectorPos3d Divide(int num_partes) const;
    boost::python::list DividePy(int num_partes) const;
    
    boost::python::dict getPyDict(void) const;
    void setPyDict(const boost::python::dict &);
    
    void Print(std::ostream &os) const;    
  };

inline GEOM_FT dist2(const Pos3d &p,const Segment3d &r)
  { return r.dist2(p); }
inline GEOM_FT dist2(const Segment3d &r,const Pos3d &p)
  { return dist2(p,r); }
GEOM_FT dist(const Pos3d &p,const Segment3d &r);
inline GEOM_FT dist(const Segment3d &r,const Pos3d &p)
  { return dist(p,r); }

inline bool parallel(const Segment3d &sg,const Line3d &r)
  { return sg.isParallel(r); }
inline bool parallel(const Line3d &r, const Segment3d &sg)
  { return parallel(sg,r); }
inline bool parallel(const Segment3d &sg,const Ray3d &sr)
  { return sg.isParallel(sr); }
inline bool parallel(const Ray3d &sr, const Segment3d &sg)
  { return parallel(sg,sr); }
inline bool parallel(const Segment3d &r1,const Segment3d &r2)
  { return r1.isParallel(r2); }

inline bool colineales(const Segment3d &sg,const Line3d &r)
  { return colineales(sg.getSupportLine(),r); }
inline bool colineales(const Line3d &r,const Segment3d &sg)
  { return colineales(sg,r); }
inline bool colineales(const Segment3d &sg,const Ray3d &sr)
  { return colineales(sg.getSupportLine(),sr); }
inline bool colineales(const Ray3d &sr,const Segment3d &sg)
  { return colineales(sg,sr); }
inline bool colineales(const Segment3d &sg1,const Segment3d &sg2)
  { return colineales(sg1,sg2.getSupportLine()); }

inline GeomObj3d::list_Pos3d intersection(const Segment3d &sg,const Line3d &r)
  { return sg.getIntersection(r); }
inline GeomObj3d::list_Pos3d intersection(const Line3d &r, const Segment3d &sg)
  { return sg.getIntersection(r); }
inline GeomObj3d::list_Pos3d intersection(const Segment3d &sg,const Ray3d &sr)
  { return sg.getIntersection(sr); }
inline GeomObj3d::list_Pos3d intersection(const Ray3d &sr, const Segment3d &sg)
  { return sg.getIntersection(sr); } 
inline GeomObj3d::list_Pos3d intersection(const Segment3d &sg1,const Segment3d &sg2)
  { return sg1.getIntersection(sg2); }

int_pair_deque getIntersections(const std::deque<Segment3d> &);


#endif
