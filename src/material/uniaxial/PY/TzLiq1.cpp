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
/* *********************************************************************
**    Module:        TzLiq1.cpp 
**
**    Purpose:        Provide a t-z material that gets pore pressure from a
**                                specified element that contains a PorousFluidSolid.
**
**    Developed by Ross XC::W. Boulanger
**    (C) Copyright 2002, All Rights Reserved.
**
** ****************************************************************** */

// $Revision: 1.0
// $Date: 2002/2/5
// $Source: /OpenSees/SRC/material/uniaxial/TzLiq1.cpp

// Written: RWB
// Created: Feb 2002
// Revision: A
//
// Description: This file contains the class implementation for XC::TzLiq1

#include <material/uniaxial/PY/TzLiq1.h>
#include <material/uniaxial/UniaxialMaterial.h>
#include <material/nD/soil/FluidSolidPorousMaterial.h>
#include <material/nD/NDMaterial.h>
#include <utility/matrix/Vector.h>

#include <cmath>
#include <domain/mesh/element/plane/fourNodeQuad/FourNodeQuad.h>
#include <iostream>
#include "domain/mesh/node/Node.h"

// Controls on internal iteration between spring components
const int TZmaxIterations = 20;
const double TZtolerance = 1.0e-12;

int XC::TzLiq1::loadStage = 0;
XC::Vector XC::TzLiq1::stressV3(3);

/////////////////////////////////////////////////////////////////////
//        Constructor with data

XC::TzLiq1::TzLiq1(int tag, int classtag, int tz_type, double t_ult, double z_50,
                double dash_pot, int solid_elem1, int solid_elem2, Domain *the_Domain)
:TzSimple1(tag, classtag, tz_type, t_ult, z_50, dash_pot),
solidElem1(solid_elem1), solidElem2(solid_elem2), theDomain(the_Domain)
{
        // Initialize TzSimple variables and history variables
        //
        this->revertToStart();
    initialTangent = Tangent;
}

//! @brief Constructor.
XC::TzLiq1::TzLiq1(int tag, int classtag)
  :TzSimple1(tag,classtag), solidElem1(0), solidElem2(0), theDomain(0)
  {}

/////////////////////////////////////////////////////////////////////
//! @brief Default constructor
XC::TzLiq1::TzLiq1(void)
:TzSimple1(), solidElem1(0), solidElem2(0), theDomain(0)
  {}


/////////////////////////////////////////////////////////////////////
int XC::TzLiq1::setTrialStrain (double newz, double zRate)
{
        // Call the base class XC::TzSimple1 to take care of the basic t-z behavior.
        //
        XC::TzSimple1::setTrialStrain(newz, zRate);
        Tz = newz;

        // Reset mean consolidation stress if loadStage switched from 0 to 1
        //                Note: currently assuming y-axis is vertical, and that the
        //                out-of-plane normal stress equals sigma-xx.
        //
        if(lastLoadStage ==0 && loadStage ==1){

                meanConsolStress = getEffectiveStress();
                if(meanConsolStress == 0.0){
                        std::cerr << "WARNING meanConsolStress is 0 in solid elements, ru will divide by zero";
                        std::cerr << "TzLiq1: " << std::endl;
                        std::cerr << "Adjacent solidElems: " << solidElem1 << ", " << solidElem2 << std::endl;
                        exit(-1);
                }
        }
        lastLoadStage = loadStage;

        // Obtain the mean effective stress from the adjacent solid elements,
        //    and calculate ru for scaling of t-z base relation.
        //
        if(loadStage == 1) {
                double meanStress = getEffectiveStress();
                Tru = 1.0 - meanStress/meanConsolStress;
                if(Tru > 0.999) Tru = 0.999;
        }
        else {
                Tru = 0.0;
        }

        // Call the base class XC::TzSimple1 to get basic t-z response,
        //
        double baseT = XC::TzSimple1::getStress();
        double baseTangent = XC::TzSimple1::getTangent();

        // Check: Only update Hru if not yet converged (avoiding small changes in Tru).
        //
        if(Tz !=Cz || Tt !=Ct) Hru = Tru;

        // During dilation of the soil (ru dropping), provide a stiff transition
        //   between the old and new scaled t-z relations. This avoids illogical
        //   hardening of the t-z relation (i.e., negative stiffnesses).
        //
        if (Tru < Cru){

                maxTangent = (matCapacity/v50)*(1.0-Cru);

                //  If unloading, follow the old scaled t-z relation until t=0.
                //
                if(Cz>0.0 && Tz<Cz && baseT>0.0){
                        Hru = Cru;
                }
                if(Cz<0.0 && Tz>Cz && baseT<0.0){
                        Hru = Cru;
                }
                
                //  If above the stiff transition line (between Tru & Cru scaled surfaces)
                //
                double zref = Cz + baseT*(Cru-Tru)/maxTangent;
                if(Cz>0.0 && Tz>Cz && Tz<zref){
                        Hru = 1.0 - (Ct + (Tz-Cz)*maxTangent)/baseT;
                }
                if(Cz<0.0 && Tz<Cz && Tz>zref){
                        Hru = 1.0 - (Ct + (Tz-Cz)*maxTangent)/baseT;
                }
        }

        //  Now set the tangent and Tt values accordingly
        //

        Tt = baseT*(1.0-Hru);
        if(Hru==Cru || Hru==Tru){
                Tangent = (1.0-Hru)*baseTangent;
        }
        else {
                Tangent = maxTangent;
        }

        return 0;
}
/////////////////////////////////////////////////////////////////////
double XC::TzLiq1::getStress(void) const
  {
        double dashForce = getStrainRate()*this->getDampTangent();

        // Limit the combined force to tult*(1-ru).
        //
        double tmax = (1.0-TZtolerance)*matCapacity*(1.0-Hru);
        if(fabs(Tt + dashForce) >= tmax)
                return tmax*(Tt+dashForce)/fabs(Tt+dashForce);
        else return Tt + dashForce;

  }

