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

// $Revision: 1.11 $
// $Date: 2003/03/05 01:02:51 $
// $Source: /usr/local/cvs/OpenSees/SRC/material/uniaxial/Steel01.h,v $


#ifndef Steel01_h
#define Steel01_h

// File: Steel01.h
//
// Written: MHS
// Created: 06/99
// Revision: A
//
// Description: This file contains the class definition for
// Steel01.h
//
//
//
// What: "@(#) Steel01.h, revA"


#include "material/uniaxial/steel/SteelBase0103.h"

namespace XC {
//! @ingroup MatUnx
//
//! @brief Clase para la modelización del acero 01.
class Steel01 : public SteelBase0103
  {
  private:
    void determineTrialState(double dStrain);
    void detectLoadReversal(double dStrain);

// AddingSensitivity:BEGIN //////////////////////////////////////////
    void libera(void);
    void alloc(const Matrix &);
    int parameterID;
    Matrix *SHVs;
// AddingSensitivity:END ///////////////////////////////////////////

  protected:
    int sendData(CommParameters &);
    int recvData(const CommParameters &);
    bool procesa_comando(CmdStatus &status);
    int setup_parameters(void);
  public:
    Steel01(int tag, double fy, double E0, double b,
       double a1 = STEEL_0103_DEFAULT_A1, double a2 = STEEL_0103_DEFAULT_A2,
       double a3 = STEEL_0103_DEFAULT_A3, double a4 = STEEL_0103_DEFAULT_A4);
    Steel01(int tag);
    Steel01(void);
    Steel01(const Steel01 &);
    Steel01 &operator=(const Steel01 &);
    ~Steel01(void);

    UniaxialMaterial *getCopy(void) const;

    int revertToStart(void);

    int sendSelf(CommParameters &);
    int recvSelf(const CommParameters &);
    void Print(std::ostream &s, int flag =0);

// AddingSensitivity:BEGIN //////////////////////////////////////////
    int    setParameter(const std::vector<std::string> &argv, Parameter &param);
    int    updateParameter          (int parameterID, Information &info);
    int    activateParameter        (int parameterID);
    double getStressSensitivity     (int gradNumber, bool conditional);
    double getInitialTangentSensitivity(int gradNumber);
    int    commitSensitivity        (double strainGradient, int gradNumber, int numGrads);
// AddingSensitivity:END ///////////////////////////////////////////

  };
} // end of XC namespace

#endif
