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
// $Source: /usr/local/cvs/OpenSees/SRC/damage/HystereticEnergy.cpp,v $

// Written: Arash Altoontash, Gregory Deierlein 
// Created: 06/03
// Revision: AA
//
// Description: This file contains the class implementation for HystereticEnergy 
// damage model. HystereticEnergy damage model calculates the damage index based on
// last half cycle energy and the cumulative energy. If an unloading stiffness is
// introduced, the calculation would be based on plastic energy
//

#include <material/damage/HystereticEnergy.h>
#include <material/damage/DamageResponse.h>


XC::HystereticEnergy::HystereticEnergy (int tag, double Etot , double Cpow)
:DamageModel(tag,DMG_TAG_HystereticEnergy),
Etotal( Etot ) , Cpower( Cpow )
{
  if ( Etot <= 0 || Cpow <= 0 )
    std::cerr << "XC::DamageModel::DamageModel : Incorrect arguments for the damage model";
  
  this->revertToStart();
}

XC::HystereticEnergy::HystereticEnergy()
:DamageModel(0,DMG_TAG_HystereticEnergy)
{
        // Does nothing
}


int XC::HystereticEnergy::setTrial(const Vector &trialVector)
  {          
    // retrieve history data
    const double CDisp= CommitInfo[0];
    const double CForce= CommitInfo[1];
    //const double CKunload= CommitInfo[2];
    const double CEnrgTot= CommitInfo[3];
    const double CEnrgc= CommitInfo[4];
    const double CExcurDmg= CommitInfo[5];
    const double CCyclicDmg= CommitInfo[6];
  
    if(trialVector.Size() < 3)
      {
        std::cerr << "WARNING: HystereticEnergy::setTrial Wrong vector size for trial data" << std::endl;
        return -1;
      }
    const double TDisp= trialVector[0];
    const double TForce= trialVector[1];
    const double TKunload= trialVector[2];
  
    if(TKunload < 0.0)
      {
        std::cerr << "WARNING: HystereticEnergy::setTrial negative unloading stiffness specified" << std::endl;
        return -1;
      }
  
    double TEnrgTot, TEnrgc, TExcurDmg, TCyclicDmg;
    if(TForce == 0.0)
      {
        // submitt the cyclic damage
        TCyclicDmg = CCyclicDmg + CExcurDmg - CExcurDmg * CCyclicDmg;
        // a new excursion just started
        TEnrgc = 0.0;
        TEnrgTot = CEnrgTot;
      } 
    else 
      if( CForce * TForce < 0.0 )
        {
          double ZeroForceDisp;
          if( fabs( CForce + TForce ) < 1.0e-6 )
            { ZeroForceDisp = 0.5 * ( TDisp + CDisp ); }
          else
            { ZeroForceDisp = ( CForce * TDisp + TForce * CDisp ) / (CForce + TForce ); }
        
          // calculate the energies to the end of last cycle
          TEnrgc = CEnrgc + 0.5 * CForce * ( ZeroForceDisp - CDisp );
          TEnrgTot = CEnrgTot + 0.5 * CForce * ( ZeroForceDisp - CDisp );
          TExcurDmg = pow( TEnrgc / ( Etotal - TEnrgTot) , Cpower );
          TCyclicDmg = CCyclicDmg + TExcurDmg - TExcurDmg * CCyclicDmg;
        
          // then add the new cycle
          TEnrgc = 0.5 * TForce * ( TDisp - ZeroForceDisp );
          TEnrgTot = CEnrgTot + 0.5 * TForce * ( TDisp - ZeroForceDisp );
        
        }
      else
        {
          TEnrgc = CEnrgc + 0.5 * ( TForce + CForce ) * ( TDisp - CDisp );
          TEnrgTot = CEnrgTot + 0.5 * ( TForce + CForce ) * ( TDisp - CDisp );
          TCyclicDmg = CCyclicDmg;
        }
      double RSE= 0.0 ;
      if(TKunload != 0.0)
        {
          // Calculate and deduct the elastic energy from the total energy
          RSE =  0.5 * TForce * TForce / TKunload;        
          if( (TEnrgc - RSE) < 0.0 ) RSE = 0.0;
          if( (TEnrgTot -RSE) < 0.0 ) RSE = 0.0;
        }
  
      TExcurDmg= pow( (TEnrgc -RSE) / ( (Etotal -RSE) - (TEnrgTot -RSE) ) , Cpower );
    
     TrialInfo[0] = TDisp;
     TrialInfo[1] = TForce;
     TrialInfo[2] = TKunload;
     TrialInfo[3] = TEnrgTot;
     TrialInfo[4] = TEnrgc;
     TrialInfo[5] = TExcurDmg;
     TrialInfo[6] = TCyclicDmg;
     return 0;
  }