/////////////////////////////////////////////////////////////////////
double  XC::TzLiq1::getTangent(void) const
  { return Tangent; }
/////////////////////////////////////////////////////////////////////
double XC::TzLiq1::getInitialTangent(void) const
  { return initialTangent; }

/////////////////////////////////////////////////////////////////////
double XC::TzLiq1::getDampTangent(void) const
  {
        // Call the base class XC::TzSimple1 to get basic t-z response,
        //    and then scale by (1-ru).
        //
    const double dampTangent = XC::TzSimple1::getDampTangent();
    return dampTangent*(1.0-Hru);
  }
/////////////////////////////////////////////////////////////////////
double XC::TzLiq1::getStrain(void) const
  { return TzSimple1::getStrain(); }
/////////////////////////////////////////////////////////////////////
double XC::TzLiq1::getStrainRate(void) const
  { return TzSimple1::getStrainRate(); }

/////////////////////////////////////////////////////////////////////
int 
XC::TzLiq1::commitState(void)
{
        // Call the XC::TzSimple1 base function to take care of details.
        //
        XC::TzSimple1::commitState();
        Cz = Tz;
        Ct = Tt;
        Cru= Hru;
        
    return 0;
}

/////////////////////////////////////////////////////////////////////
int 
XC::TzLiq1::revertToLastCommit(void)
{
        // reset to committed values
    //
        XC::TzSimple1::revertToLastCommit();
        Tz = Cz;
        Tt = Ct;
        Hru= Cru;

        return 0;
}

/////////////////////////////////////////////////////////////////////
int 
XC::TzLiq1::revertToStart(void)
{
        // Call the XC::TzSimple1 base function to take care of most details.
        //
        XC::TzSimple1::revertToStart();
        Tz = 0.0;
        Tt = 0.0;
        maxTangent = (matCapacity/v50);

        // Excess pore pressure ratio and pointers
        //
        Tru = 0.0;
        Hru = 0.0;
        meanConsolStress = -matCapacity;
        lastLoadStage = 0;
        loadStage = 0;
        elemFlag.assign("NONE");

        // Now get all the committed variables initiated
        //
        commitState();

    return 0;
}

