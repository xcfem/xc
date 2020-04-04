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
**    Module:        QzSimple1.cpp 
**
**    Purpose:        Provide a simple Q-z material for OpenSees.
**
**    Developed by Ross XC::W. Boulanger
**    (C) Copyright 2002, All Rights Reserved.
**
** ****************************************************************** */

// $Revision: 1.0
// $Date: 2001/1/22
// $Source: /OpenSees/SRC/material/uniaxial/QzSimple1.cpp

// Written: RWB
// Created: Jan 2002
// Revision: A
// tested and checked: Boris Jeremic (jeremic@ucdavis.edu) Spring 2002
//
// Description: This file contains the class implementation for XC::QzSimple1

#include <cstdlib>
#include <cmath>
#include "material/uniaxial/PY/QzSimple1.h"
#include <utility/matrix/Vector.h>



// Controls on internal iterations between spring components
const int QZmaxIterations= 20;
const double QZtolerance= 1.0e-12;

/////////////////////////////////////////////////////////////////////
//        Constructor with data

XC::QzSimple1::QzSimple1(int tag, int qzChoice, double Q_ult, double z_50,
                                 double suctionRatio, double dash_pot)
  :PQyzBase(tag,MAT_TAG_QzSimple1,qzChoice,Q_ult,z_50,dash_pot),suction(suctionRatio)
  {
    // Initialize QzSimple variables and history variables
    this->revertToStart();
    initialTangent= T.tang();
  }

//! @brief Constructor.
XC::QzSimple1::QzSimple1(int tag, int classtag)
  :PQyzBase(tag,classtag), suction(0.0)
  {
    // Initialize variables .. WILL NOT WORK AS NOTHING SET
    // this->revertToStart();

    // need to set iterations and tolerance

    // BTW maxIterations and tolerance should not be private variables, they
    // should be static .. all XC::PySimple1 materials share the same values & 
    // these values don't change
  }

/////////////////////////////////////////////////////////////////////
//! @brief Default constructor
XC::QzSimple1::QzSimple1(void)
  :PQyzBase(0,MAT_TAG_QzSimple1),suction(0.0)
  {
    // Initialize variables .. WILL NOT WORK AS NOTHING SET
    // this->revertToStart();

    // need to set iterations and tolerance
    // BTW maxIterations and tolerance should not be private variables, they
    // should be static .. all XC::PySimple1 materials share the same values & 
    // these values don't change
  }


/////////////////////////////////////////////////////////////////////
void XC::QzSimple1::getGap(double zlast, double dz, double dz_old)
  {
    // For stability in Closure spring, limit "dz" step size to avoid
    // overshooting on the "closing" or "opening" of the gap.
    //
    if(zlast > 0.0 && (zlast + dz) < -QZtolerance) dz= -QZtolerance - zlast;
    if(zlast < 0.0 && (zlast + dz) >  QZtolerance) dz= QZtolerance - zlast;
    TGap.z()= zlast + dz;

    // Combine the Suction and Closure elements in parallel
    //
    getClosure(zlast,dz);
    getSuction(zlast,dz);
    TGap.Q()= TSuction.Q() + TClose.Q();
    TGap.tang()= TSuction.tang() + TClose.tang();

    return;
}

/////////////////////////////////////////////////////////////////////
void XC::QzSimple1::getClosure(double zlast, double dz)
  {
    TClose.z()= zlast + dz;
    
    // Loading on the stiff "closed gap"
    //
    if(TClose.z() <= 0.0) 
      {
        TClose.tang()= 1000.0*matCapacity/v50;
        TClose.Q()= TClose.z() * TClose.tang();
      }

    // Loading on the soft "open gap"
    //
    if(TClose.z() > 0.0) 
      {
        TClose.tang()= 0.001*matCapacity/v50;
        TClose.Q()= TClose.z() * TClose.tang();
      }
    return;
  }

