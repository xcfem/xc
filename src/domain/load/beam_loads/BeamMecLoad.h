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
//BeamMecLoad.h
                                                                        
#ifndef BeamMecLoad_h
#define BeamMecLoad_h


#include <domain/load/beam_loads/BeamLoad.h>
#include "utility/geom/pos_vec/Pos3d.h"
class SlidingVectorsSystem3d;

namespace XC {
class Matrix;
class FVector;
class CrossSectionProperties2d;
class CrossSectionProperties3d;

//! @ingroup ElemLoads
//
//! @brief Mechanical loads (forces) over beam elements.
class BeamMecLoad: public BeamLoad
  {
  protected:
    double Trans; //!< Transverse load.
    double Axial; //!< Axial load.

    int sendData(Communicator &comm);
    int recvData(const Communicator &comm);
  public:
    BeamMecLoad(int tag, int classTag,const double &Trans,const double &Axial,const ID &theElementTags);
    BeamMecLoad(int tag, int classTag);

    inline double getTransComponent(void) const
      { return Trans; }
    inline double getAxialComponent(void) const
      { return Axial; }
    inline void setTransComponent(const double &t)
      { Trans= t; }
    inline void setAxialComponent(const double &a)
      { Axial= a; }

    virtual const Matrix &getAppliedSectionForces(const double &L,const Matrix &xi,const double &loadFactor) const;
    virtual void addReactionsInBasicSystem(const std::vector<double> &,const double &,FVector &) const;
    virtual void addFixedEndForcesInBasicSystem(const std::vector<double> &,const double &,FVector &, int release= 0) const;
    void addElasticDeformations(const double &L,const CrossSectionProperties3d &ctes_scc,const double &lpI,const double &lpJ,const double &loadFactor,FVector &v0);
    void addElasticDeformations(const double &L,const CrossSectionProperties2d &ctes_scc,const double &lpI,const double &lpJ,const double &loadFactor,FVector &v0);

    virtual size_t getForceVectorDimension(void) const;
    virtual size_t getMomentVectorDimension(void) const;
    virtual Vector getLocalForce(void) const;
    virtual Vector getLocalMoment(void) const;
    virtual const Matrix &getLocalForces(void) const;
    virtual const Matrix &getLocalMoments(void) const;
    virtual const Matrix &getGlobalVectors(const Matrix &) const;
    virtual const Matrix &getGlobalForces(void) const;
    virtual const Matrix &getGlobalMoments(void) const;
    Vector3d getVector3dLocalForce(void) const;
    Vector3d getVector3dLocalMoment(void) const;

    virtual SlidingVectorsSystem3d getResultant(const Pos3d &p= Pos3d(), bool initialGeometry= true) const;

    void Print(std::ostream &s, int flag =0) const;

  };
} // end of XC namespace

#endif

