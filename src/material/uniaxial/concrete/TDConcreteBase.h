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
   

#ifndef TDConcreteBase_h
#define TDConcreteBase_h

#include "material/uniaxial/concrete/RawConcrete.h"
#include "material/uniaxial/concrete/ConcreteHistoryVars.h"

namespace XC {

class TDConcreteBase : public RawConcrete
  {
  protected:
    // matpar : Concrete FIXED PROPERTIES
    //	double fcT; // Time Dependent Strength
    double ft; //!< concrete tensile strength               : mp(6)
    // double ftT;	//Time dependent strength (tension)
    double Ets; //!< tension stiffening slope                : mp(7)
    double Ec;  //Concrete stiffness, Added by AMK
    // double EcT; //Time dependent stiffness
    //double sigCr; //!< stress that creep curve is based on
    double beta; //!< tension softening parameter.

    // hstvP : Concerete HISTORY VARIABLES last committed step
    CreepConcreteHistoryVars hstvP; //!< = values at previous converged step

    // hstv : Concerete HISTORY VARIABLES  current step
    CreepConcreteHistoryVars hstv; //!< = values at current step (trial values)
	
    //Added by AMK:
    int iter; //Iteration number
    
    void Compr_Envlp(double epsc, double &sigc, double &Ect);
  protected:
    int sendData(Communicator &);
    int recvData(const Communicator &);    
  public:
    TDConcreteBase(int tag, int classTag);
    TDConcreteBase(int tag, int classTag, double _fc, double _ft, double _Ets, double _Ec, double _beta);
    void setup_parameters(void);

    double getInitialTangent(void) const;

    bool needsUpdate(void) const;

    double getCurrentTime(void) const; //Added by AMK
    double getStress(void) const;
    double getTangent(void) const;
    void setFt(const double &);
    double getFt(void) const;
    void setEts(const double &);
    double getEts(void) const;
    void setEc(const double &);
    double getEc(void) const;
    void setBeta(const double &);
    double getBeta(void) const;
    
    //! @brief Assign current concrete stiffness.
    virtual void setEt(const double &)= 0;
    //! @brief Returns current concrete stiffness.
    virtual double getEt(void) const= 0;
    
    //! @brief Assign the concrete age at first loading.
    virtual void setAge(const double &)= 0;
    //! @brief Return the concrete age at first loading.
    virtual double getAge(void) const= 0;
    
    //! @brief Assign the analysis time corresponding to concrete casting in days.
    virtual void setTCast(const double &)= 0;
    //! @brief Return the analysis time corresponding to concrete casting in days.
    virtual double getTCast(void) const= 0;
    
    int sendSelf(Communicator &);  
    int recvSelf(const Communicator &);
    
    void Print(std::ostream &s, int flag =0) const;

    int getVariable(const std::string &variable, Information &) const;
	
	
    //Added by AMK for recording Creep and Mechanical Strain:
    Response *setResponse(const std::vector<std::string> &, Information &);

  };

} // end of XC namespace

#endif

