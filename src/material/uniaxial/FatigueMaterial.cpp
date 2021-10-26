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
                                                                        
// $Revision: 1.1 $
// $Date: 2003/08/14 20:23:50 $
// $Source: /usr/local/cvs/OpenSees/SRC/material/uniaxial/FatigueMaterial.cpp,v $
                                                      
// Written: Patxi
// Created: Aug 2003
//
// Description: This file contains the class definition for 
// FatigueMaterial.  FatigueMaterial wraps a XC::UniaxialMaterial
// and imposes fatigue limits.

#include <cstdlib>

#include <material/uniaxial/FatigueMaterial.h>
#include <utility/matrix/ID.h>
#include <cmath>

void XC::FatigueMaterial::free(void)
  {
    if(theMaterial)
      {
	delete theMaterial;
	theMaterial= nullptr;
      }
  }

void XC::FatigueMaterial::alloc(const UniaxialMaterial &material)
  {
    free();
    theMaterial= material.getCopy();
    if(!theMaterial)
      {
	std::cerr <<  getClassName() << "::" << __FUNCTION__
	          << "; failed to get copy of material\n";
	exit(-1);
      }
  }

XC::FatigueMaterial::FatigueMaterial(int tag, const UniaxialMaterial &material,
				 double dmax, double E_0, double slope_m, 
				 double epsmin, double epsmax)
  :UniaxialMaterial(tag,MAT_TAG_Fatigue), theMaterial(nullptr), 
   Cfailed(false), trialStrain(0.0)
  {
    DI  = 0; //Damage index
    X   = 0; //Range in consideration
    Y   = 0; //Previous Adjacent Range
    A   = 0; //Peak or valley 1
    B   = 0; //Peak or valley 2
    C   = 0; //Peak or valley 2
    D   = 0; //Peak or valley 4
    PCC = 0; /*Previous Cycle counter flag if >1 then previous 'n' 
	       cycles did not flag a complete cycle */
    R1F = 0; //Flag for first  peak count
    R2F = 0; //Flag for second peak count
    cSlope  = 0; //Current Slope
    PS  = 0; //Previous slope
    EP  = 0; //Previous Strain
    SF  = 0; /*Start Flag = 0 if very first strain, (i.e. when initializing)
	       = 1 otherwise */
    DL  = 0; //Damage if current strain was last peak.

    // added 6/9/2006
    SR1 = 0;
    NC1= 0;
    SR2 = 0;
    NC2 = 0;
    SR3 = 0;
    NC3 = 0;


    if ( dmax > 1.0 || dmax < 0.0 ) {
      std::cerr << "FatigueMaterial::FatigueMaterial " <<
	"- Dmax must be between 0 and 1, assuming Dmax = 1\n" ;
      Dmax    = 1;
    } else 
      Dmax    = dmax;

    E0        = E_0;
    m         = slope_m;
    minStrain = epsmin;
    maxStrain = epsmax;
    alloc(material);


    //added by SAJalali
    energy = 0;
    CStress = 0;
  }

XC::FatigueMaterial::FatigueMaterial(int tag)
  :UniaxialMaterial(tag,MAT_TAG_Fatigue), theMaterial(nullptr), 
   Cfailed(false), trialStrain(0)
  {
    DI  = 0; //Damage index
    X   = 0; //Range in consideration
    Y   = 0; //Previous Adjacent Range
    A   = 0; //Peak or valley 1
    B   = 0; //Peak or valley 2
    C   = 0; //Peak or valley 2
    D   = 0; //Peak or valley 4
    PCC = 0; /*Previous Cycle counter flag if >1 then previous 'n' 
	       cycles did not flag a complete cycle */
    R1F = 0; //Flag for first  peak count
    R2F = 0; //Flag for second peak count
    cSlope  = 0; //Current Slope
    PS  = 0; //Previous slope
    EP  = 0; //Previous Strain
    SF  = 0; /*Start Flag = 0 if very first strain, (i.e. when initializing)
	       = 1 otherwise */
    DL  = 0; //Damage if current strain was last peak.

    Dmax    = 0;
    E0      = 0; 
    m       = 0;
    minStrain    = 0;
    maxStrain    = 0;

    // added 6/9/2006
    SR1 = 0;
    NC1 = 0;
    SR2 = 0;
    NC2 = 0;
    SR3 = 0;
    NC3 = 0;

    //added by SAJalali
    energy = 0;
    CStress = 0;    
  }