/////////////////////////////////////////////////////////////////////
double 
XC::TzLiq1::getEffectiveStress(void)
{
        // Default value for meanStress
        double meanStress = meanConsolStress;
        
        // if the elemFlag has not been set yet, then set it
        //
        if(elemFlag.compare("NONE") == 0) {        //string.compare returns zero if equal

                // if theDomain pointer is nonzero, then set pointers to attached soil elements.
                //
                if(theDomain != 0)
                {        
                        Element *theElement1 = theDomain->getElement(solidElem1);
                        Element *theElement2 = theDomain->getElement(solidElem2);
                        if (theElement1 == 0 || theElement2 == 0) {
                                std::cerr << "WARNING solid element not found in getEffectiveStress" << std::endl;
                                std::cerr << "TzLiq1: " << std::endl;
                                std::cerr << "Adjacent solidElems: " << solidElem1 << ", " << solidElem2 << std::endl;
                                exit(-1);
                        }

                        // Check each of the allowable element types, starting with 4NodeQuads
                        theQuad1 = dynamic_cast<FourNodeQuad*>(theElement1);
                        theQuad2 = dynamic_cast<FourNodeQuad*>(theElement2);
                        if(theQuad1 != 0 && theQuad2 != 0) {
                                elemFlag.assign("4NodeQuad");
                        }

                        // Check on each other type of allowable element types, only if no successful yet.
                        if(elemFlag.compare("NONE") == 0) {        //string.compare returns zero if equal

                                // try other elements like, 4NodeQuadUP
                                elemFlag.assign("NONE");
                        }
                        
                        // Check on acceptable soil materials
                        //
                        if(elemFlag.compare("4NodeQuad") == 0) {
                                XC::NDMaterial *theNDM1[4];
                                XC::NDMaterial *theNDM2[4];
                                FluidSolidPorousMaterial *theFSPM1[4];
                                FluidSolidPorousMaterial *theFSPM2[4];
                NDMaterialPhysicalProperties physProp1= theQuad1->getPhysicalProperties();
                NDMaterialPhysicalProperties physProp2= theQuad2->getPhysicalProperties();

                                int dummy = 0;
                                for (int i=0; i<4; i++)
                                  {
                                        theNDM1[i] = physProp1[i];
                                        theNDM2[i] = physProp2[i];
                                        theFSPM1[i] = dynamic_cast<FluidSolidPorousMaterial*>(theNDM1[i]);
                                        theFSPM2[i] = dynamic_cast<FluidSolidPorousMaterial*>(theNDM2[i]);
                                        if(theFSPM1 == 0 || theFSPM2 == 0) dummy = dummy + 1;
                                }
                                if(dummy == 0) elemFlag.append("-FSPM");

                                // Check other acceptable soil types.
                        }

                        if(elemFlag.compare("NONE") == 0) {        // Never obtained a valid pointer set
                                std::cerr << "WARNING: Adjoining solid elements did not return valid pointers";
                                std::cerr << "TzLiq1: " << std::endl;
                                std::cerr << "Adjacent solidElems: " << solidElem1 << ", " << solidElem2 << std::endl;
                                exit(-1);
                                return meanStress;
                        }
        
                }
        }
        
        // Get effective stresses using appropriate pointers in elemFlag not "NONE"
        //
        if(elemFlag.compare("NONE") != 0) {

                if(elemFlag.compare("4NodeQuad-FSPM") == 0) {
                        meanStress = 0.0;
                        XC::Vector *theStressVector = &stressV3;
                        double excessPorePressure = 0.0;
                        XC::NDMaterial *theNDM;
                        FluidSolidPorousMaterial *theFSPM;
                NDMaterialPhysicalProperties physProp1= theQuad1->getPhysicalProperties();
                NDMaterialPhysicalProperties physProp2= theQuad2->getPhysicalProperties();

                        for(int i=0; i < 4; i++)
                          { 
                                *theStressVector = physProp1[i]->getStress();
                                meanStress += 2.0*(*theStressVector)[0] + (*theStressVector)[1];
                                *theStressVector = physProp2[i]->getStress();
                                meanStress += 2.0*(*theStressVector)[0] + (*theStressVector)[1];

                                theNDM = physProp1[i];
                                theFSPM= dynamic_cast<FluidSolidPorousMaterial*>(theNDM);
                                excessPorePressure += (theFSPM->trialExcessPressure);
                                theNDM = physProp2[i];
                                theFSPM= dynamic_cast<FluidSolidPorousMaterial*>(theNDM);
                                excessPorePressure += (theFSPM->trialExcessPressure);
                        }
                        meanStress = meanStress/(2.0*4.0*3.0);
                        excessPorePressure = excessPorePressure/(2.0*4.0);
                        meanStress = meanStress - excessPorePressure;

                        return meanStress;
                }

                else if(elemFlag.compare("4NodeQuadUP-FSPM") == 0) {
                        
                        // expect to later have UP option on quads

                        return meanStress;
                }
                
                else {        // Never found a matching flag
                        std::cerr << "WARNING: Something wrong with specification of elemFlag in getEffectiveStress";
                        std::cerr << "TzLiq1: " << std::endl;
                        std::cerr << "Adjacent solidElems: " << solidElem1 << ", " << solidElem2 << std::endl;
                        exit(-1);
                        return meanStress;
                }
        }

        return meanStress;
}

