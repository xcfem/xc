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
**    Module:        PySimple1.cpp 
**
**    Purpose:        Provide a simple p-y spring for OpenSees.
**
**    Developed by Ross XC::W. Boulanger
**    (C) Copyright 2001, All Rights Reserved.
**
** ****************************************************************** */

// $Revision: 1.0
// $Date: 2001/12/15
// $Source: /OpenSees/SRC/material/uniaxial/PySimple1.cpp

// Written: RWB
// Created: Dec 2001
// Revision: A
// tested and checked: Boris Jeremic (jeremic@ucdavis.edu) Spring 2002
//
// Description: This file contains the class implementation for XC::PySimple1

#include <cstdlib>
#include <cmath>

#include "material/uniaxial/PY/PySimple1.h"
#include <utility/matrix/Vector.h>



// Controls on internal iteration between spring components
const int PYmaxIterations = 20;
const double PYtolerance = 1.0e-12;

/////////////////////////////////////////////////////////////////////
//! @brief Constructor with data
XC::PySimple1::PySimple1(int tag, int classtag, int soil, double p_ult, double y_50,
                             double dragratio, double dash_pot)
  :PQyzBase(tag,classtag,soil,p_ult,y_50,dash_pot), drag(dragratio)
  {
    // Initialize PySimple variables and history variables
    initialize();
  }

//! @brief Constructor.
XC::PySimple1::PySimple1(int tag,int classtag)
  :PQyzBase(tag,classtag),drag(0.0) {}

/////////////////////////////////////////////////////////////////////
//! @brief Default constructor.
XC::PySimple1::PySimple1(void)
  :PQyzBase(0,0),drag(0.0) {}

//! @brief Initial values for the material parameters.
void XC::PySimple1::initialize(void)
  {
    revertToStart();
    initialTangent= T.tang();
  }

/////////////////////////////////////////////////////////////////////
void XC::PySimple1::getGap(double ylast, double dy, double dy_old)
  {
    // For stability in Closure spring, may limit "dy" step size to avoid
    // overshooting on the closing of this gap.
    //
    TGap.y() = ylast + dy;
    if(TGap.y() > TClose.yright()) {dy = 0.75*(TClose.yright() - ylast);}
    if(TGap.y() < TClose.yleft())  {dy = 0.75*(TClose.yleft()  - ylast);}

    // Limit "dy" step size if it is oscillating in sign and not shrinking
    //
    if(dy*dy_old < 0.0 && fabs(dy/dy_old) > 0.5) dy = -dy_old/2.0;
    
    // Combine the Drag and Closure elements in parallel, starting by
    // resetting TGap.y() in case the step size was limited.
    //
    TGap.y()   = ylast + dy;
    getClosure(ylast,dy);
    getDrag(ylast,dy);
    TGap.P() = TDrag.P() + TClose.P();
    TGap.tang() = TDrag.tang() + TClose.tang();

    // Ensure that |p|<pmax.
    //
    if(fabs(TGap.P())>=matCapacity) TGap.P() =(TGap.P()/fabs(TGap.P()))*(1.0-PYtolerance)*matCapacity;

    return;
  }

/////////////////////////////////////////////////////////////////////
void XC::PySimple1::getClosure(double ylast, double dy)
  {
    // Reset the history terms to the last Committed values, and let them
    // reset if the reversal of loading persists in this step.
    //
    if(TClose.yleft() != CClose.yleft())  TClose.yleft() = CClose.yleft();
    if(TClose.yright()!= CClose.yright()) TClose.yright()= CClose.yright();

    // Check if plastic deformation in Near Field should cause gap expansion
    //
    TClose.y() = ylast + dy;
    double yrebound=1.5*v50;
    if(TNF.y()+TClose.y() > -TClose.yleft() + yrebound)
            TClose.yleft()=-(TNF.y()+TClose.y()) + yrebound;
    if(TNF.y()+TClose.y() < -TClose.yright() - yrebound)
            TClose.yright()=-(TNF.y()+TClose.y()) - yrebound;

    // Spring force and tangent stiffness
    //
    TClose.P()=1.8*matCapacity*(v50/50.0)*(pow(v50/50.0 + TClose.yright() - TClose.y(),-1.0)
            -pow(v50/50.0 + TClose.y() - TClose.yleft(),-1.0));
    TClose.tang()=1.8*matCapacity*(v50/50.0)*(pow(v50/50.0+ TClose.yright() - TClose.y(),-2.0)
            +pow(v50/50.0 + TClose.y() - TClose.yleft(),-2.0));

    // Ensure that tangent not zero or negative.
    //        
    if(TClose.tang() <= 1.0e-2*matCapacity/v50) {TClose.tang() = 1.0e-2*matCapacity/v50;}

    return;
  }

