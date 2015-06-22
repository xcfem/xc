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

// $Revision: 1.3 $
// $Date: 2002/12/05 22:49:09 $
// $Source: /usr/local/cvs/OpenSees/SRC/material/nD/ElasticIsotropicBeamFiber.cpp,v $

// Written: MHS
// Created: Aug 2001
//
// Description: Elastic isotropic model where stress components 22, 33, and 23
// are condensed out.

#include <material/nD/elastic_isotropic/ElasticIsotropicBeamFiber.h>           

#include <utility/matrix/Matrix.h>
#include "material/nD/TipoMaterialND.h"

XC::Vector XC::ElasticIsotropicBeamFiber::sigma(3);
XC::Matrix XC::ElasticIsotropicBeamFiber::D(3,3);

XC::ElasticIsotropicBeamFiber::ElasticIsotropicBeamFiber(int tag, double E, double nu, double rho) :
  ElasticIsotropicMaterial(tag, ND_TAG_ElasticIsotropicBeamFiber,3, E, nu, rho)
  {}

XC::ElasticIsotropicBeamFiber::ElasticIsotropicBeamFiber(int tag)
  : ElasticIsotropicMaterial(tag, ND_TAG_ElasticIsotropicBeamFiber,3, 0.0, 0.0)
  {}

XC::ElasticIsotropicBeamFiber::ElasticIsotropicBeamFiber()
  : ElasticIsotropicMaterial(0, ND_TAG_ElasticIsotropicBeamFiber,3, 0.0, 0.0)
  {}

int XC::ElasticIsotropicBeamFiber::setTrialStrainIncr(const XC::Vector &strain)
{
	return 0;
}

int
XC::ElasticIsotropicBeamFiber::setTrialStrainIncr(const XC::Vector &strain, const XC::Vector &rate)
{
	return 0;
}

const XC::Matrix &XC::ElasticIsotropicBeamFiber::getTangent(void) const
{
	double mu2 = E/(1.0+v);
	double lam = v*mu2/(1.0-2.0*v);
	double mu = 0.50*mu2;

	D(0,0) = mu*(mu2+3.0*lam)/(mu+lam);
	D(1,1) = mu;
	D(2,2) = mu;

	return D;
}

const XC::Matrix &XC::ElasticIsotropicBeamFiber::getInitialTangent(void) const
{
	double mu2 = E/(1.0+v);
	double lam = v*mu2/(1.0-2.0*v);
	double mu = 0.50*mu2;

	D(0,0) = mu*(mu2+3.0*lam)/(mu+lam);
	D(1,1) = mu;
	D(2,2) = mu;

	return D;
}

const XC::Vector &XC::ElasticIsotropicBeamFiber::getStress(void) const
  {
	double mu2 = E/(1.0+v);
	double lam = v*mu2/(1.0-2.0*v);
	double mu = 0.50*mu2;

	sigma(0) = mu*(mu2+3.0*lam)/(mu+lam)*epsilon(0);
	sigma(1) = mu*epsilon(1);
	sigma(2) = mu*epsilon(2);
	
	return sigma;
  }

int XC::ElasticIsotropicBeamFiber::commitState(void)
{
	return 0;
}

int XC::ElasticIsotropicBeamFiber::revertToLastCommit(void)
  { return 0; }

int XC::ElasticIsotropicBeamFiber::revertToStart(void)
  { return 0; }

XC::NDMaterial *XC::ElasticIsotropicBeamFiber::getCopy(void) const
  { return new ElasticIsotropicBeamFiber(*this); }

const std::string &XC::ElasticIsotropicBeamFiber::getType(void) const
  { return strTipoBeamFiber; }

int XC::ElasticIsotropicBeamFiber::getOrder(void) const
  { return 3; }

