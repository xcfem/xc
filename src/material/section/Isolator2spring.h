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
                                                                        
// $Revision: 1.1 $
// $Date: 2006/01/17 20:44:32 $
// $Source: /usr/local/cvs/OpenSees/SRC/material/section/Isolator2spring.h,v $
                                                                        
                                                                        
// Written: K. Ryan
// Created: September 2003
// Updates: November 2005
//
// Description: This file contains the class implementation for a "two-spring isolator" 
// material.  This material is based on the two-spring model originally developed by 
// Koh and Kelly to represent the buckling behavior of an elastomeric bearing.  The 
// material model has been modified to include material nonlinearity and optional 
// strength degradation.

#ifndef Isolator2spring_h
#define Isolator2spring_h
#include <material/section/SectionForceDeformation.h>

#include <utility/matrix/Matrix.h>
#include <utility/matrix/Vector.h>

namespace XC {
//! \ingroup MATSCC
//
//! @brief "two-spring isolator" material. This material is based on
//! the two-spring model originally developed by Koh and Kelly to
//! represent the buckling behavior of an elastomeric bearing. The 
//! material model has been modified to include material nonlinearity
//! and optional strength degradation.
class Isolator2spring : public SectionForceDeformation
  {
  private:
    double tol;
    double k1;
    double Fyo;
    double kbo;
    double kvo;
    double h;
    double Pe;
    double po;

    double utptTrial[2]; //!< Deformaciones de prueba.
    double utptInic[2]; //!< Deformaciones iniciales.
    double sP_n;
    mutable double sP_n1;

    double q_n;
    mutable double q_n1;

    double H;
    double pcr;

    mutable Vector x0;
    mutable Matrix ks;
    static Vector f0;
    static Matrix df;
    static Vector s;
    static Vector s3;
  protected:
    int sendData(CommParameters &);
    int recvData(const CommParameters &);
  public:
    Isolator2spring(int tag, double tol_in, double k1_in, double Fy_in, double kb_in, double kvo_in, double hb_in, double Pe_in, double po_in);
    Isolator2spring(int tag);
    Isolator2spring(void);

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
