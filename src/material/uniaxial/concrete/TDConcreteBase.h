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
    double age; //!< concrete age at first loading, Added by AMK
    //double sigCr; //!< stress that creep curve is based on
    double beta; //!< tension softening parameter
    double tcast; //!< the analysis time corresponding to concrete casting in days

    // hstvP : Concerete HISTORY VARIABLES last committed step
    double ecminP; //!<  hstP(1)
    double ecmaxP; //!< added by AMK
    double deptP; //!<  hstP(2)
    double epsP; //!<  = strain at previous converged step
    double sigP; //!<  = stress at previous converged step
    double eP; //!<   stiffness modulus at last converged step;

    // hstv : Concerete HISTORY VARIABLES  current step
    double ecmin;
    double ecmax; //!< added by AMK  
    double dept;   
    double sig;   
    double e;     
    double eps;
	
    //Added by AMK:
    int count;
    double epsInit;
    double sigInit;
    double eps_T;
    double eps_m;
    double epsP_m;
    double eps_total;
    double epsP_total;
    double e_total;
    double eP_total;
    double t; //Time
    double t_load; //loaded time
    double Et;
    int crack_flag;
    int crackP_flag;
    int iter; //Iteration number
    
    std::vector<float> E_i;
    std::vector<float> DSIG_i;
    std::vector<float> dsig_i;
    std::vector<float> TIME_i; //Time from the previous time step
    std::vector<float> DTIME_i;

    static int creepControl; //!< Controls creep calculation (see setTrialStrain).
    static double creepDt; 

    void Compr_Envlp (double epsc, double &sigc, double &Ect);    
  protected:
    int sendData(Communicator &);
    int recvData(const Communicator &);
    size_t resize(void);
    
  public:
    TDConcreteBase(int tag, int classTag);
    TDConcreteBase(int tag, int classTag, double _fc, double _ft, double _Ec, double _beta, double _age, double _tcast);
    void setup_parameters(void);

    double getInitialTangent(void) const;

    bool needsUpdate(void) const;

    double getCurrentTime(void) const; //Added by AMK
    double getStrain(void) const;
    double getStress(void) const;
    double getTangent(void) const;
    double getMech(void) const; //Added by AMK
    void setFt(const double &);
    double getFt(void) const;
    void setEc(const double &);
    double getEc(void) const;
    void setEt(const double &);
    double getEt(void) const;
    void setBeta(const double &);
    double getBeta(void) const;
    void setAge(const double &);
    double getAge(void) const;
    void setTCast(const double &);
    double getTCast(void) const;
    
    int sendSelf(Communicator &);  
    int recvSelf(const Communicator &);
    
    void Print(std::ostream &s, int flag =0) const;

    int getVariable(const std::string &variable, Information &) const;
	
	
    //Added by AMK for recording Creep and Mechanical Strain:
    Response *setResponse(const std::vector<std::string> &, Information &);

    static void setCreepOn(void);
    static void setCreepOff(void);
    static bool isCreepOn(void);
    static void setCreepDt(const double &);
    static double getCreepDt(void);
  };

  inline void set_creep_on(void)
    { TDConcreteBase::setCreepOn(); }
  inline void set_creep_off(void)
    { TDConcreteBase::setCreepOff(); }
  inline void set_creep_dt(const double &d)
    { TDConcreteBase::setCreepDt(d); }
  inline double get_creep_dt(void)
    { return TDConcreteBase::getCreepDt(); }

} // end of XC namespace

#endif