//! @brief Copy constructor.
XC::FatigueMaterial::FatigueMaterial(const FatigueMaterial &other)
  : UniaxialMaterial(other)
  {
    if(other.theMaterial)
      alloc(*theMaterial);
    Dmax= other.Dmax;
    E0= other.E0;
    m= other.m;
    minStrain= other.minStrain;
    maxStrain= other.maxStrain;
    Cfailed= other.Cfailed;
    trialStrain= other.trialStrain;
  }

//! @bried Assignment operator.
XC::FatigueMaterial &XC::FatigueMaterial::operator=(const FatigueMaterial &other)
  {
    UniaxialMaterial::operator=(other);
    if(other.theMaterial)
      alloc(*theMaterial);
    Dmax= other.Dmax;
    E0= other.E0;
    m= other.m;
    minStrain= other.minStrain;
    maxStrain= other.maxStrain;
    Cfailed= other.Cfailed;
    trialStrain= other.trialStrain;
    return *this;
  }

//! @brief Virtual constructor.
XC::UniaxialMaterial *XC::FatigueMaterial::getCopy(void) const
  { return new FatigueMaterial(*this); }

XC::FatigueMaterial::~FatigueMaterial(void)
  { free(); }

static int sign(double a)
  {
    if (a < 0)
      return -1;
    else
      return 1;
  }

int XC::FatigueMaterial::setTrialStrain(double strain, double strainRate)
  {
      if (Cfailed) {
      trialStrain = strain;
      // return 0;
      return theMaterial->setTrialStrain(strain, strainRate);
    } else {
      Cfailed = false;
      trialStrain = strain;
      return theMaterial->setTrialStrain(strain, strainRate);
    }
  }

double XC::FatigueMaterial::getStress(void) const
  {
    // double modifier = 1.0;
    // double damageloc = 1.0-Dmax+DL;
    if (Cfailed)
      // Reduce stress to 0.0 
      return theMaterial->getStress()*1.0e-8;

    /*
    // This portion of the code was added by Kevin Mackie, Ph.D.
    //  This is appropriate for steel material.  Uncomment to use.
    else if ( damageloc <= 0.9 )
    modifier = 1.0-725.0/2937.0*pow(damageloc,2);
    else 
    modifier = 8.0*(1.0-damageloc);

    if (modifier <= 0)
      modifier = 1.0e-8;

    else
      return theMaterial->getStress()*modifier;
    */

    else
      return theMaterial -> getStress();
  }

//! @brief Return the material tangent stiffness.
double XC::FatigueMaterial::getTangent(void) const
  {
    double modifier = 1.0;
    // double damageloc = 1.0-Dmax+DL;
    if (Cfailed)
      // Reduce tangent to 0.0 
      return 1.0e-8*theMaterial->getInitialTangent();

    /* 
    // This portion of the code was added by Kevin Mackie, Ph.D.
    //  This is appropriate for steel material.  Uncomment to use.
    else if ( damageloc <= 0.9 )
    modifier = 1.0-725.0/2937.0*pow(damageloc,2);
    else
    modifier = 8.0*(1.0-damageloc);

    if (modifier <= 0)
    // modifier = 1.0e-8;
    modifier = 1.0e-3;
    */

    else
      return theMaterial->getTangent()*modifier;
  }

double XC::FatigueMaterial::getDampTangent(void) const
  {
    if(Cfailed)
      return 0.0;
    else
      return theMaterial->getDampTangent();
  }

double XC::FatigueMaterial::getStrain(void) const
  { return theMaterial->getStrain(); }

double XC::FatigueMaterial::getStrainRate(void) const
  { return theMaterial->getStrainRate(); }

