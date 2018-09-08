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
                                                                        
// $Revision: 1.3 $
// $Date: 2003/02/14 23:00:57 $
// $Source: /usr/local/cvs/OpenSees/SRC/domain/load/beam_loads/Beam2dPointLoad.h,v $
                                                                        
#ifndef Beam2dPointLoad_h
#define Beam2dPointLoad_h

// Written: fmk 

// Purpose: This file contains the class definition for Beam2dPointLoad.

#include "domain/load/beam_loads/BeamPointLoad.h"

namespace XC {

class FVector;

//! @ingroup ElemLoads
//
//! @brief Punctual load over 2D beam elements.
class Beam2dPointLoad: public BeamPointLoad
  {
  private:
    static Vector data;
  protected:

  public:
    Beam2dPointLoad(int tag, double Pt, double x,const ID &theElementTags, double Pa = 0.0);
    Beam2dPointLoad(int tag);    
    Beam2dPointLoad(void);    

    inline const double &pTrans(void) const
      { return Trans; }
    inline const double &pAxial(void) const
      { return Axial; }
    const Vector &getData(int &type, const double &loadFactor) const;

    size_t getForceVectorDimension(void) const;
    size_t getMomentVectorDimension(void) const;
    virtual Vector getLocalForce(void) const;
    virtual Vector getLocalMoment(void) const;
    const Matrix &getLocalForces(void) const;
    const Matrix &getLocalMoments(void) const;

    const Matrix &getAppliedSectionForces(const double &L,const XC::Matrix &xi,const double &loadFactor) const;
    void addReactionsInBasicSystem(const double &,const double &,FVector &) const;
    void addFixedEndForcesInBasicSystem(const double &L,const double &loadFactor,FVector &) const;
    void addElasticDeformations(const double &L,const CrossSectionProperties2d &ctes_scc,const double &lpI,const double &lpJ,const double &loadFactor,FVector &v0);

    int sendSelf(CommParameters &);  
    int recvSelf(const CommParameters &);

    void Print(std::ostream &s, int flag =0);       

  };
} // end of XC namespace

#endif

