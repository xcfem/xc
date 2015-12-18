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
// $Source: /usr/local/cvs/OpenSees/SRC/material/section/fiber_section/FiberSection3dBase.h,v $

// Written: fmk
// Created: 04/01
//
// Description: This file contains the class definition for
// FiberSection3dBase.h. FiberSection3dBase provides the abstraction of a
// 3d beam section discretized by fibers. The section stiffness and
// stress resultants are obtained by summing fiber contributions.

#ifndef FiberSection3dBase_h
#define FiberSection3dBase_h

#include <material/section/fiber_section/FiberSectionBase.h>
#include <material/section/repres/section/contenedor_fibras.h>
#include <utility/matrix/Vector.h>

namespace XC {
class FiberSectionRepr;

//! \ingroup MATSCCModeloFibras
//
//! @brief Clase base para las secciones representadas mediante
//! un modelo de fibras, empleadas en problemas tridimensionales.
class FiberSection3dBase : public FiberSectionBase
  {
  protected:
    bool procesa_comando(CmdStatus &status);
    friend class DqFibras;
    friend class StoFibras;
    double get_strain(const double &y,const double &z) const;
  public:
    FiberSection3dBase(int classTag, int dim,MaterialLoader *mat_ldr= nullptr);
    FiberSection3dBase(int tag, int classTag, int dim,MaterialLoader *mat_ldr= nullptr);
    FiberSection3dBase(int tag, int classTag, int dim,const contenedor_fibras &fibers,MaterialLoader *mat_ldr= nullptr);
    FiberSection3dBase(const FiberSection3dBase &otro);
    FiberSection3dBase &operator=(const FiberSection3dBase &otro);

    Fiber *addFiber(Fiber &)= 0;
    Fiber *addFiber(int tag,const MaterialLoader &,const std::string &nmbMat,const double &, const Vector &position);

    virtual double getStrain(const double &y,const double &z) const;

    int setParameter(const std::vector<std::string> &argv, Parameter &param);
    int updateParameter(int parameterID, Information &info);
    inline virtual double getCdgZ(void) const
      { return fibras.getZCdg(); }

    virtual any_const_ptr GetProp(const std::string &cod) const;
  };

} // end of XC namespace

#endif
