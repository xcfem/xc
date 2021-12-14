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
                                                                        
// $Revision: 1.1 $
// $Date: 2004/09/01 03:54:28 $
// $Source: /usr/local/cvs/OpenSees/SRC/damage/ParkAng.cpp,v $

// Written: AA,GGD 
// Created: 10/02
// Revision: AA
//
// Description: This file contains the class implementation for XC::ParkAng 
// damage model. ParkAng damage model calculates the damage index based on
// primary half cycle and the summation of follower half cycles.
//

#include <material/damage/ParkAng.h>
#include <material/damage/DamageResponse.h>


XC::ParkAng::ParkAng (int tag, double deltaU , double beta , double sigmaY )
 :DamageModel(tag,DMG_TAG_ParkAng),
  DeltaU(deltaU), Beta(beta), SigmaY(sigmaY)
 {
   if(DeltaU <= 0 || Beta <= 0 || SigmaY <= 0 )
     std::cerr << "ParkAng::ParkAng : Incorrect arguments for the damage model"
               <<std::endl;
   this->revertToStart();
 }


XC::ParkAng::ParkAng(void)
  :DamageModel(0,DMG_TAG_ParkAng)
  {}

int XC::ParkAng::setTrial (double scalar, double scalarRate )
  {
    std::cerr << "WARNING: XC::ParkAng::setTrial Wrong Method called" << std::endl;
    std::cerr << "ParkAng Model uses vector based setTrial method" << std::endl;
    return -1;
  }

int XC::ParkAng::setTrial(const Vector &trialVector)
  {
        // Trial step
        double TEnergy, TMaxDefo, TDamage;

        // Committed state
        double CForce= CommitInfo[0];;
        double CDeformation= CommitInfo[1];;
        //double CUnloadingK= CommitInfo[2];;
        double CEnergy= CommitInfo[3];;
        double CMaxDefo= CommitInfo[4];;
        double CDamage= CommitInfo[5];;
        
        
        // Deformation = trialVector(0);
        // Force = trialVector(1);
        //
        if( trialVector.Size() != 3 ) {
                std::cerr << "WARNING: XC::ParkAng::setTrial Wrong vector size for trial data" << std::endl;
                return -1;
        }

        double TDeformation = trialVector(0);
        double TForce = trialVector(1);
        double TUnloadingK = trialVector(2);
        
        if ( TUnloadingK < 0.0 ) {
                std::cerr << "WARNING: XC::ParkAng::setTrial negative unloading stiffness specified" << std::endl;
                return -1;
        }

        TEnergy = CEnergy + 0.5 * ( TForce + CForce ) * ( TDeformation - CDeformation );

        double PlasticEnergy;
        if (TUnloadingK != 0.0 ) {
                PlasticEnergy = TEnergy - 0.5 * TForce * TForce / TUnloadingK;
        } else {
                PlasticEnergy = TEnergy;
        }

        TMaxDefo = ( fabs( TDeformation ) > fabs( CMaxDefo ) ) ? fabs(TDeformation) : fabs(CMaxDefo);

        TDamage = ( TMaxDefo / DeltaU ) + ( Beta * PlasticEnergy / SigmaY / DeltaU );
        if ( TDamage < CDamage )  TDamage = CDamage;

        // Trial step
        TrialInfo[0] =  TForce;
        TrialInfo[1] =  TDeformation;
        TrialInfo[2] =  TUnloadingK;
        TrialInfo[3] =  TEnergy;
        TrialInfo[4] =  TMaxDefo;
        TrialInfo[5] =  TDamage;

        return 0;
}


int
XC::ParkAng::setTrial ()
{
        std::cerr << "WARNING: XC::ParkAng::setTrial Wrong Method called" << std::endl;
        std::cerr << "ParkAng Model uses vector based setTrial method" << std::endl;
        return -1;
}


double
XC::ParkAng::getDamage (void)
{
        return TrialInfo[5];
}


double XC::ParkAng::getPosDamage (void)
{
        return TrialInfo[5];
}


