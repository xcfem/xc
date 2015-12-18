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

// $Revision: 1.8 $
// $Date: 2003/02/25 23:33:34 $
// $Source: /usr/local/cvs/OpenSees/SRC/material/section/fiber_section/FiberSection3d.h,v $

// Written: fmk
// Created: 04/01
//
// Description: This file contains the class definition for
// FiberSection3d.h. FiberSection3d provides the abstraction of a
// 3d beam section discretized by fibers. The section stiffness and
// stress resultants are obtained by summing fiber contributions.

#ifndef FiberSection3d_h
#define FiberSection3d_h

#include <material/section/fiber_section/FiberSection3dBase.h>

namespace XC {
class MaterialLoader;
class FiberSectionRepr;

//! \ingroup MATSCCModeloFibras
//
//! @brief Esta clase modeliza el comportamiento de la sección
//! discretizando la misma en un conjunto de fibras. La rigidez
//! de la sección y las resultantes de esfuerzos se obtienen sumando
//! las contribuciones de cada fibra.
class FiberSection3d: public FiberSection3dBase
  {
  private:

    friend class StoFibras;
  protected:
    bool procesa_comando(CmdStatus &status);
    FiberSection3d(int tag, int classTag, MaterialLoader *mat_ldr= nullptr);
  public:
    FiberSection3d(MaterialLoader *mat_ldr= nullptr);
    FiberSection3d(int tag,MaterialLoader *mat_ldr= nullptr);
    FiberSection3d(int tag,const contenedor_fibras &fibers,MaterialLoader *mat_ldr= nullptr);

    virtual void setupFibers(void);
    Fiber *addFiber(Fiber &theFiber);

    int setInitialSectionDeformation(const Vector &deforms); 
    int setTrialSectionDeformation(const Vector &deforms);

    const Matrix &getInitialTangent(void) const;

    int revertToLastCommit(void);
    int revertToStart(void);

    SectionForceDeformation *getCopy(void) const;
    const ResponseId &getType(void) const;
    int getOrder(void) const;

    int sendSelf(CommParameters &);
    int recvSelf(const CommParameters &);
    void Print(std::ostream &s, int flag = 0);
    virtual any_const_ptr GetProp(const std::string &cod) const;
  };

FiberSection3d FiberSectionReprToFiberSection3d(const int &tag,const FiberSectionRepr &fiberSectionRepr);

} // end of XC namespace

#endif