/////////////////////////////////////////////////////////////////////
void XC::QzSimple1::getSuction(double zlast, double dz)
  {
    TSuction.z()= zlast + dz;
    double Qmax=suction*matCapacity;
    double dzTotal=TSuction.z() - CSuction.z();

    // Treat as elastic if dzTotal is below QZtolerance
    //
    if(fabs(dzTotal*TSuction.tang()/matCapacity) < 3.0*QZtolerance) 
      {
        TSuction.Q()+= dz*TSuction.tang();
        if(fabs(TSuction.Q()) >= Qmax) 
          TSuction.Q() =(TSuction.Q()/fabs(TSuction.Q()))*(1.0-1.0e-8)*Qmax;
        return;
      }

    // Reset the history terms to the last Committed values, and let them
    // reset if the reversal of loading persists in this step.
    //
    if(TSuction.Qin() != CSuction.Qin())
      {
        TSuction.Qin()= CSuction.Qin();
        TSuction.zin()= CSuction.zin();
      }

    // Change from positive to negative direction
    //
    if(CSuction.z() > CSuction.zin() && dzTotal < 0.0)
      {
        TSuction.Qin()= CSuction.Q();
        TSuction.zin()= CSuction.z();
      }
    // Change from negative to positive direction
    //
    if(CSuction.z() < CSuction.zin() && dzTotal > 0.0)
      {
        TSuction.Qin()= CSuction.Q();
        TSuction.zin()= CSuction.z();
      }
    
    // Positive loading
    //
    if(dzTotal >= 0.0)
    {
            TSuction.Q()=Qmax-(Qmax-TSuction.Qin())*pow(0.5*v50,nd)
                                    *pow(0.5*v50 + TSuction.z() - TSuction.zin(),-nd);
            TSuction.tang()=nd*(Qmax-TSuction.Qin())*pow(0.5*v50,nd)
                                    *pow(0.5*v50 + TSuction.z() - TSuction.zin(),-nd-1.0);
    }

    // Negative loading
    //
    if(dzTotal < 0.0)
    {
            TSuction.Q()=-Qmax+(Qmax+TSuction.Qin())*pow(0.5*v50,nd)
                                    *pow(0.5*v50 - TSuction.z() + TSuction.zin(),-nd);
            TSuction.tang()=nd*(Qmax+TSuction.Qin())*pow(0.5*v50,nd)
                                    *pow(0.5*v50 - TSuction.z() + TSuction.zin(),-nd-1.0);
    }

    // Ensure that |Q|<Qmax and tangent not zero or negative.
    //
    if(fabs(TSuction.Q()) >= (1.0-QZtolerance)*Qmax) {
            TSuction.Q() =(TSuction.Q()/fabs(TSuction.Q()))*(1.0-QZtolerance)*Qmax;}
    if(TSuction.tang() <=1.0e-4*matCapacity/v50) TSuction.tang()= 1.0e-4*matCapacity/v50;

    return;
}

