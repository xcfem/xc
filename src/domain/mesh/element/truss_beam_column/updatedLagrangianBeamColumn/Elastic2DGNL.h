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
** See file 'COPYRIGHT'  in main directory for information on usage   **
** and redistribution of OpenSees, and for a DISCLAIMER OF ALL        **
** WARRANTIES.                                                        **
**                                                                    **
** Elastic2dGNL.h: interface for the Elastic2dGNL class               **
** Developed by:                                                      **
**    Rohit Kaul       (rkaul@stanford.edu)                           **
**    Greg Deierlein   (ggd@stanford.edu)                             **
**                                                                    **
**           John A. Blume Earthquake Engineering Center              **
**                    Stanford University                             **
** ****************************************************************** **/

// Written: rkaul
// Created: 7/30
//

#ifndef Elastic2dGNL_H
#define Elastic2dGNL_H

// List of included files
// UpdatedLagrangianBeam2D - parent class of this class
#include "domain/mesh/element/truss_beam_column/updatedLagrangianBeamColumn/UpdatedLagrangianBeam2D.h"

namespace XC {
//! @ingroup OneDimensionalElem
//
//! @brief Elastic2dGNL is a subclass of UpdatedLagrangianBeam2D, that can be 
// used to model 2d beam column elements with large deformation effects.
class Elastic2dGNL: public UpdatedLagrangianBeam2D  
  {
  private:
    // Data declarations
    double A, E, Iz;
  public:	
    // Arguments passed to the constructor include - tag, a unique id in 
    // the domain, A - cross section area of the beam, E - modulus of
    // Elasticity, I - Izz, Nd1 and Nd2 are the nodal numbers of connected
    // nodes.  Last two parameters are optional, by default geometric
    // nonlinearity is turned on and rho, mass density is set to zero.
    // This element assumes lumped mass for mass matrix.
    Elastic2dGNL(int tag, double A, double E, double I, int Nd1, int Nd2, bool islinear = false, double rho = 0.0);
    Element *getCopy(void) const;
  
    // Prints the element info to std::ostream
    void Print(std::ostream &s, int flag =0) const;
  
    // Methods for sending and receiving the object over a ch
    int sendSelf(Communicator &);
    int recvSelf(const Communicator &);
  
  
  protected:
    // Implementation of pure virtual subclass methods

    // Get the elastic stiffness in local coordinates,
    // stored into K
    void getLocalStiff(Matrix &K) const;
    // Get the mass matrix in local coordinate system,
    // stored in M
    void getLocalMass(Matrix &M) const;
  };
} // end of XC namespace

#endif // !defined Elastic2dGNL

/*
 *
 * WARNING/ERROR format/representation
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Warnings are generally issued if the standard implementation of a
 * method may not be possible.  Further analysis may lead to incorrect
 * solution, even though it may be possible to continue.
 *           
 * Warnings
 * --------
 *                
 * WARNING (W_Level_ID) - Class::method(..) [ElementTag]
 * Short description ...
 *
 * Errors
 * ------
 *  
 * ERROR (E_ID) - Class::method(..) [ElementTag]
 * Short description ...
 *           
 * Analysis may be halted if an error is encountered, further
 * analysis will definitely be erroneous.
 *
 * (see UpdatedLagrangianBeam2D.h for details)
 */


