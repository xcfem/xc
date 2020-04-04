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
// $Source: /usr/local/cvs/OpenSees/SRC/damage/Kratzig.cpp,v $
                                                                        
// Written: AA,GGD 
// Created: 08/02
// Revision: AA
//
// Description: This file contains the class implementation for Damage model  .
//

#include <material/damage/Kratzig.h>
#include <material/damage/DamageResponse.h>


XC::Kratzig::Kratzig( int tag, double ultimatePosVal ,  double ultimateNegVal )
:DamageModel(tag,DMG_TAG_Kratzig),
UltimatePosValue(ultimatePosVal) , UltimateNegValue(ultimateNegVal)
{
        if ( UltimatePosValue <= 0.0 )         {
                std::cerr << "XC::Kratzig::Kratzig : Incorrect arguments for the damage model"<<std::endl;
                exit (-1);
        }
        if ( UltimateNegValue < 0.0 ) UltimateNegValue = fabs( UltimateNegValue);
        if ( UltimateNegValue == 0.0 ) UltimateNegValue = UltimatePosValue;
        
        this->revertToStart();
}


XC::Kratzig::Kratzig()
:DamageModel(0,DMG_TAG_Kratzig)
{
        // Does nothing
}

int XC::Kratzig::setTrial(const Vector &trialVector)
  {
    // Trial step variables
    double TSumPosFHC= 0.0, TPosPHC= 0.0, TSumNegFHC= 0.0, TNegPHC= 0.0, TMaxPosDefo= 0.0, TMinNegDefo= 0.0, TDamage;
        
    // retrieve history data
    double CDefo= CommitInfo[0];
    double CForce= CommitInfo[1];
    //double CKunload= CommitInfo[2];
    double CSumPosFHC= CommitInfo[3];
    double CPosPHC= CommitInfo[4];
    double CSumNegFHC= CommitInfo[5];
    double CNegPHC= CommitInfo[6];
    double CMaxPosDefo= CommitInfo[7];
    double CMinNegDefo= CommitInfo[8];
    double CDamage= CommitInfo[9];

    if(trialVector.Size()<3)
      {
        std::cerr << "WARNING: XC::Kratzig::setTrial Wrong vector size for trial data" << std::endl;
        return -1;
      }
                
    double TDefo= trialVector(0);
    double TForce= trialVector(1);
    double TKunload= trialVector(2);

    if(TDefo>0.0)
      {
        if(CDefo<0.0)
          {
                        
            double ZeroDispForce;
            if( fabs ( TDefo - CDefo ) > 1.0e-6 )
              { ZeroDispForce = CForce - (TForce - CForce) * CDefo / (TDefo - CDefo ); }
	    else
              { ZeroDispForce = 0.5*( CForce + TForce ); }
                      
            TSumNegFHC = CSumNegFHC + 0.5*( ZeroDispForce + CForce )*( 0.0 - CDefo );
                        
            if(TDefo<=CMaxPosDefo)
              {
                TSumPosFHC = CSumPosFHC + 0.5*( TForce + ZeroDispForce )*( TDefo - 0.0);
                TPosPHC = CPosPHC;
              }
            else
              {
                TMaxPosDefo= TDefo;
                double MaxDispForce = CForce +(TForce - CForce ) * (TMaxPosDefo - CDefo)/( TDefo - CDefo );
                TPosPHC = CPosPHC + 0.5*( TForce + MaxDispForce )*( TDefo - TMaxPosDefo);
                TSumPosFHC = CSumPosFHC + 0.5*( MaxDispForce + CForce )*( TMaxPosDefo - CDefo);
              }
          }
        else
          {
            if(TDefo<=CMaxPosDefo)
              {
                TSumPosFHC = CSumPosFHC + 0.5*( TForce + CDefo )*( TDefo - CDefo );
                TPosPHC = CPosPHC;
              }
            else
              {
                double MaxDispForce = CForce +(TForce - CForce ) * ( CMaxPosDefo - CDefo)/( TDefo - CDefo );
                TPosPHC = CPosPHC + 0.5*( TForce + MaxDispForce )*( TDefo - CMaxPosDefo);
                TSumPosFHC = CSumPosFHC + 0.5*( MaxDispForce + CForce )*( CMaxPosDefo - CDefo);
                TMaxPosDefo = TDefo;
              }                        
          }
                
      }
    else if(TDefo<0.0)
      {
        if(CDefo>0.0)
          {
            double ZeroDispForce;
            if(fabs( TDefo - CDefo ) > 1.0e-6)
              { ZeroDispForce = CForce - (TForce - CForce) * CDefo / (TDefo - CDefo ); }
            else
              { ZeroDispForce = 0.5*( CForce + TForce ); }
                 
            TSumPosFHC = CSumPosFHC + 0.5*( ZeroDispForce + CForce )*( 0.0 - CDefo );
                       
            if(TDefo>=CMinNegDefo)
              {
                TSumNegFHC = CSumNegFHC + 0.5*( TForce + ZeroDispForce )*( TDefo - 0.0);
                TNegPHC = CNegPHC;
              }
            else
              {
                TMinNegDefo = TDefo;
                double MinDispForce = CForce +(TForce - CForce ) * ( TMinNegDefo - CDefo)/( TDefo - CDefo );
                TNegPHC = CNegPHC + 0.5*( TForce + MinDispForce )*( TDefo - TMinNegDefo);
                TSumNegFHC = CSumNegFHC + 0.5*( MinDispForce + CForce )*( TMinNegDefo - CDefo);
              }

          }
        else
          {
            if( TDefo >= CMinNegDefo )
              {
                TSumNegFHC = CSumNegFHC + 0.5*( TForce + CDefo )*( TDefo - CDefo );
                TNegPHC = CNegPHC;
              }
            else
              {
                double MinDispForce = CForce +(TForce - CForce ) * ( CMinNegDefo - CDefo)/( TDefo - CDefo );
                TNegPHC = CNegPHC + 0.5*( TForce + MinDispForce )*( TDefo - CMinNegDefo);
                TSumNegFHC = CSumNegFHC + 0.5*( MinDispForce + CForce )*( CMinNegDefo - CDefo);
                TMinNegDefo = TDefo;
              }
          }                  
      }
    else
      { 
        // TDefo = 0.0
        if(CDefo<0.0)
          { TSumNegFHC = CSumNegFHC + 0.5*( TForce + CForce )*( TDefo - CDefo ); }
        else if(CDefo>0.0)
          { TSumPosFHC = CSumPosFHC + 0.5*( TForce + CForce )*( TDefo - CDefo ); }
        else TSumPosFHC = CSumPosFHC;
      }

    double PosDamage = (TPosPHC + TSumPosFHC) / ( UltimatePosValue + TSumPosFHC ) ;
    double NegDamage = (TNegPHC + TSumNegFHC) / ( UltimateNegValue + TSumNegFHC ) ;
        
    TDamage = PosDamage + NegDamage - PosDamage * NegDamage;

    if(TDamage<CDamage) TDamage = CDamage;

    TrialInfo[0] = TDefo;
    TrialInfo[1] = TForce;
    TrialInfo[2] = TKunload;
    TrialInfo[3] = TSumPosFHC;
    TrialInfo[4] = TPosPHC;
    TrialInfo[5] = TSumNegFHC;
    TrialInfo[6] = TNegPHC;
    TrialInfo[7] = TMaxPosDefo;
    TrialInfo[8] = TMinNegDefo;
    TrialInfo[9] = TDamage;

    return 0;
  }


