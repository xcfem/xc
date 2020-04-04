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
**    Module:        PyLiq1.cpp 
**
**    Purpose:        Provide a p-y material that gets pore pressure from a
**                                specified element that contains a PorousFluidSolid.
**
**    Developed by Ross XC::W. Boulanger
**    (C) Copyright 2002, All Rights Reserved.
**
** ****************************************************************** */

// $Revision: 1.0
// $Date: 2002/5/15
// $Source: /OpenSees/SRC/material/uniaxial/PyLiq1.cpp

// Written: RWB
// Created: May 2002
// Revision: A
//
// Description: This file contains the class implementation for XC::PyLiq1

#include <material/uniaxial/PY/PyLiq1.h>
#include <material/nD/NDMaterial.h>
#include <utility/matrix/Vector.h>

#include <cmath>
#include <domain/mesh/element/plane/fourNodeQuad/FourNodeQuad.h>
#include <material/uniaxial/UniaxialMaterial.h>
#include <material/nD/soil/FluidSolidPorousMaterial.h>
#include "domain/mesh/node/Node.h"

// Controls on internal iteration between spring components
const int PYmaxIterations = 20;
const double PYtolerance = 1.0e-12;

int XC::PyLiq1::loadStage = 0;
XC::Vector XC::PyLiq1::stressV3(3);

/////////////////////////////////////////////////////////////////////
//! @brief Constructor with data
XC::PyLiq1::PyLiq1(int tag, int classtag, int soil, double p_ult, double y_50,
                double dragratio, double dash_pot, double p_res, 
                int solid_elem1, int solid_elem2, Domain *the_Domain)
:PySimple1(tag, classtag, soil, p_ult, y_50, dragratio, dash_pot),
pRes(p_res), solidElem1(solid_elem1), solidElem2(solid_elem2), theDomain(the_Domain)
{
        // Initialize PySimple variables and history variables
        //
        this->revertToStart();
    initialTangent = Tangent;
}

/////////////////////////////////////////////////////////////////////
// Default constructor
XC::PyLiq1::PyLiq1(int tag, int classtag)
  :PySimple1(tag,classtag), pRes(0.0), solidElem1(0), solidElem2(0), theDomain(0)
  {}

/////////////////////////////////////////////////////////////////////
// Default constructor
XC::PyLiq1::PyLiq1(void)
:PySimple1(), pRes(0.0), solidElem1(0), solidElem2(0), theDomain(0)
{
}

/////////////////////////////////////////////////////////////////////
int XC::PyLiq1::setTrialStrain (double newy, double yRate)
{
        // Call the base class XC::PySimple1 to take care of the basic p-y behavior.
        //
        Ty = newy;
        XC::PySimple1::setTrialStrain(Ty, yRate);

        // Reset mean consolidation stress if loadStage switched from 0 to 1
        //                Note: currently assuming y-axis is vertical, and that the
        //                out-of-plane normal stress equals sigma-xx.
        //
        if(lastLoadStage ==0 && loadStage ==1){

                meanConsolStress = getEffectiveStress();
                if(meanConsolStress == 0.0){
                        std::cerr << "WARNING meanConsolStress is 0 in solid elements, ru will divide by zero";
                        std::cerr << "PyLiq1: " << std::endl;
                        std::cerr << "Adjacent solidElems: " << solidElem1 << ", " << solidElem2 << std::endl;
                        exit(-1);
                }
        }
        lastLoadStage = loadStage;

        // Obtain the mean effective stress from the adjacent solid elements,
        //    and calculate ru for scaling of p-y base relation.
        //
        if(loadStage == 1) {
                double meanStress = getEffectiveStress();
                Tru = 1.0 - meanStress/meanConsolStress;
                if(Tru > 1.0-pRes/matCapacity) Tru = 1.0-pRes/matCapacity;
        }
        else {
                Tru = 0.0;
        }


        // Call the base class XC::PySimple1 to get basic p-y response,
        //
        double baseP = XC::PySimple1::getStress();
        double baseTangent = XC::PySimple1::getTangent();

        // Check: Only update Hru if not yet converged (avoiding small changes in Tru).
        //
        Hru = Tru;
        if(Ty==Cy && Tp==Cp) { Hru = Cru;}

        // During dilation of the soil (ru dropping), provide a stiff transition
        //   between the old and new scaled p-y relations. This avoids illogical
        //   hardening of the p-y relation (i.e., negative stiffnesses).
        //
        if (Hru < Cru){

                maxTangent = (matCapacity/v50)*(1.0-Cru);

                //  If unloading, follow the old scaled p-y relation until p=0.
                //
                if(Cy>0.0 && Ty<Cy && baseP>0.0){
                        Hru = Cru;
                }
                if(Cy<0.0 && Ty>Cy && baseP<0.0){
                        Hru = Cru;
                }
                
                //  If above the stiff transition line (between Tru & Cru scaled surfaces)
                //
                double yref = Cy + baseP*(Cru-Hru)/maxTangent;
                if(Cy>0.0 && Ty>Cy && Ty<yref){
                        Hru = 1.0 - (Cp + (Ty-Cy)*maxTangent)/baseP;
                }
                if(Cy<0.0 && Ty<Cy && Ty>yref){
                        Hru = 1.0 - (Cp + (Ty-Cy)*maxTangent)/baseP;
                }
                if(Hru > Cru) Hru = Cru;
                if(Hru < Tru) Hru = Tru;

        }

        //  Now set the tangent and Tp values accordingly
        //

        Tp = baseP*(1.0-Hru);
//        Tangent = (1.0-Hru)*baseTangent;
        if(Hru==Cru || Hru==Tru){
                Tangent = (1.0-Hru)*baseTangent;
        }
        else {
                Tangent = maxTangent;
        }

        return 0;
}
/////////////////////////////////////////////////////////////////////
double XC::PyLiq1::getStress(void) const
  {
    double dashForce = getStrainRate()*this->getDampTangent();

        // Limit the combined force to pult*(1-ru).
        //
    double pmax = (1.0-PYtolerance)*matCapacity*(1.0-Hru);
//  double pmax = (1.0-PYtolerance)*matCapacity;
    if(fabs(Tp + dashForce) >= pmax)
      return pmax*(Tp+dashForce)/fabs(Tp+dashForce);
    else
      return Tp + dashForce;
  }
