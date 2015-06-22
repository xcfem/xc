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
// $Date: 2004/09/01 03:53:13 $
// $Source: /usr/local/cvs/OpenSees/SRC/material/uniaxial/snap/CloughDamage.cpp,v $
//
//
// CloughDamage.cpp: implementation of the XC::CloughDamage class from Fortran version.
// Originally from SNAP PROGRAM by Prof H.K. Krawinkler
//
// Written: A. Altoontash & Prof. G. Deierlein 12/01
// Revised: 03/02
//
// Purpose: This file contains the implementation for the XC::CloughDamage class.
//
//////////////////////////////////////////////////////////////////////

#include <material/uniaxial/snap/CloughDamage.h>
#include <material/damage/DamageModel.h>
#include <cstdlib>

#include <cmath>
#include <cstring>

#define DEBG 0

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

XC::CloughDamage::CloughDamage(int tag, XC::Vector inputParam, DamageModel *strength, DamageModel *stiffness,DamageModel *accelerated,DamageModel *capping )
:UniaxialMaterial(tag,MAT_TAG_SnapCloughDamage)
{
        if( (inputParam.Size()) < 8) 
                std::cerr << "Error: CloughDamage(): inputParam, size <16\n" << "\a";
        
        /*
        Input parameters
        
        elstk      =  initial elastic stiffness
        fyieldPos  =  positive yield strength        fyieldNeg  =  yield strength in compression
        alpha      =  strain hardening ratio (fraction of elstk)
        elstk      =  initial elastic stiffness
        Resfac           =  residual stress after collapse
        dyieldPos  =  positive yield displacement
        dyieldNeg  =  negative yield displacement
                
        ecaps, ecapd, ecapk, ecapa = parameter expressing the hystetic
        energy dissipacion capacity.
        Enrgts, Enrgtd, Enrgtk, Enrgta = hysteretic energy dissipation capacity
        */
        
        elstk                = inputParam[0];
        fyieldPos        = inputParam[1];
        fyieldNeg        = inputParam[2];
        alpha                = inputParam[3];
        Resfac                = inputParam[4];
        capSlope        = inputParam[5];
        capDispPos        = inputParam[6];
        capDispNeg        = inputParam[7];

        
        // Error check
        
        if ( capSlope > 0.0 )
                std::cerr << "Error: XC::CloughDamage::CloughDamage  : CapSlope must be < 0\n" << "\a";
        
        if ( Resfac <  0.0  || Resfac > 1.0)
                std::cerr << "Error: XC::CloughDamage::CloughDamage  : Residual must be > 0 and <= 1\n" << "\a";
        
        if ( alpha > 0.8 || alpha < -0.8 )
                std::cerr << "Error: XC::CloughDamage::CloughDamage  : alpha must be < 0.8 and > -0.8\n" << "\a";        
        
        if ( alpha == capSlope )
                std::cerr << "Error: XC::CloughDamage::CloughDamage  : Error: alpha Hard. can not be equal to alphaCap\n" << "\a";        
        
        StrDamage = StfDamage = AccDamage = CapDamage = nullptr;
        
        if ( strength != nullptr )
        {
                StrDamage = strength->getCopy();
                if ( StrDamage == nullptr ) {
                        std::cerr << "Error: XC::CloughDamage::CloughDamage  : Can not make a copy of strength damage model\n" << "\a";        
                        exit(-1);
                }
        }

        if ( stiffness != nullptr )
        {
                StfDamage = stiffness->getCopy();
                if ( StfDamage == nullptr ) {
                        std::cerr << "Error: XC::CloughDamage::CloughDamage  : Can not make a copy of stiffness damage model\n" << "\a";        
                        exit(-1);
                }
        }
        
        if ( accelerated != nullptr )
        {
                AccDamage = accelerated->getCopy();
                if ( AccDamage == nullptr ) {
                        std::cerr << "Error: XC::CloughDamage::CloughDamage  : Can not make a copy of accelerated stiffness damage model\n" << "\a";        
                        exit(-1);
                }
        }        

        if ( capping != nullptr )
        {
                CapDamage = capping->getCopy();
                if ( CapDamage == nullptr ) {
                        std::cerr << "Error: XC::CloughDamage::CloughDamage  : Can not make a copy of capping damage model\n" << "\a";        
                        exit(-1);
                }
        }

        if ( DEBG ==1 )
    {
      // Open an output file for debugging
                char FileName[20];                                                // debugging
        sprintf(FileName, "CloughDamage%d.out", tag);
                OutputFile = fopen ( FileName , "w" );        // debugging
                fprintf( OutputFile , "Constructor\n" );        // debugging
    }
        
        // Initialize history data
        this->revertToStart();

}

