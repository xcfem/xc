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
                                                                        
// $Revision: 1.5 $
// $Date: 2003/04/04 16:55:03 $
// $Source: /usr/local/cvs/OpenSees/SRC/element/nonlinearBeamColumn/matrixutil/MatrixUtil.cpp,v $
 
#include <cmath>
#include <cstdlib>
#include <utility/matrix/Vector.h>
#include <domain/mesh/element/truss_beam_column/nonlinearBeamColumn/matrixutil/MatrixUtil.h>



void XC::getCBDIinfluenceMatrix(int nIntegrPts, const Matrix &xi_pt, double L, Matrix &ls)
{
   // setup Vandermode and CBDI influence matrices
   int i, j, i0, j0;
   double xi;
   Matrix G(nIntegrPts, nIntegrPts); 
   Matrix Ginv(nIntegrPts, nIntegrPts);
   Matrix l(nIntegrPts, nIntegrPts);
   Matrix I(nIntegrPts,nIntegrPts);      // an identity matrix for matrix inverse

   for (i = 1; i <= nIntegrPts; i++)
      for (j = 1; j <= nIntegrPts; j++)
      {
         i0 = i - 1;
         j0 = j - 1;
         xi = xi_pt(i0,0);
         G(i0,j0) =  pow(xi,j-1);
         l(i0,j0) = (pow(xi,j+1)-xi)/(j*(j+1));
      }
   
   I.Zero();
   for (i=0; i<nIntegrPts; i++)
     I(i,i) = 1.0;

   //invertMatrix(nIntegrPts, G, Ginv);
   if (G.Solve(I,Ginv) < 0)
     std::cerr << "XC::LargeDispBeamCol3d::getCBDIinfluenceMatrix() - could not invert G\n";
      
   // ls = l * Ginv * (L*L);
   ls.addMatrixProduct(0.0, l, Ginv, L*L);
}

void XC::getCBDIinfluenceMatrix(int nIntegrPts, double *pts, double L, XC::Matrix &ls)
{
   // setup Vandermode and CBDI influence matrices
   int i, j, i0, j0;
   double xi;
   Matrix G(nIntegrPts, nIntegrPts); 
   Matrix Ginv(nIntegrPts, nIntegrPts);
   Matrix l(nIntegrPts, nIntegrPts);
   Matrix I(nIntegrPts,nIntegrPts);      // an identity matrix for matrix inverse

   for (i = 1; i <= nIntegrPts; i++)
      for (j = 1; j <= nIntegrPts; j++)
      {
         i0 = i - 1;
         j0 = j - 1;
         xi = pts[i0];
         G(i0,j0) =  pow(xi,j-1);
         l(i0,j0) = (pow(xi,j+1)-xi)/(j*(j+1));
      }
   
   I.Zero();
   for (i=0; i<nIntegrPts; i++)
     I(i,i) = 1.0;

   //invertMatrix(nIntegrPts, G, Ginv);
   if (G.Solve(I,Ginv) < 0)
     std::cerr << "XC::LargeDispBeamCol3d::getCBDIinfluenceMatrix() - could not invert G\n";
      
   // ls = l * Ginv * (L*L);
   ls.addMatrixProduct(0.0, l, Ginv, L*L);
}





