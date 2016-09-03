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
                                                                        
// $Revision: 1.5 $
// $Date: 2002/12/05 22:49:10 $
// $Source: /usr/local/cvs/OpenSees/SRC/material/nD/ElasticIsotropicPlaneStress2D.cpp,v $
                                                                        
                                                                        

#include <material/nD/elastic_isotropic/ElasticIsotropicPlaneStress2D.h>           

#include <utility/matrix/Matrix.h>
#include "material/nD/TipoMaterialND.h"

XC::Vector XC::ElasticIsotropicPlaneStress2D::sigma(3);

XC::ElasticIsotropicPlaneStress2D::ElasticIsotropicPlaneStress2D(int tag, double E, double nu, double rho)
  : ElasticIsotropic2D(tag, ND_TAG_ElasticIsotropicPlaneStress2d, E, nu, rho)
  {}

XC::ElasticIsotropicPlaneStress2D::ElasticIsotropicPlaneStress2D(int tag)
  : ElasticIsotropic2D(tag, ND_TAG_ElasticIsotropicPlaneStress2d, 0.0, 0.0, 0.0)
  {}

XC::ElasticIsotropicPlaneStress2D::ElasticIsotropicPlaneStress2D():
  ElasticIsotropic2D(0, ND_TAG_ElasticIsotropicPlaneStress2d,0.0, 0.0, 0.0)
  {}

int XC::ElasticIsotropicPlaneStress2D::setTrialStrainIncr(const XC::Vector &strain)
{
  epsilon += strain;
  return 0;
}

int XC::ElasticIsotropicPlaneStress2D::setTrialStrainIncr(const XC::Vector &strain, const XC::Vector &rate)
{
  epsilon += strain;
  return 0;
}

const XC::Matrix &XC::ElasticIsotropicPlaneStress2D::getTangent(void) const
  {
    double d00 = E/(1.0-v*v);
    double d01 = v*d00;
    double d22 = 0.5*(d00-d01);

    D(0,0) = D(1,1) = d00;
    D(1,0) = D(0,1) = d01;
    D(2,2) = d22;

    return D;
  }

const XC::Matrix &XC::ElasticIsotropicPlaneStress2D::getInitialTangent(void) const
  {
    double d00 = E/(1.0-v*v);
    double d01 = v*d00;
    double d22 = 0.5*(d00-d01);

    D(0,0) = D(1,1) = d00;
    D(1,0) = D(0,1) = d01;
    D(2,2) = d22;

    return D;
  }

const XC::Vector &XC::ElasticIsotropicPlaneStress2D::getStress(void) const
  {
    double d00 = E/(1.0-v*v);
    double d01 = v*d00;
    double d22 = 0.5*(d00-d01);

    double eps0 = epsilon(0);
    double eps1 = epsilon(1);

    //sigma = D*epsilon;
    sigma(0) = d00*eps0 + d01*eps1;
    sigma(1) = d01*eps0 + d00*eps1;
    sigma(2) = d22*epsilon(2);
    return sigma;
  }

int XC::ElasticIsotropicPlaneStress2D::commitState(void)
  { return 0; }

int XC::ElasticIsotropicPlaneStress2D::revertToLastCommit(void)
  { return 0; }

int XC::ElasticIsotropicPlaneStress2D::revertToStart(void)
  {
    epsilon.Zero();
    return 0;
  }

XC::NDMaterial *XC::ElasticIsotropicPlaneStress2D::getCopy(void) const
  { return new ElasticIsotropicPlaneStress2D(*this); }

const std::string &XC::ElasticIsotropicPlaneStress2D::getType(void) const
  { return strTipoPlaneStress; }

