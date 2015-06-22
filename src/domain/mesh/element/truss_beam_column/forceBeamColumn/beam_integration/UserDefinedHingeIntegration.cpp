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

// $Revision: 1.1 $
// $Date: 2006/01/18 21:58:24 $
// $Source: /usr/local/cvs/OpenSees/SRC/element/forceBeamColumn/UserDefinedHingeIntegration.cpp,v $

#include <domain/mesh/element/truss_beam_column/forceBeamColumn/beam_integration/UserDefinedHingeIntegration.h>

#include <utility/matrix/Matrix.h>
#include <utility/matrix/Vector.h>
#include <utility/actor/objectBroker/FEM_ObjectBroker.h>
#include <domain/mesh/element/Information.h>

XC::UserDefinedHingeIntegration::UserDefinedHingeIntegration(int npL,
							 const XC::Vector &ptL,
							 const XC::Vector &wtL,
							 int npR,
							 const XC::Vector &ptR,
							 const XC::Vector &wtR):
  BeamIntegration(BEAM_INTEGRATION_TAG_UserHinge),
  ptsL(npL), wtsL(npL), ptsR(npR), wtsR(npR)
{
  int i;
  for (i = 0; i < npL; i++) {
    if (ptL(i) < 0.0 || ptL(i) > 1.0)
      std::cerr << "XC::UserDefinedHingeIntegration::UserDefinedHingeIntegration -- point lies outside [0,1]" << std::endl;
    if (wtL(i) < 0.0 || wtL(i) > 1.0)
      std::cerr << "XC::UserDefinedHingeIntegration::UserDefinedHingeIntegration -- weight lies outside [0,1]" << std::endl;
    ptsL(i) = ptL(i);
    wtsL(i) = wtL(i);
  }

  for (i = 0; i < npR; i++) {
    if (ptR(i) < 0.0 || ptR(i) > 1.0)
      std::cerr << "XC::UserDefinedHingeIntegration::UserDefinedHingeIntegration -- point lies outside [0,1]" << std::endl;
    if (wtR(i) < 0.0 || wtR(i) > 1.0)
      std::cerr << "XC::UserDefinedHingeIntegration::UserDefinedHingeIntegration -- weight lies outside [0,1]" << std::endl;
    ptsR(i) = ptR(i);
    wtsR(i) = wtR(i);
  }
}

XC::UserDefinedHingeIntegration::UserDefinedHingeIntegration():
  BeamIntegration(BEAM_INTEGRATION_TAG_UserHinge)
{

}

void XC::UserDefinedHingeIntegration::getSectionLocations(int numSections,double L, double *xi) const
{
  int npL = ptsL.Size();
  int npR = ptsR.Size();

  double lpI = 0.0;
  double lpJ = 0.0;
  int i, j;
  for (i = 0; i < npL; i++) {
    xi[i] = ptsL(i);
    lpI += wtsL(i);
  }
  for (j = 0; j < npR; j++, i++) {
    xi[i] = ptsR(j);
    lpJ += wtsR(j);
  }

  double alpha = 0.5-0.5*(lpI+lpJ);
  double beta  = 0.5+0.5*(lpI-lpJ);
  xi[i++] = alpha*(-1/sqrt(3.0)) + beta;
  xi[i++] = alpha*(1/sqrt(3.0)) + beta;

  for ( ; i < numSections; i++)
    xi[i] = 0.0;
}

void XC::UserDefinedHingeIntegration::getSectionWeights(int numSections,double L, double *wt) const
  {
    int npL = wtsL.Size();
    int npR = wtsR.Size();

    double lpI = 0.0;
    double lpJ = 0.0;
    int i, j;
    for(i = 0; i < npL; i++)
      {
        wt[i] = wtsL(i);
        lpI += wtsL(i);
      }
    for(j = 0; j < npR; j++, i++)
      {
        wt[i] = wtsR(j);
        lpJ += wtsR(j);
      }

    //double oneOverL = 1.0/L;
    wt[i++] = 0.5-0.5*(lpI+lpJ);
    wt[i++] = 0.5-0.5*(lpI+lpJ);
    for(;i<numSections;i++)
      wt[i] = 1.0;
  }

XC::BeamIntegration* XC::UserDefinedHingeIntegration::getCopy(void) const
  { return new UserDefinedHingeIntegration(*this); }

int XC::UserDefinedHingeIntegration::sendSelf(CommParameters &cp)
  { return -1; }

int XC::UserDefinedHingeIntegration::recvSelf(const CommParameters &cp)
  { return -1; }

void
XC::UserDefinedHingeIntegration::Print(std::ostream &s, int flag)
{
  s << "UserHinge" << std::endl;
  s << " Points hinge I: " << ptsL;
  s << " Weights hinge I: " << wtsL;
  s << " Points hinge J: " << ptsR;
  s << " Weights hinge J: " << wtsR;
}