/////////////////////////////////////////////////////////////////////
double XC::PyLiq1::getTangent(void) const
  { return this->Tangent; }
/////////////////////////////////////////////////////////////////////
double XC::PyLiq1::getInitialTangent(void) const
  { return initialTangent; }
/////////////////////////////////////////////////////////////////////
double XC::PyLiq1::getDampTangent(void) const
  {
    // Call the base class XC::PySimple1 to get basic p-y response,
    //    and then scale by (1-ru).
    //
    const double dampTangent = XC::PySimple1::getDampTangent();
    return dampTangent*(1.0-Hru);
  }
/////////////////////////////////////////////////////////////////////
double  XC::PyLiq1::getStrain(void) const
  { return PySimple1::getStrain(); }
/////////////////////////////////////////////////////////////////////
double XC::PyLiq1::getStrainRate(void) const
  { return PySimple1::getStrainRate(); }

/////////////////////////////////////////////////////////////////////
int XC::PyLiq1::commitState(void)
{
        // Call the XC::PySimple1 base function to take care of details.
        //
        XC::PySimple1::commitState();
        Cy = Ty;
        Cp = Tp;
        Cru= Hru;
        
    return 0;
}

/////////////////////////////////////////////////////////////////////
int 
XC::PyLiq1::revertToLastCommit(void)
{
        // reset to committed values
    //
        XC::PySimple1::revertToLastCommit();
        Ty = Cy;
        Tp = Cp;
        Hru= Cru;

        return 0;
}

/////////////////////////////////////////////////////////////////////
int XC::PyLiq1::revertToStart(void)
  {
        // Call the XC::PySimple1 base function to take care of most details.
        //
        XC::PySimple1::revertToStart();
        Ty = 0.0;
        Tp = 0.0;
        maxTangent = (matCapacity/v50);
        
        // Excess pore pressure ratio and pointers
        //
        Tru = 0.0;
        Hru = 0.0;
        meanConsolStress = -matCapacity;
        lastLoadStage = 0;
        loadStage = 0;
        if(pRes <= 0.0) pRes = 0.01*matCapacity;
        if(pRes > matCapacity) pRes = matCapacity;
        elemFlag.assign("NONE");

        // Now get all the committed variables initiated
        //
        commitState();

    return 0;
  }

