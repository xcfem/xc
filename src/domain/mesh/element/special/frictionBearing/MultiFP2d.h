// -*-c++-*-
//----------------------------------------------------------------------------
//  XC program; finite element analysis code
//  for structural analysis and design.
//
//  Copyright (C)  Luis C. Pérez Tato
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
                                                                        
// $Revision$
// $Date$
// $Source$
                                                                        
#ifndef MultiFP2d_h
#define MultiFP2d_h

// Written: fmk
// Conversion from matlab to c++: fmk
//
// Description: This file contains the interface for the MultiFP2d class.
//
// What: "@(#) MultiFP2d.h, revA"

#include "domain/mesh/element/Element0D.h"
#include "utility/matrix/Vector.h"
#include "utility/matrix/Matrix.h"

namespace XC {
class UniaxialMaterial;

class MultiFP2d : public Element0D
  {
  private:
    // UniaxialMaterial *theVerticalModel;
    UniaxialMaterial *theFrictionModel;
    UniaxialMaterial *theVerticalModel;
  
    int type;
    int axialCase;
    double W0;   // original weight
    double cW;   // current vertical force (obtained form vertical spring)
  
    
    mutable Matrix theMatrix;
    mutable Vector theVector;  

  private:
    void free_friction_model(void);
    void alloc_friction_model(const UniaxialMaterial &);
    void free_vertical_model(void);
    void alloc_vertical_model(const UniaxialMaterial &);
    
  protected:    
    int sendData(Communicator &);
    int recvData(const Communicator &);
  public:
    // constructors
    MultiFP2d(int tag, 
	      int Nd1, int Nd2, 
	      const UniaxialMaterial &theFrictionModel,
	      const UniaxialMaterial &theVerticalModel,
	      double w0, int axialCase);

    MultiFP2d(int tag, 
	      int Nd1, int Nd2,
	      int type,
	      const Vector &R,
	      const Vector &h, 
	      const Vector &D,
	      const Vector &d,
	      const Vector &mu,
	      double Kvert,
	      double w0, int axialCase);
    MultiFP2d(const MultiFP2d &);
    MultiFP2d &operator=(const MultiFP2d &);
  
    MultiFP2d();    
  
    // destructor
    ~MultiFP2d();
  
    // public methods to obtain information about dof & connectivity
    void setDomain(Domain *theDomain);
  
    // public methods to set the state of the element    
    int commitState(void);
    int revertToLastCommit(void);        
    int revertToStart(void);        
    int update(void);
  
    // public methods to obtain stiffness
    const Matrix &getTangentStiff(void) const;
    const Matrix &getInitialStiff(void) const;
  
    // public method to obtain resisting force
    const Vector &getResistingForce(void) const;
  
    // public methods for output    
    int sendSelf(Communicator &);
    int recvSelf(const Communicator &);
    void Print(std::ostream &s, int flag = 0) const;    

    Response *setResponse(const std::vector<std::string> &argv, Information &eleInformation);
    int getResponse(int responseID, Information &eleInformation);
  };
} // end of XC namespace

#endif

