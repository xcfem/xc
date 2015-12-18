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
// $Date: 2003/02/14 23:01:38 $
// $Source: /usr/local/cvs/OpenSees/SRC/material/uniaxial/DrainMaterial.h,v $
                                                                      
// Written: MHS
// Created: June 2001
//
// Description: This file contains the class definition for 
// DrainMaterial. DrainMaterial wraps a Drain spring element subroutine
// and converts it to the UniaxialMaterial interface for use in
// zero length elements, beam sections, or anywhere else
// UniaxialMaterials may be used.
//
// For more information see the Drain-2DX user guide:
//    Allahabadi, R.; Powell, G. H.
//    UCB/EERC-88/06, Berkeley: Earthquake Engineering Research Center,
//    University of California, Mar. 1988, 1 vol.

#ifndef DrainMaterial_h
#define DrainMaterial_h

#include <material/uniaxial/UniaxialMaterial.h>

namespace XC {
//! @ingroup MatUnx
//
//! @brief ??.
class DrainMaterial : public UniaxialMaterial
  {
  private:
    double epsilon; //!< Trial strain
    double epsilonDot; //!< Trial strain rate
    double sigma; //!< Trial stress
    double tangent; //!< Trial tangent
  protected:
    
    std::vector<double> data; //!< Material parameters array
    std::vector<double> hstv; //!< History array: first half is committed, second half is trial

    int numData; //!< Number of material parameters
    int numHstv; //!< Number of history variables

    double epsilonP; //!< Committed strain
    double sigmaP; //!< Committed stress
    double tangentP; //!< Committed tangent

    double beto; //!< Stiffness proportional damping factor
    double initialTangent; //!< initial tangent

    // Invokes the Drain subroutine
    virtual int invokeSubroutine(void);
    int sendData(CommParameters &);
    int recvData(const CommParameters &);
  public:
    DrainMaterial(int tag, int classTag, int numHV, int numData, double beto = 0.0);

    virtual int setTrialStrain(double strain, double strainRate = 0.0);
    virtual int setTrial(double strain, double &stress, double &tangent, double strainRate = 0.0); 
    virtual double getStrain(void) const;
    virtual double getStrainRate(void) const;
    virtual double getStress(void) const;
    virtual double getTangent(void) const;
    virtual double getDampTangent(void) const;
    virtual double getInitialTangent(void) const;

    virtual int commitState(void);
    virtual int revertToLastCommit(void);    
    virtual int revertToStart(void);        

    // WARNING -- if you wish to override any method in this base class, you must
    // also override the getCopy method to return a pointer to the derived class!!!
    virtual UniaxialMaterial *getCopy(void) const;

    virtual int sendSelf(CommParameters &);  
    virtual int recvSelf(const CommParameters &);
    
    virtual void Print(std::ostream &s, int flag = 0);
  };
} // end of XC namespace


#endif