/////////////////////////////////////////////////////////////////////
void XC::QzSimple1::getNearField(double zlast, double dz, double dz_old)
{
    // Limit "dz" step size if it is oscillating in sign and not shrinking
    //
    if(dz*dz_old < 0.0 && fabs(dz/dz_old) > 0.5) dz= -dz_old/2.0;

    // Set "dz" so "z" is at middle of elastic zone if oscillation is large.
    //
    if(dz*dz_old < -v50*v50) {
            dz= (TNF.zrightIn() + TNF.zleftIn())/2.0 - zlast;
    }
    
    // Establish trial "z" and direction of loading (with NFdz) for entire step
    //
    TNF.z()= zlast + dz;
    double NFdz= TNF.z() - CNF.z();

    // Treat as elastic if NFdz is below QZtolerance
    //
    if(fabs(NFdz*TNF.tang()/matCapacity) < 3.0*QZtolerance) 
    {
            TNF.Q()= TNF.Q() + dz*TNF.tang();
            if(fabs(TNF.Q()) >=matCapacity) TNF.Q()=(TNF.Q()/fabs(TNF.Q()))*(1.0-QZtolerance)*matCapacity;
            return;
    }

    // Reset the history terms to the last Committed values, and let them
    // reset if the reversal of loading persists in this step.
    //
    if(TNF.QleftIn() != CNF.QleftIn() || TNF.QrightIn() != CNF.QrightIn())
    {
            TNF.QleftIn()= CNF.QleftIn();
            TNF.QrightIn()= CNF.QrightIn();
            TNF.zrightIn()= CNF.zrightIn();
            TNF.zleftIn()= CNF.zleftIn();
    }

    // For stability, may have to limit "dz" step size if direction changed.
    //
    bool changeDirection= false;
    
    // Direction change from a yield point triggers new Elastic range
    //
    if(CNF.Q() > CNF.QleftIn() && NFdz <0.0){                                // from pos to neg
            changeDirection= true;
            if((CNF.Q() - CNF.QrightIn()) > 2.0*matCapacity*Elast) Elast=(CNF.Q() - CNF.QrightIn())/(2.0*matCapacity);
            if(2.0*Elast > maxElast) Elast=maxElast/2.0;
            TNF.QleftIn()= CNF.Q();
            TNF.QrightIn()= TNF.QleftIn() - 2.0*matCapacity*Elast;
            TNF.zrightIn()= CNF.z();
            TNF.zleftIn()= TNF.zrightIn() - (TNF.QleftIn()-TNF.QrightIn())/NFkrig; 
    }
    if(CNF.Q() < CNF.QrightIn() && NFdz > 0.0){                                // from neg to pos
            changeDirection= true;
            if((CNF.QleftIn() - CNF.Q()) > 2.0*matCapacity*Elast) Elast=(CNF.QleftIn() - CNF.Q())/(2.0*matCapacity);
            if(2.0*Elast > maxElast) Elast=maxElast/2.0;
            TNF.QrightIn()= CNF.Q();
            TNF.QleftIn()= TNF.QrightIn() + 2.0*matCapacity*Elast;
            TNF.zleftIn()= CNF.z();
            TNF.zrightIn()= TNF.zleftIn() + (TNF.QleftIn()-TNF.QrightIn())/NFkrig; 
    }

    // Now if there was a change in direction, limit the step size "dz"
    //
    if(changeDirection == true) {
            double maxdz= Elast*matCapacity/NFkrig;
            if(fabs(dz) > maxdz) dz= (dz/fabs(dz))*maxdz;
    }

    // Now, establish the trial value of "z" for use in this function call.
    //
    TNF.z()= zlast + dz;

    // Positive loading
    //
    if(NFdz >= 0.0)
      {
            // Check if elastic using z < zinr
            if(TNF.z() <= TNF.zrightIn())
          { // stays elastic
                TNF.tang()= NFkrig;
                TNF.Q()= TNF.QrightIn() + (TNF.z() - TNF.zleftIn())*NFkrig;
          }
        else
          {
                TNF.tang()= np * (matCapacity-TNF.QleftIn()) * pow(vRef,np) * pow(vRef - TNF.zrightIn() + TNF.z(), -np-1.0);
                TNF.Q()= matCapacity - (matCapacity-TNF.QleftIn())* pow(vRef/(vRef-TNF.zrightIn()+TNF.z()),np);
          }
      }

    // Negative loading
    //
    if(NFdz < 0.0)
      {
        // Check if elastic using z < zinl
        if(TNF.z() >= TNF.zleftIn())
          { // stays elastic
            TNF.tang()= NFkrig;
            TNF.Q()= TNF.QleftIn() + (TNF.z() - TNF.zrightIn())*NFkrig;
          }
        else
          {
            TNF.tang()= np * (matCapacity+TNF.QrightIn()) * pow(vRef,np)*pow(vRef + TNF.zleftIn() - TNF.z(), -np-1.0);
            TNF.Q()= -matCapacity + (matCapacity+TNF.QrightIn())* pow(vRef/(vRef+TNF.zleftIn()-TNF.z()),np);
          }
      }

    // Ensure that |Q|<matCapacity and tangent not zero or negative.
    //
    if(fabs(TNF.Q()) >= (1.0-QZtolerance)*matCapacity)
      { 
        TNF.Q()=(TNF.Q()/fabs(TNF.Q()))*(1.0-QZtolerance)*matCapacity;
        TNF.tang()= 1.0e-4*matCapacity/v50;
      }
    if(TNF.tang() <= 1.0e-4*matCapacity/v50) TNF.tang()= 1.0e-4*matCapacity/v50;
    return;
  }

