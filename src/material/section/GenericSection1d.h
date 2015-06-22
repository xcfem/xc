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
// $Date: 2003/02/14 23:01:34 $
// $Source: /usr/local/cvs/OpenSees/SRC/material/section/GenericSection1d.h,v $
                                                                        
                                                                        
#ifndef GenericSection1d_h
#define GenericSection1d_h

// File: ~/material/GenericSection1d.h
//
// Written: MHS
// Created: Apr 2000
// Revision: A
//
// Description: This file contains the class definition for GenericSection1d.
//
// What: "@(#) GenericSection1d.h, revA"

#include <material/section/SeccionBarraPrismatica.h>
#include <utility/matrix/Matrix.h>

namespace XC {
class Information;
class UniaxialMaterial;

//! \ingroup MATSCC
//
//! @brief ??.
class GenericSection1d : public SeccionBarraPrismatica
  {
    //XXX Agregar constructor de copia y operador de asignación.
  protected:
    UniaxialMaterial *theModel;//!< Modelización del material.
    int code;

    static Vector s;
    static Matrix ks;
    static ResponseId c;

    int sendData(CommParameters &);
    int recvData(const CommParameters &);
  public:
    GenericSection1d(int tag, UniaxialMaterial& m, int code);
    GenericSection1d(int tag);
    GenericSection1d(void);
    ~GenericSection1d(void);

    int setInitialSectionDeformation (const Vector&);
    int setTrialSectionDeformation (const Vector&);
    const Vector &getInitialSectionDeformation(void) const;
    const Vector &getSectionDeformation(void) const;

    const Vector &getStressResultant(void) const;
    const Matrix &getSectionTangent(void) const;
    const Matrix &getInitialTangent(void) const;
    const Matrix &getSectionFlexibility(void) const;
    const Matrix &getInitialFlexibility(void) const;
    
    int commitState(void);
    int revertToLastCommit(void);
    int revertToStart(void);
    
    SectionForceDeformation *getCopy(void) const;
    const ResponseId &getType(void) const;
    int getOrder(void) const;
    
    int sendSelf(CommParameters &);
    int recvSelf(const CommParameters &);
    
    void Print(std::ostream &s, int flag = 0);
  };
} // fin namespace XC


#endif