//! @brief Commit the state of the material.
int XC::FatigueMaterial::commitState(void)
  {        
    // NOTE: Do not accumulate damage if peaks are too small (e.g. if X < 1e-10)
    // may get floating point errors.  This is essentially a filter for 
    // strain cycles smaller than 1e-10 strain.  
    //  THIS FATIGE MATERIAL CODE WAS NOT INTENDED FOR HIGH CYCLE FATIGUE, 
    //  THE LINEAR ACCUMULATION OF DAMAGE IS NOT AS APPROPRIATE FOR HIGH
    //  CYCLE FATIGUE. 

    //added 6/9/2006
    // for recorder
    SR1 = 0;
    NC1 = 0;


    // No need to continue if the uniaxial material copy 
    // has already failed.
    if (Cfailed) {
      return 0;
    }

    //Simple check to see if we reached max strain capacities
    if (trialStrain >= maxStrain || trialStrain <= minStrain) { 
	Cfailed = true;
	std::cerr << "FatigueMaterial: material tag " << this->getTag() << " failed from excessive strain\n";
	DI = Dmax;
	DL = Dmax;
	return 0;
    }

    //Initialize the fatigue parameters if they have 
    // not been initialized yet
    if (SF == 0) {

      A   = trialStrain;
      SF  = 1  ;
      EP  = trialStrain;
      // Initialize other params if not done so already
      PCC = 0;
      B   = 0;
      C   = 0;
      D   = 0;

    }

    /* Now we need to determine if we are at a peak or not 
       If we are, then we need to do some calcs to determine
       the amount of damage suffered. If we are not at a peak, we need to
       pretend like we are at a peak, so that we can calculate the damage
       as if it WERE a peak.   
    */

    // Determine the slope of the strain hysteresis
    if ( EP == trialStrain ) {
      cSlope = PS;         // No real slope here....
    } else {
      cSlope = trialStrain - EP;   // Determine Current Slope
    }


    // If we are at a peak or a valley, then check for damage
    if (sign(PS) != sign(cSlope) && sign(PS) != 0 ) {

      if ( R1F == 0 )  {    // mark second peak

	B = EP; 
	Y = fabs(B-A);
	R1F = 1;

      } else {   // start at least the third peak

	// begin modified Rainflow cycle counting
	if (PCC == 1) { 

	  D = EP;
	  X = fabs(D-C);

	} else {

	  C = EP;
	  X = fabs(C-B);

	}

	if (X < Y) {

	  PCC = PCC + 1;

	  if (PCC == 1) {
	    Y = fabs(C-B);
	  } else if (PCC == 2 ) {
	    // Count X = |D-C| as a 1.0 cycle
	    DI = DI + 1.0 / fabs(pow( (X/E0) , 1/m )) ;
	    //added 6/9/2006
	    SR1 = X;
	    NC1 = 1.0;
	    // Reset parameters
	    D = 0;
	    C = 0;
	    Y = fabs(B-A);
	    PCC = 0;

	  }

	} else {

	  if (PCC == 1 ) {

	    // Count Y = |C-B| as a 1.0 cycle
	    DI = DI + 1.0 / fabs(pow( (Y/E0) , 1/m ));
	    //added 6/9/2006
	    SR1 = Y;
	    NC1 = 1.0;

	    // Reset parameters
	    B = D;
	    C = 0;
	    D = 0;
	    Y = fabs(B-A);
	    PCC = 0;



	  } else {

	    // Count Y = |A-B| as a 0.5 cycle
	    DI = DI + 0.5 / fabs(pow( (Y/E0), 1/m ));

	    //added 6/9/2006
	    // For recorder
	    SR1 = Y;
	    NC1 = 0.5;

	    // Reset parameters
	    A = B;
	    B = C;
	    C = 0;
	    D = 0;
	    Y = X;
	    PCC = 0;


	  }

	}

      }

      // Flag failure if we have reached that point
      if (DI >= Dmax )  {
	// Most likely will not fail at this point, more 
	// likely at the pseudo peak. But this step is
	// is important for accumulating damage
	Cfailed = true;
	std::cerr << getClassName() << ": material tag " << this->getTag() << " failed at peak\n";
	DL=DI;
      } else {
	Cfailed = false;
	DL=DI;
      }

      // Modified by Patxi 3/5/2006
    // } else {
    }
    if (Cfailed == false) {

      // Now check for damage, although we may not be at a peak at all.
      // Store temporary damage only as if it were the last peak: DL
      // Commit to DI only if failure occurs.
      if (B == 0 && C == 0 &&  D == 0) {

	// If we have not yet found the second peak
	X = fabs(trialStrain - A);

	if (fabs(X) < 1e-10) {
	  DL = DI ;
	  // added 6/9/2006
	  //values for recorder
	  SR2 = 0.0;
	  NC2 = 0.0;
	  SR3 = 0.0;
	  NC3 = 0.0;
	} else {
	  DL = DI +  0.5 / fabs(pow( (X/E0), 1/m ));
	  // added 6/9/2006
	  //values for recorder
	  SR2 = X;
	  NC2 = 0.5;
	  SR3 = 0.0;
	  NC3 = 0.0;
	}

      } else if (B != 0 && C == 0 &&  D == 0) {

	// On our way to find point C. Range Y defined, no X yet
	X = fabs(trialStrain - B);

	if (fabs(X) < 1e-10) {
	  DL = DI;
	  // added 6/9/2006
	  //values for recorder
	  SR2 = 0.0;
	  NC2 = 0.0;
	} else {
	  DL = DI +  0.5 / fabs(pow( (X/E0) , 1/m ));
	  // added 6/9/2006
	  //values for recorder
	  SR2 = X;
	  NC2 = 0.5;
	}	

	if (fabs(Y) < 1e-10) {
	  DL = DL;
	  // added 6/9/2006
	  //values for recorder
	  SR3 = 0.0;
	  NC3 = 0.0;
	} else {
	  DL = DL +  0.5 / fabs(pow( (Y/E0) , 1/m ));
	  // added 6/9/2006
	  //values for recorder
	  SR3 = Y;
	  NC3 = 0.5;
	}

      } else if (B != 0 && C != 0 &&  D == 0) {

	// Two ranges stored, but no cycles for either stored
	// There are two scenarios that can result:
	// 1.)  |A-D| is the predominate 1/2 cycle, 
	//      and |B-C| is a small full cycle.
	//
	// 2.)  One full cylce at |D-C|, 1/2 cycle at |A-B|

	if (fabs(A-trialStrain)> fabs(A-B)) {

	  //   count 1/2 cycle at |D-A|, and one full cycle at |B-C|.
	  X = fabs(trialStrain-A);

	  if (fabs(Y) < 1e-10) {
	    DL = DI;
	    // added 6/9/2006
	    //values for recorder
	    SR3 = 0.0;
	    NC3 = 0.0;
	  } else {
	    DL = DI +  1.0 / fabs(pow( (Y/E0) , 1/m ));
	    // added 6/9/2006
	    //values for recorder
	    SR3 = Y;
	    NC3 = 1.0;
	  } 

	  if (fabs(X) < 1e-10) {
	    DL = DL;
	    // added 6/9/2006
	    //values for recorder
	    SR2 = 0.0;
	    NC2 = 0.0;
	  } else {
	    DL = DL +  0.5 / fabs(pow( (X/E0) , 1/m ));
	    // added 6/9/2006
	    //values for recorder
	    SR2 = X;
	    NC2 = 0.5;
	  }

	} else {

	  // One full cycle of |C-D| and 1/2 cyle of |A-B|

	  if (fabs(C-trialStrain) < 1e-10) {
	    DL = DI;
	    // added 6/9/2006
	    //values for recorder
	    SR3 = 0.0;
	    NC3 = 0.0;
	  } else {
	    DL = DI +  1.0 / fabs(pow( ( fabs(C-trialStrain)/E0 ) , 1/m ));
	    // added 6/9/2006
	    //values for recorder
	    SR3 = fabs(C-trialStrain);
	    NC3 = 1.0;
	  } 

	  if (fabs(A-B) < 1e-10) {
	    DL = DL;
	    // added 6/9/2006
	    //values for recorder
	    SR2 = 0.0;
	    NC2 = 0.0;
	  } else {
	    DL = DL +  0.5 / fabs(pow( (fabs(A-B) /E0) , 1/m ));
	    // added 6/9/2006
	    //values for recorder
	    SR2 = fabs(A-B);
	    NC2 = 0.5;
	  }
	}
      }

      // Did we fail before a peak?
      double mStress = theMaterial->getStress();
      if (DL > Dmax && mStress > 0.0 ) {
	DI = DL;
	Cfailed = true;
	std::cerr << getClassName() << ": material tag " << this->getTag() << " failed at pseudo peak\n";
      } else {
	Cfailed = false;
      }

    }

    //added by SAJalali
    if (!Cfailed)
    {
	    double TStress = getStress();
	    energy += 0.5*(trialStrain - PS)*(TStress + CStress);
	    CStress = TStress;
    }

    PS = cSlope;            // Previous Slope
    EP = trialStrain;   // Keep track of previous strain

    // Check if failed at current step
    if (Cfailed) {
      return 0;
    }
    else 
      return theMaterial->commitState();
  }

