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
**    Module:        TzSimple1.cpp 
**
**    Purpose:        Provide a simple t-z spring for OpenSees.
**
**    Developed by Ross XC::W. Boulanger
**    (C) Copyright 2002, All Rights Reserved.
**
** ****************************************************************** */

// $Revision: 1.0
// $Date: 2002/1/19
// $Source: /OpenSees/SRC/material/uniaxial/TzSimple1.cpp

// Written: RWB
// Created: Jan 2002
// Revision: A
// tested and checked: Boris Jeremic (jeremic@ucdavis.edu) Spring 2002
//
// Description: This file contains the class implementation for XC::TzSimple1

#include <cstdlib>
#include "material/uniaxial/PY/TzSimple1.h"
#include <utility/matrix/Vector.h>

#include <cmath>


// Controls on internal iteration between spring components
const int TZmaxIterations= 20;
const double TZtolerance= 1.0e-12;

/////////////////////////////////////////////////////////////////////
//        Constructor with data

XC::TzSimple1::TzSimple1(int tag,int classtag, int tz_type,double t_ult,double z_50,double dash_pot)
  :PYBase(tag,classtag,tz_type,t_ult,z_50,dash_pot)
  {
    // Initialize TzSimple variables and history variables
    //
    this->revertToStart();
    initialTangent= T.tang();
  }

XC::TzSimple1::TzSimple1(int tag,int classtag)
  :PYBase(tag,classtag)
  {
    // Initialize variables .. WILL NOT WORK AS NOTHING SET
    // this->revertToStart();

    // need to set iterations and tolerance

    // BTW maxIterations and tolerance should not be private variables, they
    // should be static .. all XC::PySimple1 materials share the same values & 
    // these values don't change
  }

/////////////////////////////////////////////////////////////////////
//        Default constructor

XC::TzSimple1::TzSimple1(void)
 :PYBase(0,0)
  {
    // Initialize variables .. WILL NOT WORK AS NOTHING SET
    // this->revertToStart();

    // need to set iterations and tolerance

    // BTW maxIterations and tolerance should not be private variables, they
    // should be static .. all XC::PySimple1 materials share the same values & 
    // these values don't change
  }

/////////////////////////////////////////////////////////////////////
void XC::TzSimple1::getNearField(double zlast, double dz, double dz_old)
{
    // Limit "dz" step size if it is osillating and not shrinking.
    //
    if(dz*dz_old < 0.0 && fabs(dz/dz_old) > 0.5) dz= -dz_old/2.0;

    // Establish trial "z" and direction of loading (with dzTotal) for entire step.
    //        
    TNF.z()= zlast + dz;
    double dzTotal= TNF.z() - CNF.z();

    // Treat as elastic if dzTotal is below TZtolerance
    //
    if(fabs(dzTotal*TNF.tang()/matCapacity) < 10.0*TZtolerance) 
    {
            TNF.T()= TNF.T() + dz*TNF.tang();
            if(fabs(TNF.T()) >=(1.0-TZtolerance)*matCapacity) 
                    TNF.T() =(TNF.T()/fabs(TNF.T()))*(1.0-TZtolerance)*matCapacity;
            return;
    }

    // Reset the history terms to the last Committed values, and let them
    // reset if the reversal of loading persists in this step.
    //
    if(TNF.Tin() != CNF.Tin())
      {
        TNF.Tin()= CNF.Tin();
        TNF.zin()= CNF.zin();
      }

    // Change from positive to negative direction
    //
    if(CNF.z() > CNF.zin() && dzTotal < 0.0)
      {
        TNF.Tin()= CNF.T();
        TNF.zin()= CNF.z();
      }

    // Change from negative to positive direction
    //
    if(CNF.z() < CNF.zin() && dzTotal > 0.0)
      {
        TNF.Tin()= CNF.T();
        TNF.zin()= CNF.z();
      }
    
    // Positive loading
    //
    if(dzTotal > 0.0)
      {
        TNF.T()=matCapacity-(matCapacity-TNF.Tin())*pow(vRef,np)*pow(vRef + TNF.z() - TNF.zin(),-np);
        TNF.tang()=np*(matCapacity-TNF.Tin())*pow(vRef,np)*pow(vRef + TNF.z() - TNF.zin(),-np-1.0);
      }
    // Negative loading
    //
    if(dzTotal < 0.0)
      {
        TNF.T()=-matCapacity+(matCapacity+TNF.Tin())*pow(vRef,np)*pow(vRef - TNF.z() + TNF.zin(),-np);
        TNF.tang()=np*(matCapacity+TNF.Tin())*pow(vRef,np)*pow(vRef - TNF.z() + TNF.zin(),-np-1.0);
      }

    // Ensure that |t|<matCapacity and tangent not zero or negative.
    //
    if(fabs(TNF.T()) >=matCapacity)
      { TNF.T() =(TNF.T()/fabs(TNF.T()))*(1.0-TZtolerance)*matCapacity;}
    if(TNF.tang() <=1.0e-4*matCapacity/v50) TNF.tang()= 1.0e-4*matCapacity/v50;
    return;
  }

