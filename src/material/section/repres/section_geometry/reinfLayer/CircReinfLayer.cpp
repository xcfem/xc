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
                                                                        
// $Revision: 1.3 $
// $Date: 2003/02/14 23:01:37 $
// $Source: /usr/local/cvs/OpenSees/SRC/material/section/repres/section_geometry/reinfLayer/CircReinfLayer.cpp,v $
                                                                        
                                                                        
// File: CircReinfLayer.C 
// Written by Remo M. de Souza 
// December 1998

#include <cmath>
#include <utility/matrix/Matrix.h>
#include <utility/matrix/Vector.h>

#include <material/section/repres/section_geometry/reinfBar/ReinfBar.h>
#include <material/section/repres/section_geometry/reinfLayer/CircReinfLayer.h>
#include "utility/utils/misc_utils/matem.h"


//! @brief Constructor.
XC::CircReinfLayer::CircReinfLayer(ListReinfLayer *owr,Material *mat)
  : ReinfLayer(owr,mat,0,0.0,0.0),
    centerPosit(), arcRad(0.0),
    initAng(0.0), finalAng(0.0)          
  {
    // Figure out final angle so that complete circle does not put
    // two bars at the same location
    const int num_barras= getNumReinfBars();
    if(num_barras > 0)
      finalAng = 360.0 - 360.0/num_barras;
  }


//! @brief Constructor.
XC::CircReinfLayer::CircReinfLayer(ListReinfLayer *owr,Material *mat, int numReinfBars, 
                                   double reinfBarArea,
                                   const Pos2d &centerPosition,
                                   double arcRadius, double initialAngle,
                                   double finalAngle)
  : ReinfLayer(owr,mat,numReinfBars,0.0,reinfBarArea),
    centerPosit(centerPosition), arcRad(arcRadius),
    initAng(initialAngle), finalAng(finalAngle)
  {}

//! @brief Constructor.
XC::CircReinfLayer::CircReinfLayer(ListReinfLayer *owr,Material *mat, int numReinfBars, double  reinfBarArea,
						  const Pos2d &centerPosition, double radius)
  : ReinfLayer(owr,mat,numReinfBars,0.0,reinfBarArea),
    centerPosit(centerPosition), arcRad(radius),
    initAng(0.0), finalAng(0.0)
  {
    // Figure out final angle so that complete circle does not put
    // two bars at the same location
    const int num_barras= getNumReinfBars();
    if(num_barras > 0)
      finalAng = 360.0 - 360.0/num_barras;
  }

double XC::CircReinfLayer::getMaxY(void) const
  {
    double retval= centerPosit(0) + arcRad*cos(initAng);
    const int num_barras= getNumReinfBars();
    if(num_barras>1)
      {
        const double dtheta= (finalAng-initAng)/(num_barras - 1);

        double theta;
        for(int i= 0;i<num_barras;i++)
          {
            theta= initAng + dtheta * i;
            retval= std::max(retval,centerPosit(0) + arcRad*cos(theta));
          }
      }
    return retval;
  }
double XC::CircReinfLayer::getMaxZ(void) const
  {
    double retval= centerPosit(1) + arcRad*sin(initAng);
    const int num_barras= getNumReinfBars();
    if(num_barras>1)
      {
        const double dtheta= (finalAng-initAng)/(num_barras - 1);

        double theta;
        for(int i= 0;i<num_barras;i++)
          {
            theta= initAng + dtheta * i;
            retval= std::max(retval,centerPosit(1) + arcRad*sin(theta));
          }
      }
    return retval;
  }
double XC::CircReinfLayer::getMinY(void) const
  {
    double retval= centerPosit(0) + arcRad*cos(initAng);
    const int num_barras= getNumReinfBars();
    if(num_barras>1)
      {
        const double dtheta= (finalAng-initAng)/(num_barras - 1);

        double theta;
        for(int i= 0;i<num_barras;i++)
          {
            theta= initAng + dtheta * i;
            retval= std::min(retval,centerPosit(0) + arcRad*cos(theta));
          }
      }
    return retval;
  }
double XC::CircReinfLayer::getMinZ(void) const
  {
    double retval= centerPosit(1) + arcRad*sin(initAng);
    const int num_barras= getNumReinfBars();
    if(num_barras>1)
      {
        const double dtheta= (finalAng-initAng)/(num_barras - 1);

        double theta;
        for(int i= 0;i<num_barras;i++)
          {
            theta= initAng + dtheta * i;
            retval= std::min(retval,centerPosit(1) + arcRad*sin(theta));
          }
      }
    return retval;
  }

//! @brief Return a vector that contains the bars of the reinforcement layer.
const XC::VectorReinfBar &XC::CircReinfLayer::getReinfBars(void) const
  {
    Vector barPosit(2);

    const int num_barras= getNumReinfBars();
    const double area_barra= getReinfBarArea();

    if(num_barras > 1)
      {
        const double dtheta = (finalAng-initAng) /(num_barras - 1);

        reinfBars.resize(num_barras);

        double theta;
        for(int i= 0;i<num_barras;i++)
          {
            theta = initAng + dtheta * i;
            barPosit(0) = centerPosit(0) + arcRad*cos(theta);
            barPosit(1) = centerPosit(1) + arcRad*sin(theta);
            reinfBars.put(i,ReinfBar(area_barra,getMaterialPtr(),barPosit));
          }
      }
    return reinfBars;
  }


Pos2d XC::CircReinfLayer::getCenterPosition(void) const
  { return centerPosit; }

double XC::CircReinfLayer::getArcRadius(void) const 
  { return arcRad; }

double XC::CircReinfLayer::getInitAngle(void) const 
  { return initAng; }

double XC::CircReinfLayer::getFinalAngle(void) const 
  { return finalAng; }

void XC::CircReinfLayer::setCenterPosition(const Pos2d &p)
  { centerPosit= p; }

void XC::CircReinfLayer::setArcRadius(const double &d)
  { arcRad= d; }

void XC::CircReinfLayer::setInitAngle(const double &d)
  { initAng= d; }

void XC::CircReinfLayer::setFinalAngle(const double &d)
  { finalAng= d; }


XC::ReinfLayer *XC::CircReinfLayer::getCopy(void) const
  { return new CircReinfLayer(*this); }


void XC::CircReinfLayer::Print(std::ostream &s, int flag) const
  {
     s << "\nReinforcing Layer type:  Circ";
     ReinfLayer::Print(s,flag);
     s << "\nCenter Position: " << centerPosit;
     s << "\nArc Radius: " << arcRad;
     s << "\nInitial angle: " << initAng;
     s << "\nFinal angle: " << finalAng;
  }


 
