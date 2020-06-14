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



// $Source: /usr/local/cvs/OpenSees/SRC/element/zeroLength/ZeroLengthContact3D.h,v $
// $Revision: 1.2 $
// $Date: 2007/11/28 00:08:58 $

#ifndef ZeroLengthContact3D_h
#define ZeroLengthContact3D_h

// Written: Gang Wang  (wang@ce.berkeley.edu)
//          Prof. Nicholas Sitar  (nsitar@ce.berkeley.edu)
//
// Created: 27/08/2003
//


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
 |  Created       -- Aug/27/2003                                            |
 +  Final Release -- June 2004                                              +
 |                                                                          |
 |                                                                          |
 +----+----+----+----+----+----+----+----+----+----+----+----+----+----+----*/

/* command

 element zeroLengthContact3D  $eleID $sNdID $mNdID $Kn $Kt $fs $c $dir

 where:
  $eleID: element ID of this contact element
  $sNdID: constrained node ID
  $mNdID: retained node ID
  $Kn   : penalty in normal directions
  $Kt:  : penalty in tangential directions
  $c    : cohesion
  $dir  : direction of the contact (0,1,2,3)
          0: circular contact (constrained node is inside)
                  1: out normal of primary plane pointing to +X direction
                  2: out normal of primary plane pointing to +Y direction
                  3: out normal of primary plane pointing to +Z direction

 Description: This file contains the class definition for ZeroLengthContact3D.
 (1) A ZeroLengthContact3D element is defined by two nodes with the same coordinate
     in R^3
 (2) Normal to the contact plane through these contact nodes is preset by the user
     to be either circular or any of x,y,z axis.
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
*/


#include "ZeroLengthContact.h"

namespace XC {
class Node;
class Channel;
class Response;

//! @ingroup ElemZL
//
//! @brief Zero length element for 3D problems.
class ZeroLengthContact3D: public ZeroLengthContact
  {
  private:
    int    directionID;
    // parameters
    double cohesion; // cohesion
  
    //contact point and contact plane stuffs
    Vector stickPt; // (keci_1, keci_2)
    mutable Vector xi;
    Vector origin; // (x0,y0) for circular impact
    //Tangental Vectors for Elemental Nodes, (6*1)
    mutable Vector T1;
    mutable Vector T2;
  
    
    int contactDetect(void) const; // detect the contact and set flag
    void formResidAndTangent(int tang_flag) const;//form residual and tangent
  
  
    Matrix *Ki; // pointer to objects matrix (a class Matrix)
  
    //static variables for 3D contact
    static Matrix stiff; // for stiff matrix
    static Vector resid; // for force residual vector
    static Matrix zeroMatrix;
  public:
    ZeroLengthContact3D(int tag= 0);
    ZeroLengthContact3D(int tag,int Nd1, int Nd2,int direction, double Kn, double Kt, double fRatio, double c, double originX, double originY);
    Element *getCopy(void) const;
  
    void setDomain(Domain *theDomain);
  
    // public methods to set the state of the element
    int commitState(void);
    int revertToLastCommit(void);
    int revertToStart(void);
    //int update(void);
  
    // public methods to obtain stiffness, mass, damping and residual information
    const Matrix &getTangentStiff(void) const;
    const Matrix &getInitialStiff(void) const;
    const Matrix &getDamp(void) const;
    const Matrix &getMass(void) const;
  
    const Vector &getResistingForce(void) const;
    const Vector &getResistingForceIncInertia(void) const;
  
    // public methods for element output
    int sendSelf(Communicator &);
    int recvSelf(const Communicator &);
    void Print(std::ostream &s, int flag =0) const;
  
    Response *setResponse(const std::vector<std::string> &argv, Information &eleInformation);
    int getResponse(int responseID, Information &eleInformation);
  };
} // end of XC namespace

#endif
