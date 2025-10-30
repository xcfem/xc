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
//Element1D.h

#ifndef Element1D_h
#define Element1D_h

#include <domain/mesh/element/ElementBase.h>

class Segment3d;

namespace XC {
class Node;
class Material;
class Domain;
class CrdTransf;
class CrdTransf2d;
class CrdTransf3d;
class DeformationPlane;

//! @ingroup Elem
//!
//! @defgroup OneDimensionalElem One-dimensional elements.
//
//! @ingroup OneDimensionalElem
//! @brief Base class for one-dimensional elements (beam,truss,...)
class Element1D: public ElementBase<2>
  {
    void unidimensional_meshing(const NodePtrArray3d &,ElemPtrArray3d &) const;
  protected:
    mutable std::vector<double> tributaryLengths;

    static void shape1d(const double &, double shp[2]);
    
    ElemPtrArray3d put_on_mesh(const NodePtrArray3d &,meshing_dir dm) const;
    ElemPtrArray3d sew(const SetEstruct &f1,const SetEstruct &f2) const;

    int sendCoordTransf(int posFlag,const int &,const int &,Communicator &);
    CrdTransf *recvCoordTransf(int posFlag,const int &,const int &,const Communicator &);
    CrdTransf2d *recvCoordTransf2d(int posFlag,const int &,const int &,const Communicator &);

    CrdTransf3d *recvCoordTransf3d(int posFlag,const int &,const int &,const Communicator &);


  public:
    Element1D(int tag, int classTag);
    Element1D(int tag, int classTag,int Nd1,int Nd2);

    BoolArray3d getNodePattern(void) const;
    void setDomain(Domain *theDomain);
    Matrix getLocalAxes(bool) const;
    virtual CrdTransf *getCoordTransf(void);
    virtual const CrdTransf *getCoordTransf(void) const;
    int resetNodalCoordinates(void);
    virtual CrdTransf *checkCoordTransf(void);
    virtual const CrdTransf *checkCoordTransf(void) const;
    virtual const Matrix &getCooPoints(const size_t &ndiv) const;
    virtual const Vector &getCooPoint(const double &xrel) const;
    Segment3d getLineSegment(bool initialGeometry= true) const;
    double getDist2(const Pos2d &p,bool initialGeometry= true) const;
    double getDist(const Pos2d &p,bool initialGeometry= true) const;
    double getDist2(const Pos3d &p,bool initialGeometry= true) const;
    double getDist(const Pos3d &p,bool initialGeometry= true) const;
    Pos2d getProjection(const Pos2d &p,bool initialGeometry= true) const;
    Pos3d getProjection(const Pos3d &p,bool initialGeometry= true) const;

    size_t getDimension(void) const;
    double getLength(bool initialGeometry= true) const;
    int getVtkCellType(void) const;

    double getLocalCoordinates(const Pos3d &, bool initialGeometry= true) const;
    ParticlePos3d getNaturalCoordinates(const Pos3d &, bool initialGeometry= true) const;    
    Vector getInterpolationFactors(const ParticlePos3d &) const;
    Vector getInterpolationFactors(const Pos3d &) const;

    void computeTributaryLengths(bool initialGeometry= true) const;
    double getTributaryLength(const Node *) const;
    void computeTributaryAreas(bool initialGeometry= true) const;
    double getTributaryArea(const Node *) const;

    ElementalLoad *vector2dUniformLoadGlobal(const Vector &);
    ElementalLoad *vector2dUniformLoadLocal(const Vector &);
    ElementalLoad *vector2dPartialUniformLoadGlobal(const double &, const double &, const Vector &);
    ElementalLoad *vector2dPartialUniformLoadLocal(const double &, const double &, const Vector &);
    ElementalLoad *vector2dPointByRelDistLoadGlobal(const double &,const Vector &);
    ElementalLoad *vector2dPointByRelDistLoadLocal(const double &,const Vector &);
    ElementalLoad *vector2dPointLoadGlobal(const Vector &,const Vector &);
    ElementalLoad *vector2dPointLoadLocal(const Vector &,const Vector &);
    ElementalLoad *vector3dUniformLoadGlobal(const Vector &);
    ElementalLoad *vector3dUniformLoadLocal(const Vector &);
    ElementalLoad *vector3dPointByRelDistLoadGlobal(const double &,const Vector &);
    ElementalLoad *vector3dPointByRelDistLoadLocal(const double &,const Vector &);
    ElementalLoad *vector3dPointLoadGlobal(const Vector &,const Vector &);
    ElementalLoad *vector3dPointLoadLocal(const Vector &,const Vector &);
    ElementalLoad *strainLoad(const DeformationPlane &p1, const DeformationPlane &p2);
  };

} //end of XC namespace
#endif