int XC::FatigueMaterial::revertToLastCommit(void)
{
  // Check if failed at last step
  if (Cfailed)
    return 0;
  else
    return theMaterial->revertToLastCommit();
}

//! @brief Return element to its initial state.
int XC::FatigueMaterial::revertToStart(void)
  {
    int retval= UniaxialMaterial::revertToStart();
    Cfailed = false;
    DI  = 0; //Damage index
    X   = 0; //Range in consideration
    Y   = 0; //Previous Adjacent Range
    A   = 0; //Peak or valley 1
    B   = 0; //Peak or valley 2
    C   = 0; //Peak or valley 2
    D   = 0; //Peak or valley 4
    PCC = 0; /*Previous Cycle counter flag if >1 then previous 'n' 
	       cycles did not flag a complete cycle */
    R1F = 0; //Flag for first  peak count
    R2F = 0; //Flag for second peak count
    cSlope  = 0; //Current Slope
    PS  = 0; //Previous slope
    EP  = 0; //Previous Strain
    SF  = 0; /*Start Flag = 0 if very first strain, (i.e. when initializing)
	       = 1 otherwise */
    DL  = 0; //Damage if current strain was last peak.

    // added 6/9/2006
    //values for recorder
    SR1 = 0;
    NC1 = 0;
    SR2 = 0;
    NC2 = 0;
    SR3 = 0;
    NC3 = 0;

    retval+= theMaterial->revertToStart();

    return retval;
  }