double XC::Kratzig::getDamage (void)
  { return TrialInfo[9]; }
    

double XC::Kratzig::getPosDamage (void)
{
        double PosDamage = ( TrialInfo[4] + TrialInfo[3] ) / ( UltimatePosValue + TrialInfo[3] ) ;
        return PosDamage;
}


double XC::Kratzig::getNegDamage (void)
{
        double         NegDamage = ( fabs(TrialInfo[6]) + fabs(TrialInfo[5]) ) / ( fabs(UltimateNegValue) + fabs(TrialInfo[5]) ) ;
        return NegDamage;
}


int
XC::Kratzig::commitState (void)
{
        for ( int i=0 ; i<10 ; i++ )
        {
                LastCommitInfo[i] = CommitInfo[i];
                CommitInfo[i] = TrialInfo[i];
        }

        return 0;
}

int
XC::Kratzig::revertToLastCommit (void)
{
        for ( int i=0 ; i<10 ; i++ )
        {
                CommitInfo[i] = LastCommitInfo[i];
        }

        return 0;
}

int
XC::Kratzig::revertToStart (void)
{
        for ( int i = 0 ; i< 10 ; i++ ){
                TrialInfo[i] = 0.0;
                CommitInfo[i] = 0.0;
                LastCommitInfo[i] = 0.0;
        }

        return 0;
}


