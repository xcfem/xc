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
                                                                        
// $Revision: 1.2 $
// $Date: 2003/02/14 23:00:57 $
// $Source: /usr/local/cvs/OpenSees/SRC/domain/load/beam_loads/Beam3dUniformLoad.h,v $
                                                                        
#ifndef Beam3dUniformLoad_h
#define Beam3dUniformLoad_h

// Written: fmk 

// Purpose: This file contains the class definition for Beam3dUniformLoad.

#include "domain/load/beam_loads/BeamUniformLoad.h"

namespace XC {
class FVector;

//! @ingroup ElemLoads
//
//! @brief Uniform load over 3D beams.
class Beam3dUniformLoad: public BeamUniformLoad
  {
  private:
    double wz;  // Transverse
    double tx;  // Torsor.
    static Vector data;
  protected:
    DbTagData &getDbTagData(void) const;
    int sendData(CommParameters &cp);
    int recvData(const CommParameters &cp);

  public:
    Beam3dUniformLoad(int tag, double wy, double wz, double wx, double tx,const ID &theElementTags);
    Beam3dUniformLoad(int tag, const Vector &Fxyz, double tx,const ID &theElementTags);
    Beam3dUniformLoad(int tag);    
    Beam3dUniformLoad(void);

    int getType(void);
    inline const double &Wx(void) const
      { return Axial; }
    inline const double &Wy(void) const
      { return Trans; }
    inline const double &Wz(void) const
      { return wz; }
    inline const double &Tx(void) const
      { return tx; }
    inline double getTransZComponent(void)
      { return wz; }
    inline void setTransZComponent(const double &d)
      { wz= d; }
    inline void setTorsor(const double &d)
      { tx= d; }
    inline double getTorsor(void)
      { return tx; }

    size_t getDimVectorFuerza(void) const;
    size_t getDimVectorMomento(void) const;
    const Matrix &getLocalForces(void) const;
    const Matrix &getLocalMoments(void) const;

    const Matrix &getAppliedSectionForces(const double &L,const Matrix &xi,const double &loadFactor) const;
    void addReactionsInBasicSystem(const double &,const double &,FVector &) const;
    void addFixedEndForcesInBasicSystem(const double &L,const double &loadFactor,FVector &) const;
    void addElasticDeformations(const double &L,const CrossSectionProperties3d &ctes_scc,const double &lpI,const double &lpJ,const double &loadFactor,FVector &v0);

    int sendSelf(CommParameters &);  
    int recvSelf(const CommParameters &);
    void Print(std::ostream &s, int flag =0);

  };
} // end of XC namespace

#endif

