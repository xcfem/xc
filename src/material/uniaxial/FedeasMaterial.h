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
                                                                        
// $Revision: 1.7 $
// $Date: 2004/07/15 21:34:10 $
// $Source: /usr/local/cvs/OpenSees/SRC/material/uniaxial/FedeasMaterial.h,v $
                                                                      
// Written: MHS
// Created: Jan 2001
//
// Description: This file contains the class definition for 
// FedeasMaterial. FedeasMaterial provides a FORTRAN interface
// for programming uniaxial material models, using the subroutine
// interface from the FEDEAS ML1D library, developed by F.C. Filippou.
//
// For more information visit the FEDEAS web page:
//    http://www.ce.berkeley.edu/~filippou/Research/fedeas.htm

#ifndef FedeasMaterial_h
#define FedeasMaterial_h

#include "UniaxialMaterial.h"
#include "UniaxialStateVars.h"

namespace XC {
//! @ingroup MatUnx
//
//! @brief FedeasMaterial provides a FORTRAN interface
//! for programming uniaxial material models, using the subroutine
//! interface from the FEDEAS ML1D library, developed by F.C. Filippou.
class FedeasMaterial : public UniaxialMaterial
  {
  protected:
    // Invokes the FORTRAN subroutine
    virtual int invokeSubroutine(int ist);
  
    double *data; //!< Material parameters array
    double *hstv; //!< History array: first half is committed, second half is trial
  
    int numData; //!< Number of material parameters
    int numHstv; //!< Number of history variables
  
    UniaxialStateVars converged; //!< Committed state
    UniaxialStateVars trial; //!< Trial state

    int sendData(CommParameters &);
    int recvData(const CommParameters &);
  public:
    FedeasMaterial(int tag, int classTag, int numHV, int numData);
    virtual ~FedeasMaterial(void);
  
    virtual int setTrialStrain(double strain, double strainRate = 0.0);
    virtual int setTrial(double strain, double &stress, double &tangent, double strainRate= 0.0);
    virtual double getStrain(void) const;
    virtual double getStress(void) const;
    virtual double getTangent(void) const;
  
    virtual int commitState(void);
    virtual int revertToLastCommit(void);    
    virtual int revertToStart(void);
  
    virtual int sendSelf(CommParameters &);  
    virtual int recvSelf(const CommParameters &);
  
    virtual void Print(std::ostream &s, int flag = 0);
  };
} // fin namespace XC

#endif
