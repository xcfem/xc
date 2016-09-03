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
                                                                        
// $Revision: 1.3 $
// $Date: 2002/12/05 22:49:09 $
// $Source: /usr/local/cvs/OpenSees/SRC/material/nD/ElasticIsotropicAxiSymm.cpp,v $

#include "ElasticIsotropicAxiSymm.h"                                                                 

#include <utility/matrix/Matrix.h>
#include "material/nD/TipoMaterialND.h"

XC::Vector XC::ElasticIsotropicAxiSymm::sigma(4);
XC::Matrix XC::ElasticIsotropicAxiSymm::D(4,4);

XC::ElasticIsotropicAxiSymm::ElasticIsotropicAxiSymm(int tag, double E, double nu, double rho) :
  ElasticIsotropicMaterial(tag, ND_TAG_ElasticIsotropicAxiSymm,4, E, nu, rho)
  {}

XC::ElasticIsotropicAxiSymm::ElasticIsotropicAxiSymm(int tag)
  : ElasticIsotropicMaterial(tag, ND_TAG_ElasticIsotropicAxiSymm,4, 0.0, 0.0)
  {}

XC::ElasticIsotropicAxiSymm::ElasticIsotropicAxiSymm(void)
  : ElasticIsotropicMaterial(0, ND_TAG_ElasticIsotropicAxiSymm,4, 0.0, 0.0)
  {}

int XC::ElasticIsotropicAxiSymm::setTrialStrainIncr(const XC::Vector &strain)
  {
    epsilon+=strain;
    return 0;
  }

int
XC::ElasticIsotropicAxiSymm::setTrialStrainIncr(const XC::Vector &strain, const XC::Vector &rate)
{
	epsilon+=strain;

	return 0;
}

const XC::Matrix &XC::ElasticIsotropicAxiSymm::getTangent(void) const
{
	double mu2 = E/(1.0+v);
	double lam = v*mu2/(1.0-2.0*v);
	double mu = 0.50*mu2;

	D(0,0) = D(1,1) = D(2,2) = mu2+lam;
	D(0,1) = D(1,0) = lam;
	D(0,2) = D(2,0) = lam;
	D(1,2) = D(2,1) = lam;
	D(3,3) = mu;

	return D;
}

const XC::Matrix &XC::ElasticIsotropicAxiSymm::getInitialTangent(void) const
{
	double mu2 = E/(1.0+v);
	double lam = v*mu2/(1.0-2.0*v);
	double mu = 0.50*mu2;

	D(0,0) = D(1,1) = D(2,2) = mu2+lam;
	D(0,1) = D(1,0) = lam;
	D(0,2) = D(2,0) = lam;
	D(1,2) = D(2,1) = lam;
	D(3,3) = mu;

	return D;
}

const XC::Vector &XC::ElasticIsotropicAxiSymm::getStress(void) const
{
  double mu2 = E/(1.0+v);
  double lam = v*mu2/(1.0-2.0*v);
  double mu = 0.50*mu2;

  double eps0 = epsilon(0);
  double eps1 = epsilon(1);
  double eps2 = epsilon(2);

  mu2 += lam;

  //sigma = D*epsilon;
  sigma(0) = mu2*eps0 + lam*(eps1+eps2);
  sigma(1) = mu2*eps1 + lam*(eps0+eps2);
  sigma(2) = mu2*eps2 + lam*(eps0+eps1);
  sigma(3) = mu*epsilon(3);
	
  return sigma;
}

int XC::ElasticIsotropicAxiSymm::commitState(void)
{
  return 0;
}

int
XC::ElasticIsotropicAxiSymm::revertToLastCommit(void)
{
  return 0;
}

int
XC::ElasticIsotropicAxiSymm::revertToStart(void)
{
  epsilon.Zero();
  return 0;
}

XC::NDMaterial *XC::ElasticIsotropicAxiSymm::getCopy(void) const
  { return new ElasticIsotropicAxiSymm(*this); }

const std::string &XC::ElasticIsotropicAxiSymm::getType(void) const
  { return strTipoAxiSymmetric; }

int XC::ElasticIsotropicAxiSymm::getOrder(void) const
  { return 4; }