/////////////////////////////////////////////////////////////////////
void XC::PySimple1::getDrag(double ylast, double dy)
{
    TDrag.y() = ylast + dy;
    double pmax=drag*matCapacity;
    double dyTotal=TDrag.y() - CDrag.y();

    // Treat as elastic if dyTotal is below PYtolerance
    //
    if(fabs(dyTotal*TDrag.tang()/matCapacity) < 10.0*PYtolerance) 
      {
        TDrag.P() = TDrag.P() + dy*TDrag.tang();
        if(fabs(TDrag.P()) >=pmax) TDrag.P() =(TDrag.P()/fabs(TDrag.P()))*(1.0-1.0e-8)*pmax;
        return;
      }
    // Reset the history terms to the last Committed values, and let them
    // reset if the reversal of loading persists in this step.
    //
    if(TDrag.Pin() != CDrag.Pin())
      {
        TDrag.Pin() = CDrag.Pin();
        TDrag.yin() = CDrag.yin();
      }

    // Change from positive to negative direction
    //
    if(CDrag.y() > CDrag.yin() && dyTotal < 0.0)
      {
        TDrag.Pin() = CDrag.P();
        TDrag.yin() = CDrag.y();
      }
    // Change from negative to positive direction
    //
    if(CDrag.y() < CDrag.yin() && dyTotal > 0.0)
      {
        TDrag.Pin() = CDrag.P();
        TDrag.yin() = CDrag.y();
      }
    
    // Positive loading
    //
    if(dyTotal >= 0.0)
      {
        TDrag.P()=pmax-(pmax-TDrag.Pin())*pow(v50/2.0,nd)*pow(v50/2.0 + TDrag.y() - TDrag.yin(),-nd);
        TDrag.tang()=nd*(pmax-TDrag.Pin())*pow(v50/2.0,nd)*pow(v50/2.0 + TDrag.y() - TDrag.yin(),-nd-1.0);
      }
    // Negative loading
    //
    if(dyTotal < 0.0)
      {
        TDrag.P()=-pmax+(pmax+TDrag.Pin())*pow(v50/2.0,nd)*pow(v50/2.0 - TDrag.y() + TDrag.yin(),-nd);
        TDrag.tang()=nd*(pmax+TDrag.Pin())*pow(v50/2.0,nd)*pow(v50/2.0 - TDrag.y() + TDrag.yin(),-nd-1.0);
      }
    // Ensure that |p|<pmax and tangent not zero or negative.
    //
    if(fabs(TDrag.P()) >=pmax) {
            TDrag.P() =(TDrag.P()/fabs(TDrag.P()))*(1.0-PYtolerance)*pmax;}
    if(TDrag.tang() <=1.0e-2*matCapacity/v50) TDrag.tang() = 1.0e-2*matCapacity/v50;

    return;
}

