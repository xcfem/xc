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
                                                                        
// $Revision: 1.5 $
// $Date: 2003/02/14 23:01:12 $
// $Source: /usr/local/cvs/OpenSees/SRC/element/fourNodeQuad/NineNodeMixedQuad.h,v $

// Ed "C++" Love
//
// Constant Presssure/Volume Four Node Quadrilateral
// Plane Strain (NOT PLANE STRESS)
//

#include <domain/mesh/element/ElemWithMaterial.h>
#include <utility/matrix/Vector.h>
#include <utility/matrix/Matrix.h>
#include "domain/mesh/element/utils/physical_properties/NDMaterialPhysicalProperties.h"

namespace XC {

class NDMaterial;

//! @ingroup PlaneElements
//
//! @brief Nine node quad.
class NineNodeMixedQuad: public ElemWithMaterial<9,NDMaterialPhysicalProperties>
  {
  private: 
    static double xl[][9]; //!< nodal coordinates, two coordinates for each of nine nodes
    mutable Matrix *Ki;

    //static data
    static Matrix stiff;
    static Vector resid;
    static Matrix mass;
    static Matrix damping;
    
    
    //quadrature data
    static double root06;
    static double sg[3];
    static double wg[3];

    //node information
					
    
    //form residual and tangent					  
    void formResidAndTangent(int tang_flag) const;
    //inertia terms
    void formInertiaTerms(int tangFlag) const;

    const Matrix& computeBbar( int node, 
			       const double natCoor[2], 
			       const double shp[3][9], 
			       double shpBar[3][9][3] ) const;

    static void shape2dNine( double coor[2], const double x[2][9], double shp[3][9], double &xsj );
    void computeBasis(void) const;
    static double shape1d( int code, int node, double xi );

  protected:
    int sendData(CommParameters &);
    int recvData(const CommParameters &);
  public :
    
    //null constructor
    NineNodeMixedQuad(void);
  
    //full constructor
    NineNodeMixedQuad(int tag, 
		       int node1, int node2, int node3, int node4, int node5, int node6, int node7, int node8, int node9,
		       NDMaterial &theMaterial);
    Element *getCopy(void) const;
    //destructor 
    virtual ~NineNodeMixedQuad(void);

    //set domain 
    void setDomain(Domain *);

    //return number of dofs
    int getNumDOF(void) const;

    //print out element data
    void Print(std::ostream &s, int flag) const;
	
    //return stiffness matrix 
    const Matrix &getTangentStiff(void) const;
    const Matrix &getInitialStiff(void) const;     
    const Matrix &getMass(void) const;

    int addLoad(ElementalLoad *theLoad, double loadFactor);
    int addInertiaLoadToUnbalance(const Vector &accel);

    //get residual
    const Vector &getResistingForce(void) const;
    
    //get residual with inertia terms
    const Vector &getResistingForceIncInertia(void) const;

    virtual int sendSelf(CommParameters &);
    virtual int recvSelf(const CommParameters &);
  }; 
} // end of XC namespace
