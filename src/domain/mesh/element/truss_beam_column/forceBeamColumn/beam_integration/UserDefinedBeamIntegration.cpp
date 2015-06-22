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
** file 'COPYRIGHT'  in XC::main directory for information on usage and   **
** redistribution,  and for a DISCLAIMER OF ALL WARRANTIES.           **
**                                                                    **
** Developed by:                                                      **
**   Frank McKenna (fmckenna@ce.berkeley.edu)                         **
**   Gregory L. Fenves (fenves@ce.berkeley.edu)                       **
**   Filip C. Filippou (filippou@ce.berkeley.edu)                     **
**                                                                    **
** ****************************************************************** */

// $Revision: 1.4 $
// $Date: 2003/06/10 00:36:09 $
// $Source: /usr/local/cvs/OpenSees/SRC/element/forceBeamColumn/UserDefinedBeamIntegration.cpp,v $

#include <domain/mesh/element/truss_beam_column/forceBeamColumn/beam_integration/UserDefinedBeamIntegration.h>

#include <utility/matrix/Vector.h>
#include <utility/actor/objectBroker/FEM_ObjectBroker.h>

XC::UserDefinedBeamIntegration::UserDefinedBeamIntegration(int nIP,
						       const XC::Vector &pt,
						       const XC::Vector &wt):
  BeamIntegration(BEAM_INTEGRATION_TAG_UserDefined),
  pts(nIP), wts(nIP)
  {
    for(int i = 0; i < nIP; i++)
      {
    if (pt(i) < 0.0 || pt(i) > 1.0)
      std::cerr << "XC::UserDefinedBeamIntegration::UserDefinedBeamIntegration -- point lies outside [0,1]" << std::endl;
    if (wt(i) < 0.0 || wt(i) > 1.0)
      std::cerr << "XC::UserDefinedBeamIntegration::UserDefinedBeamIntegration -- weight lies outside [0,1]" << std::endl;
    pts(i) = pt(i);
    wts(i) = wt(i);
  }
}

XC::UserDefinedBeamIntegration::UserDefinedBeamIntegration():
  BeamIntegration(BEAM_INTEGRATION_TAG_UserDefined)
{
 
}

void XC::UserDefinedBeamIntegration::getSectionLocations(int numSections,double L, double *xi) const
{
  int nIP = pts.Size();

  int i;
  for(i = 0; i < nIP; i++)
    xi[i] = pts(i);
  for( ; i < numSections; i++)
    xi[i] = 0.0;
}

void XC::UserDefinedBeamIntegration::getSectionWeights(int numSections,double L, double *wt) const
  {
    int nIP = wts.Size();

    int i;
    for(i = 0; i < nIP; i++)
      wt[i] = wts(i);
    for(;i<numSections;i++)
      wt[i] = 1.0;
  }

XC::BeamIntegration *XC::UserDefinedBeamIntegration::getCopy(void) const
  { return new UserDefinedBeamIntegration(*this); }

int XC::UserDefinedBeamIntegration::sendSelf(CommParameters &cp)
  { return -1; }

int XC::UserDefinedBeamIntegration::recvSelf(const CommParameters &cp)
  { return -1; }

void XC::UserDefinedBeamIntegration::Print(std::ostream &s, int flag)
  {
    s << "UserDefined" << std::endl;
    s << " Points: " << pts;
    s << " Weights: " << wts;
  }