int XC::HystereticEnergy::setTrial(void)
  {
    std::cerr << "WARNING: HystereticEnergy::setTrial Wrong Method called" << std::endl;
    std::cerr << "HystereticEnergy Model uses vector based setTrial method" << std::endl;
    return -1;
  }


double XC::HystereticEnergy::getDamage (void)
  {
    TrialInfo[7] =  CommitInfo[6] + TrialInfo[5] - TrialInfo[5] * CommitInfo[6];
    if( TrialInfo[7] < CommitInfo[7] ) TrialInfo[7] = CommitInfo[7];
    return TrialInfo[7];
  }


double XC::HystereticEnergy::getPosDamage (void)
  { return this->getDamage(); }


double XC::HystereticEnergy::getNegDamage (void)
  { return this->getDamage(); }


int XC::HystereticEnergy::commitState (void)
  {
        for ( int i=0 ; i<8 ; i++ )
        {
                LastCommitInfo[i] = CommitInfo[i];
                CommitInfo[i] = TrialInfo[i];
        }

        return 0;
}


int
XC::HystereticEnergy::revertToLastCommit (void)
{
        for ( int i=0 ; i<8 ; i++ )
        {
                CommitInfo[i] = LastCommitInfo[i];
        }

        return 0;
}


int
XC::HystereticEnergy::revertToStart (void)
{
        for ( int i = 0 ; i< 8 ; i++ ){
                TrialInfo[i] = 0.0;
                CommitInfo[i] = 0.0;
                LastCommitInfo[i] = 0.0;
        }

        return 0;
}


XC::DamageModel *XC::HystereticEnergy::getCopy(void) const
  {
    HystereticEnergy *theCopy = new HystereticEnergy(this->getTag(), Etotal , Cpower);
 
        for ( int i=0 ; i<8 ; i++ )
        {
                theCopy->TrialInfo[i] = TrialInfo[i];
                theCopy->CommitInfo[i] = CommitInfo[i];
                theCopy->LastCommitInfo[i] = LastCommitInfo[i];
        }

        return theCopy;
}


//! @brief Returns the identificador de la variable cuyo nombre
//! se pasa como parámetro.
int XC::HystereticEnergy::setVariable(const std::string &argv)
  { return -1; }


//! @brief Returns the valor de la variable cuyo nombre
//! se pasa como parámetro.
int XC::HystereticEnergy::getVariable(int variableID, double &info)
  { return -1; }


int XC::HystereticEnergy::setParameter(const std::vector<std::string> &argv, Information &eleInformation)
  { return -1; }


int XC::HystereticEnergy::updateParameter(int responseID, Information &eleInformation)
  { return -1; }


XC::Response *XC::HystereticEnergy::setResponse(const std::vector<std::string> &argv, Information &info)
{
//
// we compare argv[0] for known response types for the XC::Truss
//

        if ( argv[0] == "damage" || argv[0] == "damageindex" )
    return new DamageResponse( this , 1 , 0.0 );

        else if (argv[0] == "trial" || argv[0] == "trialinfo" )
    return new DamageResponse( this , 2 , Vector(7) );

        else 
                return 0;

}


int XC::HystereticEnergy::getResponse(int responseID, Information &info)
  {
        switch (responseID) {
        case -1:
                return -1;
        
        case 1:
                return info.setDouble( this->getDamage() );

        case 2:
                if(info.theVector!=0)
                {
                        for (int i = 0 ; i < 8 ; i++ ) (*(info.theVector))(i) = TrialInfo[i];
                }
                return 0;

        default:
                return -1;
        }
}


int XC::HystereticEnergy::sendSelf(CommParameters &cp)
  { return 0; }


int XC::HystereticEnergy::recvSelf(const CommParameters &cp)
  { return 0; }


void XC::HystereticEnergy::Print(std::ostream &s, int flag )
  {
    s << "HystereticEnergy tag: " << this->getTag() << std::endl;
    s << "  Etotal: " << Etotal << " Cpower: " << Cpower << std::endl;
  }



int XC::HystereticEnergy::setInputResponse ( Element *elem , const std::vector<std::string> &argv, int ndof )
{
        return -1;
}
