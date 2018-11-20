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



// $Source: /usr/local/cvs/OpenSees/SRC/element/zeroLength/ZeroLengthContact2D.h,v $
// $Revision: 1.2 $
// $Date: 2007/11/28 00:08:58 $



#ifndef ZeroLengthContact2D_h
#define ZeroLengthContact2D_h

/*----+----+----+----+----+----+----+----+----+----+----+----+----+----+----*
 |                                                                          |
 |                    ZeroLengthContact2D element                           |
 +                                                                          +
 |--------------------------------------------------------------------------|
 |                                                                          |
 +             Authors: Gang Wang  AND  Professor Nicholas Sitar            +
 |                                                                          |
 |             Department of Civil and Environmental Engineering            |
 +             University of California, Berkeley, CA 94720, USA            +
 |                                                                          |
 |             Email: wang@ce.berkeley.edu (G.W.)                           |
 +                                                                          +
 |  Disclaimers:                                                            |
 |  (1) Frame of this code is based on zeroLength element                   |
 +  (2) Questions regarding this code should be directed to Gang Wang       +
 |  (3) Documentation could be found at                                     |
 |             www.ce.berkeley.edu/~wang/OpenSees.html                      |
 +                                                                          +
 |  Development History:                                                    |
 |  Created       -- Jan/23/2004                                            |
 +  Final Release -- June 2004                                              +
 |  Add Augmented Lagrange  --Oct 21,2004                                   |
 |                                                                          |
 +----+----+----+----+----+----+----+----+----+----+----+----+----+----+----*/





/*
 element zeroLengthContact2D  $eleID $sNdID $mNdID $Kn $Kt $fs -normal $Nx $Ny
 Description: This file contains the class definition for ZeroLengthContact2D.
 (1) A ZeroLengthContact2D element is defined by two nodes with the same coordinate
     in R^2
 (2) Normal to the contact plane through these contact nodes is preset by the user
     to be either radial or any of x,y,z axis.
 (3) Penalty (Kn,Kt) is used to enforce the constraints, i.e.
           No (in fact, very small) penetration in the normal direction, and
               Coulomb frictional law in the tangential direction.
 (4) Backward Euler return mapping scheme is used for implicit formulation of
     residual and consistent tangent modulus.
 (5) This element is a modified version of continuum-based implicit formulation
     (Laursen and Simo,1993) for small deformation. Contact detection is simplified
         to be only related to these two nodes, and contributions of geometric
         nonlinearities to consistent tangent moudulus are neglected.

  References:
  Laursen,T.A. and Simo, J.C. "A Continuum-based Finite Element Formulation for the
  Implicit Solution of Multibody, Large Deformation Frictional Contact Problems".
  Int. J. Numer. Methods Eng. , 36, 3451-3485 (1993)

  Armero, F. and Petocz, E. "A New Dissipative Time-Stepping Algorithm for Frictional
  Contact Problems: Fromulation and Analysis", Comp. Methods Appl. Mech. Eng., 179,
  151-178 (1999)
  www.ce.berkeley.edu/~wang
*/
#include "ZeroLengthContact.h"

namespace XC {
class Node;
class Channel;
class Response;

//! @ingroup ElemZL
//
//! @brief Zero length element for 2D contact problems.
class ZeroLengthContact2D: public ZeroLengthContact
  {
  private:
    // contact forces
    double lambda; //!< lagrange multiplier, used for augmented Lagrange

    // Normal and Tangental Vectors for Elemental Nodes, (4*1)
    mutable Vector T;
    Vector ContactNormal; //!< out normal of master element
    
    int contactDetect(void) const;
    void formResidAndTangent(int tang_flag ) const;
 
    // contact point and contact plane stuffs
    double stickPt; //!< (keci)
    mutable double xi; //!< trial stick point in local coord

    //static variables for 2D contact
    static Matrix stiff; //!< for stiff matrix
    static Vector resid; //!< for force residual vector
    static Matrix zeroMatrix;
        
  public:
    ZeroLengthContact2D(int tag= 0);
    ZeroLengthContact2D(int tag, int Nd1, int Nd2,double Kn, double Kt, double fRatio, const Vector& normal);
    Element *getCopy(void) const;

    void setDomain(Domain *theDomain);

  
    // public methods to set the state of the element
    int commitState(void);
    int revertToLastCommit(void);
    int revertToStart(void);
    int update(void);


    // public methods to obtain stiffness, mass, damping and residual information
    const Matrix &getTangentStiff(void) const;
    const Matrix &getInitialStiff(void) const;
    const Matrix &getDamp(void) const;
    const Matrix &getMass(void) const;

    const Vector &getResistingForce(void) const;
    const Vector &getResistingForceIncInertia(void) const;


    // public methods for element output
    int sendSelf(CommParameters &);
    int recvSelf(const CommParameters &);
    void Print(std::ostream &os, int flag =0);

    Response *setResponse(const std::vector<std::string> &argv, Information &eleInformation);
    int getResponse(int responseID, Information &eleInformation);
    //void updateDir (const Vector& x, const Vector& y);
  };

} // end of XC namespace


#endif











