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
// $Source: /usr/local/cvs/OpenSees/SRC/element/forceBeamColumn/UserDefinedHingeIntegration2d.cpp,v $

#include <domain/mesh/element/truss_beam_column/forceBeamColumn/beam_integration/UserDefinedHingeIntegration2d.h>

#include <utility/matrix/Matrix.h>
#include <utility/matrix/Vector.h>
#include <utility/actor/objectBroker/FEM_ObjectBroker.h>
#include <domain/mesh/element/Information.h>
#include "domain/component/Parameter.h"

XC::UserDefinedHingeIntegration2d::UserDefinedHingeIntegration2d(int npL,
                                                             const XC::Vector &ptL,
                                                             const XC::Vector &wtL,
                                                             int npR,
                                                             const XC::Vector &ptR,
                                                             const XC::Vector &wtR,
                                                             double ee,
                                                             double aa,
                                                             double ii):
  BeamIntegration(BEAM_INTEGRATION_TAG_UserHinge2d),
  ptsL(npL), wtsL(npL), ptsR(npR), wtsR(npR),
  E(ee), A(aa), I(ii)
{
  int i;
  for (i = 0; i < npL; i++) {
    if (ptL(i) < 0.0 || ptL(i) > 1.0)
      std::cerr << "XC::UserDefinedHingeIntegration2d::UserDefinedHingeIntegration2d -- point lies outside [0,1]" << std::endl;
    if (wtL(i) < 0.0 || wtL(i) > 1.0)
      std::cerr << "XC::UserDefinedHingeIntegration2d::UserDefinedHingeIntegration2d -- weight lies outside [0,1]" << std::endl;
    ptsL(i) = ptL(i);
    wtsL(i) = wtL(i);
  }

  for (i = 0; i < npR; i++) {
    if (ptR(i) < 0.0 || ptR(i) > 1.0)
      std::cerr << "XC::UserDefinedHingeIntegration2d::UserDefinedHingeIntegration2d -- point lies outside [0,1]" << std::endl;
    if (wtR(i) < 0.0 || wtR(i) > 1.0)
      std::cerr << "XC::UserDefinedHingeIntegration2d::UserDefinedHingeIntegration2d -- weight lies outside [0,1]" << std::endl;
    ptsR(i) = ptR(i);
    wtsR(i) = wtR(i);
  }
}

XC::UserDefinedHingeIntegration2d::UserDefinedHingeIntegration2d():
  BeamIntegration(BEAM_INTEGRATION_TAG_UserHinge2d),
  E(0.0), A(0.0), I(0.0)
{

}

void XC::UserDefinedHingeIntegration2d::getSectionLocations(int numSections,double L, double *xi) const
{
  int npL = ptsL.Size();
  int npR = ptsR.Size();

  int i, j;
  for (i = 0; i < npL; i++)
    xi[i] = ptsL(i);
  for (j = 0; j < npR; j++, i++)
    xi[i] = ptsR(j);
  for ( ; i < numSections; i++)
    xi[i] = 0.0;
}

void XC::UserDefinedHingeIntegration2d::getSectionWeights(int numSections,double L, double *wt) const
{
  int npL = wtsL.Size();
  int npR = wtsR.Size();

  int i, j;
  for (i = 0; i < npL; i++)
    wt[i] = wtsL(i);
  for (j = 0; j < npR; j++, i++)
    wt[i] = wtsR(j);
  for ( ; i < numSections; i++)
    wt[i] = 1.0;
}

int
XC::UserDefinedHingeIntegration2d::addElasticFlexibility(double L, Matrix &fElastic)
{
  int npL = wtsL.Size();
  int npR = wtsR.Size();

  double betaI = 0.0;
  double betaJ = 0.0;

  int i;
  for (i = 0; i < npL; i++)
    betaI += wtsL(i);
  for (i = 0; i < npR; i++)
    betaJ += wtsR(i);

  // Length of elastic interior
  double Le = L*(1.0-betaI-betaJ);
  double LoverEA  = Le/(E*A);
  double Lover3EI = Le/(3*E*I);
  double Lover6EI = 0.5*Lover3EI;
  
  // Elastic flexibility of element interior
  static XC::Matrix fe(2,2);
  fe(0,0) = fe(1,1) =  Lover3EI;
  fe(0,1) = fe(1,0) = -Lover6EI;
  
  // Equilibrium transformation matrix
  static XC::Matrix B(2,2);
  B(0,0) = 1.0 - betaI;
  B(1,1) = 1.0 - betaJ;
  B(0,1) = -betaI;
  B(1,0) = -betaJ;
  
  // Transform the elastic flexibility of the element
  // interior to the basic system
  static XC::Matrix ftmp(2,2);
  ftmp.addMatrixTripleProduct(0.0, B, fe, 1.0);

  fElastic(0,0) += LoverEA;
  fElastic(1,1) += ftmp(0,0);
  fElastic(1,2) += ftmp(0,1);
  fElastic(2,1) += ftmp(1,0);
  fElastic(2,2) += ftmp(1,1);

  return -1;
}

void XC::UserDefinedHingeIntegration2d::addElasticDeformations(ElementalLoad *theLoad, double loadFactor, double L, double *v0)
  { return; }

XC::BeamIntegration *XC::UserDefinedHingeIntegration2d::getCopy(void) const
  { return new UserDefinedHingeIntegration2d(*this); }

int XC::UserDefinedHingeIntegration2d::sendSelf(CommParameters &cp)
{
  return -1;
}

int
XC::UserDefinedHingeIntegration2d::recvSelf(const CommParameters &cp)
{
  return -1;
}

int XC::UserDefinedHingeIntegration2d::setParameter(const std::vector<std::string> &argv, Parameter &param)
  {
    if(argv[0] == "E")
      return param.addObject(1, this);
    else if(argv[0] == "A")
      return param.addObject(2, this);
    else if(argv[0] == "I")
      return param.addObject(3, this);
    else 
      return -1;
  }

int
XC::UserDefinedHingeIntegration2d::updateParameter(int parameterID,
                                             Information &info)
{
  switch (parameterID) {
  case 1:
    E = info.theDouble;
    return 0;
  case 2:
    A = info.theDouble;
    return 0;
  case 3:
    I = info.theDouble;
    return 0;
  default:
    return -1;
  }
}

int
XC::UserDefinedHingeIntegration2d::activateParameter(int parameterID)
{
  // For Terje to do
  return 0;
}

void
XC::UserDefinedHingeIntegration2d::Print(std::ostream &s, int flag)
{
  s << "UserHinge2d" << std::endl;
  s << " E = " << E;
  s << " A = " << A;
  s << " I = " << I << std::endl;
  s << " Points left hinge: " << ptsL;
  s << " Weights left hinge: " << wtsL;
  s << " Points right hinge: " << ptsR;
  s << " Weights right hinge: " << wtsR;
}