//! @brief Constructor.
XC::CloughDamage::CloughDamage(int tag)
  :UniaxialMaterial(tag,MAT_TAG_SnapCloughDamage) 
  {
    if ( DEBG ==1 ) fprintf( OutputFile , "Empty constructor\n" );        // debugging
  }

XC::CloughDamage::CloughDamage()
  :UniaxialMaterial(0,MAT_TAG_SnapCloughDamage) 
{
        if ( DEBG ==1 ) fprintf( OutputFile , "Empty constructor\n" );        // debugging
}



XC::CloughDamage::~CloughDamage()
{
        if ( DEBG ==1 )
    {
                fprintf( OutputFile , "Distructor\n" );                // debugging
                fclose( OutputFile );                                                // debugging
    }
        
          if ( StrDamage != 0 ) delete StrDamage;
        if ( StfDamage != 0 ) delete StfDamage;
        if ( AccDamage != 0 ) delete AccDamage;
        if ( CapDamage != 0 ) delete CapDamage;
}


int XC::CloughDamage::revertToStart()
{
        if ( DEBG ==1 ) fprintf( OutputFile , "Revert to start\n" );                // debugging

        dyieldPos = fyieldPos/elstk;
        dyieldNeg = fyieldNeg/elstk;
        
        double ekhard = elstk*alpha;
        double fPeakPos = fyieldPos+ekhard*(capDispPos-dyieldPos);
        double fPeakNeg = fyieldNeg+ekhard*(capDispNeg-dyieldNeg);        

        hsTrial[0] = 0.0;                                                                        // d
        hsTrial[1] = 0.0;                                                                        // f
        hsTrial[2]  = elstk;                                                                // ek
        hsTrial[3]  = elstk;                                                                // ekunload
        hsTrial[4]  = elstk;                                                                // ekexcurs
        hsTrial[5]  = 0.0;                                                                        // Enrgtot
        hsTrial[6]  = 0.0;                                                                        // Enrgc
        hsTrial[7]  = 0.0;                                                                        // sp
        hsTrial[8]  = 0.0;                                                                        // sn
        hsTrial[9]  = 0.0;                                                                        // kon
        hsTrial[10] = dyieldPos;                                                        // dmax
        hsTrial[11] = dyieldNeg;                                                        // dmin
        hsTrial[12] = fyieldPos;                                                        // fyPos
        hsTrial[13] = fyieldNeg;                                                        // fyNeg
        hsTrial[14] = capDispPos;                                                        // cpPos
        hsTrial[15] = capDispNeg;                                                        // cpNeg
        hsTrial[16] = 0.0;                                                                        // dlstPos
        hsTrial[17] = 0.0;                                                                        // flstPos
        hsTrial[18] = 0.0;                                                                        // dlstNeg
        hsTrial[19] = 0.0;                                                                        // flstNeg
        hsTrial[20] = alpha;                                                                // alphaPos
        hsTrial[21] = alpha;                                                                // alphaNeg
        hsTrial[22] = -capSlope*elstk*capDispPos+fPeakPos;        // fCapRefPos
        hsTrial[23] = -capSlope*elstk*capDispNeg+fPeakNeg;        // fCapRefNeg : indicates cap reference point
                
        for( int i=0 ; i<24; i++) {
                hsCommit[i]                = hsTrial[i];
                hsLastCommit[i] = hsTrial[i];
        }
        if ( StrDamage != nullptr ) StrDamage->revertToStart();
        if ( StfDamage != nullptr ) StfDamage->revertToStart();
        if ( AccDamage != nullptr ) AccDamage->revertToStart();
        if ( CapDamage != nullptr ) CapDamage->revertToStart();

        return 0;
}

void XC::CloughDamage::Print(std::ostream &s, int flag)
{
        if ( DEBG ==1 ) fprintf( OutputFile , "Print\n" );        // debugging
        s << "BondSlipMaterial Tag: " << this->getTag() << std::endl;
        s << "D : " << hsTrial[0] << std::endl;
        s << "F : " << hsTrial[1] << std::endl;
        s << "EK: " << hsTrial[2]  << std::endl;
        
        s << std::endl;
}


