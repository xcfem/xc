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
// $Date: 2003/02/14 23:01:34 $
// $Source: /usr/local/cvs/OpenSees/SRC/material/section/MembranePlateFiberSection.h,v $

// Ed "C++" Love
//
// Generic Plate Section with membrane
//

#ifndef MEMBRANEPLATEFIBERSECTION_H
#define MEMBRANEPLATEFIBERSECTION_H

#include <utility/matrix/Vector.h>
#include <utility/matrix/Matrix.h>
#include "PlateBase.h"


namespace XC {

class NDMaterial;

//! \ingroup MATPLAC
//
//! @brief Sección modelizada mediante fibras para materiales capaces de
//! trabajar como placa y como membrana.
class MembranePlateFiberSection: public PlateBase
  {
  private:
    //quadrature data
    static const double sg[5];
    static const double wg[5];
    NDMaterial *theFibers[5];  //pointers to five materials (fibers)
    static const double root56; // =sqrt(5/6) 
    Vector strainResultant;
    static Vector stressResultant;
    static Matrix tangent;
  protected:
    int sendData(CommParameters &);
    int recvData(const CommParameters &);
  public : 
    MembranePlateFiberSection(int tag);
    MembranePlateFiberSection(void);
    MembranePlateFiberSection(int tag, double thickness, NDMaterial &Afiber);
    virtual ~MembranePlateFiberSection(void);

    SectionForceDeformation *getCopy(void) const;
    double getRho(void) const;
    int getOrder(void) const;
    const ResponseId &getType(void) const;

    
    int commitState(void); //swap history variables
    int revertToLastCommit(void); //revert to last saved state
    int revertToStart(void); //revert to start

    int setInitialSectionDeformation(const Vector &strain_from_element);
    int setTrialSectionDeformation(const Vector &strain_from_element);
    const Vector &getInitialSectionDeformation(void) const;
    const Vector& getSectionDeformation(void) const; //send back the strain
    const Vector &getStressResultant(void) const; //send back the stress 
    const Matrix &getSectionTangent(void) const; //send back the tangent 
    const Matrix &getInitialTangent(void) const //send back the initial tangent 
      {return this->getSectionTangent();}

    //print out data
    void Print( std::ostream &s, int flag );

    int sendSelf(CommParameters &);
    int recvSelf(const CommParameters &);
  }; //end of MembranePlateFiberSection declarations

} // end of XC namespace

#endif
