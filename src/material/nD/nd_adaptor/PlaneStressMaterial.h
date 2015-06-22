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
// $Date: 2003/02/14 23:01:25 $
// $Source: /usr/local/cvs/OpenSees/SRC/material/nD/PlaneStressMaterial.h,v $

// Ed "C++" Love
//
// Generic Plane Stress Material
//
#ifndef PLANE_STRESS_MATERIAL_H
#define PLANE_STRESS_MATERIAL_H


#include "material/nD/NDAdaptorMaterial.h"

namespace XC{
//! @ingroup NDMat
//
//! @brief Material para problemas de tensión plana.
class PlaneStressMaterial: public NDAdaptorMaterial
  {
  private :
    //out of plane strains .. trial and committed
    double Tgamma02;
    double Tgamma12; 
    double Cgamma02;
    double Cgamma12; 

    static Vector stress;
    static Matrix tangent;

    int indexMap(int i) const;
  protected:
    int sendData(CommParameters &);
    int recvData(const CommParameters &);
  public: 
    PlaneStressMaterial(int tag= 0);
    PlaneStressMaterial(int tag, NDMaterial &the3DMaterial );


    //make a clone of this material
    NDMaterial *getCopy(void) const;
    NDMaterial *getCopy(const std::string &) const;

    //send back order of strain in vector form
    int getOrder(void) const;

    //send back order of strain in vector form
    const std::string &getType(void) const;

    //swap history variables
    int commitState(void); 

    //revert to last saved state
    int revertToLastCommit(void);

    //revert to start
    int revertToStart(void);

    //get the strain 
    int setTrialStrain( const Vector &strainFromElement );

    //send back the stress 
    const Vector& getStress(void) const;

    //send back the tangent 
    const Matrix& getTangent(void) const;

    //print out data
    void Print( std::ostream &s, int flag );

    int sendSelf(CommParameters &);
    int recvSelf(const CommParameters &);

  }; //end of PlaneStressMaterial declarations

} //fin namespace XC

#endif


