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
                                                                        
// $Revision: 1.4 $
// $Date: 2003/02/14 23:01:24 $
// $Source: /usr/local/cvs/OpenSees/SRC/material/nD/BeamFiberMaterial.h,v $

// Written: MHS
// Created: Aug 2001
//
// Description: This file contains the class definition of BeamFiberMaterial.
// The BeamFiberMaterial class is a wrapper class that performs static
// condensation on a three-dimensional material model to give the 11, 12, and 13
// stress components which can then be integrated over an area to model a
// shear flexible 3D beam.

#include <utility/matrix/Matrix.h>
#include <material/nD/NDAdaptorMaterial.h>

namespace XC {
//! @ingroup NDMat
//
//! @brief Material para fibras una sección.
class BeamFiberMaterial: public NDAdaptorMaterial
  {
  private:
    double Tstrain33;
    double Tgamma23;
    double Cstrain33;
    double Cgamma23;

    static Vector stress;
    static Matrix tangent;

    int indexMap(int i) const;
  protected:
    int sendData(CommParameters &);
    int recvData(const CommParameters &);
  public:
    BeamFiberMaterial(int tag, NDMaterial &theMat);
    BeamFiberMaterial(void);

    int setTrialStrain( const Vector &strainFromElement);
    const Vector &getStress(void) const;
    const Matrix &getTangent(void) const;
    const Matrix &getInitialTangent(void) const;

    int commitState(void);
    int revertToLastCommit(void);
    int revertToStart(void);

    NDMaterial *getCopy(void) const;
    NDMaterial *getCopy(const std::string &) const;
    const std::string &getType(void) const;
    int getOrder(void) const; 

    void Print(std::ostream &s, int flag);

    int sendSelf(CommParameters &);
    int recvSelf(const CommParameters &);
  };
} // fin namespace XC





