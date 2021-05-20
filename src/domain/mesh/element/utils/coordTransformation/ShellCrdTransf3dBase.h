// -*-c++-*-
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
//ShellCrdTransf3dBase.h

#ifndef ShellCrdTransf3dBase_h
#define ShellCrdTransf3dBase_h

#include "utility/kernel/CommandEntity.h"
#include "utility/actor/actor/MovableObject.h"
#include "utility/matrix/Vector.h"
#include "domain/mesh/element/utils/ParticlePos3d.h"

class Plane;
class Ref2d3d;

namespace XC {

class Matrix;
class NodePtrs;

//! @ingroup PlaneElements
//
//! @brief Base class for 3D coordinate transformations.
class ShellCrdTransf3dBase: public CommandEntity, public MovableObject
  {
  protected:
    const NodePtrs *theNodes;
    Vector vpos_centroide; //!< Element centroid position.
    Vector g1; //!< Vector 1 of the reference trihedron.
    Vector g2; //!< Vector 2 of the reference trihedron.
    Vector g3; //!< Vector 3 of the reference trihedron.
    DbTagData &getDbTagData(void) const;
    int sendData(Communicator &);
    int recvData(const Communicator &);

    Vector local_to_global(const Vector &) const;
    Matrix local_to_global(const Matrix &) const;
    const Vector &local_to_global_resisting_force(const Vector &pl) const;
    const Matrix &local_to_global_stiff_matrix(const Matrix &kl) const;

  public:
    ShellCrdTransf3dBase(void);
    inline virtual ~ShellCrdTransf3dBase(void)
      {}
    ShellCrdTransf3dBase(const Vector &,const Vector &,const Vector &);
    virtual ShellCrdTransf3dBase *getCopy(void) const= 0;

    //! @brief Returns the transformation matrix.
    Matrix getTrfMatrix(void) const;
    //! @brief Returns the first local axis (contained in the plane of the element)
    inline const Vector &G1(void) const
      { return g1; }
    //! @brief Returns the second local axis (contained in the plane of the element)
    inline const Vector &G2(void) const
      { return g2; }
    //! @brief Returns the third local axis (normal to the plane of the element)
    inline const Vector &G3(void) const
      { return g3; }
    //! @brief Returns the element centroid.
    inline const Vector &getVPosCentroide(void) const
      { return vpos_centroide; }
    Plane getPlane(void) const;
    Pos3d getProj(const Pos3d &);

    virtual int initialize(const NodePtrs &)= 0;
    virtual int setup_nodal_local_coordinates(void) const;
    virtual int update(void)= 0;

    virtual int commitState(void) = 0;
    virtual int revertToLastCommit(void) = 0;
    virtual int revertToStart(void) = 0;
    
    virtual Vector getBasicTrialDisp(const int &) const= 0;
    virtual Vector getBasicTrialVel(const int &) const= 0;
    virtual Vector getBasicTrialAccel(const int &) const= 0;

    virtual void getGlobalTangent(Matrix &stiff) const;
    virtual void getGlobalResidAndTangent(Vector &resid,Matrix &stiff) const;
    virtual const Vector &getGlobalResistingForce(const Vector &p0) const;

    const Vector &getVectorGlobalCoordFromLocal(const Vector &localCoords) const;
    const Matrix &getVectorGlobalCoordFromLocal(const Matrix &localCoords) const;
    const Vector &getVectorLocalCoordFromGlobal(const Vector &globalCoords) const;

    virtual Matrix getLocalAxes(bool initialGeometry= true) const;
    Ref2d3d getLocalReference(bool initialGeometry= true) const;
    Pos2d getLocalCoordinates(const Pos3d &) const;
    ParticlePos3d getNaturalCoordinates(const Pos3d &,double xl[2][4]) const;    
  };

} // end of XC namespace

#endif
