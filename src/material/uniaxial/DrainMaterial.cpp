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
                                                                        
// $Revision: 1.5 $
// $Date: 2003/04/02 22:02:42 $
// $Source: /usr/local/cvs/OpenSees/SRC/material/uniaxial/DrainMaterial.cpp,v $
                                                                        
// Written: MHS
// Created: June 2001
//
// Description: This file contains the class definition for 
// DrainMaterial. DrainMaterial wraps a Drain spring element subroutine
// and converts it to the XC::UniaxialMaterial interface for use in
// zero length elements, beam sections, or anywhere else
// UniaxialMaterials may be used.
//
// For more information see the Drain-2DX user guide:
//    Allahabadi, R.; Powell, G. H.
//    UCB/EERC-88/06, Berkeley: Earthquake Engineering Research Center,
//    University of California, Mar. 1988, 1 vol.

#include <material/uniaxial/DrainMaterial.h>
#include <utility/matrix/Vector.h>
#include <utility/matrix/ID.h>

#include <cstdlib>
#include <cfloat>

XC::DrainMaterial::DrainMaterial(int tag, int classTag, int nhv, int ndata, double b)
  :UniaxialMaterial(tag,classTag),epsilon(0.0), epsilonDot(0.0), sigma(0.0), tangent(0.0),
   matParams(ndata), hstv(2*nhv,0.0), numData(ndata), numHstv(nhv),
   epsilonP(0.0), sigmaP(0.0), tangentP(0.0), beto(b)
  {
    if(numHstv < 0)
      numHstv = 0;
    
    if(numData < 0)
      numData = 0;
  
    // determine initial tangent
    this->invokeSubroutine();
    initialTangent = tangent;
  }

int XC::DrainMaterial::setTrialStrain(double strain, double strainRate)
  {
        // Store the strain
        epsilon = strain;
        epsilonDot = strainRate;

        // Invoke Drain subroutine
        return this->invokeSubroutine();
  }

int XC::DrainMaterial::setTrial(double strain, double &stress, double &stiff, double strainRate)
  {
        // Store the strain
        epsilon = strain;
        epsilonDot = strainRate;

        // Invoke Drain subroutine
        int res = this->invokeSubroutine();

        stress = sigma;
        stiff = tangent;

        return res;
  }

double XC::DrainMaterial::getStrain(void) const
  { return epsilon; }

double XC::DrainMaterial::getStrainRate(void) const
  { return epsilonDot; }

double XC::DrainMaterial::getStress(void) const
  { return sigma; }

double XC::DrainMaterial::getTangent(void) const
  { return tangent; }

double XC::DrainMaterial::getInitialTangent(void) const
  { return initialTangent; }

double XC::DrainMaterial::getDampTangent(void) const
  {
    // Damping computed here using the last committed tangent
    // rather than the initial tangent!
    return beto*tangentP;
  }

int XC::DrainMaterial::commitState(void)
  {
        // Set committed values equal to corresponding trial values
        for (int i = 0; i < numHstv; i++)
                hstv[i] = hstv[i+numHstv];

        epsilonP = epsilon;
        sigmaP   = sigma;
        tangentP = tangent;

        return 0;
  }

int XC::DrainMaterial::revertToLastCommit(void)
  {
        // Set trial values equal to corresponding committed values
        for (int i = 0; i < numHstv; i++)
                hstv[i+numHstv] = hstv[i];

        epsilon = epsilonP;
        sigma   = sigmaP;
        tangent = tangentP;

        return 0;
  }

int XC::DrainMaterial::revertToStart(void)
  {
        // Set all trial and committed values to zero
        for (int i = 0; i < 2*numHstv; i++)
                hstv[i] = 0.0;

        epsilonP = 0.0;
        sigmaP   = 0.0;
        tangentP = 0.0;

        return 0;
  }

// WARNING -- if you wish to override any method in this base class, you must
// also override the getCopy method to return a pointer to the derived class!!!
XC::UniaxialMaterial *XC::DrainMaterial::getCopy(void) const
  { return new DrainMaterial(*this); }