/////////////////////////////////////////////////////////////////////
void XC::PySimple1::getNearField(double ylast, double dy, double dy_old)
  {
    // Limit "dy" step size if it is oscillating in sign and not shrinking
    //
    if(dy*dy_old < 0.0 && fabs(dy/dy_old) > 0.5) dy = -dy_old/2.0;

    // Set "dy" so "y" is at middle of elastic zone if oscillation is large.
    // Note that this criteria is based on the min step size in setTrialStrain.
    //
    if(dy*dy_old < -v50*v50) dy = (TNF.yrightIn() + TNF.yleftIn())/2.0 - ylast;
    
    // Establish trial "y" and direction of loading (with NFdy) for entire step
    //
    TNF.y() = ylast + dy;
    double NFdy = TNF.y() - CNF.y();

    // Treat as elastic if NFdy is below PYtolerance
    //
    if(fabs(NFdy*TNF.tang()/matCapacity) < 10.0*PYtolerance) 
      {
        TNF.P() = TNF.P() + dy*TNF.tang();
        if(fabs(TNF.P()) >=matCapacity) TNF.P()=(TNF.P()/fabs(TNF.P()))*(1.0-PYtolerance)*matCapacity;
        return;
      }

    // Reset the history terms to the last Committed values, and let them
    // reset if the reversal of loading persists in this step.
    //
    if(TNF.PrightIn() != CNF.PrightIn() || TNF.PleftIn() != CNF.PleftIn())
      {
            TNF.PrightIn() = CNF.PrightIn();
            TNF.PleftIn() = CNF.PleftIn();
            TNF.yrightIn() = CNF.yrightIn();
            TNF.yleftIn() = CNF.yleftIn();
      }

    // For stability, may have to limit "dy" step size if direction changed.
    //
    bool changeDirection = false;
    
    // Direction change from a yield point triggers new Elastic range
    //
    double minE = 0.25;                // The min Elastic range on +/- side of p=0
    if(CNF.P() > CNF.PrightIn() && NFdy <0.0){                                // from pos to neg
            changeDirection = true;
            TNF.PrightIn() = CNF.P();
            if(fabs(TNF.PrightIn())>=(1.0-PYtolerance)*matCapacity){TNF.PrightIn()=(1.0-2.0*PYtolerance)*matCapacity;}
            TNF.PleftIn() = TNF.PrightIn() - 2.0*matCapacity*Elast;
            if (TNF.PleftIn() > -minE*matCapacity) {TNF.PleftIn() = -minE*matCapacity;}
            TNF.yrightIn() = CNF.y();
            TNF.yleftIn() = TNF.yrightIn() - (TNF.PrightIn()-TNF.PleftIn())/NFkrig; 
    }
    if(CNF.P() < CNF.PleftIn() && NFdy > 0.0){                                // from neg to pos
            changeDirection = true;
            TNF.PleftIn() = CNF.P();
            if(fabs(TNF.PleftIn())>=(1.0-PYtolerance)*matCapacity){TNF.PleftIn()=(-1.0+2.0*PYtolerance)*matCapacity;}
            TNF.PrightIn() = TNF.PleftIn() + 2.0*matCapacity*Elast;
            if (TNF.PrightIn() < minE*matCapacity) {TNF.PrightIn() = minE*matCapacity;}
            TNF.yleftIn() = CNF.y();
            TNF.yrightIn() = TNF.yleftIn() + (TNF.PrightIn()-TNF.PleftIn())/NFkrig; 
    }
    // Now if there was a change in direction, limit the step size "dy"
    //
    if(changeDirection == true) {
            double maxdy = 0.25*matCapacity/NFkrig;
            if(fabs(dy) > maxdy) dy = (dy/fabs(dy))*maxdy;
    }

    // Now, establish the trial value of "y" for use in this function call.
    //
    TNF.y() = ylast + dy;

    // Positive loading
    //
    if(NFdy >= 0.0)
      {
        // Check if elastic using y < .yrightIn()
        if(TNF.y() <= TNF.yrightIn())
          { // stays elastic
            TNF.tang() = NFkrig;
            TNF.P() = TNF.PleftIn() + (TNF.y() - TNF.yleftIn())*NFkrig;
          }
        else
          {
            TNF.tang()= np * (matCapacity-TNF.PrightIn()) * pow(vRef,np) * pow(vRef - TNF.yrightIn() + TNF.y(), -np-1.0);
            TNF.P() = matCapacity - (matCapacity-TNF.PrightIn())* pow(vRef/(vRef-TNF.yrightIn()+TNF.y()),np);
          }
      }

    // Negative loading
    //
    if(NFdy < 0.0)
      {
        // Check if elastic using y < .yleftIn()
        if(TNF.y() >= TNF.yleftIn())
          { // stays elastic
            TNF.tang() = NFkrig;
            TNF.P() = TNF.PrightIn() + (TNF.y() - TNF.yrightIn())*NFkrig;
          }
        else
          {
            TNF.tang() = np * (matCapacity+TNF.PleftIn()) * pow(vRef,np) * pow(vRef + TNF.yleftIn() - TNF.y(), -np-1.0);
            TNF.P() = -matCapacity + (matCapacity+TNF.PleftIn())* pow(vRef/(vRef+TNF.yleftIn()-TNF.y()),np);
          }
      }
    // Ensure that |p|<matCapacity and tangent not zero or negative.
    //
    if(fabs(TNF.P()) >=matCapacity) TNF.P()=(TNF.P()/fabs(TNF.P()))*(1.0-PYtolerance)*matCapacity;
    if(TNF.tang() <= 1.0e-2*matCapacity/v50) TNF.tang() = 1.0e-2*matCapacity/v50;
    return;
  }