/////////////////////////////////////////////////////////////////////
int 
XC::TzLiq1::updateParameter(int snum,Information &eleInformation)
{
        // TclUpdateMaterialStageCommand will call this routine with the
        // command:
        //
        //      updateMaterialStage - material tag -stage snum
        //
        // If snum = 0; running linear elastic for soil elements,
        //              so excess pore pressure should be zero.
        // If snum = 1; running plastic soil element behavior,
        //              so this marks the end of the "consol" gravity loading.

        if(snum !=0 && snum !=1){
                std::cerr << "WARNING updateMaterialStage for XC::TzLiq1 material must be 0 or 1";
                std::cerr << std::endl;
                exit(-1);
        }
        loadStage = snum;

        return 0;
}

/////////////////////////////////////////////////////////////////////
 XC::UniaxialMaterial *XC::TzLiq1::getCopy(void) const
{
        // Make a new instance of this class and then assign it "this" to make a copy.
        //
        TzLiq1 *clone;                        // pointer to a XC::TzLiq1 class
        clone = new TzLiq1();        // pointer gets a new instance of XC::TzLiq1
        *clone = *this;                        // the clone (dereferenced pointer) = dereferenced this.
        
        return clone;
}

//! @brief Send object members through the communicator argument.
int XC::TzLiq1::sendData(Communicator &comm)
  {
    int res= TzSimple1::sendData(comm);
    res+= comm.sendDoubles(Tz,Cz,Tt,Ct,Tangent,getDbTagData(),CommMetaData(19));
    res+= comm.sendDoubles(maxTangent,Tru,Cru,Hru,getDbTagData(),CommMetaData(20));
    res+= comm.sendInts(solidElem1,solidElem2,lastLoadStage,getDbTagData(),CommMetaData(21));
    res+= comm.sendDoubles(meanConsolStress,ru,initialTangent,getDbTagData(),CommMetaData(22));
    res+= comm.sendString(elemFlag,getDbTagData(),CommMetaData(23));
    return res;
  }

//! @brief Receives object members through the communicator argument.
int XC::TzLiq1::recvData(const Communicator &comm)
  {
    int res= TzSimple1::recvData(comm);
    res+= comm.receiveDoubles(Tz,Cz,Tt,Ct,Tangent,getDbTagData(),CommMetaData(19));
    res+= comm.receiveDoubles(maxTangent,Tru,Cru,Hru,getDbTagData(),CommMetaData(20));
    res+= comm.receiveInts(solidElem1,solidElem2,lastLoadStage,getDbTagData(),CommMetaData(21));
    res+= comm.receiveDoubles(meanConsolStress,ru,initialTangent,getDbTagData(),CommMetaData(22));
    res+= comm.receiveString(elemFlag,getDbTagData(),CommMetaData(23));
    return res;
  }

/////////////////////////////////////////////////////////////////////
int XC::TzLiq1::sendSelf(Communicator &comm)
  {
    setDbTag(comm);
    const int dataTag= getDbTag();
    inicComm(24);
    int res= sendData(comm);

    res+= comm.sendIdData(getDbTagData(),dataTag);
    if(res < 0)
      std::cerr << getClassName() << "sendSelf() - failed to send data\n";
    return res;
  }

/////////////////////////////////////////////////////////////////////
int XC::TzLiq1::recvSelf(const Communicator &comm)
  {
    inicComm(24);
    const int dataTag= getDbTag();
    int res= comm.receiveIdData(getDbTagData(),dataTag);

    if(res<0)
      std::cerr << getClassName() << "::recvSelf - failed to receive ids.\n";
    else
      {
        res+= recvData(comm);
        if(res<0)
          std::cerr << getClassName() << "::recvSelf - failed to receive data.\n";
      }
    return res;
  }

/////////////////////////////////////////////////////////////////////
void XC::TzLiq1::Print(std::ostream &s, int flag) const
  {
    s << "TzLiq1, tag: " << this->getTag() << std::endl;
    s << "  tzType: " << soilType << std::endl;
    s << "  tult: " << matCapacity << std::endl;
    s << "  z50: " << v50 << std::endl;
    s << "  dashpot: " << dashpot << std::endl;
    s << "  solidElem1: " << solidElem1 << std::endl;
    s << "  solidElem2: " << solidElem2 << std::endl;
  }
