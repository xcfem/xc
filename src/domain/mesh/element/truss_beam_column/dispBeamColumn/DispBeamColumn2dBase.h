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
                                                                        
// $Revision: 1.14 $
// $Date: 2003/03/04 00:48:14 $
// $Source: /usr/local/cvs/OpenSees/SRC/element/dispBeamColumn/DispBeamColumn2d.h,v $

// Written: MHS
// Created: Feb 2001
//
// Description: This file contains the class definition for DispBeamColumn2d.
// The element displacement field gives rise to constant axial strain and
// linear curvature.

#ifndef DispBeamColumn2dBase_h
#define DispBeamColumn2dBase_h

#include <domain/mesh/element/truss_beam_column/BeamColumnWithSectionFDTrf2d.h>
#include "domain/mesh/element/utils/fvectors/FVectorBeamColumn2d.h"
#include "utility/matrix/Matrix.h"
#include "utility/matrix/Vector.h"

namespace XC {
class Node;
class PrismaticBarCrossSection;
class CrdTransf2d;
class Response;
class BeamIntegration;

//! @ingroup OneDimensionalElem
//
//! @brief displacement based 2D beam element with PrismaticBarCrossSection type material.
class DispBeamColumn2dBase: public BeamColumnWithSectionFDTrf2d
  {
  private:
    void free_mem(void);
    void alloc(const BeamIntegration &);
  protected:
    BeamIntegration *beamIntegration; //!< Integration along the beam length.
    mutable Vector q; //!< Basic force
    FVectorBeamColumn2d q0; //!< Fixed end forces in basic system
    FVectorBeamColumn2d p0; //!< Reactions in basic system

    // AddingSensitivity:BEGIN //////////////////////////////////////////
    int parameterID;
    // AddingSensitivity:END ///////////////////////////////////////////

    static Matrix K;		// Element stiffness, damping, and mass Matrix
    static Vector P;		// Element resisting force vector

    static double workArea[];

    int sendData(CommParameters &cp);
    int recvData(const CommParameters &cp);
  public:
    DispBeamColumn2dBase(int tag, int classTag, int nd1, int nd2,
		         int numSections,const std::vector<PrismaticBarCrossSection *> &s,
			 const BeamIntegration &bi, const CrdTransf2d &coordTransf, double rho = 0.0);
    DispBeamColumn2dBase(int tag= 0, int classTag= 0);
    DispBeamColumn2dBase(int tag, int classTag, int numSec,const Material *theSection,const BeamIntegration *bi, const CrdTransf *coordTransf);
    DispBeamColumn2dBase(const DispBeamColumn2dBase &);
    DispBeamColumn2dBase &operator=(const DispBeamColumn2dBase &);
    ~DispBeamColumn2dBase(void);

    int getNumDOF(void) const;
    void setDomain(Domain *theDomain);

    // public methods to set the state of the element    
    int commitState(void);
    int revertToLastCommit(void);
    int revertToStart(void);

    // public methods to obtain stiffness, mass, damping and residual information    
    const Matrix &getMass(void) const;

    void zeroLoad();
    int addLoad(ElementalLoad *theLoad, double loadFactor);
    int addInertiaLoadToUnbalance(const Vector &accel);

    const Vector &getResistingForceIncInertia(void) const;            

    // public methods for element output
    int sendSelf(CommParameters &);
    int recvSelf(const CommParameters &);
    void Print(std::ostream &s, int flag =0) const;

  };
} // end of XC namespace

#endif

