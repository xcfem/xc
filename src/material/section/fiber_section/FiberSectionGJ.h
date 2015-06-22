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
// $Date: 2003/02/25 23:33:34 $
// $Source: /usr/local/cvs/OpenSees/SRC/material/section/fiber_section/FiberSectionGJ.h,v $

// Written: fmk
// Created: 04/01
//
// Description: This file contains the class definition for
// FiberSectionGJ.h. FiberSectionGJ provides the abstraction of a
// 3d beam section discretized by fibers. The section stiffness and
// stress resultants are obtained by summing fiber contributions.

#ifndef FiberSectionGJ_h
#define FiberSectionGJ_h

#include <material/section/fiber_section/FiberSection3dBase.h>
#include <utility/matrix/Matrix.h>

namespace XC {
class MaterialLoader;
class FiberSectionRepr;

//! \ingroup MATSCCModeloFibras
//
//! @brief Sección formada por fibras de material, con
//! rigidez a torsión.
class FiberSectionGJ: public FiberSection3dBase
  {
  private:
    double GJ;
  protected:
    friend class DqFibras;
     bool procesa_comando(CmdStatus &status);
  public:
    FiberSectionGJ(MaterialLoader *mat_ldr= nullptr);
    FiberSectionGJ(int tag,MaterialLoader *mat_ldr= nullptr);
    FiberSectionGJ(int tag,const contenedor_fibras &fibers, double GJ = 1.0e10,MaterialLoader *mat_ldr= nullptr);

    Fiber *addFiber(Fiber &theFiber);

    int setInitialSectionDeformation(const Vector &deforms);
    int setTrialSectionDeformation(const Vector &deforms);

    virtual void setupFibers(void);

    inline const double &getGJ(void) const
      { return GJ; }
    const Matrix &getInitialTangent(void) const;

    int revertToLastCommit(void);
    int revertToStart(void);

    SectionForceDeformation *getCopy(void) const;
    const ResponseId &getType(void) const;
    int getOrder (void) const;

    int sendSelf(CommParameters &);
    int recvSelf(const CommParameters &);
    void Print(std::ostream &s, int flag = 0);
    virtual any_const_ptr GetProp(const std::string &cod) const;
  };

FiberSectionGJ FiberSectionReprToFiberSectionGJ(const int &tag,const FiberSectionRepr &fiberSectionRepr,const double &GJ);


} // fin namespace XC

#endif
