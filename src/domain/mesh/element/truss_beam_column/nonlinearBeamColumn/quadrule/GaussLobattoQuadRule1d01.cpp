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
                                                                        
// $Revision: 1.4 $
// $Date: 2003/04/02 22:02:39 $
// $Source: /usr/local/cvs/OpenSees/SRC/element/nonlinearBeamColumn/quadrule/GaussLobattoQuadRule1d01.cpp,v $
                                                                        
                                                                        
// written: rms
// Created: 12/98
//
// Description: This file contains the implementation of 
// GaussLobattoQuadRule1d01 (Quadrature Rule,0).

#include <cstdlib>

#include <utility/matrix/Vector.h>
#include <utility/matrix/Matrix.h>

#include "GaussLobattoQuadRule1d01.h"

bool XC::GaussLobattoQuadRule1d01::dataSet = false;

double XC::GaussLobattoQuadRule1d01::ptsArray[maxOrder*(maxOrder+1)/2];
double XC::GaussLobattoQuadRule1d01::wtsArray[maxOrder*(maxOrder+1)/2];

 XC::Matrix XC::GaussLobattoQuadRule1d01::pts2 (&ptsArray[1],  2,  1);
 XC::Matrix XC::GaussLobattoQuadRule1d01::pts3 (&ptsArray[3],  3,  1);
 XC::Matrix XC::GaussLobattoQuadRule1d01::pts4 (&ptsArray[6],  4,  1);
 XC::Matrix XC::GaussLobattoQuadRule1d01::pts5 (&ptsArray[10], 5,  1);
 XC::Matrix XC::GaussLobattoQuadRule1d01::pts6 (&ptsArray[15], 6,  1);
 XC::Matrix XC::GaussLobattoQuadRule1d01::pts7 (&ptsArray[21], 7,  1);
 XC::Matrix XC::GaussLobattoQuadRule1d01::pts8 (&ptsArray[28], 8,  1);
 XC::Matrix XC::GaussLobattoQuadRule1d01::pts9 (&ptsArray[36], 9,  1);
 XC::Matrix XC::GaussLobattoQuadRule1d01::pts10(&ptsArray[45], 10, 1);

 XC::Vector XC::GaussLobattoQuadRule1d01::wts2 (&wtsArray[1],  2);
 XC::Vector XC::GaussLobattoQuadRule1d01::wts3 (&wtsArray[3],  3);
 XC::Vector XC::GaussLobattoQuadRule1d01::wts4 (&wtsArray[6],  4);
 XC::Vector XC::GaussLobattoQuadRule1d01::wts5 (&wtsArray[10], 5);
 XC::Vector XC::GaussLobattoQuadRule1d01::wts6 (&wtsArray[15], 6);
 XC::Vector XC::GaussLobattoQuadRule1d01::wts7 (&wtsArray[21], 7);
 XC::Vector XC::GaussLobattoQuadRule1d01::wts8 (&wtsArray[28], 8);
 XC::Vector XC::GaussLobattoQuadRule1d01::wts9 (&wtsArray[36], 9);
 XC::Vector XC::GaussLobattoQuadRule1d01::wts10(&wtsArray[45], 10);