/////////////////////////////////////////////////////////////////////
int XC::PySimple1::setTrialStrain(double newy, double yRate)
  {
    // Set trial values for displacement and load in the material
    // based on the last Tangent modulus.
    //
    double dy = newy - T.y();
    double dp = T.tang() * dy;
    TvRate    = yRate;

    // Limit the size of step (dy or dp) that can be imposed. Prevents
    // numerical difficulties upon load reversal at high loads
    // where a soft loading modulus becomes a stiff unloading modulus.
    //
    int numSteps = 1;
    double stepSize = 1.0;
    if(fabs(dp/matCapacity) > 0.5) numSteps = 1 + int(fabs(dp/(0.5*matCapacity)));
    if(fabs(dy/v50)  > 1.0 ) numSteps = 1 + int(fabs(dy/(1.0*v50)));
    stepSize = 1.0/float(numSteps);
    if(numSteps > 100) numSteps = 100;

    dy = stepSize * dy;

    // Main loop over the required number of substeps
    //
    for(int istep=1; istep <= numSteps; istep++)
      {
        T.y()+= dy;
        dp = T.tang() * dy;
            
    // May substep within Gap or NearField element if oscillating, which can happen
    // when they jump from soft to stiff.
    //
        double dy_gap_old = ((T.P() + dp) - TGap.P())/TGap.tang();
        double dy_nf_old  = ((T.P() + dp) - TNF.P()) /TNF.tang();

    // Iterate to distribute displacement among the series components.
    // Use the incremental iterative strain & iterate at this strain.
    //
    for (int j=1; j < PYmaxIterations; j++)
      {
        T.P()+= dp;

        // Stress & strain update in Near Field element
        double dy_nf = (T.P() - TNF.P())/TNF.tang();
        getNearField(TNF.y(),dy_nf,dy_nf_old);
            
        // Residuals in Near Field element
        double p_unbalance = T.P() - TNF.P();
        double yres_nf = (T.P() - TNF.P())/TNF.tang();
        dy_nf_old = dy_nf;

        // Stress & strain update in Gap element
        double dy_gap= TGap.residual(T.P());
        getGap(TGap.y(),dy_gap,dy_gap_old);

        // Residuals in Gap element
        double p_unbalance2= TGap.unbalance(T.P());
        double yres_gap = TGap.residual(T.P());
        dy_gap_old = dy_gap;

        // Stress & strain update in Far Field element
        const double y_far= TFar.updateField(T.P());
        getFarField(y_far);

        // Residuals in Far Field element
        double p_unbalance3= TFar.unbalance(T.P());
        double yres_far = TFar.residual(T.P());

        // Update the combined tangent modulus
        T.tang() = pow(1.0/TGap.tang() + 1.0/TNF.tang() + 1.0/TFar.tang(), -1.0);

        // Residual deformation across combined element
        double dv= T.y() - (TGap.y() + yres_gap) - (TNF.y() + yres_nf) - (TFar.y() + yres_far);

        // Residual "p" increment 
        dp= T.tang() * dv;

        // Test for convergence
        double psum = fabs(p_unbalance) + fabs(p_unbalance2) + fabs(p_unbalance3);
        if(psum/matCapacity < PYtolerance) break;
      }
    }

    return 0;
  }