/////////////////////////////////////////////////////////////////////
double XC::PyLiq1::getEffectiveStress(void)
  {
    // Default value for meanStress
    double meanStress = meanConsolStress;
        
    // if the elemFlag has not been set yet, then set it
    //
    if(elemFlag.compare("NONE") == 0)
      {
        //string.compare returns zero if equal
        // if theDomain pointer is nonzero, then set pointers to attached soil elements.
        //
                if(theDomain != 0)
                {        
                        Element *theElement1 = theDomain->getElement(solidElem1);
                        Element *theElement2 = theDomain->getElement(solidElem2);
                        if (theElement1 == 0 || theElement2 == 0) {
                                std::cerr << "WARNING solid element not found in getEffectiveStress" << std::endl;
                                std::cerr << "PyLiq1: " << std::endl;
                                std::cerr << "Adjacent solidElems: " << solidElem1 << ", " << solidElem2 << std::endl;
                                exit(-1);
                        }

                        // Check each of the allowable element types, starting with 4NodeQuads
                        theQuad1 = dynamic_cast<FourNodeQuad *>(theElement1);
                        theQuad2 = dynamic_cast<FourNodeQuad *>(theElement2);
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
                        if(elemFlag.compare("4NodeQuad") == 0)
                          {
                            NDMaterial *theNDM1[4];
                            NDMaterial *theNDM2[4];
                            FluidSolidPorousMaterial *theFSPM1[4];
                            FluidSolidPorousMaterial *theFSPM2[4];
                            NDMaterialPhysicalProperties physProp1= theQuad1->getPhysicalProperties();
                            NDMaterialPhysicalProperties physProp2= theQuad2->getPhysicalProperties();
                            int dummy = 0;
                            for(int i=0; i<4; i++)
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
                                std::cerr << "PyLiq1: " << std::endl;
                                std::cerr << "Adjacent solidElems: " << solidElem1 << ", " << solidElem2 << std::endl;
                                exit(-1);
                                return meanStress;
                        }
        
                }
        }
        
        // Get effective stresses using appropriate pointers in elemFlag not "NONE"
        //
        if(elemFlag.compare("NONE") != 0)
          {
            if(elemFlag.compare("4NodeQuad-FSPM") == 0)
              {
                meanStress = 0.0;
                Vector *theStressVector = &stressV3;
                double excessPorePressure = 0.0;
                NDMaterial *theNDM;
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
            else if(elemFlag.compare("4NodeQuadUP-FSPM") == 0)
              {
                 // expect to later have UP option on quads
                 return meanStress;
              }
            else
              {        // Never found a matching flag
                std::cerr << "WARNING: Something wrong with specification of elemFlag in getEffectiveStress";
                std::cerr << "PyLiq1: " << std::endl;
                std::cerr << "Adjacent solidElems: " << solidElem1 << ", " << solidElem2 << std::endl;
                exit(-1);
                return meanStress;
              }
          }
        return meanStress;
  }

/////////////////////////////////////////////////////////////////////
int 
XC::PyLiq1::updateParameter(int snum,Information &eleInformation)
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
                std::cerr << "WARNING updateMaterialStage for XC::PyLiq1 material must be 0 or 1";
                std::cerr << std::endl;
                exit(-1);
        }
        loadStage = snum;

        return 0;
}

/////////////////////////////////////////////////////////////////////
XC::UniaxialMaterial *XC::PyLiq1::getCopy(void) const
  { return new PyLiq1(*this); }


//! @brief Send object members through the channel being passed as parameter.
int XC::PyLiq1::sendData(Communicator &comm)
  {
    int res= PySimple1::sendData(comm);
    res+= comm.sendDoubles(pRes,Ty,Cy,Tp,Cp,Tangent,getDbTagData(),CommMetaData(19));
    res+= comm.sendDoubles(maxTangent,Tru,Cru,Hru,getDbTagData(),CommMetaData(20));
    res+= comm.sendInts(solidElem1,solidElem2,lastLoadStage,getDbTagData(),CommMetaData(21));
    res+= comm.sendDoubles(meanConsolStress,initialTangent,getDbTagData(),CommMetaData(22));
    res+= comm.sendString(elemFlag,getDbTagData(),CommMetaData(23));
    return res;
  }

//! @brief Receives object members through the channel being passed as parameter.
int XC::PyLiq1::recvData(const Communicator &comm)
  {
    int res= PySimple1::recvData(comm);
    res+= comm.receiveDoubles(pRes,Ty,Cy,Tp,Cp,Tangent,getDbTagData(),CommMetaData(19));
    res+= comm.receiveDoubles(maxTangent,Tru,Cru,Hru,getDbTagData(),CommMetaData(20));
    res+= comm.receiveInts(solidElem1,solidElem2,lastLoadStage,getDbTagData(),CommMetaData(21));
    res+= comm.receiveDoubles(meanConsolStress,initialTangent,getDbTagData(),CommMetaData(22));
    res+= comm.receiveString(elemFlag,getDbTagData(),CommMetaData(23));
    return res;
  }

//! @brief Sends object through the channel being passed as parameter.
int XC::PyLiq1::sendSelf(Communicator &comm)
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

//! @brief Receives object through the channel being passed as parameter.
int XC::PyLiq1::recvSelf(const Communicator &comm)
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
void XC::PyLiq1::Print(std::ostream &s, int flag) const
  {
    s << "PyLiq1, tag: " << this->getTag() << std::endl;
    s << "  soilType: " << soilType << std::endl;
    s << "  pult: " << matCapacity << std::endl;
    s << "  y50: " << v50 << std::endl;
    s << "  drag: " << drag << std::endl;
    s << "  pResidual: " << vRef << std::endl;
    s << "  dashpot: " << dashpot << std::endl;
    s << "  solidElem1: " << solidElem1 << std::endl;
    s << "  solidElem2: " << solidElem2 << std::endl;
  }