XC::GaussLobattoQuadRule1d01::GaussLobattoQuadRule1d01()
  :order(0), myPts(0), myWts(0)
{
  if (dataSet == false) {

    // Not used!!!
    ptsArray[0] = 0.0;
    wtsArray[0] = 2.0;

    // Two points
    ptsArray[1] = -1.0;
    ptsArray[2] =  1.0;
    
    wtsArray[1] = 1.0;
    wtsArray[2] = 1.0;

    // Three points
    ptsArray[3] = -1.0;
    ptsArray[4] =  0.0;
    ptsArray[5] =  1.0;
    
    wtsArray[3] = 0.333333333333333;
    wtsArray[4] = 1.333333333333333;
    wtsArray[5] = 0.333333333333333;

    // Four points
    ptsArray[6] = -1.0;
    ptsArray[7] = -0.44721360;
    ptsArray[8] =  0.44721360;
    ptsArray[9] =  1.0;
    
    wtsArray[6] = 0.166666666666667;
    wtsArray[7] = 0.833333333333333;
    wtsArray[8] = 0.833333333333333;
    wtsArray[9] = 0.166666666666667;
      
    // Five points
    ptsArray[10] = -1.0;
    ptsArray[11] = -0.65465367;
    ptsArray[12] =  0.0;
    ptsArray[13] =  0.65465367;
    ptsArray[14] =  1.0;
    
    wtsArray[10] = 0.1;
    wtsArray[11] = 0.5444444444;
    wtsArray[12] = 0.7111111111;
    wtsArray[13] = 0.5444444444;
    wtsArray[14] = 0.1;

    // Six points
    ptsArray[15] = -1.0;
    ptsArray[16] = -0.7650553239;
    ptsArray[17] = -0.2852315164;
    ptsArray[18] =  0.2852315164;
    ptsArray[19] =  0.7650553239;
    ptsArray[20] =  1.0;
    
    wtsArray[15] = 0.06666666667;
    wtsArray[16] = 0.3784749562;
    wtsArray[17] = 0.5548583770;
    wtsArray[18] = 0.5548583770;
    wtsArray[19] = 0.3784749562;
    wtsArray[20] = 0.06666666667;

    // Seven points
    ptsArray[21] = -1.0;
    ptsArray[22] = -0.8302238962;
    ptsArray[23] = -0.4688487934;
    ptsArray[24] =  0.0;
    ptsArray[25] =  0.4688487934;
    ptsArray[26] =  0.8302238962;
    ptsArray[27] =  1.0;
    
    wtsArray[21] = 0.04761904762;
    wtsArray[22] = 0.2768260473;
    wtsArray[23] = 0.4317453812;
    wtsArray[24] = 0.4876190476;
    wtsArray[25] = 0.4317453812;
    wtsArray[26] = 0.2768260473;
    wtsArray[27] = 0.04761904762;

    // Eight points
    ptsArray[28] = -1.0;
    ptsArray[29] = -0.8717401485;
    ptsArray[30] = -0.5917001814;
    ptsArray[31] = -0.2092992179;
    ptsArray[32] =  0.2092992179;
    ptsArray[33] =  0.5917001814;
    ptsArray[34] =  0.8717401485;
    ptsArray[35] =  1.0;
    
    wtsArray[28] = 0.03571428571;
    wtsArray[29] = 0.2107042271;
    wtsArray[30] = 0.3411226924;
    wtsArray[31] = 0.4124587946;
    wtsArray[32] = 0.4124587946;
    wtsArray[33] = 0.3411226924;
    wtsArray[34] = 0.2107042271;
    wtsArray[35] = 0.03571428571;
      
    // Nine points
    ptsArray[36] = -1.0;
    ptsArray[37] = -0.8997579954;
    ptsArray[38] = -0.6771862795;
    ptsArray[39] = -0.3631174638;
    ptsArray[40] =  0.0;
    ptsArray[41] =  0.3631174638;
    ptsArray[42] =  0.6771862795;
    ptsArray[43] =  0.8997579954;
    ptsArray[44] =  1.0;
    
    wtsArray[36] = 0.02777777778;
    wtsArray[37] = 0.1654953615;
    wtsArray[38] = 0.2745387125;
    wtsArray[39] = 0.3464285109;
    wtsArray[40] = 0.3715192743;
    wtsArray[41] = 0.3464285109;
    wtsArray[42] = 0.2745387125;
    wtsArray[43] = 0.1654953615;
    wtsArray[44] = 0.02777777778;

    // Ten points
    ptsArray[45] = -1.0;
    ptsArray[46] = -0.9195339082;
    ptsArray[47] = -0.7387738651;
    ptsArray[48] = -0.4779249498;
    ptsArray[49] = -0.1652789577;
    ptsArray[50] =  0.1652789577;
    ptsArray[51] =  0.4779249498;
    ptsArray[52] =  0.7387738651;
    ptsArray[53] =  0.9195339082;
    ptsArray[54] =  1.0;
    
    wtsArray[45] = 0.02222222222;
    wtsArray[46] = 0.1333059908;
    wtsArray[47] = 0.2248893421;
    wtsArray[48] = 0.2920426836;
    wtsArray[49] = 0.3275397611;
    wtsArray[50] = 0.3275397611;
    wtsArray[51] = 0.2920426836;
    wtsArray[52] = 0.2248893421;
    wtsArray[53] = 0.1333059908;
    wtsArray[54] = 0.02222222222;

    for (int i = 0; i < 55; i++) {
      ptsArray[i]  = 0.5*(ptsArray[i] + 1.0);
      wtsArray[i] *= 0.5;
    }

    dataSet = true;
  }
}


