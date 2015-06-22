//----------------------------------------------------------------------------
//  programa XC; cálculo mediante el método de los elementos finitos orientado
//  a la solución de problemas estructurales.
//
//  Copyright (C)  Luis Claudio Pérez Tato
//
//  El programa deriva del denominado OpenSees <http://opensees.berkeley.edu>
//  desarrollado por el «Pacific earthquake engineering research center».
//
//  Salvo las restricciones que puedan derivarse del copyright del
//  programa original (ver archivo copyright_opensees.txt) este
//  software es libre: usted puede redistribuirlo y/o modificarlo 
//  bajo los términos de la Licencia Pública General GNU publicada 
//  por la Fundación para el Software Libre, ya sea la versión 3 
//  de la Licencia, o (a su elección) cualquier versión posterior.
//
//  Este software se distribuye con la esperanza de que sea útil, pero 
//  SIN GARANTÍA ALGUNA; ni siquiera la garantía implícita
//  MERCANTIL o de APTITUD PARA UN PROPÓSITO DETERMINADO. 
//  Consulte los detalles de la Licencia Pública General GNU para obtener 
//  una información más detallada. 
//
// Debería haber recibido una copia de la Licencia Pública General GNU 
// junto a este programa. 
// En caso contrario, consulte <http://www.gnu.org/licenses/>.
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
** file 'COPYRIGHT'  in XC::main directory for information on usage and   **
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

        // Commited state
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

//! @brief Devuelve el identificador de la variable cuyo
//! nombre se pasa como parámetro.
int XC::ParkAng::setVariable(const std::string &argv)
  { return -1; }

//! @brief Devuelve el valor de la variable cuyo
//! identificador se pasa como parámetro.
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
    return new XC::DamageResponse( this , 1 , 0.0 );

        else if (argv[0] == "Value" || argv[0] == "Values" || argv[0] == "Data" )
    return new XC::DamageResponse( this , 2 , Vector(3) );

        else if (argv[0] == "trial" || argv[0] == "trialinfo" )
    return new XC::DamageResponse( this , 3 , Vector(6) );

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


int XC::ParkAng::sendSelf(CommParameters &cp)
  { return 0; }


int XC::ParkAng::recvSelf(const CommParameters &cp)
  { return 0; }


void
XC::ParkAng::Print(std::ostream &s, int flag )
{
    s << "ParkAng tag: " << this->getTag() << std::endl;
    s << "  DeltaU: " << DeltaU << " Beta: " << Beta << "  SigmaY: " << SigmaY << std::endl;
}



int XC::ParkAng::setInputResponse ( Element *elem , const std::vector<std::string> &argv, int ndof )
{
        return -1;
}
