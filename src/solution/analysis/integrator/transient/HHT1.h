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
                                                                        
// $Revision: 1.4 $
// $Date: 2003/02/14 23:00:48 $
// $Source: /usr/local/cvs/OpenSees/SRC/analysis/integrator/HHT1.h,v $
                                                                        
                                                                        
#ifndef HHT1_h
#define HHT1_h

// Written: fmk 
//
// Description: This file contains the class definition for HHT1.
// HHT1 is an algorithmic class for performing a transient analysis
// using the HHT1 integration scheme.
//
// What: "@(#) HHT1.h, revA"

#include "DampingFactorsIntegrator.h"
#include "ResponseQuantities.h"

namespace XC {

//! @ingroup TransientIntegrator
//
//! @brief The three parameter Hilbert-Hughes-Taylor time-stepping method.
class HHT1: public DampingFactorsIntegrator
  {
  private:
    double alpha;
    double gamma;
    double beta;

    double c1, c2, c3; //!< some constants we need to keep
    ResponseQuantities Ut; //!< response quantities at time t
    ResponseQuantities U; //!< response quantities at time t + delta t
    ResponseQuantities Ualpha; //!< response quantities at time t+alpha delta t
  protected:
    int sendData(CommParameters &);
    int recvData(const CommParameters &);

    friend class AnalysisAggregation;
    HHT1(AnalysisAggregation *);
    HHT1(AnalysisAggregation *,double alpha);
    HHT1(AnalysisAggregation *,double alpha,const RayleighDampingFactors &rF);        
    Integrator *getCopy(void) const;
  public:
    // methods which define what the FE_Element and DOF_Groups add
    // to the system of equation object.
    int formEleTangent(FE_Element *theEle);
    int formNodTangent(DOF_Group *theDof);        

    int domainChanged(void);    
    int initialize(void);        
    int newStep(double deltaT);    
    int update(const Vector &deltaU);

    int commit(void);

    virtual int sendSelf(CommParameters &);
    virtual int recvSelf(const CommParameters &);

    void Print(std::ostream &s, int flag =0) const;        
  };
inline Integrator *HHT1::getCopy(void) const
  { return new HHT1(*this); }
} // end of XC namespace

#endif

