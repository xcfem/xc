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

// $Revision: 1.2
// $Date: 2010-11-10
// $Source: /OpenSees/SRC/material/nD/ContactMaterialBase.cpp,v $
                                                                        
// Written: Kathryn Petek
// Created: February 2004
// Modified: Chris McGann
//           November 2010 -> changes for incorporation into main source code
// Modified: Chris McGann
//           Jan 2011 -> added update for frictional state

// Description: This file contains the implementation for the ContactMaterialBase class.
//				

#include "ContactMaterialBase.h"

#include <domain/mesh/element/utils/Information.h>
#include <utility/recorder/response/MaterialResponse.h>
#include <domain/component/Parameter.h>

//full constructor
XC::ContactMaterialBase::ContactMaterialBase(int tag, int classTag, int dim, double mu, double G, double c, double t)
  : NDMaterial(tag, classTag),
    frictionCoeff(mu), stiffness(G), cohesion(c), tensileStrength(t),
    mFrictFlag(true),
    strain_vec(dim),
    stress_vec(dim),
    tangent_matrix(dim,dim)
  {
#ifdef DEBUG
    opserr << "XC::ContactMaterialBase::ContactMaterialBase" << endln;
#endif
    mMu = mu;
    mCo = c;
    mTen = t;

  }
   
//null constructor
XC::ContactMaterialBase::ContactMaterialBase(int tag, int classTag, int dim) 
 : NDMaterial(tag, classTag),
   frictionCoeff(0.0), stiffness(1.0), cohesion(0.0), tensileStrength(0.0),
   mFrictFlag(true),
   strain_vec(dim),
   stress_vec(dim),
   tangent_matrix(dim,dim)
  {
    mMu= 0.0;
    mCo= 0.0;
    mTen= 0.0;
  }

const XC::Matrix & XC::ContactMaterialBase::getInitialTangent(void) const
  {
    return tangent_matrix;      //tangent is empty matrix
  }


const XC::Vector & XC::ContactMaterialBase::getStress(void) const
  {
    return stress_vec;
  }


const XC::Vector & XC::ContactMaterialBase::getStrain(void) const
  {
    return strain_vec;
  }


int XC::ContactMaterialBase::revertToLastCommit (void)
  {
        return 0;
  }

int XC::ContactMaterialBase::UpdateFrictionalState(void)
  {
    if(mFrictFlag && mFlag == 1)
      {
	frictionCoeff = mMu;
	cohesion = mCo;
	tensileStrength = mTen;
	mFlag = 0;

	// ensure tensile strength is inbounds
	if (tensileStrength > cohesion / frictionCoeff )
	  {
	    tensileStrength = cohesion / frictionCoeff;
	  }
		
      }
    else if (!mFrictFlag)
      {
	frictionCoeff = 0.0;
	cohesion = 0.0;
	tensileStrength = 0.0;
	mFlag = 1;
      }
    return 0;
  }


double XC::ContactMaterialBase::getcohesion(void) const
  { return cohesion; }

void XC::ContactMaterialBase::ScaleCohesion(const double len) 
  { cohesion *= len; }

double XC::ContactMaterialBase::getTensileStrength(void) const
  { return tensileStrength; }

void XC::ContactMaterialBase::ScaleTensileStrength(const double len)
  { tensileStrength *= len; }

int XC::ContactMaterialBase::updateParameter(int responseID, Information &info)
  {
    if(responseID == 1)
      {
	if(info.theDouble==0.0)
	  this->mFrictFlag= false;
	else
	  this->mFrictFlag= true;
      }
    return 0;
  }

int XC::ContactMaterialBase::setParameter(const std::vector<std::string> &, Parameter &)
  {
    return -1;
  }

//! @brief Send material.
int XC::ContactMaterialBase::sendData(Communicator &comm)
  {
    int res= NDMaterial::sendData(comm);
    res+= comm.sendDoubles(frictionCoeff, stiffness, cohesion, tensileStrength, getDbTagData(), CommMetaData(1));
    res+= comm.sendInt(mFlag, getDbTagData(), CommMetaData(2));
    res+= comm.sendDoubles(mMu, mCo, mTen, getDbTagData(), CommMetaData(3));
    res+= comm.sendVector(strain_vec, getDbTagData(), CommMetaData(4));
    res+= comm.sendVector(stress_vec, getDbTagData(), CommMetaData(5));
    res+= comm.sendMatrix(tangent_matrix, getDbTagData(), CommMetaData(6));
    res+= comm.sendBools(inSlip, mFrictFlag, getDbTagData(), CommMetaData(7));
    return res;
  }

//! @brief Receive material.
int XC::ContactMaterialBase::recvData(const Communicator &comm)
  {
    int res= NDMaterial::recvData(comm);
    res+= comm.receiveDoubles(frictionCoeff, stiffness, cohesion, tensileStrength, getDbTagData(),CommMetaData(1));
    res+= comm.receiveInt(mFlag, getDbTagData(), CommMetaData(2));
    res+= comm.receiveDoubles(mMu, mCo, mTen, getDbTagData(), CommMetaData(3));
    res+= comm.receiveVector(strain_vec, getDbTagData(), CommMetaData(4));
    res+= comm.receiveVector(stress_vec, getDbTagData(), CommMetaData(5));
    res+= comm.receiveMatrix(tangent_matrix, getDbTagData(), CommMetaData(6));
    res+= comm.receiveBools(inSlip, mFrictFlag, getDbTagData(), CommMetaData(7));
    return res;
  }
