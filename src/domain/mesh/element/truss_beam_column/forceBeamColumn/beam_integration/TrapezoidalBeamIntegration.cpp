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

// $Revision: 1.1 $
// $Date: 2007-10-12 20:57:53 $
// $Source: /usr/local/cvs/OpenSees/SRC/element/forceBeamColumn/TrapezoidalBeamIntegration.cpp,v $

#include "TrapezoidalBeamIntegration.h"

XC::TrapezoidalBeamIntegration::TrapezoidalBeamIntegration()
  : BeamIntegration(BEAM_INTEGRATION_TAG_Trapezoidal)
  {
    // Nothing to do
  }

XC::BeamIntegration *XC::TrapezoidalBeamIntegration::getCopy(void) const
  { return new TrapezoidalBeamIntegration(*this); }

void XC::TrapezoidalBeamIntegration::getSectionLocations(int numSections, double L,double *xi) const
  {
    if(numSections > 1)
      {
        xi[0] = -1.0;
        xi[numSections-1] = 1.0;

        double dxi = 2.0/(numSections-1);

        for(int i = 1; i < numSections-1; i++)
          xi[i] = -1.0 + dxi*i;
      }
    for (int i = 0; i < numSections; i++)
      xi[i]  = 0.5*(xi[i] + 1.0);
  }

void XC::TrapezoidalBeamIntegration::getSectionWeights(int numSections, double L, double *wt) const
  {
    if(numSections > 1)
      {
        const double wti= 2.0/(numSections-1);
        for(int i = 1; i < numSections-1; i++)
          wt[i] = wti;

        wt[0] = wt[numSections-1] = 0.5*wti;
      }
    for(int i = 0; i < numSections; i++)
      wt[i] *= 0.5;
  }

void XC::TrapezoidalBeamIntegration::Print(std::ostream &s, int flag)
  {
    s << "Trapezoidal" << std::endl;
  }