/////////////////////////////////////////////////////////////////////
double XC::PySimple1::getStress(void) const
  {
    // Dashpot force is only due to velocity in the far field.
    // If converged, proportion by Tangents.
    // If not converged, proportion by ratio of displacements in components.
    //
    double ratio_disp= (1.0/TFar.tang())/(1.0/TFar.tang() + 1.0/TNF.tang() + 1.0/TGap.tang());
    if(T.y() != C.y())
      {
        ratio_disp = (TFar.y() - CFar.y())/(T.y() - C.y());
        if(ratio_disp > 1.0) ratio_disp = 1.0;
        if(ratio_disp < 0.0) ratio_disp = 0.0;
      }
    const double dashForce = dashpot * TvRate * ratio_disp;

    // Limit the combined force to matCapacity.
    //
    if(fabs(T.P() + dashForce) >= (1.0-PYtolerance)*matCapacity)
      return (1.0-PYtolerance)*matCapacity*(T.P()+dashForce)/fabs(T.P()+dashForce);
    else
      return T.P() + dashForce;
  }

/////////////////////////////////////////////////////////////////////
double XC::PySimple1::getDampTangent(void) const
  {
    // Damping tangent is produced only by the far field component.
    // If converged, proportion by Tangents.
    // If not converged, proportion by ratio of displacements in components.
    //
    double ratio_disp =(1.0/TFar.tang())/(1.0/TFar.tang() + 1.0/TNF.tang() + 1.0/TGap.tang());
    if(T.y() != C.y())
      {
        ratio_disp = (TFar.y() - CFar.y())/(T.y() - C.y());
        if(ratio_disp > 1.0) ratio_disp = 1.0;
        if(ratio_disp < 0.0) ratio_disp = 0.0;
      }

    double DampTangent = dashpot * ratio_disp;

    // Minimum damping tangent referenced against Farfield spring
    //
    if(DampTangent < TFar.tang() * 1.0e-12) DampTangent = TFar.tang() * 1.0e-12;

    // Check if damping force is being limited
    //
    double totalForce = T.P() + dashpot * TvRate * ratio_disp;
    if(fabs(totalForce) >= (1.0-PYtolerance)*matCapacity) DampTangent = 0.0;

    return DampTangent;
  }

/////////////////////////////////////////////////////////////////////
int XC::PySimple1::commitState(void)
  {
    PQyzBase::commitState();
    CDrag= TDrag; // Commit trial history variables for Drag component
    CClose= TClose; // Commit trial history variables for Closure component
    return 0;
  }

/////////////////////////////////////////////////////////////////////
int XC::PySimple1::revertToLastCommit(void)
  {
    // Reset to committed values
    PQyzBase::revertToLastCommit();
    TDrag= CDrag;
    TClose= CClose;
    return 0;
  }

