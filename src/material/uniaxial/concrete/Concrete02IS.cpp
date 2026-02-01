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
// $Source: /usr/local/cvs/OpenSees/SRC/material/uniaxial/Concrete02IS.cpp,v $
                                                                      
// Written: fmk
// Created: 03/06
//
// Description: This file contains the class implementation of Concrete02IS. 
// This Concrete02IS is based on an f2c of the FEDEAS material
// Concr2.f which is:
//-----------------------------------------------------------------------
// concrete model with damage modulus    
//       by MOHD YASSIN (1993)
// adapted to FEDEAS material library
// by D. Sze and Filip C. Filippou in 1994
// Modified by Nasser A. Marafi (2018) so that the concrete initial stiffness
// can be specified. 
//-----------------------------------------------------------------------


#include <stdlib.h>
#include <string.h>
#include <math.h>

#include <material/uniaxial/concrete/Concrete02IS.h>
#include "domain/mesh/element/utils/Information.h"
#include <float.h>

//! @brief Default constructor.
XC::Concrete02IS::Concrete02IS(int tag)
  : Concrete02(tag, MAT_TAG_Concrete02IS), E0(0.0)
  {}

//! @brief Constructor.
XC::Concrete02IS::Concrete02IS(int tag, double _E0, double _fc, double _epsc0,
			       double _fcu, double _epscu, double _rat,
			       double _ft, double _Ets)
  : Concrete02(tag, MAT_TAG_Concrete02IS, _fc, _epsc0, _fcu, _epscu, _rat, _ft,
	       _Ets), E0(_E0)
  {}

//! @brief Constructor.
XC::Concrete02IS::Concrete02IS(int tag, double _E0, double _fc, double _epsc0,
			       double _fcu, double _epscu)
  : Concrete02(tag, MAT_TAG_Concrete02IS, _fc, _epsc0, _fcu, _epscu, 0.1, 0.1*fabs(_fc), 0.1*fabs(_fc)/_epsc0), E0(_E0)
  {}

//! @brief Constructor.
XC::UniaxialMaterial *XC::Concrete02IS::getCopy(void)
  { return new Concrete02IS(*this); }

double XC::Concrete02IS::getInitialTangent(void) const
  {
    //return 2.0*fpc/epsc0; // Marafi Change 2018/01/31
    return E0;
  }

//! @brief Assigns concrete compressive strength.
void XC::Concrete02IS::setInitialTangent(const double &d)
  {
    this->E0= d;
    this->setup_parameters();
  }

//! @brief Send object members through the communicator argument.
int XC::Concrete02IS::sendData(Communicator &comm)
  {
    int res= Concrete02::sendData(comm);
    res+= comm.sendDouble(E0,getDbTagData(),CommMetaData(6));
    return res;
  }

//! @brief Receives object members through the communicator argument.
int XC::Concrete02IS::recvData(const Communicator &comm)
  {
    int res= Concrete02::recvData(comm);
    res+= comm.receiveDouble(E0,getDbTagData(),CommMetaData(6));
    return res;
  }

int XC::Concrete02IS::sendSelf(Communicator &comm)
  {
    setDbTag(comm);
    const int dataTag= getDbTag();
    inicComm(7);
    int res= sendData(comm);

    res+= comm.sendIdData(getDbTagData(),dataTag);
    if(res < 0)
      std::cerr << getClassName() << "::" << __FUNCTION__
		<< "; failed to send data\n";
    return res;
  }

int XC::Concrete02IS::recvSelf(const Communicator &comm)
  {
    inicComm(7);
    const int dataTag= getDbTag();
    int res= comm.receiveIdData(getDbTagData(),dataTag);

    if(res<0)
      std::cerr << getClassName() << "::" << __FUNCTION__
		<< "; failed to receive ids.\n";
    else
      {
        //setTag(getDbTagDataPos(0));
        res+= recvData(comm);
        if(res<0)
          std::cerr << getClassName() << "::" << __FUNCTION__
		    << "; failed to receive data.\n";
      }
    return res;
  }


void XC::Concrete02IS::Compr_Envlp(double epsc, double &sigc, double &Ect) 
    {
    /*-----------------------------------------------------------------------
    ! monotonic envelope of concrete in compression (negative envelope)
    !
    !   fpc    = concrete compressive strength
    !   epsc0 = strain at concrete compressive strength
    !   fcu   = stress at ultimate (crushing) strain 
    !   epscu = ultimate (crushing) strain
    !   Ec0   = initial concrete tangent modulus
    !   epsc  = strain
    !
    !   returned variables
    !   sigc  = current stress
    !   Ect   = tangent concrete modulus
    -----------------------------------------------------------------------*/

      const double Ec0= this->getInitialTangent();

      //double ratLocal = epsc/epsc0;
      if (epsc>=epsc0)
	{
	  // Modify this to use Popovics Prepeak Curve
	  const double Esec= this->fpc/epsc0;
	  double r = 0.0;
	  if (Esec >= Ec0)
	    { r = 400.0; }
	  else
	    { r = Ec0 / (Ec0 - Esec); }
	  const double eta = epsc / epsc0;
	  sigc= fpc * eta*r / (r - 1 + pow(eta, r));
	  Ect = fpc * r*(r - 1)*(1 - pow(eta, r)) / (pow((r - 1 + pow(eta, r)), 2)*epsc0);
	  //sigc = fc*ratLocal*(2.0-ratLocal); // Marafi Change 2018/01/31
	  //Ect  = Ec0*(1.0-ratLocal); // Marafi Change 2018/01/31
	}
      else
	{
	  //   linear descending branch between epsc0 and epscu
	  if(epsc>epscu)
	    {
	      sigc = (this->fpcu-this->fpc)*(epsc-epsc0)/(epscu-epsc0)+this->fpc;
	      Ect  = (this->fpcu-this->fpc)/(epscu-epsc0);
	    }
	  else
	    {
	      // flat friction branch for strains larger than epscu
	      sigc = this->fpcu;
	      Ect  = 1.0e-10;
	      //       Ect  = 0.0
	    }
	}
    }

int XC::Concrete02IS::getVariable(const std::string &varName, Information &theInfo) const
  {
    if(varName=="ec")
      {
        theInfo.theDouble = epsc0;
        return 0;
      }
    else
      return -1;
  }
