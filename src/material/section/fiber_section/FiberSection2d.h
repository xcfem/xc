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
                                                                        
// $Revision: 1.10 $
// $Date: 2003/03/04 00:48:16 $
// $Source: /usr/local/cvs/OpenSees/SRC/material/section/fiber_section/FiberSection2d.h,v $
                                                                        
// Written: fmk
// Created: 04/01
//
// Description: This file contains the class definition for 
// FiberSection2d.h. FiberSection2d provides the abstraction of a 
// 2d beam section discretized by fibers. The section stiffness and
// stress resultants are obtained by summing fiber contributions.

#ifndef FiberSection2d_h
#define FiberSection2d_h

#include <material/section/fiber_section/FiberSectionBase.h>
#include <material/section/repres/section/fiber_list.h>
#include <utility/matrix/Vector.h>
#include <utility/matrix/Matrix.h>

namespace XC {
class FiberSectionRepr;

//! @ingroup MATSCCFiberModel
//
//! @brief Fiber section model in a bi-dimensional space. Sections stiffness
//! and internal forces are obtained by addition of the fibers contribution.
class FiberSection2d: public FiberSectionBase
  {
  private:
// AddingSensitivity:BEGIN //////////////////////////////////////////
    int parameterID;
// AddingSensitivity:END ///////////////////////////////////////////
    friend class FiberPtrDeque;
    double get_strain(const double &y) const;
  protected:

  public:
    FiberSection2d(MaterialHandler *mat_ldr= nullptr); 
    FiberSection2d(int tag,MaterialHandler *mat_ldr= nullptr); 
    FiberSection2d(int tag,const fiber_list &fibers,MaterialHandler *mat_ldr= nullptr);

    int setInitialSectionDeformation(const Vector &deforms); 
    int setTrialSectionDeformation(const Vector &deforms); 
    virtual double getStrain(const double &y,const double &z= 0) const;

    virtual void setupFibers(void);

    const Matrix &getInitialTangent(void) const;

    int revertToLastCommit(void);    
    int revertToStart(void);

    SectionForceDeformation *getCopy(void) const;
    const ResponseId &getType (void) const;
    int getOrder(void) const;
    
    int sendSelf(CommParameters &);
    int recvSelf(const CommParameters &);
    void Print(std::ostream &s, int flag = 0);
	    
    int getResponse(int responseID, Information &info);

    Fiber *addFiber(Fiber &theFiber);
    Fiber *addFiber(int tag,const MaterialHandler &,const std::string &nmbMat,const double &, const Vector &position);

    // AddingSensitivity:BEGIN //////////////////////////////////////////
    int setParameter(const std::vector<std::string> &argv, Parameter &param);
    int updateParameter(int parameterID, Information &info);
    int activateParameter(int parameterID);
    const Vector & getStressResultantSensitivity(int gradNumber, bool conditional);
    const Vector & getSectionDeformationSensitivity(int gradNumber);
    const Matrix & getSectionTangentSensitivity(int gradNumber);
    int   commitSensitivity(const Vector& sectionDeformationGradient, int gradNumber, int numGrads);
    // AddingSensitivity:END ///////////////////////////////////////////
    inline virtual double getCenterOfMassZ(void) const
      { return 0.0; }

  };

FiberSection2d FiberSectionReprToFiberSection2d(const int &tag,const FiberSectionRepr &fiberSectionRepr);
} // end of XC namespace

#endif
