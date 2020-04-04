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
/* *********************************************************************
**    Module:	PyLiq1.h 
**
**    Purpose:	Provide a p-y material that gets pore pressure from a
**				specified element that contains a PorousFluidSolid.
**              
**
**    Developed by Ross W. Boulanger
**    (C) Copyright 2002, All Rights Reserved.
**
** ****************************************************************** */

// $Revision: 1.0
// $Date: 2002/5/15
// $Source: /OpenSees/SRC/material/uniaxial/PyLiq1.h

#ifndef PYLIQ1_H
#define PYLIQ1_H

// Written: RWB
// Created: May 2002
//
// Description: This file contains the class definition for PyLiq1.
// 

#include "PySimple1.h"

namespace XC{
  class FourNodeQuad;

//! @ingroup PYMat
//
//! @brief Uniaxial p-y material that incorporates liquefaction effects.
class PyLiq1: public PySimple1
  {
  private:
    // other parameters in PySimple1 base class
    double pRes; //!< Residual p.

    // Committed and trial values for p, y, and ru
    double Ty;
    double Cy;
    double Tp;
    double Cp;
    double Tangent;
    double maxTangent;
    double Tru;
    double Cru;
    double Hru;

    // Solid element from which pore pressures are obtained, domain pointer
    // and stage information to get the initial vertical effective stress.
    int solidElem1;
    int solidElem2;
    double meanConsolStress;
    static int loadStage;
    int lastLoadStage;
    std::string elemFlag;
    Domain *theDomain;
    FourNodeQuad *theQuad1;
    FourNodeQuad *theQuad2;

    // Initial tangent
    double initialTangent;
     
    // Function for obtaining effective stresses from adjoining solid soil elements
    double getEffectiveStress(void);
    static Vector stressV3;
  protected:
    int sendData(Communicator &);
    int recvData(const Communicator &);
  public:
    PyLiq1(int tag, int classtag, int soilType, double pult, double y50, double drag,
           double dashpot, double pRes, int solidElem1, int solidElem2, Domain *theDomain);
    PyLiq1(int tag, int classtag= MAT_TAG_PyLiq1);
    PyLiq1(void);

    int setTrialStrain(double y, double yRate); 
    double getStrain(void) const;          
    double getStress(void) const;
    double getTangent(void) const;
    double getStrainRate(void) const;
    double getDampTangent(void) const;
    double getInitialTangent(void) const;

    int commitState(void);
    int revertToLastCommit(void);    
    int revertToStart(void);        

    UniaxialMaterial *getCopy(void) const;
    
    int sendSelf(Communicator &);  
    int recvSelf(const Communicator &);

    //  Command for initiating vertConsolStress from TclUpdateMaterialStageCommand
    int updateParameter(int snum, Information &eleInformation);
    void Print(std::ostream &s, int flag =0) const;
  };
}// fin namespace
#endif // PYLIQ1_H