/////////////////////////////////////////////////////////////////////
int XC::TzSimple1::setTrialStrain (double newz, double zRate)
{
    // Set trial values for displacement and load in the material
    // based on the last Tangent modulus.
    //
    double dz= newz - T.z();
    double dt= T.tang() * dz;
    TvRate= zRate;

    // Limit the size of step (dz or dt) that can be imposed. Prevents
    // oscillation under large load reversal steps
    //
    int numSteps= 1;
    double stepSize= 1.0;
    if(fabs(dt/matCapacity) > 0.5)  numSteps= 1 + int(fabs(dt/(0.5*matCapacity)));
    if(fabs(dz/v50)  > 1.0 ) numSteps= 1 + int(fabs(dz/(1.0*v50)));
    stepSize= 1.0/float(numSteps);
    if(numSteps > 100) numSteps= 100;

    dz= stepSize * dz;

    // Main loop over the required number of substeps
    //
    for(int istep=1; istep <= numSteps; istep++)
    {
            T.z()= T.z() + dz;
            dt= T.tang() * dz;
            
            // May substep in NearField component if not making progress due to oscillation
            // The following history term is initialized here.
            //
            double dz_nf_old= ((T.T()+dt) - TNF.T())/TNF.tang();
            
    // Iterate to distribute displacement between elastic & plastic components.
    // Use the incremental iterative strain & iterate at this strain.
    //
    for (int j=1; j < TZmaxIterations; j++)
    {
            T.T()= T.T() + dt;
            if(fabs(T.T()) >(1.0-TZtolerance)*matCapacity) T.T()=(1.0-TZtolerance)*matCapacity*(T.T()/fabs(T.T()));

            // Stress & strain update in Near Field element
            double dz_nf= (T.T() - TNF.T())/TNF.tang();
            getNearField(TNF.z(),dz_nf,dz_nf_old);
            
            // Residuals in Near Field element
            double t_unbalance= T.T() - TNF.T();
            double zres_nf= (T.T() - TNF.T())/TNF.tang();
            dz_nf_old= dz_nf;

        // Stress & strain update in Far Field element
        const double z_far= TFar.updateField(T.T());
        getFarField(z_far);

            // Residuals in Far Field element
        double t_unbalance2= TFar.unbalance(T.T());
        double zres_far= TFar.residual(T.T());

            // Update the combined tangent modulus
        T.tang()= pow(1.0/TNF.tang() + 1.0/TFar.tang(), -1.0);

            // Residual deformation across combined element
        double dv= T.z() - (TNF.z() + zres_nf) - (TFar.z() + zres_far);

            // Residual "t" increment 
            dt= T.tang() * dv;

            // Test for convergence
            double tsum= fabs(t_unbalance) + fabs(t_unbalance2);
            if(tsum/matCapacity < TZtolerance) break;
    }
    }

    return 0;
  }
/////////////////////////////////////////////////////////////////////
double XC::TzSimple1::getStress(void) const
  {
    // Dashpot force is only due to velocity in the far field.
    // If converged, proportion by Tangents.
    // If not converged, proportion by ratio of displacements in components.
    //
    double ratio_disp =(1.0/TFar.tang())/(1.0/TFar.tang() + 1.0/TNF.tang());
    if(T.z() != C.z())
      {
        ratio_disp= (TFar.z() - CFar.z())/(T.z() - C.z());
            if(ratio_disp > 1.0) ratio_disp= 1.0;
            if(ratio_disp < 0.0) ratio_disp= 0.0;
      }
    const double dashForce= dashpot * TvRate * ratio_disp;

    // Limit the combined force to matCapacity.
    //
    if(fabs(T.T() + dashForce) >= (1.0-TZtolerance)*matCapacity)
      return (1.0-TZtolerance)*matCapacity*(T.T()+dashForce)/fabs(T.T()+dashForce);
    else return T.T() + dashForce;
  }

/////////////////////////////////////////////////////////////////////
double XC::TzSimple1::getDampTangent(void) const
  {
    // Damping tangent is produced only by the far field component.
    // If converged, proportion by Tangents.
    // If not converged, proportion by ratio of displacements in components.
    //
    double ratio_disp =(1.0/TFar.tang())/(1.0/TFar.tang() + 1.0/TNF.tang());
    if(T.z() != C.z())
      {
        ratio_disp= (TFar.z() - CFar.z())/(T.z() - C.z());
            if(ratio_disp > 1.0) ratio_disp= 1.0;
            if(ratio_disp < 0.0) ratio_disp= 0.0;
      }

    double DampTangent= dashpot * ratio_disp;

    // Minimum damping tangent referenced against Farfield spring
    //
    if(DampTangent < TFar.tang() * 1.0e-12) DampTangent= TFar.tang() * 1.0e-12;

    return DampTangent;
  }