/////////////////////////////////////////////////////////////////////
int XC::PySimple1::revertToStart(void)
  {

    // If soilType = 0, then it is entering with the default constructor.
    // To avoid division by zero, set small nonzero values for terms.
    //
    if(soilType == 0)
      {
        matCapacity = 1.0e-12;
        v50  = 1.0e12;
      }

    // Reset gap "drag" if zero (or negative).
    //
    if(drag <= PYtolerance) drag = PYtolerance;

    // Only allow zero or positive dashpot values
    //
    if(dashpot < 0.0) dashpot = 0.0;

    // Do not allow zero or negative values for v50 or matCapacity.
    //
    if(matCapacity <= 0.0 || v50 <= 0.0)
      {
        std::cerr << "WARNING -- only accepts positive nonzero pult and y50" << std::endl;
        std::cerr << "PyLiq1: " << std::endl;
        std::cerr << "matCapacity: " << matCapacity << "   y50: " << v50 << std::endl;
        exit(-1);
      }

    // Initialize variables for Near Field rigid-plastic spring
    //
    if(soilType == 0)
      {        // This will happen with default constructor
        vRef= 10.0*v50;
        np= 5.0;
        Elast= 0.35;
        nd= 1.0;
        TFar.revertToStart(matCapacity/(8.0*pow(Elast,2.0)*v50));
      }
    else if(soilType ==1)
      {
        vRef  = 10.0*v50;
        np    = 5.0;
        Elast = 0.35;
        nd    = 1.0;
        TFar.revertToStart(matCapacity/(8.0*pow(Elast,2.0)*v50));
      }
    else if(soilType == 2)
      {
        vRef  = 0.5*v50;
        np    = 2.0;
        Elast = 0.2;
        nd    = 1.0;
        // This TFar.tang() assumes Elast=0.2, but changes very little for other
        // reasonable Elast values. i.e., API curves are quite linear initially.
        TFar.revertToStart(0.542*matCapacity/v50);
      }
    else
      {
        std::cerr << "WARNING -- only accepts soilType of 1 or 2" << std::endl;
        std::cerr << "PyLiq1: " << std::endl;
        std::cerr << "soilType: " << soilType << std::endl;
        exit(-1);
      }

    // Near Field components
    //
    NFkrig= 100.0 * (0.5 * matCapacity) / v50;
    TNF.PrightIn()= Elast*matCapacity;
    TNF.PleftIn()= -TNF.PrightIn();
    TNF.yrightIn()= TNF.PrightIn() / NFkrig;
    TNF.yleftIn()= -TNF.yrightIn();
    TNF.revertToStart(NFkrig);

    // Drag components
    //
    TDrag.revertToStart(nd*(matCapacity*drag-TDrag.P())*pow(v50/2.0,nd)*pow(v50/2.0 - TDrag.y() + TDrag.yin(),-nd-1.0));

    // Closure components
    //
    TClose.yleft() = -v50/100.0;
    TClose.yright()=  v50/100.0;
    TClose.revertToStart(1.8*matCapacity*(v50/50.0)*(pow(v50/50.0+ TClose.yright() - TClose.y(),-2.0)+pow(v50/50.0 + TClose.y() - TClose.yleft(),-2.0)));

    // Gap (Drag + Closure in parallel)
    //
    TGap.revertToStart(TClose.tang() + TDrag.tang());

    // Entire element (Far field + Near field + Gap in series)
    //
    T.revertToStart(pow(1.0/TGap.tang() + 1.0/TNF.tang() + 1.0/TFar.tang(), -1.0));
    TvRate= 0.0;

    // Now get all the committed variables initiated
    //
    this->commitState();

    return 0;
  }

//! @brief Set the variable that sets the drag resistance within a
//! fully-mobilized gap as Cd*getUltimateCapacity(). 
void XC::PySimple1::setDragResistanceFactor(const double &Cd)
  { drag= Cd; }
//! @brief Return the variable that sets the drag resistance within a
//! fully-mobilized gap as Cd*getUltimateCapacity().
double XC::PySimple1::getDragResistanceFactor(void) const
  { return drag; }

/////////////////////////////////////////////////////////////////////
XC::UniaxialMaterial *XC::PySimple1::getCopy(void) const
  { return new PySimple1(*this); }

//! @brief Send object members through the communicator argument.
int XC::PySimple1::sendData(Communicator &comm)
  {
    int res= PQyzBase::sendData(comm);
    res+= comm.sendDouble(drag,getDbTagData(),CommMetaData(14));
    res+= comm.sendMovable(CDrag,getDbTagData(),CommMetaData(15));
    res+= comm.sendMovable(TDrag,getDbTagData(),CommMetaData(16));
    res+= comm.sendMovable(CClose,getDbTagData(),CommMetaData(17));
    res+= comm.sendMovable(TClose,getDbTagData(),CommMetaData(18));
    return res;
  }

//! @brief Receives object members through the communicator argument.
int XC::PySimple1::recvData(const Communicator &comm)
  {
    int res= PQyzBase::recvData(comm);
    res+= comm.receiveDouble(drag,getDbTagData(),CommMetaData(14));
    res+= comm.receiveMovable(CDrag,getDbTagData(),CommMetaData(15));
    res+= comm.receiveMovable(TDrag,getDbTagData(),CommMetaData(16));
    res+= comm.receiveMovable(CClose,getDbTagData(),CommMetaData(17));
    res+= comm.receiveMovable(TClose,getDbTagData(),CommMetaData(18));
    return res;
  }

/////////////////////////////////////////////////////////////////////
int XC::PySimple1::sendSelf(Communicator &comm)
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
int XC::PySimple1::recvSelf(const Communicator &comm)
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
void XC::PySimple1::Print(std::ostream &s, int flag) const
  {
    s << "PySimple1" << std::endl;
    PQyzBase::Print(s,flag);
    s << "  drag: " << drag << std::endl;
  }

/////////////////////////////////////////////////////////////////////