//! @brief Send its members through the channel being passed as parameter.
int XC::DrainMaterial::sendData(CommParameters &cp)
  {
    int res= UniaxialMaterial::sendData(cp);
    res+= cp.sendDoubles(epsilon,epsilonDot,sigma,tangent,getDbTagData(),CommMetaData(3));
    res+= cp.sendVector(matParams,getDbTagData(),CommMetaData(4));
    res+= cp.sendVector(hstv,getDbTagData(),CommMetaData(5));
    res+= cp.sendInts(numData,numHstv,getDbTagData(),CommMetaData(6));
    res+= cp.sendDoubles(epsilonP,sigmaP,tangentP,beto,initialTangent,getDbTagData(),CommMetaData(7));
    return res;
  }

//! @brief Receives its members through the channel being passed as parameter.
int XC::DrainMaterial::recvData(const CommParameters &cp)
  {
    int res= UniaxialMaterial::recvData(cp);
    res+= cp.receiveDoubles(epsilon,epsilonDot,sigma,tangent,getDbTagData(),CommMetaData(3));
    res+= cp.receiveVector(matParams,getDbTagData(),CommMetaData(4));
    res+= cp.receiveVector(hstv,getDbTagData(),CommMetaData(5));
    res+= cp.receiveInts(numData,numHstv,getDbTagData(),CommMetaData(6));
    res+= cp.receiveDoubles(epsilonP,sigmaP,tangentP,beto,initialTangent,getDbTagData(),CommMetaData(7));
    return res;
  }

//! @brief Sends object through the channel being passed as parameter.
int XC::DrainMaterial::sendSelf(CommParameters &cp)
  {
    setDbTag(cp);
    const int dataTag= getDbTag();
    inicComm(8);
    int res= sendData(cp);

    res+= cp.sendIdData(getDbTagData(),dataTag);
    if(res < 0)
      std::cerr << getClassName() << "sendSelf() - failed to send data\n";
    return res;
  }

//! @brief Receives object through the channel being passed as parameter.
int XC::DrainMaterial::recvSelf(const CommParameters &cp)
  {
    inicComm(8);
    const int dataTag= getDbTag();
    int res= cp.receiveIdData(getDbTagData(),dataTag);

    if(res<0)
      std::cerr << getClassName() << "::recvSelf - failed to receive ids.\n";
    else
      {
        //setTag(getDbTagDataPos(0));
        res+= recvData(cp);
        if(res<0)
          std::cerr << getClassName() << "::recvSelf - failed to receive data.\n";
      }
    return res;
  }
    
void XC::DrainMaterial::Print(std::ostream &s, int flag) const
  {
        s << "DrainMaterial, type: ";
        
        switch (this->getClassTag()) {
        case MAT_TAG_DrainHardening:
                s << "Hardening" << std::endl;
                break;
        case MAT_TAG_DrainBilinear:
                s << "Bilinear" << std::endl;
                break;
        case MAT_TAG_DrainClough1:
                s << "Clough1" << std::endl;
                break;
        case MAT_TAG_DrainClough2:
                s << "Clough2" << std::endl;
                break;
        case MAT_TAG_DrainPinch1:
                s << "Pinch1" << std::endl;
                break;
        // Add more cases as needed

        default:
                s << "Material identifier = " << this->getClassTag() << std::endl;
                break;
        }
  }

// Declarations for the Hardening subroutines
extern "C" int fill00_(double *, double *hstv, double *stateP);
extern "C" int resp00_(int *kresis, int *ksave, int *kgem, int *kstep,
                                                                int *ndof, int *kst, int *kenr,
                                                                double *ener, double *ened, double *enso, double *beto,
                                                                double *relas, double *rdamp, double *rinit,
                                                                double *ddise, double *dise, double *vele);
extern "C" int stif00_(int *kstt, int *ktype, int *ndof, double *fk);
extern "C" int get00_(double *hstv);


// I don't know which subroutines to call, so fill in the XX for XC::Bilinear later -- MHS
// Declarations for the XC::Bilinear subroutines
//extern "C" int fillXX_(double *, double *hstv, double *stateP);
//extern "C" int respXX_(int *kresis, int *ksave, int *kgem, int *kstep,
//                                                                int *ndof, int *kst, int *kenr,
//                                                                double *ener, double *ened, double *enso, double *beto,
//                                                                double *relas, double *rdamp, double *rinit,
//                                                                double *ddise, double *dise, double *vele);
//extern "C" int stifXX_(int *kstt, int *ktype, int *ndof, double *fk);
//extern "C" int getXX_(double *hstv);