int XC::CloughDamage::revertToLastCommit()
{
  if ( DEBG ==1 ) fprintf( OutputFile , "Revert to last commit\n" );        // debugging
  
  for(int i=0; i<24; i++) {
          hsTrial[i] = hsCommit[i];
          hsCommit[i] = hsLastCommit[i];
  }
  if ( StrDamage != nullptr ) StrDamage->revertToLastCommit();
  if ( StfDamage != nullptr ) StfDamage->revertToLastCommit();
  if ( AccDamage != nullptr ) AccDamage->revertToLastCommit();
  if ( CapDamage != nullptr ) CapDamage->revertToLastCommit();

  return 0;
}


double XC::CloughDamage::getTangent(void) const
{
        if ( DEBG ==1 ) fprintf( OutputFile , "Get tangent\n" );        // debugging
        return hsTrial[2];
}

double XC::CloughDamage::getInitialTangent(void) const
{
        if ( DEBG ==1 ) fprintf( OutputFile , "Get initial tangent\n" );        // debugging
        return elstk;
}

double XC::CloughDamage::getStress(void) const
{
        if ( DEBG ==1 ) fprintf( OutputFile , "Get stress\n" );        // debugging
        return hsTrial[1];
}


double XC::CloughDamage::getStrain(void) const
{
        if ( DEBG ==1 ) fprintf( OutputFile , "Get strain\n" );        // debugging
        return hsTrial[0];
}


int XC::CloughDamage::recvSelf(const CommParameters &cp)
{
        if ( DEBG ==1 ) fprintf( OutputFile , "Receive self\n" );        // debugging
        return 0;
}


int XC::CloughDamage::sendSelf(CommParameters &cp)
{
        if ( DEBG ==1 ) fprintf( OutputFile , "Send self\n" );        // debugging
        return 0;
}


 XC::UniaxialMaterial *XC::CloughDamage::getCopy(void) const
{
        if ( DEBG ==1 ) fprintf( OutputFile , "Get copy\n" );        // debugging
        XC::Vector inp(8);
        
        inp[0]  =         elstk;
        inp[1]  =          fyieldPos;
        inp[2]  =          fyieldNeg;
        inp[3]  =         alpha;
        inp[4]  =         Resfac;
        inp[5]  =         capSlope;
        inp[6]  =         capDispPos;
        inp[7]  =         capDispNeg;
  
  CloughDamage *theCopy = new XC::CloughDamage(this->getTag(), inp ,StrDamage, StfDamage, AccDamage, CapDamage);
  
  for (int i=0; i<24; i++) {
    theCopy->hsTrial[i] = hsTrial[i];
        theCopy->hsCommit[i] = hsCommit[i];
    theCopy->hsLastCommit[i] = hsLastCommit[i];
  }
  
  return theCopy;
}