int XC::GaussLobattoQuadRule1d01::setOrder(int quadOrder)
{
  if (quadOrder < 2 || quadOrder > maxOrder) {
    std::cerr << "GaussLobattoQuadRule1d01::" << __FUNCTION__
	      << "XC::GaussLobattoQuadRule1d01::setOrder() -- Invalid quadrature order, " << quadOrder << std::endl;
    exit(-1);
  }
  
  // Nothing needs to change if this is true
  if (order == quadOrder)
    return 0;
  
  order = quadOrder;

  switch (order) {
  case 2:
    myPts = &pts2;
    myWts = &wts2;
    break;
    
  case 3:
    myPts = &pts3;
    myWts = &wts3;
    break;
    
  case 4:
    myPts = &pts4;
    myWts = &wts4;
    break;
    
  case 5:
    myPts = &pts5;
    myWts = &wts5;
    break;
    
  case 6:
    myPts = &pts6;
    myWts = &wts6;
    break;
    
  case 7:
    myPts = &pts7;
    myWts = &wts7;
    break;
    
  case 8:
    myPts = &pts8;
    myWts = &wts8;
    break;
    
  case 9:
    myPts = &pts9;
    myWts = &wts9;
    break;
    
  case 10:
    myPts = &pts10;
    myWts = &wts10;
    break;

  default:
    std::cerr << "GaussLobattoQuadRule1d01::" << __FUNCTION__
	      << "; Invalid quadrature order."
              << std::endl;
    return -1;
    break;
  }    

  return 0;
}

int XC::GaussLobattoQuadRule1d01::getOrder (void) const
{
  return order;
}

int XC::GaussLobattoQuadRule1d01::getNumIntegrPoints (void) const
{
  return order;
}

const XC::Matrix &XC::GaussLobattoQuadRule1d01::getIntegrPointCoords (void) const
{
  if (order < 2 || order > maxOrder)
    std::cerr << "GaussLobattoQuadRule1d01::" << __FUNCTION__
	      << "; order " << order <<  " is currently invalid\n";

  return *myPts;
}

const XC::Vector &XC::GaussLobattoQuadRule1d01::getIntegrPointWeights (void) const
{
  if (order < 2 || order > maxOrder)
   std::cerr << "GaussLobattoQuadRule1d01::" << __FUNCTION__
	     << "; order " << order <<  " is currently invalid\n";
			    
  return *myWts;
}

const XC::Matrix & 
XC::GaussLobattoQuadRule1d01::getIntegrPointCoords (int quadOrder)
{
  if (order != quadOrder)
    this->setOrder(quadOrder);

  return *myPts;
}

const XC::Vector & 
XC::GaussLobattoQuadRule1d01::getIntegrPointWeights (int quadOrder)
{
  if (order != quadOrder)
    this->setOrder(quadOrder);

  return *myWts;
}
