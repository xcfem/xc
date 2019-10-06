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
//Shell4NBase.h

#ifndef Shell4NBase_h
#define Shell4NBase_h

#include "domain/mesh/element/plane/QuadBase4N.h"
#include "domain/mesh/element/utils/physical_properties/SectionFDPhysicalProperties.h"
#include "ShellCrdTransf3dBase.h"
#include "ShellBData.h"
#include <utility/matrix/Vector.h>
#include <utility/matrix/Matrix.h>
#include "domain/mesh/element/utils/fvectors/FVectorShell.h"

class Polygon3d;

namespace XC {

class ShellUniformLoad;

//! @ingroup PlaneElements
//
//! @brief Base class for four node shell elements.
class Shell4NBase: public QuadBase4N<SectionFDPhysicalProperties>
  {
    void free_mem(void);
    void alloc(const ShellCrdTransf3dBase *);
  protected:
    mutable double xl[2][4]; //!< local nodal coordinates, two coordinates for each of four nodes

    ShellCrdTransf3dBase *theCoordTransf; //!< Coordinate transformation.
    int applyLoad;
    double appliedB[3]; //!< Body forces applied with load

    mutable Matrix Ki;


    //static data
    static Matrix stiff;
    static Vector resid;
    static Matrix mass;
    static Matrix damping;

    void formInertiaTerms(int tangFlag) const;
    virtual void formResidAndTangent(int tang_flag) const= 0;
    static void shape2d(const double &,const double &, const double x[2][4], double shp[3][4], double &xsj, double sx[2][2]);
    int sendCoordTransf(int posFlag,const int &,const int &,CommParameters &);
    int recvCoordTransf(int posFlag,const int &posClassTag,const int &posDbTag,const CommParameters &);
    int sendData(CommParameters &);
    int recvData(const CommParameters &);

    friend class ShellCrdTransf3dBase;
    friend class ShellLinearCrdTransf3d;
    friend class ShellUpBasisCrdTransf3d;
    typedef double (*pointer_to_xl)[4]; // To return xl.
    inline const pointer_to_xl get_xl(void) const
      { return xl; }
    inline pointer_to_xl get_xl(void)
      { return xl; }
  public:
    //null constructor
    Shell4NBase(int classTag,const ShellCrdTransf3dBase *);
    Shell4NBase(int tag,int classTag,const SectionForceDeformation *ptr_mat,const ShellCrdTransf3dBase *); 
    //full constructor
    Shell4NBase(int tag,int classTag, int node1, int node2, int node3, int node4, const SectionFDPhysicalProperties &,const ShellCrdTransf3dBase *);
    Shell4NBase(const Shell4NBase &);
    Shell4NBase &operator=(const Shell4NBase &);
    ~Shell4NBase(void);

    //return number of dofs
    int getNumDOF(void) const;
	
    int update(void);

    //return stiffness matrix 
    const Matrix &getTangentStiff(void) const;
    const Matrix &getMass(void) const;

    const GaussModel &getGaussModel(void) const;

    Vector getInterpolationFactors(const ParticlePos3d &) const;
    Vector getInterpolationFactors(const Pos3d &) const;
    Vector getInterpolatedDisplacements(const ParticlePos3d &) const;
    Vector getInterpolatedDisplacements(const Pos3d &) const;

    //Load definition methods.
    const ShellUniformLoad *vector3dUniformLoadLocal(const Vector &);
    const ShellUniformLoad *vector3dUniformLoadGlobal(const Vector &);
    void strainLoad(const Matrix &);

    // methods for applying loads
    void zeroLoad(void);
    int addLoad(ElementalLoad *, double);
    int addInertiaLoadToUnbalance(const Vector &accel);

    int commitState(void);
    int revertToLastCommit(void);
    int revertToStart(void);

    double getMeanInternalForce(const std::string &) const;
    double getMeanInternalDeformation(const std::string &) const;

    virtual Matrix getLocalAxes(bool initialGeometry= true) const;
    virtual ShellCrdTransf3dBase *getCoordTransf(void);
    virtual const ShellCrdTransf3dBase *getCoordTransf(void) const;

    void computeBasis(void);
    ParticlePos3d getLocalCoordinatesOfNode(const int &) const;
    ParticlePos3d getNaturalCoordinates(const Pos3d &) const;

    void Print(std::ostream &s, int flag );
  }; 

} // end of XC namespace
#endif