/////////////////////////////////////////////////////////////////////
int XC::TzSimple1::commitState(void)
  {    
    PYBase::commitState();
    CNF= TNF; // Commit trial history variables for Near Field component
    return 0;
  }

/////////////////////////////////////////////////////////////////////
int XC::TzSimple1::revertToLastCommit(void)
  {
    // Nothing to do here -- WRONG -- have a look at setTrialStrain() .. everything
    // calculated based on trial values & trial values updated in method .. need to 
    // reset to committed values
  
    // for convenience i am just gonna do the reverse of commit
    PYBase::revertToLastCommit();
    TNF= CNF;
    return 0;
  }

/////////////////////////////////////////////////////////////////////
int XC::TzSimple1::revertToStart(void)
  {

    // If tzType= 0, then it is entering with the default constructor.
    // To avoid division by zero, set small nonzero values for terms.
    //
    if(soilType == 0)
      {
        matCapacity= 1.0e-12;
        v50= 1.0e12;
      }

    // Only allow zero or positive dashpot values
    //
    if(dashpot < 0.0) dashpot= 0.0;

    // Do not allow zero or negative values for v50 or matCapacity.
    //
    if(matCapacity <= 0.0 || v50 <= 0.0)
      {
        std::cerr << "WARNING -- only accepts positive nonzero tult and z50" << std::endl;
        std::cerr << "TzLiq1: " << std::endl;
        std::cerr << "tzType: " << soilType << std::endl;
        exit(-1);
      }
            
    // Initialize variables for Near Field plastic component
    //
    if(soilType == 0)
      {                        // This will happen with default constructor
        vRef= 0.5*v50;
        np= 1.5;
        TFar.revertToStart(0.70791*matCapacity/(v50));
      }
    else if(soilType == 1)
      {                // Backbone approximates Reese & O'Neill 1987
        vRef= 0.5*v50;
        np= 1.5;
        TFar.revertToStart(0.70791*matCapacity/(v50));
      }
    else if (soilType == 2)
      {                // Backbone approximates Mosher 1984
        vRef= 0.6*v50;
        np= 0.85;
        TFar.revertToStart(2.0504*matCapacity/v50);
      }
    else
      {
        std::cerr << "WARNING -- only accepts tzType of 1 or 2" << std::endl;
        std::cerr << "TzLiq1: " << std::endl;
        std::cerr << "tzType: " << soilType << std::endl;
        exit(-1);
      }

    // Far Field components: TFar.tang() was set under "tzType" statements.
    //
    TFar.T()= 0.0;
    TFar.z()= 0.0;

    // Near Field components
    TNF.revertToStart(np*matCapacity*pow(vRef,np)*pow(vRef,-np-1.0));

    // Entire element (Far field + Near field + Gap in series)
    T.revertToStart(pow(1.0/TNF.tang() + 1.0/TFar.tang(), -1.0));
    TvRate= 0.0;

    // Now get all the committed variables initiated
    this->commitState();

    return 0;
  }

/////////////////////////////////////////////////////////////////////
XC::UniaxialMaterial *XC::TzSimple1::getCopy(void) const
  { return new TzSimple1(*this); }

//! @brief Send object members through the channel being passed as parameter.
int XC::TzSimple1::sendData(CommParameters &cp)
  {
    int res= PYBase::sendData(cp);
    res+= cp.sendMovable(CNF,getDbTagData(),CommMetaData(9));
    res+= cp.sendMovable(TNF,getDbTagData(),CommMetaData(10));
    return res;
  }

//! @brief Receives object members through the channel being passed as parameter.
int XC::TzSimple1::recvData(const CommParameters &cp)
  {
    int res= PYBase::recvData(cp);
    res+= cp.receiveMovable(CNF,getDbTagData(),CommMetaData(9));
    res+= cp.receiveMovable(TNF,getDbTagData(),CommMetaData(10));
    return res;
  }

//! @brief Sends object through the channel being passed as parameter.
int XC::TzSimple1::sendSelf(CommParameters &cp)
  {
    setDbTag(cp);
    const int dataTag= getDbTag();
    inicComm(3);
    int res= sendData(cp);

    res+= cp.sendIdData(getDbTagData(),dataTag);
    if(res < 0)
      std::cerr << getClassName() << "sendSelf() - failed to send data\n";
    return res;
  }

//! @brief Receives object through the channel being passed as parameter.
int XC::TzSimple1::recvSelf(const CommParameters &cp)
  {
    inicComm(3);
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

/////////////////////////////////////////////////////////////////////
void XC::TzSimple1::Print(std::ostream &s, int flag)
  {
    s << "TzSimple1" << std::endl;
    PYBase::Print(s,flag);
  }

/////////////////////////////////////////////////////////////////////

