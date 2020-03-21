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
                                                                        
// $Revision: 1.11 $
// $Date: 2006/01/10 18:41:34 $
// $Source: /usr/local/cvs/OpenSees/SRC/element/brick/Brick.h,v $

// Ed "C++" Love
//
// Eight node Brick element 
//

#include <domain/mesh/element/volumetric/BrickBase.h>
#include "domain/mesh/element/utils/body_forces/BodyForces3D.h"

namespace XC {
//! @ingroup ElemVol
//
//! @brief Eight node hexahedron.
class Brick: public BrickBase
  {
  private :
    // static constants
    static const int numberGauss= 8; //!< Number of Gauss points.
    static const int nShape = 4;
    
    BodyForces3D bf; //!< Body forces
    double appliedB[ndf];	//!< Body forces applied with load
    bool applyLoad;
    
    mutable Matrix *Ki;
    void shape_functions_loop(void) const;

    //
    // static attributes
    //

    static Matrix stiff;
    static Vector resid;
    static Matrix mass;
    static Matrix damping;
    static double gaussPoint[numberGauss][ndm]; //!< Gauss points coordinates.
    static double dvol[numberGauss]; //!< volume element
    static double shp[nShape][numberNodes]; //!< shape functions at a gauss point
    static double Shape[nShape][numberNodes][numberGauss]; //!< all the shape functions

    // quadrature data
    static const double wg[numberGauss];
  
    //
    // private methods
    //

    //inertia terms
    void formInertiaTerms(int tangFlag) const;
    //form residual and tangent					  
    void formResidAndTangent(int tang_flag) const;

    //compute B matrix
    const Matrix& computeB( int node, const double shp[4][8]) const;
  
    static size_t getVectorIndex(const size_t &,const size_t &);
  protected:
    int sendData(CommParameters &cp);
    int recvData(const CommParameters &cp);
  public :
    
    Brick(void);
    Brick(int tag,const NDMaterial *ptr_mat);
  
    //full constructor
    Brick( int tag, int node1,int node2,int node3,int node4,int node5,int node6,int node7,int node8, NDMaterial &theMaterial, const BodyForces3D &bf= BodyForces3D());
    Element *getCopy(void) const;
    //destructor 
    virtual ~Brick(void);

    //set domain
    void setDomain( Domain *theDomain );

    //return number of dofs
    int getNumDOF(void) const;

    // update
    int update(void);

    Matrix getGaussPointsPositions(void) const;
    //return stiffness matrix 
    const Matrix &getTangentStiff(void) const;
    const Matrix &getInitialStiff(void) const;    
    const Matrix &getMass(void) const;    

    Vector getAvgStress(void) const;
    double getAvgStress(const size_t &,const size_t &) const;
    Vector getAvgStrain(void) const;
    double getAvgStrain(const size_t &,const size_t &) const;

    void zeroLoad(void);
    int addLoad(ElementalLoad *theLoad, double loadFactor);
    int addInertiaLoadToUnbalance(const Vector &accel);

    Vector getShapeFunctionsValues(const double &r, const double &s, const double &t) const;
    
    const Vector &getResistingForce(void) const; //get residual
    const Vector &getResistingForceIncInertia(void) const; //get residual with inertia terms

    // public methods for element output
    virtual int sendSelf(CommParameters &);
    virtual int recvSelf(const CommParameters &);
      
    //print out element data
    void Print( std::ostream &s, int flag ) const;

    Response *setResponse(const std::vector<std::string> &argv, Information &eleInformation);
    int getResponse(int responseID, Information &eleInformation);
  }; 

} // end of XC namespace