int XC::FatigueMaterial::sendSelf(Communicator &comm)
  {
    std::cerr << getClassName() << "::" << __FUNCTION__
	      << " not migrated from OpenSees yet." << std::endl;
    return 0;
  }

int XC::FatigueMaterial::recvSelf(const Communicator &comm)
  {
    std::cerr << getClassName() << "::" << __FUNCTION__
	      << " not migrated from OpenSees yet." << std::endl;
    return 0;
  }

void XC::FatigueMaterial::Print(std::ostream &s, int flag) const
  {
    std::cerr << getClassName() << "::" << __FUNCTION__
	      << " not migrated from OpenSees yet." << std::endl;
    if (flag == 100)
      {
        s << DL << std::endl;
      }

      // if (flag == OPS_PRINT_PRINTMODEL_MATERIAL) {
      // 	      s << "FatigueMaterial tag: " << this->getTag() << endln;
      // 	      s << "\tMaterial: " << theMaterial->getTag() << endln;
      // 	      s << "\tDI: " << DI << " Dmax: " << Dmax << endln;
      // 	      s << "\tE0: " << E0 << " m: " << m << endln;
      // 	      s << "\tDL: " << DL << endln;
      // }

      // if (flag == OPS_PRINT_PRINTMODEL_JSON) {
      // 	      s << "\t\t\t{";
      // 	      s << "\"name\": \"" << this->getTag() << "\", ";
      // 	      s << "\"type\": \"FatigueMaterial\", ";
      // 	      s << "\"material\": \"" << theMaterial->getTag() << "\", ";
      // 	      s << "\"tDI\": " << DI << ", ";
      // 	      s << "\"Dmax\": " << Dmax << ", ";
      // 	      s << "\"tE0\": " << E0 << ", ";
      // 	      s << "\"m\": " << m << ", ";
      // 	      s << "\"tDL\": " << DL << "}";
      // }
  }

XC::Response *XC::FatigueMaterial::setResponse(const std::vector<std::string> &, Information &)
  {
    std::cerr << getClassName() << "::" << __FUNCTION__
	      << " not migrated from OpenSees yet." << std::endl;
    return nullptr;
  }

int XC::FatigueMaterial::getResponse(int responseID, Information &matInformation)
  {
    std::cerr << getClassName() << "::" << __FUNCTION__
	      << " not migrated from OpenSees yet." << std::endl;
    return 0;
  }

bool XC::FatigueMaterial::hasFailed(void)
  { return Cfailed; }
