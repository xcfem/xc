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
** ****************************************************************** */
                                                                        
// $Revision: 1.2 $
// $Date: 2010-02-04 20:50:27 $
// $Source: /usr/local/cvs/OpenSees/SRC/material/nD/DruckerPrager3D.cpp,v $

// Written: K.Petek, U.Washington

#include "DruckerPrager3D.h"


//null constructor
XC::DruckerPrager3D::DruckerPrager3D( ) : 
DruckerPrager( )
{  }

//full constructor
XC::DruckerPrager3D::DruckerPrager3D(int tag, double bulk, double shear, double s_y,
							 double r, double r_bar, double Kinfinity, double Kinit, 
							 double d1, double d2, double H, double t, double mDen, double atm) : 
DruckerPrager(tag, ND_TAG_DruckerPrager3D, bulk, shear, s_y, r, r_bar, Kinfinity,
	                                       Kinit, d1, d2, H, t, mDen, atm)
{
}

   
//destructor
XC::DruckerPrager3D::~DruckerPrager3D( ) 
{} 


//! @brief Virtual constructor.
XC::NDMaterial* XC::DruckerPrager3D::getCopy(void) const 
  { return new DruckerPrager3D(*this); }


//send back type of material
const std::string &XC::DruckerPrager3D::getType(void) const 
{
  static const std::string retval= "ThreeDimensional";
  return retval;
}


//send back order of strain in vector form
int XC::DruckerPrager3D::getOrder( ) const 
{ 
  return 6 ; 
} 


//get the strain and integrate plasticity equations
int XC::DruckerPrager3D::setTrialStrain( const Vector &strain_from_element) 
{
	mEpsilon = strain_from_element;
	this->plastic_integrator( ) ;
	return 0 ;
}


//unused trial strain functions
int XC::DruckerPrager3D::setTrialStrain (const Vector &v, const Vector &r)
{
  std::cerr << "YOU SHOULD NOT SEE THIS: DruckerPrager::setTrialStrain (const Vector &v, const Vector &r)" << std::endl;
  return this->setTrialStrain (v);
}


//send back the strain
const XC::Vector& XC::DruckerPrager3D::getStrain( ) 
{
  return mEpsilon ;
} 


//send back the stress 
const XC::Vector& XC::DruckerPrager3D::getStress( ) 
  { return mSigma; }

//! @brief Return the material tangent stiffness.
const XC::Matrix& XC::DruckerPrager3D::getTangent(void) 
  { return mCep; } 

//! @brief Return the material initial stiffness.
const XC::Matrix& XC::DruckerPrager3D::getInitialTangent(void) 
  { return mCe; } 

