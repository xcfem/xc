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
//ShellMITC4Base.h

#ifndef ShellMITC4Base_h
#define ShellMITC4Base_h

#include "domain/mesh/element/plane/QuadBase4N.h"
#include "domain/mesh/element/utils/physical_properties/SectionFDPhysicalProperties.h"
#include "ShellCrdTransf3dBase.h"
#include "ShellBData.h"
#include <utility/matrix/Vector.h>
#include <utility/matrix/Matrix.h>
#include "domain/mesh/element/utils/fvectors/FVectorShell.h"

class Poligono3d;

namespace XC {

class ShellUniformLoad;

//! @ingroup PlaneElements
//
//! @brief Base class for MIT C4 shell elements.
class ShellMITC4Base : public QuadBase4N<SectionFDPhysicalProperties>
  {
  protected:
    double Ktt; //!<drilling stiffness
    mutable double xl[2][4]; //!< local nodal coordinates, two coordinates for each of four nodes

    ShellCrdTransf3dBase *theCoordTransf; //!< Coordinate transformation.

    FVectorShell p0; //!< Reactions in the basic system due to element loads

    mutable Matrix Ki;

    std::vector<Vector> inicDisp; //!< Initial displacements.

    //static data
    static Matrix stiff;
    static Vector resid;
    static Matrix mass;
    static Matrix damping;

    static ShellBData BData; //!< B-bar data

    void free_mem(void);
    void alloc(const ShellCrdTransf3dBase *);

    void setupInicDisp(void);
    void capturaInicDisp(void);
    void zeroInicDisp(void);

    void formInertiaTerms(int tangFlag) const;
    void formResidAndTangent(int tang_flag) const;
    const Matrix calculateG(void) const;
    double *computeBdrill(int node, const double shp[3][4]) const;
    const Matrix& assembleB(const Matrix &Bmembrane, const Matrix &Bbend, const Matrix &Bshear) const;
    const Matrix& computeBmembrane(int node, const double shp[3][4] ) const;
    const Matrix& computeBbend(int node, const double shp[3][4] ) const;
    static void shape2d(const double &,const double &, const double x[2][4], double shp[3][4], double &xsj);
    int sendCoordTransf(int posFlag,const int &,const int &,CommParameters &);
    int recvCoordTransf(int posFlag,const int &posClassTag,const int &posDbTag,const CommParameters &);
    int sendData(CommParameters &);
    int recvData(const CommParameters &);

  public:
    //null constructor
    ShellMITC4Base(int classTag,const ShellCrdTransf3dBase *);
    ShellMITC4Base(int tag,int classTag,const SectionForceDeformation *ptr_mat,const ShellCrdTransf3dBase *); 
    //full constructor
    ShellMITC4Base(int tag,int classTag, int node1, int node2, int node3, int node4, const SectionFDPhysicalProperties &,const ShellCrdTransf3dBase *);
    ShellMITC4Base(const ShellMITC4Base &otro);
    ShellMITC4Base &operator=(const ShellMITC4Base &otro);
    ~ShellMITC4Base(void);

    //set domain because frank is a dumb ass 
    void setDomain(Domain *theDomain);
  
    //return number of dofs
    int getNumDOF(void) const;
	
    int update(void);

    //return stiffness matrix 
    const Matrix &getTangentStiff(void) const;
    const Matrix &getInitialStiff(void) const;
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
    int addLoad(ElementalLoad *theLoad, double loadFactor);
    int addInertiaLoadToUnbalance(const Vector &accel);

    int commitState(void);
    int revertToLastCommit(void);
    int revertToStart(void);
    void alive(void);

    const Vector &getResistingForce(void) const;
    const Vector &getResistingForceIncInertia(void) const;
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