// I don't know which subroutines to call, so fill in the XX for Clough1 later -- MHS
// Declarations for the Clough1 subroutines
//extern "C" int fillXX_(double *, double *hstv, double *stateP);
//extern "C" int respXX_(int *kresis, int *ksave, int *kgem, int *kstep,
//                                                                int *ndof, int *kst, int *kenr,
//                                                                double *ener, double *ened, double *enso, double *beto,
//                                                                double *relas, double *rdamp, double *rinit,
//                                                                double *ddise, double *dise, double *vele);
//extern "C" int stifXX_(int *kstt, int *ktype, int *ndof, double *fk);
//extern "C" int getXX_(double *hstv);


// I don't know which subroutines to call, so fill in the XX for Clough2 later -- MHS
// Declarations for the Clough2 subroutines
//extern "C" int fillXX_(double *, double *hstv, double *stateP);
//extern "C" int respXX_(int *kresis, int *ksave, int *kgem, int *kstep,
//                                                                int *ndof, int *kst, int *kenr,
//                                                                double *ener, double *ened, double *enso, double *beto,
//                                                                double *relas, double *rdamp, double *rinit,
//                                                                double *ddise, double *dise, double *vele);
//extern "C" int stifXX_(int *kstt, int *ktype, int *ndof, double *fk);
//extern "C" int getXX_(double *hstv);


// I don't know which subroutines to call, so fill in the XX for Pinch1 later -- MHS
// Declarations for the Pinch1 subroutines
//extern "C" int fillXX_(double *, double *hstv, double *stateP);
//extern "C" int respXX_(int *kresis, int *ksave, int *kgem, int *kstep,
//                                                                int *ndof, int *kst, int *kenr,
//                                                                double *ener, double *ened, double *enso, double *beto,
//                                                                double *relas, double *rdamp, double *rinit,
//                                                                double *ddise, double *dise, double *vele);
//extern "C" int stifXX_(int *kstt, int *ktype, int *ndof, double *fk);
//extern "C" int getXX_(double *hstv);


// Add more declarations as needed