double XC::ParkAng::getNegDamage (void)
{
        return TrialInfo[5];
}

    
int
XC::ParkAng::commitState (void)
{
        for ( int i=0 ; i<6 ; i++ )
        {
                LastCommitInfo[i] = CommitInfo[i];
                CommitInfo[i] = TrialInfo[i];
        }

        return 0;
}

int
XC::ParkAng::revertToLastCommit (void)
{
        for ( int i=0 ; i<6 ; i++ )
        {
                CommitInfo[i] = LastCommitInfo[i];
        }

        return 0;
}

int
XC::ParkAng::revertToStart (void)
{
        for ( int i = 0 ; i< 6 ; i++ ){
                TrialInfo[i] = 0.0;
                CommitInfo[i] = 0.0;
                LastCommitInfo[i] = 0.0;
        }

        return 0;
}

XC::DamageModel *XC::ParkAng::getCopy(void) const
  {
        ParkAng *theCopy = new ParkAng(this->getTag(), DeltaU , Beta , SigmaY );
    
         for ( int i=0 ; i<6 ; i++ )
        {
                theCopy->TrialInfo[i] = TrialInfo[i];
                theCopy->CommitInfo[i] = CommitInfo[i];
                theCopy->LastCommitInfo[i] = LastCommitInfo[i];
        }

        return theCopy;
}

//! @brief Returns the identifier of the variable cuyo
//! name being passed as parameter.
int XC::ParkAng::setVariable(const std::string &argv)
  { return -1; }

//! @brief Returns the value of the variable cuyo
//! identifier being passed as parameter.
int XC::ParkAng::getVariable(int variableID, double &info)
  { return -1; }

int XC::ParkAng::setParameter(const std::vector<std::string> &argv, Information &eleInformation)
  { return -1; }

int
XC::ParkAng::updateParameter(int responseID, Information &eleInformation)
{
    return -1;
}

XC::Response *XC::ParkAng::setResponse(const std::vector<std::string> &argv, Information &info)
{
//
// we compare argv[0] for known response types for the XC::Truss
//

        if ( argv[0] == "damage" || argv[0] == "damageindex" )
    return new DamageResponse( this , 1 , 0.0 );

        else if (argv[0] == "Value" || argv[0] == "Values" || argv[0] == "Data" )
    return new DamageResponse( this , 2 , Vector(3) );

        else if (argv[0] == "trial" || argv[0] == "trialinfo" )
    return new DamageResponse( this , 3 , Vector(6) );

        else 
                return 0;

}

int 
XC::ParkAng::getResponse(int responseID, Information &info)
{
        switch (responseID) {
        case -1:
                return -1;
        
        case 1:
                return info.setDouble( this->getDamage() );

        case 2:
                if(info.theVector!=0)
                {
                        (*(info.theVector))(0) = TrialInfo[1];
                        (*(info.theVector))(1) = TrialInfo[0];
                        (*(info.theVector))(2) = TrialInfo[2];
                }
                return 0;

        case 3:
                if(info.theVector!=0)
                {
                        (*(info.theVector))(0) = TrialInfo[0];
                        (*(info.theVector))(1) = TrialInfo[1];
                        (*(info.theVector))(2) = TrialInfo[2];
                        (*(info.theVector))(3) = TrialInfo[3];
                        (*(info.theVector))(4) = TrialInfo[4];
                        (*(info.theVector))(5) = TrialInfo[5];
                }

                return 0;

        default:
                return -1;
        }
}


int XC::ParkAng::sendSelf(Communicator &comm)
  { return 0; }


int XC::ParkAng::recvSelf(const Communicator &comm)
  { return 0; }


void
XC::ParkAng::Print(std::ostream &s, int flag ) const
{
    s << "ParkAng tag: " << this->getTag() << std::endl;
    s << "  DeltaU: " << DeltaU << " Beta: " << Beta << "  SigmaY: " << SigmaY << std::endl;
}



int XC::ParkAng::setInputResponse ( Element *elem , const std::vector<std::string> &argv, int ndof )
{
        return -1;
}
