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
                                                                        
// $Revision: 1.8 $
// $Date: 2003/02/14 23:01:10 $
// $Source: /usr/local/cvs/OpenSees/SRC/element/fourNodeQuad/EnhancedQuad.h,v $
                                                                        
#include "domain/mesh/element/plane/QuadBase4N.h"
#include "domain/mesh/element/utils/physical_properties/SolidMech2D.h"
#include <utility/matrix/Vector.h>
#include <utility/matrix/Matrix.h>

namespace XC {
//! @ingroup PlaneElements
//
//! @brief Four-node quadrilateral element, which uses a
//! bilinear isoparametric formulation with enhanced strain modes.
class EnhancedQuad: public QuadBase4N<SolidMech2D>
  {
  private:

    mutable Vector alpha; //!< enhanced strain parameters
    mutable Matrix *Ki;

    //static data
    static Matrix stiff;
    static Vector resid;
    static Matrix mass;
    static Matrix damping;

    //quadrature data
    static const double sg[4];
    static const double tg[4];
    static const double wg[4];

    
    static double stressData[][4]; //!< stress data
    static double tangentData[][3][4]; //!< tangent data 


    //local nodal coordinates, two coordinates for each of four nodes
    //    static double xl[2][4]; 
    static double xl[][4]; 

    //save stress and tangent data
    static void saveData(int gp, const Vector &stress,const Matrix &tangent);

    //recover stress and tangent data
    void getData(int gp,Vector &stress,Matrix &tangent) const;

    //compute enhanced strain B-matrices
    const Matrix& computeBenhanced( int node, double L1, double L2, double j, const Matrix &Jinv) const;

			   
    //compute local coordinates and basis
    void computeBasis(void) const;
        
    //form residual and tangent					  
    void formResidAndTangent(int tang_flag) const;

    //inertia terms
    void formInertiaTerms(int tangFlag) const;


    //compute Jacobian matrix and inverse at point {L1,L2}
    static void computeJacobian( double L1, double L2, const double x[2][4], Matrix &JJ, Matrix &JJinv );

    //compute Bbend matrix
    const Matrix& computeB(int node, const double shp[3][4]) const;

    //Matrix transpose of a 3x2 matrix
    static const Matrix& transpose(const Matrix &M);

    //shape function routine for four node quads
    static void shape2d( double ss, double tt, const double x[2][4], double shp[3][4], double &xsj );


    bool check_material_type(const std::string &type) const;
  protected:
    int sendData(Communicator &);
    int recvData(const Communicator &);
  public:
    EnhancedQuad(void);
    EnhancedQuad(int tag,const NDMaterial *ptr_mat);
    //full constructor
    EnhancedQuad(int tag, int nd1, int nd2, int nd3, int nd4, NDMaterial &, const std::string &);

    Element *getCopy(void) const;
    //destructor
    virtual ~EnhancedQuad(void);

    //set domain
    void setDomain(Domain *);

    //return number of dofs
    int getNumDOF(void) const;

    // methods dealing with state updates
    int update(void);

    //print out element data
    void Print(std::ostream &s, int flag) const;
	
    //return stiffness matrix 
    const Matrix &getTangentStiff(void) const;
    const Matrix &getInitialStiff(void) const;
    const Matrix &getMass(void) const;

    int addLoad(ElementalLoad *theLoad, double loadFactor);
    int addInertiaLoadToUnbalance(const Vector &accel);

    //get residual and residual with inertia terms
    const Vector &getResistingForce(void) const;
    const Vector &getResistingForceIncInertia(void) const;

    virtual int sendSelf(Communicator &);
    virtual int recvSelf(const Communicator &);
  }; 
} // end of XC namespace
