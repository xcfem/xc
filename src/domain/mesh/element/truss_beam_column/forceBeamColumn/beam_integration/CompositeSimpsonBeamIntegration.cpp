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
** file 'COPYRIGHT'  in main directory for information on usage and   **
** redistribution,  and for a DISCLAIMER OF ALL WARRANTIES.           **
**                                                                    **
** Developed by:                                                      **
**   Frank McKenna (fmckenna@ce.berkeley.edu)                         **
**   Gregory L. Fenves (fenves@ce.berkeley.edu)                       **
**   Filip C. Filippou (filippou@ce.berkeley.edu)                     **
**                                                                    **
** ****************************************************************** */

// $Revision$
// $Date$
// $Source$

#include "CompositeSimpsonBeamIntegration.h"

XC::CompositeSimpsonBeamIntegration::CompositeSimpsonBeamIntegration():
  BeamIntegration(BEAM_INTEGRATION_TAG_CompositeSimpson)
  {
    // Nothing to do
  }

XC::BeamIntegration *XC::CompositeSimpsonBeamIntegration::getCopy(void) const
  { return new CompositeSimpsonBeamIntegration(*this); }

void XC::CompositeSimpsonBeamIntegration::getSectionLocations(int numSections, double L, double *xi) const
  {
    // Check that num sections is odd
    if(numSections % 2 == 1)
      {
        int numIntervals = (numSections+1)/2; // Num intervals is even
        double h = 1.0/numIntervals;
        xi[0] = 0.0;
        xi[numSections-1] = 1.0;
        for(int i = 1; i < numSections-1; i++)
          xi[i] = h*i;    
      }
    else
      {
	std::cerr << "CompositeSimpson, numSections must be odd (" << numSections << " was input)" << std::endl;
      }
  }

void XC::CompositeSimpsonBeamIntegration::getSectionWeights(int numSections, double L, double *wt) const
  {
    // Check that num sections is odd
    if(numSections % 2 == 1)
      {
        const int numIntervals = (numSections+1)/2; // Num intervals is even
        const double h = 1.0/numIntervals;
        wt[0] = h/3.0;
        wt[numSections-1] = h/3.0;
        for(int i = 1; i < numSections; i += 2)
          wt[i] = 4*h/3.0;  
        for(int i = 2; i < numSections-1; i += 2)
          wt[i] = 2*h/3.0;  
      }
    else 
      {
        std::cerr << "CompositeSimpson, numSections must be odd (" << numSections << " was input)" << std::endl;
      }
  }

void XC::CompositeSimpsonBeamIntegration::Print(std::ostream &s, int flag)
  {
    s << "CompositeSimpson" << std::endl;
  }
