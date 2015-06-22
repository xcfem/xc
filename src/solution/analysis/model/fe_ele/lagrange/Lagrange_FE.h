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

// File: ~/analysis/model/fe_ele/lagrange/Lagrange_FE.h
// 
// Written: lcpt 
// Created: 03/2015
// Revision: A
//
// Description: This file contains the class definition for Lagrange_FE.
// Lagrange_FE is a subclass of FE_Element which handles _Constraints
// using the Lagrange method.
//
// What: "@(#) Lagrange_FE.h, revA"


#ifndef Lagrange_FE_h
#define Lagrange_FE_h

namespace XC {
class DOF_Group;
class ID;

//! @ingroup AnalisisFE
//
//! @brief Lagrange_FE is a subclass of FE_Element
//! which handles _Constraints using the Lagrange method.
class Lagrange_FE
  {
  private:
    DOF_Group *theDofGroup; //!< Lagrange DOF group.

  public:
    Lagrange_FE(DOF_Group &);
    DOF_Group *getLagrangeDOFGroup(void)
      { return theDofGroup; } 
    inline const DOF_Group *getLagrangeDOFGroup(void) const
      { return theDofGroup; } 
    int getLagrangeDOFGroupTag(void) const;

  };
} // fin namespace XC

#endif