/////////////////////////////////////////////////////////////////////
int XC::QzSimple1::setTrialStrain (double newz, double zRate)
  {
    // Set trial values for displacement and load in the material
    // based on the last Tangent modulus.
    //
    double dz= newz - T.z();
    double dQ= T.tang() * dz;
    TvRate= zRate;

    // Limit the size of step (dz or dQ) that can be imposed. Prevents
    // numerical difficulties upon load reversal at high loads
    // where a soft loading modulus becomes a stiff unloading modulus.
    //
    int numSteps= 1;
    double stepSize= 1.0;
    if(fabs(dQ/matCapacity) > 0.5) numSteps= 1 + int(fabs(dQ/(0.5*matCapacity)));
    if(fabs(dz/v50)  > 1.0 ) numSteps= 1 + int(fabs(dz/(1.0*v50)));
    stepSize= 1.0/float(numSteps);
    if(numSteps > 100) numSteps= 100;

    dz= stepSize * dz;

    // Main loop over the required number of substeps
    //
    for(int istep=1; istep <= numSteps; istep++)
      {
        T.z()= T.z() + dz;
        dQ= T.tang() * dz;
            
        // May substep within Gap or NearField element if oscillating, which can happen
        // when they jump from soft to stiff. Initialize history terms here.
        //
        double dz_gap_old= ((T.Q() + dQ) - TGap.Q())/TGap.tang();
        double dz_nf_old= ((T.Q() + dQ) - TNF.Q()) /TNF.tang();

        // Iterate to distribute displacement among the series components.
        // Use the incremental iterative strain & iterate at this strain.
        //
        for(int j=1; j < QZmaxIterations; j++)
          {
            T.Q()+= dQ;
            if(fabs(T.Q()) >(1.0-QZtolerance)*matCapacity) T.Q()=(1.0-QZtolerance)*matCapacity*(T.Q()/fabs(T.Q()));

            // Stress & strain update in Near Field element
            double dz_nf= (T.Q() - TNF.Q())/TNF.tang();
            getNearField(TNF.z(),dz_nf,dz_nf_old);
            
            // Residuals in Near Field element
            double Q_unbalance= T.Q() - TNF.Q();
            double zres_nf= (T.Q() - TNF.Q())/TNF.tang();
            dz_nf_old= dz_nf;

            // Stress & strain update in Gap element
            double dz_gap= TGap.residual(T.Q());
            getGap(TGap.z(),dz_gap,dz_gap_old);

            // Residuals in Gap element
            double Q_unbalance2= TGap.unbalance(T.Q());
            double zres_gap= TGap.residual(T.Q());
            dz_gap_old= dz_gap;

            // Stress & strain update in Far Field element
            const double z_far= TFar.updateField(T.Q());
            getFarField(z_far);

            // Residuals in Far Field element
            double Q_unbalance3= TFar.unbalance(T.Q());
            double zres_far= TFar.residual(T.Q());

        // Update the combined tangent modulus
        T.tang()= pow(1.0/TGap.tang() + 1.0/TNF.tang() + 1.0/TFar.tang(), -1.0);

            // Residual deformation across combined element
        double dv= T.z() - (TGap.z() + zres_gap) - (TNF.z() + zres_nf) - (TFar.z() + zres_far);

            // Residual "Q" increment 
            dQ= T.tang() * dv;

            // Test for convergence
            double Qsum= (fabs(Q_unbalance) + fabs(Q_unbalance2) + fabs(Q_unbalance3))/3.0;
            if(Qsum/matCapacity < QZtolerance) break;
    }
    }

    return 0;
}
/////////////////////////////////////////////////////////////////////
double XC::QzSimple1::getStress(void) const
  {
    // Dashpot force is only due to velocity in the far field.
    // If converged, proportion by Tangents.
    // If not converged, proportion by ratio of displacements in components.
    //
    double ratio_disp =(1.0/TFar.tang())/(1.0/TFar.tang() + 1.0/TNF.tang() + 1.0/TGap.tang());
    if(T.z() != C.z())
      {
        ratio_disp= (TFar.z() - CFar.z())/(T.z() - C.z());
            if(ratio_disp > 1.0) ratio_disp= 1.0;
            if(ratio_disp < 0.0) ratio_disp= 0.0;
      }
    const double dashForce= dashpot * TvRate * ratio_disp;

    // Limit the combined force to matCapacity.
    //
    if(fabs(T.Q() + dashForce) >= (1.0-QZtolerance)*matCapacity)
      return (1.0-QZtolerance)*matCapacity*(T.Q()+dashForce)/fabs(T.Q()+dashForce);
    else return T.Q() + dashForce;
  }

