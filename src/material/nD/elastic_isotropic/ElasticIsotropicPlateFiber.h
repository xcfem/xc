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
                                                                        
// $Revision: 1.6 $
// $Date: 2002/12/05 22:49:10 $
// $Source: /usr/local/cvs/OpenSees/SRC/material/nD/ElasticIsotropicPlateFiber.h,v $
                                                                        
                                                                        
#ifndef ElasticIsotropicPlateFiber_h
#define ElasticIsotropicPlateFiber_h

// File: ~/material/ElasticIsotropicPlateFiber.h
//
// Written: MHS
// Created: Feb 2000
// Revision: A
//
// Description: 
//
// What: "@(#) ElasticIsotropicPlateFiber.h, revA"

#include <material/nD/ElasticIsotropicMaterial.h>

namespace XC {
//! @ingroup EINDMat
//
//! @brief Material elástico e isótropo para
//! problemas de placas.
class ElasticIsotropicPlateFiber : public ElasticIsotropicMaterial
  {
  private:
    static Vector sigma;	// Stress vector ... class-wide for returns
    static Matrix D;		// Elastic constants
  public:
    ElasticIsotropicPlateFiber(int tag, double E, double nu, double rho);
    ElasticIsotropicPlateFiber(int tag);
    ElasticIsotropicPlateFiber(void);

    int setTrialStrainIncr(const Vector &v);
    int setTrialStrainIncr(const Vector &v, const Vector &r);
    const Matrix &getTangent(void) const;
    const Matrix &getInitialTangent(void) const;

    const Vector &getStress(void) const;
        
    int commitState(void);
    int revertToLastCommit(void);
    int revertToStart(void);
    
    NDMaterial *getCopy(void) const;
    const std::string &getType(void) const;
    int getOrder(void) const;
  };
} // fin namespace XC


#endif
