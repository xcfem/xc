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
// $Date: 2004/09/01 03:54:28 $
// $Source: /usr/local/cvs/OpenSees/SRC/damage/NormalizedPeak.cpp,v $

// Written: Arash Altoontash, Gregory Deierlein
// Created: 08/02
// Revision: AA
//
// Description: This file contains the class implementation for Damage model  .
//

#include <material/damage/NormalizedPeak.h>
#include <material/damage/DamageResponse.h>
#include <domain/mesh/element/Element.h>


XC::NormalizedPeak::NormalizedPeak(int tag, double maxVal, double minVal , const char *argv)
 :DamageModel(tag,DMG_TAG_NormalizedPeak), damagetype( NotSpecified ), 
  MaxValue(maxVal), MinValue(minVal),
  TrialDmg(0.0),TrialVector(3),
  CommitDmg(0.0),CommitVector(3),
  LCommitDmg(0.0), LCommitVector(3)
{
  if( MaxValue < 0.0 || MinValue > 0.0 || argv == nullptr )
    {
      std::cerr << "XC::NormalizedPeak::NormalizedPeak : Incorrect arguments for the damage model";
      exit (-1);
    }
  
  damagename= argv; //strcpy ( damagename, argv);
  
  
  if( (damagename == "force") || (damagename == "Force") )
    {
      damagetype = Force;
    }
  else if( (damagename == "strain") || (damagename == "Strain") ||
	    (damagename == "defo") || (damagename == "deformation") ||
	    (damagename == "Deformation") )
    {
      damagetype = Deformation;
    }
  else if( (damagename == "plasticDefo") || (damagename == "PlasticDefo") ||
	    (damagename == "plasticStrain") || (damagename == "PlasticStrain") ||
	    (damagename == "plasticDeformation") || (damagename == "PlasticDeformation") )
    {
      damagetype = PlasticDefo;
    }
  else if( (damagename == "energy") || (damagename == "Energy") ||
	    (damagename == "totalEnergy") || (damagename == "TotalEnergy") )
    {
      damagetype = TotalEnergy;
    }
  else if( (damagename == "plasticDefo") || (damagename == "PlasticDefo") ||
	    (damagename == "plasticStrain") || (damagename == "PlasticStrain") ||
	    (damagename == "plasticDeformation") || (damagename == "PlasticDeformation") ){
    damagetype = 	PlasticEnergy;
  }
  else
    {
      std::cerr << "XC::NormalizedPeak::NormalizedPeak : The damage type specified is not supported";
      exit (-1);	
    }
  
  this->revertToStart();
}


XC::NormalizedPeak::NormalizedPeak()
  :DamageModel(0,DMG_TAG_NormalizedPeak)
{
  // Does nothing
}

XC::DamageModel *XC::NormalizedPeak::getCopy (void) const
  { return new NormalizedPeak(*this); }

    
int XC::NormalizedPeak::commitState (void)
{
  LCommitScalar = CommitScalar;
  LCommitDmg = CommitDmg;
  LCommitVector = CommitVector;
  
  CommitScalar = TrialScalar;
  CommitDmg = TrialDmg;
  CommitVector = TrialVector;
  
  return 0;
}

int
XC::NormalizedPeak::revertToLastCommit (void)
{
  CommitScalar = LCommitScalar;
  CommitDmg = LCommitDmg;
  
  CommitVector = LCommitVector;
  
  return 0;
}

int
XC::NormalizedPeak::revertToStart (void)
{
  TrialScalar = CommitScalar = LCommitScalar = 0.0;
  TrialDmg = CommitDmg = LCommitDmg = 0.0;
  TrialVector.Zero();
  CommitVector.Zero();
  LCommitVector.Zero();
  
  return 0;
}


int XC::NormalizedPeak::setTrial (const Vector &trialVector)
  {
    if( trialVector.Size() < 3 )
      {
        std::cerr << "WARNING: NormalizedPeak::setTrial Wrong vector size for trial data" << std::endl;
        return -1;
      }
  
    TrialVector = trialVector;
  
//     double TrialDefo = trialVector(0);
//     double TrialForce = trialVector(1);
//     double TrialKU = trialVector(2);
  
    TrialScalar = 0.0;
  
    switch(damagetype)
      {
      case Force:
        TrialScalar = TrialVector(1);
        break;
      case Deformation:
        TrialScalar = TrialVector(0);
        break;
      case PlasticDefo:
        if( TrialVector(2) != 0.0 )
	  { TrialScalar = TrialVector(0) - TrialVector(1)/TrialVector(2); }
        else
          { TrialScalar = TrialVector(0); }
        break;
      case TotalEnergy:
        TrialScalar = CommitScalar + 0.5*( TrialVector(1) + CommitVector(1) )*( TrialVector(0) - CommitVector(0) );
        break;
      case PlasticEnergy:
        if( TrialVector(2) > 0.0 )
          TrialScalar = CommitScalar + 0.5*( TrialVector(1) + CommitVector(1) )*( TrialVector(0) - CommitVector(0) )
	  - 0.5* TrialVector(1) * TrialVector(1) / TrialVector(2);
        break;
      default:
	std::cerr << "Unknown value for damagetype :" << damagetype
		  << " es desconocido." << std::endl;
        break;
      }
  
    // Now calculate damage
    if(TrialScalar >= 0.0)
      { TrialDmg = TrialScalar / MaxValue; }
    else
      { TrialDmg = fabs( TrialScalar / MinValue ); }

    if( fabs(TrialDmg) < CommitDmg )
      TrialDmg = CommitDmg;
    return 0;
  }

double XC::NormalizedPeak::getDamage(void)
  { return TrialDmg; }

double XC::NormalizedPeak::getPosDamage(void)
  { return TrialDmg; }


double XC::NormalizedPeak::getNegDamage(void)
  { return TrialDmg; }


XC::Response *XC::NormalizedPeak::setResponse(const std::vector<std::string> &argv, Information &info)
  {
    //
    // we compare argv[0] for known response types for the XC::Truss
    //
  
    if(argv[0] == "damage" || argv[0] == "damageindex" )
      return new DamageResponse(this,1,0.0);
    else
      return 0;
  }

int XC::NormalizedPeak::getResponse(int responseID, Information &info)
{
  switch (responseID) {
  case -1:
    return -1;
    
  case 1:
    return info.setDouble( this->getDamage() );
    
  default:
    return -1;
  }
}


int XC::NormalizedPeak::sendSelf(CommParameters &cp)
  { return 0; }


int XC::NormalizedPeak::recvSelf(const CommParameters &cp)
  { return 0; }


void XC::NormalizedPeak::Print(std::ostream &s, int flag )
  {
    s << "NormalizedPeak tag: " << this->getTag() << std::endl;
    s << "  MaximumValue: " << MaxValue << " MinimumValue: " << MinValue << std::endl;
    s << " Response type: " <<damagename << std::endl;
    
  }
