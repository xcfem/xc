//----------------------------------------------------------------------------
//  programa XC; cálculo mediante el método de los elementos finitos orientado
//  a la solución de problemas estructurales.
//
//  Copyright (C)  Luis Claudio Pérez Tato
//
//  El programa deriva del denominado OpenSees <http://opensees.berkeley.edu>
//  desarrollado por el «Pacific earthquake engineering research center».
//
//  Salvo las restricciones que puedan derivarse del copyright del
//  programa original (ver archivo copyright_opensees.txt) este
//  software es libre: usted puede redistribuirlo y/o modificarlo 
//  bajo los términos de la Licencia Pública General GNU publicada 
//  por la Fundación para el Software Libre, ya sea la versión 3 
//  de la Licencia, o (a su elección) cualquier versión posterior.
//
//  Este software se distribuye con la esperanza de que sea útil, pero 
//  SIN GARANTÍA ALGUNA; ni siquiera la garantía implícita
//  MERCANTIL o de APTITUD PARA UN PROPÓSITO DETERMINADO. 
//  Consulte los detalles de la Licencia Pública General GNU para obtener 
//  una información más detallada. 
//
// Debería haber recibido una copia de la Licencia Pública General GNU 
// junto a este programa. 
// En caso contrario, consulte <http://www.gnu.org/licenses/>.
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
// Description: This file contains the class definition for Elastic2dGNL.

// Elastic2dGNL is a subclass of UpdatedLagrangianBeam2D, that can be 
// used to model 2d beam column elements with large deformation effects. 
// Most of the virtual Element methods have been implemented by the parent 
// class (see UpdatedLagrangianBeam2D.h for details, 
// including the recorder arguments)

#ifndef Elastic2dGNL_H
#define Elastic2dGNL_H

// List of included files
// UpdatedLagrangianBeam2D - parent class of this class
#include "domain/mesh/element/truss_beam_column/updatedLagrangianBeamColumn/UpdatedLagrangianBeam2D.h"

namespace XC {
//! \ingroup ElemBarra
//
//! @brief Elemento barra con formulación lagrangiana para problemas bidimensionales.
class Elastic2dGNL : public UpdatedLagrangianBeam2D  
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
    void Print(std::ostream &s, int flag =0);
  
    // Methods for sending and receiving the object over a ch
    int sendSelf(CommParameters &);
    int recvSelf(const CommParameters &);
  
  
  protected:
    // Implementation of pure virtual subclass methods

    // Get the elastic stiffness in local coordinates,
    // stored into K
    void getLocalStiff(Matrix &K) const;
    // Get the mass matrix in local coordinate system,
    // stored in M
    void getLocalMass(Matrix &M) const;
  };
} // fin namespace XC

#endif // !defined Elastic2dGNL

/*
 *
 * WARNING/ERROR format/representation
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Warnings are generally issued if the standard implementation of a
 * method may not be possible.  Further analysis may lead to incorrect
 * solution, eventhough it may be possible to continue.
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


