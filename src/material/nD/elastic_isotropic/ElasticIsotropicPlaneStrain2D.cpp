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
                                                                        
// $Revision: 1.5 $
// $Date: 2002/12/05 22:49:09 $
// $Source: /usr/local/cvs/OpenSees/SRC/material/nD/ElasticIsotropicPlaneStrain2D.cpp,v $
                                                                        
                                                                        
#include <material/nD/elastic_isotropic/ElasticIsotropicPlaneStrain2D.h>                                                                        

#include <utility/matrix/Matrix.h>
#include "material/nD/TipoMaterialND.h"

XC::Vector XC::ElasticIsotropicPlaneStrain2D::sigma(3);

XC::ElasticIsotropicPlaneStrain2D::ElasticIsotropicPlaneStrain2D(int tag, double E, double nu, double rho)
  : ElasticIsotropic2D(tag, ND_TAG_ElasticIsotropicPlaneStrain2d, E, nu, rho)
  {}

XC::ElasticIsotropicPlaneStrain2D::ElasticIsotropicPlaneStrain2D(int tag)
  : ElasticIsotropic2D(tag, ND_TAG_ElasticIsotropicPlaneStrain2d, 0.0, 0.0, 0.0)
  {}

XC::ElasticIsotropicPlaneStrain2D::ElasticIsotropicPlaneStrain2D(void)
  : ElasticIsotropic2D(0, ND_TAG_ElasticIsotropicPlaneStrain2d, 0.0, 0.0, 0.0)
  {}

int XC::ElasticIsotropicPlaneStrain2D::setTrialStrainIncr(const XC::Vector &strain)
  {
    epsilon+= strain;
    return 0;
  }

int XC::ElasticIsotropicPlaneStrain2D::setTrialStrainIncr(const XC::Vector &strain, const XC::Vector &rate)
  {
    epsilon += strain;
    return 0;
  }

const XC::Matrix &XC::ElasticIsotropicPlaneStrain2D::getTangent(void) const
  {
    double mu2 = E/(1.0+v);
    double lam = v*mu2/(1.0-2.0*v);
    double mu = 0.50*mu2;

        D(0,0) = D(1,1) = mu2+lam;
        D(0,1) = D(1,0) = lam;
        D(2,2) = mu;

        return D;
}

const XC::Matrix &XC::ElasticIsotropicPlaneStrain2D::getInitialTangent(void) const
  {
        double mu2 = E/(1.0+v);
        double lam = v*mu2/(1.0-2.0*v);
        double mu = 0.50*mu2;

        D(0,0) = D(1,1) = mu2+lam;
        D(0,1) = D(1,0) = lam;
        D(2,2) = mu;

        return D;
  }

const XC::Vector &XC::ElasticIsotropicPlaneStrain2D::getStress(void) const
  {
    double mu2 = E/(1.0+v);
    const double lam = v*mu2/(1.0-2.0*v);
    const double mu = 0.50*mu2;

    const double eps0 = epsilon(0);
    const double eps1 = epsilon(1);

    mu2+= lam;

    //sigma = D*epsilon;
    sigma(0) = mu2*eps0 + lam*eps1;
    sigma(1) = lam*eps0 + mu2*eps1;
    sigma(2) = mu*epsilon(2);
    return sigma;
  }

int XC::ElasticIsotropicPlaneStrain2D::commitState(void)
  { return 0; }

int XC::ElasticIsotropicPlaneStrain2D::revertToLastCommit(void)
  { return 0; }

int XC::ElasticIsotropicPlaneStrain2D::revertToStart(void)
  {
    epsilon.Zero();
    return 0;
  }

XC::NDMaterial *XC::ElasticIsotropicPlaneStrain2D::getCopy(void) const
  { return new XC::ElasticIsotropicPlaneStrain2D(*this); }

const std::string &XC::ElasticIsotropicPlaneStrain2D::getType(void) const
  { return strTipoPlaneStrain; }