int XC::CloughDamage::setTrialStrain( double d, double strainRate)
  {
        if ( DEBG ==1 ) fprintf( OutputFile , "Set trial displacement\n" );        // debugging

        int kon,Unl;

        double ekP,ek,ekunload,sp,sn,dP,fP,f;
        double deltaD,f1,f2,dmax,dmin,fmax,fmin,ekt;
        double betas,betak,betaa;
        double Enrgtot,Enrgc,dyPos,dyNeg;
        double fyPos,fyNeg;
        double betad,cpPos,cpNeg;
        double dlstPos,flstPos,dlstNeg,flstNeg,ekc,tst,ekexcurs;
        double dCap1Pos,dCap2Pos,dCap1Neg,dCap2Neg;
        double ekhardNeg,ekhardPos,alphaPos,alphaNeg;
        double fCapRefPos,fCapRefNeg;
        
        //        Relationship between basic variables and hsTrial array
        int flagDeg= 0;
        int flgstop;
        Unl = 1;

        dP                        = hsCommit[0];
        fP                        = hsCommit[1];
        ekP                        = hsCommit[2];
        ekunload        = hsCommit[3];
        ekexcurs        = hsCommit[4];
        Enrgtot                = hsCommit[5];
        Enrgc                = hsCommit[6];
        sp                        = hsCommit[7];
        sn                        = hsCommit[8];
        kon                        = (int) hsCommit[9];
        dmax                = hsCommit[10];
        dmin                = hsCommit[11];
        fyPos                = hsCommit[12];
        fyNeg                = hsCommit[13];
        cpPos                = hsCommit[14];
        cpNeg                = hsCommit[15];
        dlstPos                = hsCommit[16];
        flstPos                = hsCommit[17];
        dlstNeg                = hsCommit[18];
        flstNeg                = hsCommit[19];
        alphaPos        = hsCommit[20];
        alphaNeg        = hsCommit[21];
        fCapRefPos        = hsCommit[22];
        fCapRefNeg        = hsCommit[23];

        ekhardPos = alphaPos * elstk;
        ekhardNeg = alphaNeg * elstk;
        deltaD = d-dP;

                betas = betak = betaa = betad = 0.0;
        //        Loop to initialize parameters 
        
        if ( kon == 0 ) {
                if( deltaD >= 0.0) {
                        kon = 1;
                }
                else {
                        kon = 2;
                }
        }
        
        //        STARTS BIG LOOP
        //        Positive Delta indicating loading
        
        if ( deltaD >= 0.0 ) {
                
                if ( kon==2 ) {
                        kon = 1;
                        Unl = 0;

                        if( StfDamage != nullptr ) {
                
                                betak = StfDamage->getDamage();
                                if( betak >= 1.0 ) {
                                        std::cerr << "Total loss for stiffness degradation\n";
                                        betak = 1.0;
                                }
                                ekunload = ekexcurs * ( 1 - betak );
                                if( ekunload <= ekhardNeg ) flgstop=1;
                        }
                        
                        //        Determination of sn according to the hysteresis status
                        if( ekunload <= 1.e-7 ) flgstop=1;
                        
                        if( fP < 0.0) {
                                tst = dP - fP / ekunload;
                                if( fabs(dmax-dyieldPos) >= 1.e-10 && fabs(tst) <= 1.e-10) {
                                        sn = 1.e-9;
                                }
                                else {
                                        sn = dP - fP / ekunload;
                                }
                        }
            if( fabs(dmin-dP) <= 1.e-10 ) sp = sn + 1.0e-10;
                }
                
                //        LOADING
                //        Push envelope
                
                if ( d >= dmax ) {
                        this->envelPosCap (fyPos,alphaPos,capSlope,cpPos,d,&f,&ek);
                        dmax = d;
                        fmax = f;
                        dlstPos = dmax+ 1.0e-10;
                        flstPos = f;
                }
                else if ( fabs(sn) > 1.0e-10) {
                        this->envelPosCap (fyPos,alphaPos,capSlope,cpPos,dmax,&fmax,&ekt);
                        if ( d<=sn) {
                                ek = ekunload;
                                f  = fP+ek*deltaD;
                                if ( Unl == 0 && fabs(ek-ekP) > 1.0e-10 && dP != dmin) {
                                        dlstNeg = dP;
                                        flstNeg = fP;
                                }
                        }
                        else {
                                ek = fmax/(dmax-sn);
                                
                                if (ek>=ekunload) {
                                        flgstop = 1;
                                        std::cerr << "Unloading stiffness < reloading stiff";
                                }
                                
                                f2 = ek *( d - sn );
                                if( dlstPos > sn && dlstPos < dmax ) {
                                        ekc = flstPos / ( dlstPos - sn );
                                        if( ekc > ek && flstPos < fmax ) {
                                                
                                                if( d < dlstPos ) {
                                                        ek = flstPos/(dlstPos-sn);
                                                        f2 = ek*(d-sn);
                                                }
                                                else {
                                                        ek = (fmax-flstPos)/(dmax-dlstPos);
                                                        f2 = flstPos+ek*(d-dlstPos);
                                                }
                                        }
                                }
                                
                                f1 = fP + ekunload * deltaD;
                                f = (f1 < f2) ? f1 : f2;
                                
                                if ( fabs(f-f1) < 1.0e-10 ) ek=ekunload;
                        }
                }
                else {
                        if ( d > 0.0 ) {
                                this->envelPosCap (fyPos,alphaPos,capSlope,cpPos,d,&f,&ek);
                        }
                        else {
                                this->envelNegCap (fyNeg,alphaNeg,capSlope,cpNeg,d,&f,&ek);
                        }
                }
        }
        else {
                if (kon==1) {
                        kon = 2;
                        Unl = 0;
                        
                        if( StfDamage != nullptr ) {
        
                                betak = StfDamage->getDamage();
                                if( betak >= 1.0 ) {
                                        std::cerr << "Total loss for stiffness degradation\n";
                                        betak = 1.0;
                                }
                                ekunload = ekexcurs*(1-betak);
                                if(ekunload<=ekhardPos) flgstop=1;
                        }
                        
                        //        Determination of sn according to the hysteresis status
                        
                        if( fP > 0.0 ) {
                                tst = dP-fP/ekunload;
                                if( fabs(dmin-dyieldNeg) >= 1.e-10 && fabs(tst) <= 1.e-10 ) {
                                        sp = 1.e-9;
                                }
                                else {
                                        sp = dP-fP/ekunload;
                                }
                        }
                        
                        if( fabs(dmax-dP) <= 1.e-10 ) sn=sp-1.0e-10;
                }
                
                //        UNLOADING
                //        Push envelope
                
                if (d <= dmin) {
                        this->envelNegCap (fyNeg,alphaNeg,capSlope,cpNeg,d,&f,&ek);
                        dmin = d;
                        fmin = f;
                        dlstNeg = dmin - 1.0e-10;
                        flstNeg = f;
                        
                }
                else if ( fabs(sp) > 1.0e-10) {
                        this->envelNegCap (fyNeg,alphaNeg,capSlope,cpNeg,dmin,&fmin,&ekt);
                        if ( d>=sp) {
                                ek = ekunload;
                                f = fP+ek*deltaD;
                                if( Unl==0 && fabs(ek-ekP) > 1.0e-10 && dP != dmax ) {
                                        dlstPos = dP;
                                        flstPos = fP;
                                }
                        }
                        else {
                                ek = fmin/(dmin-sp);
                                
                                if ( ek >= ekunload ) {
                                        flgstop = 1;
                                        std::cerr << "Unloading stiffness < reloading stiff\n";
                                }
                                
                                f2 = ek * ( d - sp );
                                if( dlstNeg < sp && dlstNeg > dmin ) {
                                        ekc = flstNeg / ( dlstNeg - sp );
                                        
                                        if( ekc > ek && flstNeg > fmin ) {
                                                if( d > dlstNeg ) {
                                                        ek = flstNeg / ( dlstNeg - sp );
                                                        f2 = ek * ( d - sp );
                                                }
                                                else {
                                                        ek = ( fmin - flstNeg ) / ( dmin - dlstNeg );
                                                        f2 = flstNeg + ek * ( d - dlstNeg );
                                                }
                                        }
                                }
                                
                                f1 = fP + ekunload * deltaD;
                                f = (f1 > f2) ? f1 : f2;
                                if ( fabs(f-f1) < 1.e-10 ) ek=ekunload;
                        }
                }
                else {
                        if ( d > 0.0 ) {
                                this->envelPosCap (fyPos,alphaPos,capSlope,cpPos,d,&f,&ek);
                        }
                        else {
                                this->envelNegCap (fyNeg,alphaNeg,capSlope,cpNeg,d,&f,&ek);
                        }
                }
        }
        
        //        ENDS BIG LOOP ---------------------------------------------------
        
        //        DAMAGE CALCULATIONS ---------------------------------------------
        // Calling the damage object
        if ( StfDamage != nullptr ) { 
                betak = StrDamage->getDamage();
                if( fabs(betak) >= 1.0) betak = 1.0;
        }
        
        if ( StrDamage != nullptr ) { 
                betas = StrDamage->getDamage();
                if( fabs(betas) >= 1.0) betas = 1.0;
        }

        if ( AccDamage != nullptr ) {
                betaa = AccDamage->getDamage();
                if( fabs(betaa) >= 1.0) betaa = 1.0;
        }

        if ( CapDamage != nullptr ) {
                betad = CapDamage->getDamage();
                if( fabs(betad) >= 1.0) betad = 1.0;
        }

        //        Flag to deteriorate parameters on the opposite side of the loop --        

        if( f * fP < 0.0) {
                if( fP >0.0 && dmax >dyieldPos) flagDeg = 1;        // positive half cycle
                if( fP < 0.0 && dmin < dyieldNeg) flagDeg = 2;        // negative half cycle 
        }        

        //        UPDATING OF DETERIORATION PARAMETERS ----------------------------
        //        Check the remaining capacity of the system
        
        if( flagDeg == 1 || flagDeg == 2 ) {

                //        Update beta values for strength, acc. stiff. and capping

                if( StrDamage != nullptr ) betas = StrDamage->getDamage();
                if( betas>=1.0 ) {
                        std::cerr << "Total loss for strength degradation\n";
                        betas = 1.0;
                }
                if( AccDamage != nullptr ) betaa = AccDamage->getDamage();
                if( betaa>=1.0 ) {
                        std::cerr << "Total loss for accelerated stiffness degradation\n";
                        betaa = 1.0;
                }
                if( CapDamage != nullptr ) betad = CapDamage->getDamage();
                if( betad>=1.0 ) {
                        std::cerr << "Total loss for capping degradation\n";
                        betad = 1.0;
                }
                //        Update values for the next half cycle
                ekexcurs = ekunload;
                Enrgc = 0.0;
                
                //        Deteriorate parameters for the next half cycle
                
                if( deltaD < 0.0 ) {
                        
                        fyNeg = fyNeg * ( 1 - betas );
                        alphaNeg = alphaNeg * ( 1 - betas );
                        fCapRefNeg = fCapRefNeg * ( 1 - betad );
                        dmin = dmin * ( 1 + betaa );
                        
                        dyNeg = fyNeg / elstk;
                        ekhardNeg = alphaNeg * elstk;
                        
                        dCap1Neg = fCapRefNeg/(elstk-capSlope*elstk);
                        dCap2Neg = (fCapRefNeg+ekhardNeg*dyNeg-fyNeg)/(ekhardNeg-capSlope*elstk);
                        cpNeg = ( dCap1Neg < dCap2Neg ) ? dCap1Neg : dCap2Neg;
                }
                else {
                        fyPos = fyPos * ( 1 - betas );
                        alphaPos = alphaPos * ( 1 - betas );
                        fCapRefPos = fCapRefPos * ( 1 - betad );
                        dmax = dmax * ( 1 + betaa );

                        dyPos = fyPos / elstk;
                        ekhardPos = alphaPos * elstk;

                        dCap1Pos = fCapRefPos / ( elstk - capSlope * elstk );
                        dCap2Pos = (fCapRefPos+ekhardPos*dyPos-fyPos)/(ekhardPos-capSlope*elstk);
                        cpPos= (dCap1Pos > dCap2Pos ) ? dCap1Pos : dCap2Pos;
                }
                flagDeg = 0;
        }
        
        // Relationship between basic variables and hsTrial array        for next cycle
        
        hsTrial[0] = d;
        hsTrial[1] = f;
        hsTrial[2] = ek;
        hsTrial[3] = ekunload;
        hsTrial[4] = ekexcurs;
        hsTrial[5] = Enrgtot;
        hsTrial[6] = Enrgc;
        hsTrial[7] = sp;
        hsTrial[8] = sn;
        hsTrial[9] = (double) kon;
        hsTrial[10] = dmax;
        hsTrial[11] = dmin;
        hsTrial[12] = fyPos;
        hsTrial[13] = fyNeg;
        hsTrial[14] = cpPos;
        hsTrial[15] = cpNeg;
        hsTrial[16] = dlstPos;
        hsTrial[17] = flstPos;
        hsTrial[18] = dlstNeg;
        hsTrial[19] = flstNeg;
        hsTrial[20] = alphaPos;
        hsTrial[21] = alphaNeg;
        hsTrial[22] = fCapRefPos;
        hsTrial[23] = fCapRefNeg;
        
        return 0;
}


