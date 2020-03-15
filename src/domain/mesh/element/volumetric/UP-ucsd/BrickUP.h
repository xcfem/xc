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

///////////////////////////////////////////////////////////////////////////////
// Description: This file contains the class declaration for                 //
// BrickUP, an 8-node cubic element for solid-fluid fully coupled analysis.  //
// This implementation is a simplified u-p formulation of Biot theory        //
// (u - solid displacement, p - fluid pressure). Each element node has three //
// DOFs for u and 1 DOF for p.                                               //
//                                                                           //
// Written by Zhaohui Yang	(March 2004)                                     //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////

// $Revision: 1.1 $
// $Date: 2005/09/22 21:28:36 $
// $Source: /usr/local/cvs/OpenSees/SRC/element/UP-ucsd/BrickUP.h,v $

// by Zhaohui Yang (Modified based on Ed "C++" Love's Brick element)
//
// Eight node BrickUP element
//

#include <domain/mesh/element/volumetric/BrickBase.h>
#include "domain/mesh/element/utils/body_forces/BodyForces3D.h"

namespace XC{
class NDMaterial;
//! @ingroup ElemVol
//
//! @brief Eight node exahedron.
class BrickUP : public BrickBase
  {
  private :
    BodyForces3D bf; //!< Body forces
    double rho_f; //!< Fluid mass per unit volume
    double kc;  //!< combined bulk modulus
    double perm[3]; //!< permeability
    mutable Matrix *Ki;

    //static data
    static Matrix stiff ;
    static Vector resid ;
    static Matrix mass ;
    static Matrix damp ;

    //quadrature data
    static const double sg[2] ;
    static const double wg[8] ;


    void formInertiaTerms(int tangFlag) const;
    void formDampingTerms(int tangFlag) const;
    void formResidAndTangent(int tang_flag) const;
    double mixtureRho(int ipt) const;
    const Matrix& computeBbar(int node,const double shp[4][8],const double shpBar[4][8] ) ;
    const Matrix& computeB( int node, const double shp[4][8] ) const;
    Matrix transpose( int dim1, int dim2, const Matrix &M ) ;
  protected:
    int sendData(CommParameters &);
    int recvData(const CommParameters &);
  public :
    //null constructor
    BrickUP(void);

    //full constructor
    BrickUP( int tag, int node1,int node2,int node3,int node4,int node5,int node6,int node7,int node8,NDMaterial &theMaterial,double bulk, double rhof, double perm1, double perm2, double perm3,  const BodyForces3D &bForces= BodyForces3D()) ;
    Element *getCopy(void) const;
    //destructor
    virtual ~BrickUP(void) ;

    //set domain
    void setDomain( Domain *theDomain );

    //return number of dofs
    int getNumDOF(void) const;

    //print out element data
    void Print(std::ostream &s, int flag) const;

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
} ;

} // end of XC namespace
