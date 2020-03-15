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
/* ****************************************************************** **
**    OpenSees - Open System for Earthquake Engineering Simulation    **
**          Pacific Earthquake Engineering Research Center            **
**                                                                    **
**                                                                    **
** (C) Copyright 1999, The Regents of the University of California    **
** All Rights Reserved.                                               **
**                                                                    **
** Commercial use of this program without express permission of the   **
** University of California, Berkeley, is strictly prohibited.  See   **
** file 'COPYRIGHT'  in main directory for information on usage and   **
** redistribution,  and for a DISCLAIMER OF ALL WARRANTIES.           **
**                                                                    **
** Developed by:                                                      **
**   Frank McKenna (fmckenna@ce.berkeley.edu)                         **
**   Gregory L. Fenves (fenves@ce.berkeley.edu)                       **
**   Filip C. Filippou (filippou@ce.berkeley.edu)                     **
**                                                                    **
** ****************************************************************** */

// by Jinchi Lu and Zhaohui Yang (May 2004)
//
// 20-8 Noded brick element: TwentyEightNodeBrickUP
//

#include <domain/mesh/element/ElemWithMaterial.h>
#include "domain/mesh/element/utils/physical_properties/NDMaterialPhysicalProperties.h"
#include "domain/mesh/element/utils/body_forces/BodyForces3D.h"

namespace XC {
//! @ingroup ElemVol
//
//! @brief Twenty eight node exahedron.
class TwentyEightNodeBrickUP : public ElemWithMaterial<20,NDMaterialPhysicalProperties>
  {
  private:
    BodyForces3D bf; //!< Body forces
    double rho_f;	//! Fluid mass per unit volume
    double kc;  //! combined bulk modulus
    double perm[3]; //! permeability
    mutable Matrix *Ki;

    //static data
    static Matrix stiff ;
    static Vector resid ;
    static Matrix mass ;
    static Matrix damp ;

    //quadrature data
    static const int nintu;
    static const int nintp;
    static const int nenu;
    static const int nenp;


    //local nodal coordinates, three coordinates for each of twenty nodes
    //    static double xl[3][20] ;
    static double xl[3][20] ;

    static double shgu[4][20][27];	// Stores shape functions and derivatives (overwritten)
    static double shgp[4][8][8];	// Stores shape functions and derivatives (overwritten)
    static double shgq[4][20][8];	// Stores shape functions and derivatives (overwritten)
    static double shlu[4][20][27];	// Stores shape functions and derivatives
    static double shlp[4][8][8];	// Stores shape functions and derivatives
    static double shlq[4][20][8];	// Stores shape functions and derivatives
    static double wu[27];		// Stores quadrature weights
    static double wp[8];		// Stores quadrature weights
    static double dvolu[27];  // Stores detJacobian (overwritten)
    static double dvolp[8];  // Stores detJacobian (overwritten)
    static double dvolq[8];  // Stores detJacobian (overwritten)

    //inertia terms
    void formInertiaTerms( int tangFlag ) const;
    //damping terms
    void formDampingTerms( int tangFlag ) const;

	// Mixture mass density at integration point i
    double mixtureRho(int ipt) const;

    //compute coordinate system
    void computeBasis(void) const;

    // compute local shape functions
    void compuLocalShapeFunction();
    void Jacobian3d(int gaussPoint, double& xsj, int mode) const;
    const Matrix &getStiff( int flag) const;
  protected:
    int sendData(CommParameters &);
    int recvData(const CommParameters &);
  public :
    //null constructor
    TwentyEightNodeBrickUP( ) ;

    //full constructor
    TwentyEightNodeBrickUP( int tag,int node1,
		int node2,
		int node3,
		int node4,
		int node5,
		int node6,
		int node7,
		int node8,
		int node9,
		int node10,
		int node11,
		int node12,
		int node13,
		int node14,
		int node15,
		int node16,
		int node17,
		int node18,
		int node19,
		int node20,
		NDMaterial &theMaterial,double bulk, double rhof,
		double perm1, double perm2, double perm3,
		 const BodyForces3D &bForces= BodyForces3D());

    //destructor
    virtual ~TwentyEightNodeBrickUP(void) ;
    Element *getCopy(void) const;
    //set domain
    void setDomain( Domain *theDomain ) ;

    //return number of dofs
    int getNumDOF(void) const;

    //print out element data
    void Print( std::ostream &s, int flag ) const;

    int update(void);

    //return stiffness matrix
    const Matrix &getTangentStiff(void) const;
    const Matrix &getInitialStiff(void) const;
    const Matrix &getDamp(void) const;
    const Matrix &getMass(void) const;

    int addLoad(ElementalLoad *theLoad, double loadFactor);
    int addInertiaLoadToUnbalance(const Vector &accel);

    //get residual
    const Vector &getResistingForce(void) const;
    const Vector &getResistingForceIncInertia(void) const;

    virtual int sendSelf(CommParameters &);
    virtual int recvSelf(const CommParameters &);

    Response *setResponse(const std::vector<std::string> &argv, Information &eleInformation);
    int getResponse(int responseID, Information &eleInformation);

    // RWB; PyLiq1 & TzLiq1 need to see the excess pore pressure and initial stresses.
    friend class PyLiq1;
    friend class TzLiq1;
  };

} // end of XC namespace