int XC::CloughDamage::commitState()
{
  if ( DEBG ==1 ) fprintf( OutputFile , "Commit State\n" );        // debugging
  int i;
  for( i=0; i<24; i++ ) {
          hsLastCommit[i] = hsCommit[i];
          hsCommit[i] = hsTrial[i];
  }

            // Calling the damage object
        XC::Vector InforForDamage(3);
        InforForDamage(0) = hsCommit[0];
        InforForDamage(1) = hsCommit[1];
        InforForDamage(2) = hsCommit[3];

        if ( StfDamage != nullptr ) { 
                StfDamage->setTrial(InforForDamage);
                StfDamage->commitState();
        }

        InforForDamage(2) = 0.0;

        if ( StrDamage != nullptr ) { 
                StrDamage->setTrial(InforForDamage);
                StrDamage->commitState();
        }

        if ( AccDamage != nullptr ) {
                AccDamage->setTrial(InforForDamage);
                AccDamage->commitState();
        }
        if ( CapDamage != nullptr ) {
                CapDamage->setTrial(InforForDamage);
                CapDamage->commitState();
        }

  this->recordInfo();
  
  return 0;
}


void XC::CloughDamage::recordInfo(int cond )
{

}


void XC::CloughDamage::envelPosCap( double fy, double alphaPos, double alphaCap,
                            double cpDsp, double d, double *f, double *ek )
{
  
  double dy,Res,rcap,dres;
  
  dy = fy / elstk;
  
  if (dy < cpDsp)
    {
      Res = Resfac * fyieldPos;
      rcap = fy+alphaPos * elstk * ( cpDsp - dy );
      dres = cpDsp + ( Res - rcap ) / ( alphaCap * elstk );
      
      if ( d < 0.0 )  
        {
          *f = 0.0;
          *ek = 0.0;
        }
      else
        {
          if ( d <= dy )
            {
              *ek = elstk;
              *f = (*ek) * d;
            }
          else
            {
              if( d <= cpDsp )
                {
                  *ek = elstk * alphaPos;
                  *f = fy + (*ek) * ( d - dy );
                }
              else
                {
                  if( d <=  dres )
                    {
                      *ek = alphaCap * elstk;
                      *f = rcap + (*ek) * ( d - cpDsp );
                    }
                  else
                    {
                      *ek = 0;
                      // ek = 1.e-10;
                      *f = Res + d * (*ek);
                    }
                }
            }
        }
    }
  else
    {
      rcap = elstk * cpDsp;
      Res = Resfac * rcap;
      dres = cpDsp + ( Res - rcap ) / ( alphaCap * elstk );
      
      if ( d < 0.0 ) 
        {
          *f = 0.0;
          *ek = 0.0;
        }
      else
        {
          if( d <= cpDsp )
            {
              *ek = elstk;
              *f = (*ek) * d;
            }
          else
            {
              if( d <= dres )
                {
                  *ek = alphaCap * elstk;
                  *f = rcap + (*ek) * ( d - cpDsp );
                }
              else
                {
                  *ek = 0;
                  // ek = 1.e-10;
                  *f = Res + d * (*ek);
                }
            }
        }
    }
  
  return;
}


