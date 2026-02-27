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
#include "material/uniaxial/concrete/MC10CreepSteps.h"

namespace XC {
  
class TDConcreteMC10Base : public TDConcreteBase
  {
  protected:
    // matpar : Concrete FIXED PROPERTIES
    double Ecm; //! 28-day modulus, necessary for normalizing creep coefficient.
    double epsba; //!< ultimate basic shrinkage strain, εcbs,0, as per Model Code 2010
    double epsbb; //!< fitting parameter within the basic shrinkage time evolution function as per Model Code 2010 and prEN1992-1-1:2017.
    double epsda; //!< product of εcds,0 and βRH, as per Model Code 2010.
    double epsdb; //!< fitting parameter within the drying shrinkage time evolution function as per Model Code 2010 and prEN1992-1-1:2017.
    double phiba; //!< parameter for the effect of compressive strength on basic creep βbc(fcm), as per Model Code 2010.
    double phibb; //!< fitting parameter within the basic creep time evolution function as per Model Code 2010 and prEN1992-1-1:2017.
    double phida; //!< product of βdc(fcm) and β(RH), as per Model Code 2010.
    double phidb; //!< fitting constant within the drying creep time evolution function as per Model Code 2010.
    double cem; //!< coefficient dependent on the type of cement: –1 for 32.5N, 0 for 32.5R and 42.5N and 1 for 42.5R, 52.5N and 52.5R.

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
    
    MC10CreepSteps creepSteps;
  protected:
    int sendData(Communicator &);
    int recvData(const Communicator &);
  public:
    TDConcreteMC10Base(int tag, int classTag);
    TDConcreteMC10Base(int tag, int classTag, double _fc, double _ft, double _Ec, double _Ecm, double _beta, double _age, double _epsba, double _epsbb, double _epsda, double _epsdb, double _phiba, double _phibb, double _phida, double _phidb, double _tcast, double _cem);
    void setup_parameters(void);

    double getEcm(void) const;
    void setEcm(const double &);

    double getEpsba(void) const;
    void setEpsba(const double &);
    double getEpsbb(void) const;
    void setEpsbb(const double &);
    double getEpsda(void) const;
    void setEpsda(const double &);
    double getEpsdb(void) const;
    void setEpsdb(const double &);

    double getPhiba(void) const;
    void setPhiba(const double &);
    double getPhibb(void) const;
    void setPhibb(const double &);
    double getPhida(void) const;
    void setPhida(const double &);
    double getPhidb(void) const;
    void setPhidb(const double &);

    double getCem(void) const;
    void setCem(const double &);
    
    double getPHIB_i(void) const; //Added by AMK //ntosic: split into basic and drying creep
    double getPHID_i(void) const; //Added by AMK //ntosic: split into basic and drying creep
    double getCreepBasic(void) const; //Added by AMK //ntosic: split into basic and drying creep
    double getCreepDrying(void) const; //Added by AMK //ntosic: split into basic and drying creep
    double setPhiBasic(double time, double tp) const; //Added by AMK //ntosic: split into basic and drying creep
    double setPhiDrying(double time, double tp) const; //Added by AMK //ntosic: split into basic and drying creep
    double setShrinkBasic(double time); //Added by AMK //ntosic: split into basic and drying shrinkage
    double setShrinkDrying(double time); //Added by AMK //ntosic: split into basic and drying shrinkage
    double getShrinkBasic(void) const; //Added by AMK //ntosic: split into basic and drying
    double getShrinkDrying(void) const; //Added by AMK //ntosic: split into basic and drying
    
    double setCreepBasicStrain(double time, double stress); //Added by AMK //ntosic: split into basic and drying creep
    double setCreepDryingStrain(double time, double stress); //Added by AMK //ntosic: split into basic and drying creep
    
    int revertToLastCommit(void);    
    int revertToStart(void);        
    
    int sendSelf(Communicator &);  
    int recvSelf(const Communicator &);
    
  };

} // end of XC namespace

#endif

