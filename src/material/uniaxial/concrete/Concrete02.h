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
// $Date: 2006/08/03 23:42:19 $
// $Source: /usr/local/cvs/OpenSees/SRC/material/uniaxial/Concrete02.h,v $
                                                                      
// Written: fmk
// Created: 03/06
//
// Description: This file contains the class definition for 
// Concrete02. Concrete02 is based on an f2c of the FEDEAS material
// Concr2.f which is:
/*-----------------------------------------------------------------------
! concrete model with damage modulus    
!       by MOHD YASSIN (1993)
! adapted to FEDEAS material library
! by D. Sze and Filip C. Filippou in 1994
-----------------------------------------------------------------------*/



#ifndef Concrete02_h
#define Concrete02_h

#include <material/uniaxial/concrete/RawConcrete.h>

namespace XC {

//! @ingroup MatUnx
//
//! @brief Concrete02 history variables.
struct Conc02HistoryVars
  {
    double ecmin;  //!<  hstP(1)
    double dept;   //!<  hstP(2)
    double eps;  //!< strain
    double sig;  //!< stress
    double e;    //!< stiffness modulus
    inline Conc02HistoryVars(void)
      : ecmin(0.0), dept(0.0), eps(0.0), sig(0.0), e(0.0) {}
    inline void setup_parameters(const double &initialTangent)
      {
	e= initialTangent;
        eps= 0.0;
        sig= 0.0;
      }
    inline double getStrain(void) const
      { return eps; }
    inline double getStress(void) const
      { return sig; }
    inline double getTangent(void) const
      { return e; }
    void cutStress(const double &sigmin,const double &sigmax,const double &er)
      {
        if(sig <= sigmin)
          {
            sig= sigmin;
            e= er;
          }
        else if(sig >= sigmax)
	  {
            sig= sigmax;
            e= 0.5 * er;
          }
      }
    void Print(std::ostream &os) const
      {
        os << "Concrete02:(strain, stress, tangent) " << eps
	  << ", " << sig << ", " << e << std::endl;
      }
  };

inline std::ostream &operator<<(std::ostream &os,const Conc02HistoryVars &hv)
   {
     hv.Print(os);
     return os;
   }

//! @ingroup MatUnx
//
//! @brief Modelización 1D del hormigón con módulo de daño.
class Concrete02: public RawConcrete
  {
  private:

    // matpar : Concrete FIXED PROPERTIES
    double fpcu;   //!< stress at ultimate (crushing) strain    : mp(3)
    double rat;   //!< ratio between unloading slope at epscu and original slope : mp(5)
    double ft;    //!< concrete tensile strength               : mp(6)
    double Ets;   //!< tension stiffening slope                : mp(7)

    // hstvP : Concrete HISTORY VARIABLES last committed step
    Conc02HistoryVars hstvP; //!< = values at previous converged step
    // hstv : Concrete HISTORY VARIABLES  current step
    Conc02HistoryVars hstv; //!< = values at current step (trial values)

    void Tens_Envlp(double epsc, double &sigc, double &Ect);
    void Compr_Envlp(double epsc, double &sigc, double &Ect);
  protected:
    int sendData(CommParameters &);
    int recvData(const CommParameters &);
    void setup_parameters(void);
  public:
    Concrete02(int tag, double _fpc, double _epsc0, double _fpcu,
	     double _epscu, double _rat, double _ft, double _Ets);
    Concrete02(int tag= 0);
 
    void setFpcu(const double &);
    double getFpcu(void) const;
    void setFt(const double &);
    double getFt(void) const;
    void setEts(const double &);
    double getEts(void) const;
    void setLambda(const double &);
    double getLambda(void) const;

    inline double getInitialTangent(void) const
      { return 2.0*fpc/epsc0; }
    UniaxialMaterial *getCopy(void) const;

    int setTrialStrain(double strain, double strainRate = 0.0); 
    inline double getStrain(void) const
      { return hstv.getStrain(); }
    inline double getStress(void) const
      { return hstv.getStress(); }
    inline double getTangent(void) const
      { return hstv.getTangent(); }
    
    int commitState(void);
    int revertToLastCommit(void);    
    int revertToStart(void);
    
    int sendSelf(CommParameters &);  
    int recvSelf(const CommParameters &);    
    
    void Print(std::ostream &s, int flag =0);
  };
} // end of XC namespace


#endif