void XC::CloughDamage::envelNegCap( double fy, double alphaNeg, double alphaCap,
                            double cpDsp, double d, double *f, double *ek)
{
  
  double dy,Res,rcap,dres;
  
  dy = fy / elstk;
  
  if( dy > cpDsp )
    {
      
      Res = Resfac * fyieldNeg;
      rcap = fy + alphaNeg * elstk * ( cpDsp - dy );
      dres = cpDsp + ( Res - rcap ) / ( alphaCap * elstk );
      
      if (d > 0.0) 
        {
          *f = 0.0;
          *ek = 0.0;
        }
      else
        {
          if ( d >= dy )
            {
              *ek = elstk;
              *f = (*ek) * d;
            }
          else
            {
              if ( d >= cpDsp )
                {
                  *ek = elstk * alphaNeg;
                  *f = fy + (*ek) * ( d - dy );
                }
              else
                {
                  if ( d >= dres )
                    {
                      *ek = elstk * alphaCap;
                      *f = rcap + (*ek) * ( d - cpDsp );
                    }
                  else
                    {
                      *ek = 0;
                      // *ek = 1.d-10
                      *f = Res + (*ek) * d;
                    }
                }
            }
        }
    }
  else
    {
      rcap = elstk * cpDsp;
      Res = Resfac * rcap;
      dres = cpDsp + ( Res - rcap ) / ( alphaCap * elstk );
      
      if (d > 0.0) 
        {
          *f = 0.0;
          *ek = 0.0;
        }
      else
        {
          if ( d >= cpDsp )
            {
              *ek = elstk;
              *f = (*ek) * d;
            }
          else
            {
              if ( d >= dres )
                {
                  *ek = elstk * alphaCap;
                  *f = rcap + (*ek) * ( d - cpDsp );
                }
              else
                {
                  *ek = 0;
                  // *ek = 1.e-10;
                  *f = Res + (*ek) * d;
                }
            }
        }
    }
  return;
}
