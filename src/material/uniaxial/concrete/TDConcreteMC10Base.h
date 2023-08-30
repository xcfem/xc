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
   
#ifndef TDConcreteMC10Base_h
#define TDConcreteMC10Base_h

#include "material/uniaxial/concrete/TDConcreteBase.h"

namespace XC {
  
class TDConcreteMC10Base : public TDConcreteBase
  {
  protected:
    // matpar : Concrete FIXED PROPERTIES
    double fc; //!< concrete compression strength           : mp(1)
    double epsc0; //!< strain at compression strength          : mp(2)
    double epscu; //!< ultimate (crushing) strain              : mp(4)       
    double Ecm; //! 28-day modulus, necessary for normalizing creep coefficient
    double epsba; //ntosic
    double epsbb; //ntosic
    double epsda; //ntosic
    double epsdb; //ntosic
    double phiba; //ntosic
    double phibb; //ntosic
    double phida; //ntosic
    double phidb; //ntosic
    double cem; //ntosic

    // hstv : Concrete HISTORY VARIABLES  current step
	
    //Added by AMK:
    double eps_crb; //!< split into basic and drying creep (ntosic)
    double eps_crd; //!< split into basic and drying creep (ntosic)
    double eps_shb; //!< split into basic and drying shrinkage (ntosic)
    double eps_shd; //!< split into basic and drying shrinkage (ntosic)
    
    double epsP_crb; //!< split into basic and drying creep (ntosic)
    double epsP_crd; //!< split into basic and drying creep (ntosic)
    double epsP_shb; //!< split into basic and drying shrinkage (ntosic)
    double epsP_shd; //!< split into basic and drying shrinkage (ntosic)
    
    double phib_i; //!< split into basic and drying creep (ntosic)
    double phid_i; //!< split into basic and drying creep (ntosic)
    
    std::vector<float> PHIB_i; //!< split into basic and drying creep (ntosic)
    std::vector<float> PHID_i; //!< split into basic and drying creep (ntosic)

  protected:
    int sendData(Communicator &);
    int recvData(const Communicator &);
    size_t resize(void);
  public:
    TDConcreteMC10Base(int tag, int classTag);
    TDConcreteMC10Base(int tag, int classTag, double _fc, double _ft, double _Ec, double _Ecm, double _beta, double _age, double _epsba, double _epsbb, double _epsda, double _epsdb, double _phiba, double _phibb, double _phida, double _phidb, double _tcast, double _cem);
    void setup_parameters(void);

    double getPHIB_i(void) const; //Added by AMK //ntosic: split into basic and drying creep
    double getPHID_i(void) const; //Added by AMK //ntosic: split into basic and drying creep
    double getCreepBasic(void) const; //Added by AMK //ntosic: split into basic and drying creep
    double getCreepDrying(void) const; //Added by AMK //ntosic: split into basic and drying creep
    double setPhiBasic(double time, double tp); //Added by AMK //ntosic: split into basic and drying creep
    double setPhiDrying(double time, double tp); //Added by AMK //ntosic: split into basic and drying creep
    double setShrinkBasic(double time); //Added by AMK //ntosic: split into basic and drying shrinkage
    double setShrinkDrying(double time); //Added by AMK //ntosic: split into basic and drying shrinkage
    double getShrinkBasic(void) const; //Added by AMK //ntosic: split into basic and drying
    double getShrinkDrying(void) const; //Added by AMK //ntosic: split into basic and drying
    
    int revertToLastCommit(void);    
    int revertToStart(void);        
    
    int sendSelf(Communicator &);  
    int recvSelf(const Communicator &);
    
  };

} // end of XC namespace

#endif