int XC::DrainMaterial::invokeSubroutine(void)
  {
    // Number of degrees of freedom
    static const int NDOF = 2;

    // Flags sent into RESPXX subroutine
    int kresis = 2;                // Compute static and damping forces
    int ksave  = 0;                // Do not save results
    int kgem   = 0;                // Geometric effects (not used)
    int kstep  = 1;                // Step number (set by subroutine)
    int ndof   = NDOF;        // Number of degrees of freedom
    int kst    = 1;                // Stiffness formation code
    int kenr   = 2;                // Calculate static and dynamic energy

    // Energy terms computed in RESPXX subroutine
    double ener = 0.0;        // Change in elasto-plastic energy
    double ened = 0.0;        // Change in damping energy
    double enso = 0.0;        // Change in second-order energy (not used)

    // Force terms computed in RESPXX subroutine
    static double relas[NDOF];        // Resisting force vector
    static double rdamp[NDOF];        // Damping force vector
    static double rinit[NDOF];        // Initial force vector (not used)

    // Total displacement vector
    static double dise[NDOF];
    dise[0] = 0.0;
    dise[1] = epsilon;

    // Incremental displacement vector
    static double ddise[NDOF];
    ddise[0] = 0.0;
    ddise[1] = epsilon-epsilonP;

    // Velocity vector
    static double vele[NDOF];
    vele[0] = 0.0;
    vele[1] = epsilonDot;

    // Fill in committed state array
    static double stateP[3];
    stateP[0] = epsilonP;
    stateP[1] = sigmaP;
    stateP[2] = tangentP;

    // Flags sent into STIFXX subroutine
    int kstt  = 1;                        // Total stiffness
    int ktype = 1;                        // Elastic stiffness only

    // Stiffness computed in STIFXX subroutine
    static double fk[NDOF*NDOF];

    double *dblDataPtr= new double[numData];
    assert(dblDataPtr);
    for(int i= 0;i<numData;i++)
      dblDataPtr[i]= matParams[i];
    double *hstvPtr= new double[2*numHstv];
    assert(hstvPtr);
    for(int i= 0;i<2*numHstv;i++)
      hstvPtr[i]= hstv[i];
    
    switch (this->getClassTag())
      {
      case MAT_TAG_DrainHardening:
        // Fill the common block with parameters and history variables
        fill00_(dblDataPtr, hstvPtr, stateP);

                // Call the response subroutine
                resp00_(&kresis, &ksave, &kgem, &kstep, &ndof, &kst, &kenr,
                        &ener, &ened, &enso, &beto, relas, rdamp, rinit, ddise, dise, vele);
                
                // Call the stiffness subroutine
                stif00_(&kstt, &ktype, &ndof, fk);
                
                // Get the trial history variables
                get00_(&hstvPtr[numHstv]);
                break;

        case MAT_TAG_DrainBilinear:
                // I don't know which subroutines to call, so fill in the XX for Bilinear later -- MHS
                std::cerr << "DrainMaterial::invokeSubroutine -- Bilinear subroutine not yet linked\n"; exit(-1);


                //fillXX_(dblDataPtr, hstvPtr, stateP);
                //respXX_(&kresis, &ksave, &kgem, &kstep, &ndof, &kst, &kenr,
                //        &ener, &ened, &enso, &beto, relas, rdamp, rinit, ddise, dise, vele);
                //stifXX_(&kstt, &ktype, &ndof, fk);
                //getXX_(&hstvPtr[numHstv]);
                break;

        case MAT_TAG_DrainClough1:
                // I don't know which subroutines to call, so fill in the XX for Clough1 later -- MHS
                std::cerr << "XC::DrainMaterial::invokeSubroutine -- Clough1 subroutine not yet linked\n"; exit(-1);

                //fillXX_(dblDataPtr, hstvPtr, stateP);
                //respXX_(&kresis, &ksave, &kgem, &kstep, &ndof, &kst, &kenr,
                //        &ener, &ened, &enso, &beto, relas, rdamp, rinit, ddise, dise, vele);
                //stifXX_(&kstt, &ktype, &ndof, fk);
                //getXX_(&hstvPtr[numHstv]);
                break;

        case MAT_TAG_DrainClough2:
                // I don't know which subroutines to call, so fill in the XX for Clough2 later -- MHS
                std::cerr << "XC::DrainMaterial::invokeSubroutine -- Clough2 subroutine not yet linked\n"; exit(-1);
                          
                //fillXX_(dblDataPtr, hstvPtr, stateP);
                //respXX_(&kresis, &ksave, &kgem, &kstep, &ndof, &kst, &kenr,
                //        &ener, &ened, &enso, &beto, relas, rdamp, rinit, ddise, dise, vele);
                //stifXX_(&kstt, &ktype, &ndof, fk);
                //getXX_(&hstvPtr[numHstv]);
                break;

        case MAT_TAG_DrainPinch1:
                // I don't know which subroutines to call, so fill in the XX for Pinch1 later -- MHS
                std::cerr << "XC::DrainMaterial::invokeSubroutine -- Pinch1 subroutine not yet linked\n"; exit(-1);
                
                //fillXX_(dblDataPtr, hstvPtr, stateP);
                //respXX_(&kresis, &ksave, &kgem, &kstep, &ndof, &kst, &kenr,
                //        &ener, &ened, &enso, &beto, relas, rdamp, rinit, ddise, dise, vele);
                //stifXX_(&kstt, &ktype, &ndof, fk);
                //getXX_(&hstvPtr[numHstv]);
                break;

        // Add more cases as needed

        default:
                std::cerr << "XC::DrainMaterial::invokeSubroutine -- unknown material type\n"; exit(-1);
                return -1;
        }
    for(int i= 0;i<numData;i++)
      matParams[i]= dblDataPtr[i];
    for(int i= 0;i<2*numHstv;i++)
      hstv[i]= hstvPtr[i];

    // Total stress is elastic plus damping force
    sigma = relas[1] + rdamp[1];

    // Get tangent stiffness
    tangent = fk[0];
    return 0;
  }