/////////////////////////////////////////////////////////////////////
double XC::QzSimple1::getDampTangent(void) const
  {
    // Damping tangent is produced only by the far field component.
    // If converged, proportion by Tangents.
    // If not converged, proportion by ratio of displacements in components.
    //
    double ratio_disp =(1.0/TFar.tang())/(1.0/TFar.tang() + 1.0/TNF.tang() + 1.0/TGap.tang());
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

//! @brief Set the suction parameter (Uplift resistance
//! is equal to suction*qult). The value of suction must
//! be 0.0 to 0.1.
void XC::QzSimple1::set_suction(const double &s)
  {
    if((s<0.0) or (s>0.1))
      std::clog << getClassName() << "::" << __FUNCTION__
	        << "; the value of suction must be 0.0 to 0.1."
	        << std::endl;
    suction= s;
  }

//! @brief Return the suction parameter (Uplift resistance
//! is equal to suction*qult)
double XC::QzSimple1::get_suction(void) const
  { return suction;  }

/////////////////////////////////////////////////////////////////////
int XC::QzSimple1::commitState(void)
  {
    PQyzBase::commitState();
    CSuction= TSuction; // Commit trial history variables for Suction component
    CClose= TClose; // Commit trial history variables for Closure component
    return 0;
  }

/////////////////////////////////////////////////////////////////////
int XC::QzSimple1::revertToLastCommit(void)
  {
    // Nothing to do here -- WRONG -- have a look at setTrialStrain() .. everything
    // calculated based on trial values & trial values updated in method .. need to 
    // reset to committed values
  
    // for convenience i am just gonna do the reverse of commit 
    PQyzBase::revertToLastCommit();
    TSuction= CSuction;
    TClose= CClose;
    return 0;
  }

/////////////////////////////////////////////////////////////////////
int XC::QzSimple1::revertToStart(void)
  {

    // Reset gap "suction" if zero (or negative) or exceeds max value of 0.1
    //
    if(suction <= QZtolerance) suction= QZtolerance;
    if(suction > 0.1)
      {
        suction= 0.1;
        std::cerr << "XC::QzSimple1::QzSimple1 -- setting suction to max value of 0.1\n";
      }

    // Only allow zero or positive dashpot values
    //
    if(dashpot < 0.0) dashpot= 0.0;

    // Do not allow zero or negative values for v50 or matCapacity.
    //
    if(matCapacity <= 0.0 || v50 <= 0.0)
      {
        std::cerr << "XC::QzSimple1::QzSimple1 -- only accepts positive nonzero Qult and z50\n";
        exit(-1);
      }

    // Initialize variables for Near Field rigid-plastic spring
    //
    if(soilType ==1)
      {  // Approx Reese & O'Neill (1987) drilled shafts on clay
        vRef= 0.35*v50;
        np= 1.2;
        Elast= 0.2;
        maxElast= 0.7;
        nd= 1.0;
        TFar.revertToStart(0.525*matCapacity/v50);
      }
    else if(soilType == 2)
      {
        vRef= 12.3*v50;
        np= 5.5;
        Elast= 0.3;
        maxElast= 0.7;
        nd= 1.0;
        TFar.revertToStart(1.39*matCapacity/v50);
      }
    else
      {
        std::cerr << "XC::QzSimple1::QzSimple1 -- only accepts QzType of 1 or 2\n";
        exit(-1);
      }

    // Near Field components
    //
    NFkrig= 10000.0 * matCapacity / v50;
    TNF.QleftIn()= Elast*matCapacity;
    TNF.QrightIn()= -TNF.QleftIn();
    TNF.zrightIn()= TNF.QleftIn() / NFkrig;
    TNF.zleftIn()= -TNF.zrightIn();
    TNF.Q()= 0.0;
    TNF.z()= 0.0;
    TNF.tang()= NFkrig;

    // Suction components
    TSuction.revertToStart(nd*(matCapacity*suction-TSuction.Q())*pow(v50/2.0,nd)*pow(v50/2.0 - TSuction.z() + TSuction.zin(),-nd-1.0));

    // Closure components
    TClose.revertToStart(100.0*matCapacity/v50);

    // Gap (Suction + Closure in parallel)
    TGap.revertToStart(TClose.tang()+TSuction.tang());

    // Entire element (Far field + Near field + Gap in series)
    T.revertToStart(pow(1.0/TGap.tang() + 1.0/TNF.tang() + 1.0/TFar.tang(), -1.0));
    TvRate= 0.0;

    // Now get all the committed variables initiated
    this->commitState();

    return 0;
  }

/////////////////////////////////////////////////////////////////////
XC::UniaxialMaterial *XC::QzSimple1::getCopy(void) const
  { return new QzSimple1(*this); }

//! @brief Send object members through the communicator argument.
int XC::QzSimple1::sendData(Communicator &comm)
  {
    int res= PQyzBase::sendData(comm);
    res+= comm.sendDoubles(suction,maxElast,getDbTagData(),CommMetaData(14));
    res+= comm.sendMovable(CSuction,getDbTagData(),CommMetaData(15));
    res+= comm.sendMovable(TSuction,getDbTagData(),CommMetaData(16));
    res+= comm.sendMovable(CClose,getDbTagData(),CommMetaData(17));
    res+= comm.sendMovable(TClose,getDbTagData(),CommMetaData(18));
    return res;
  }

//! @brief Receives object members through the communicator argument.
int XC::QzSimple1::recvData(const Communicator &comm)
  {
    int res= PQyzBase::recvData(comm);
    res+= comm.receiveDoubles(suction,maxElast,getDbTagData(),CommMetaData(14));
    res+= comm.receiveMovable(CSuction,getDbTagData(),CommMetaData(15));
    res+= comm.receiveMovable(TSuction,getDbTagData(),CommMetaData(16));
    res+= comm.receiveMovable(CClose,getDbTagData(),CommMetaData(17));
    res+= comm.receiveMovable(TClose,getDbTagData(),CommMetaData(18));
    return res;
  }

/////////////////////////////////////////////////////////////////////
int XC::QzSimple1::sendSelf(Communicator &comm)
   {
    setDbTag(comm);
    const int dataTag= getDbTag();
    inicComm(19);
    int res= sendData(comm);

    res+= comm.sendIdData(getDbTagData(),dataTag);
    if(res < 0)
      std::cerr << getClassName() << "sendSelf() - failed to send data\n";
    return res;
  }

/////////////////////////////////////////////////////////////////////
int XC::QzSimple1::recvSelf(const Communicator &comm)
  {
    inicComm(19);
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
void XC::QzSimple1::Print(std::ostream &s, int flag) const
  {
    s << "QzSimple1, tag: " << this->getTag() << std::endl;
    PQyzBase::Print(s,flag);
    s << "  suction: " << suction << std::endl;
  }

