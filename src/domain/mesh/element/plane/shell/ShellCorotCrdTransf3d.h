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
//ShellCorotCrdTransf3d.h

#ifndef ShellCorotCrdTransf3d_h
#define ShellCorotCrdTransf3d_h

#include "ShellCrdTransf3dBase.h"

namespace XC {

//! \ingroup ElemPlanos
//
//! @brief Base class for 3D coordinate transformation.
class ShellCorotCrdTransf3d: public ShellCrdTransf3dBase
  {
  protected:
    Vector v130;
    Vector v240;

    Vector g1trial; //!< Vector 1 of the reference trihedron.
    Vector g2trial; //!< Vector 2 of the reference trihedron.
    Vector g3trial; //!< Vector 3 of the reference trihedron.

    Vector g1commit; //!< Vector 1 of the reference trihedron.
    Vector g2commit; //!< Vector 2 of the reference trihedron.
    Vector g3commit; //!< Vector 3 of the reference trihedron.

    Vector global_to_local(const Vector &,const Vector &) const;
    Vector global_to_local_disp_nod(const int &) const;
    Vector global_to_local_vel_nod(const int &) const;
    Vector global_to_local_accel_nod(const int &) const;
    Vector local_to_global(const Matrix &,const Matrix &,const Vector &) const;
    Matrix local_to_global(const Matrix &,const Matrix &,const Matrix &) const;
    const Vector &local_to_global_resisting_force(const Vector &pl) const;
  public:
    ShellCorotCrdTransf3d(void);
    ShellCorotCrdTransf3d(const Vector &,const Vector &,const Vector &);
    ShellCorotCrdTransf3d(const NodePtrs &t);
    virtual ShellCrdTransf3dBase *getCopy(void) const;

    //! @brief Devuelve el eje 1 local (contenido en el plano del elemento)
    inline const Vector &G1trial(void) const
      { return g1trial; }
    //! @brief Devuelve el eje 2 local (contenido en el plano del elemento)
    inline const Vector &G2trial(void) const
      { return g2trial; }
    //! @brief Devuelve el eje 3 local (normal al plano del elemento)
    inline const Vector &G3trial(void) const
      { return g3trial; }

    Matrix getR0(void) const;
    Matrix getR(void) const;
    Matrix getR0T(void) const;

    virtual int initialize(const NodePtrs &);
    virtual int update(void);

    virtual int commitState(void);
    virtual int revertToLastCommit(void);        
    virtual int revertToStart(void);
    
    virtual Vector getBasicTrialDisp(const int &) const;
    virtual Vector getBasicTrialVel(const int &) const;
    virtual Vector getBasicTrialAccel(const int &) const;

    void getGlobalTangent(Matrix &stiff) const;
    void getGlobalResidAndTangent(Vector &resid,Matrix &stiff) const;
    virtual const Vector &getGlobalResistingForce(const Vector &p0) const;

    int sendSelf(CommParameters &);
    int recvSelf(const CommParameters &);

  };

} // end of XC namespace

#endif
