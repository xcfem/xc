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
                                                                        
// $Revision: 1.5 $
// $Date: 2003/02/25 23:33:40 $
// $Source: /usr/local/cvs/OpenSees/SRC/material/uniaxial/SeriesMaterial.h,v $

#ifndef SeriesMaterial_h
#define SeriesMaterial_h

// Written: MHS
// Created: Sept 2000
//
// Description: This file contains the class definition for 
// SeriesMaterial. SeriesMaterial is an aggregation
// of UniaxialMaterial objects all considered acting in Series.

#include "ConnectedMaterial.h"

namespace XC {
//! @ingroup MatUnx
//
//! @brief Serially connected uniaxial materials.
class SeriesMaterial : public ConnectedMaterial
  {
  private:
    double Tstrain;
    double Cstrain;

    double Tstress;
    double Cstress;

    double Ttangent;
    double Ctangent;

    int maxIterations;
    double tolerance;

    std::vector<double> stress;
    std::vector<double> flex;
    std::vector<double> strain;

    bool initialFlag;

  protected:
    int sendData(CommParameters &);
    int recvData(const CommParameters &);
  public:
    SeriesMaterial(int tag,const DqUniaxialMaterial &theMaterials, int maxIter = 1, double tol = 1.0e-10);
    SeriesMaterial(int tag= 0);


    int setTrialStrain(double strain, double strainRate = 0.0); 
    double getStrain(void) const;          
    double getStress(void) const;
    double getTangent(void) const;
    double getInitialTangent(void) const;

    int commitState(void);
    int revertToLastCommit(void);    
    int revertToStart(void);        

    UniaxialMaterial *getCopy(void) const;
    
    int sendSelf(CommParameters &);  
    int recvSelf(const CommParameters &);
    
    void Print(std::ostream &s, int flag =0) const;

    Response *setResponse(const std::vector<std::string> &argv, Information &matInformation);
    int getResponse(int responseID, Information &matInformation);
  };
} // end of XC namespace


#endif

