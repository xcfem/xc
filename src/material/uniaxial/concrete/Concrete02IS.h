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
// $Date: 2007-06-08 00:38:39 $
// $Source: /usr/local/cvs/OpenSees/SRC/material/uniaxial/Concrete02IS.h,v $
                                                                      
// Written: fmk
// Created: 03/06
//
// Description: This file contains the class definition for 
// Concrete02IS. Concrete02IS is based on an f2c of the FEDEAS material
// Concr2.f which is:
/*-----------------------------------------------------------------------
! concrete model with damage modulus    
!       by MOHD YASSIN (1993)
! adapted to FEDEAS material library
! by D. Sze and Filip C. Filippou in 1994
! Modified by Nasser A. Marafi (2018) so that the concrete initial stiffness
! can be specified. 
-----------------------------------------------------------------------*/



#ifndef Concrete02IS_h
#define Concrete02IS_h

#include "material/uniaxial/Concrete02.h"

namespace XC {

class Concrete02IS : public Concrete02
  {
  private:
    // matpar : Concrete FIXED PROPERTIES
    // double fc --> fpc // concrete compression strength           : mp(1)
    // double fcu --> fpcu // stress at ultimate (crushing) strain    : mp(3)
    double E0; // Initial Stiffness

    void Compr_Envlp(double epsc, double &sigc, double &Ect);

  public:
    Concrete02IS(int tag= 0);
    Concrete02IS(int tag, double _E0, double _fc, double _epsc0, double _fcu,
		 double _epscu, double _rat, double _ft, double _Ets);
    Concrete02IS(int tag, double _E0, double _fc, double _epsc0, double _fcu,
		 double _epscu);  

    virtual ~Concrete02IS(void);

    double getInitialTangent(void);
    UniaxialMaterial *getCopy(void);

    int sendSelf(Communicator &);  
    int recvSelf(const Communicator &);    
    
    int getVariable(const std::string &variable, Information &) const;
  };

} // end of XC namespace

#endif

