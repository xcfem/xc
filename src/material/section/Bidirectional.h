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
// $Date: 2003/02/25 23:33:33 $
// $Source: /usr/local/cvs/OpenSees/SRC/material/section/Bidirectional.h,v $
                                                                        
                                                                        
#ifndef Bidirectional_h
#define Bidirectional_h

// File: ~/material/Bidirectional.h
//
// Written: MHS
// Created: Feb 2000
// Revision: A
//
// Description: 
//
// What: "@(#) Bidirectional.h, revA"

#include <material/section/SectionForceDeformation.h>

namespace XC {
//! \ingroup MATSCC
//
//! @brief ??
class Bidirectional: public SectionForceDeformation
  {
  private:
    double E;
    double sigY;
    double Hiso;
    double Hkin;

    double e_n1Trial[2]; //!< Deformación de prueba.
    double e_n1Inic[2]; //!< Deformación inicial.
    double eP_n[2];
    mutable double eP_n1[2];

    double q_n[2];
    mutable double q_n1[2];

    double alpha_n;
    mutable double alpha_n1;
	
    static Vector s;
    static Matrix ks;
  protected:
    int sendData(CommParameters &);
    int recvData(const CommParameters &);
  public:
    Bidirectional(int tag, double E, double sigY, double Hiso, double Hkin);
    Bidirectional(int tag);
    Bidirectional(void);

    int setInitialSectionDeformation(const Vector &v);
    int setTrialSectionDeformation(const Vector &v);
    const Matrix &getSectionTangent(void) const;
    const Matrix &getInitialTangent(void) const;
    const Vector &getStressResultant(void) const;
    const Vector &getInitialSectionDeformation(void) const;
    const Vector &getSectionDeformation(void) const;

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
} // end of XC namespace


#endif