XC::DamageModel *XC::Kratzig::getCopy(void) const
{
        Kratzig *theCopy = new Kratzig(this->getTag() , UltimatePosValue , UltimateNegValue );
    
        for ( int i=0 ; i<10 ; i++ )
        {
                theCopy->TrialInfo[i] = TrialInfo[i];
                theCopy->CommitInfo[i] = CommitInfo[i];
                theCopy->LastCommitInfo[i] = LastCommitInfo[i];
        }

        return theCopy;
}

//! @brief Returns the identifier of the variable cuyo
//! name being passed as parameter.
int XC::Kratzig::setVariable(const std::string &argv)
  { return -1; }

//! @brief Returns the value of the variable cuyo
//! identifier being passed as parameter.
int XC::Kratzig::getVariable(int variableID, double &info)
  { return -1; }

int XC::Kratzig::setParameter(const std::vector<std::string> &argv, Information &eleInformation)
{
    return -1;
}

int
XC::Kratzig::updateParameter(int responseID, Information &eleInformation)
{
    return -1;
}

XC::Response *XC::Kratzig::setResponse(const std::vector<std::string> &argv, Information &info)
  {
//
// we compare argv[0] for known response types for the XC::Truss
//

        if ( argv[0] == "damage" || argv[0] == "damageindex" )
    return new DamageResponse( this , 1 , 0.0 );

        else if ( argv[0] == "defo" || argv[0] == "deformation" )
    return new DamageResponse( this , 2 , 0.0 );

        else if (argv[0] == "trial" || argv[0] == "trialinfo" )
    return new DamageResponse( this , 3 , Vector(4) );

        else 
                return 0;

}

int 
XC::Kratzig::getResponse(int responseID, Information &info)
{
        switch (responseID) {
        case -1:
                return -1;
        
        case 1:
                return info.setDouble( this->getDamage() );

        case 2:
                return info.setDouble( TrialInfo[0] );

        case 3:
                if(info.theVector!=0)
                {
                        (*(info.theVector))(0) = TrialInfo[4];
                        (*(info.theVector))(1) = TrialInfo[3];
                        (*(info.theVector))(2) = TrialInfo[6];
                        (*(info.theVector))(3) = TrialInfo[5];
                }
                return 0;

        default:
                return -1;
        }
}


int XC::Kratzig::sendSelf(Communicator &comm)
  { return 0; }


int XC::Kratzig::recvSelf(const Communicator &comm)
  { return 0; }


void
XC::Kratzig::Print(std::ostream &s, int flag ) const
{
    s << "CumulativePeak tag: " << this->getTag() << std::endl;
        s << " UltimatePosValue: " << UltimatePosValue << " UltimateNegValue: " << UltimateNegValue << std::endl;
}


int XC::Kratzig::setInputResponse ( Element *elem , const std::vector<std::string> &argv, int ndof )
{
        return -1;
}
