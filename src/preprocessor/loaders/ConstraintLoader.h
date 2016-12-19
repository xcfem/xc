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
//ConstraintLoader.h
//Objeto encargado de interpretar la entrada de datos
//del preproceso y generar la malla de elementos finitos.

#ifndef CONSTRAINTLOADER_H
#define CONSTRAINTLOADER_H

#include "Loader.h"

namespace XC {

class SFreedom_Constraint;
class MFreedom_Constraint;
class MRMFreedom_Constraint;

//! \ingroup Ldrs
//! 
//! @brief Cargador de condiciones de contorno.
//!  
class ConstraintLoader: public Loader
  {
    int tag_sp_constraint; //!< Tag por defecto para el constraint.
    int tag_mp_constraint; //!< Tag por defecto para el constraint.
    int tag_mrmp_constraint; //!< Tag por defecto para el constraint.
  protected:

    friend class Node;
    friend class SetMeshComp;
    SFreedom_Constraint *addSFreedom_Constraint(const int &,const SFreedom_Constraint &);
    SFreedom_Constraint *addSFreedom_Constraint(const int &,const int &,const double &valor);
  public:
    ConstraintLoader(Preprocessor *owr);
    virtual ~ConstraintLoader(void);
    inline const int &getTagNextSPConstraint(void) const
      { return tag_sp_constraint; }
    inline const int &getTagNextMPConstraint(void) const
      { return tag_mp_constraint; }
    inline const int &getTagNextMRMPConstraint(void) const
      { return tag_mrmp_constraint; }
    int getNumSPs(void) const;
    int getNumMPs(void) const;
    int getNumMRMPs(void) const;
    int getNumLPs(void) const;
    SFreedom_Constraint *newSPConstraint(const int &,const int &,const double &);
    void removeSPConstraint(const int &tagC);
    MFreedom_Constraint *newMPConstraint(const int &, const int &, const ID &, const ID &);
    MFreedom_Constraint *newEqualDOF(const int &, const int &, const ID &);
    MFreedom_Constraint *newRigidBeam(const int &, const int &);
    MFreedom_Constraint *newRigidRod(const int &, const int &);
    //MFreedom_Constraint *newRigidDiaphragm(void);
    MRMFreedom_Constraint *newMRMPConstraint(const ID &, const int &, const ID &);
    MRMFreedom_Constraint *newGlueNodeToElement(const Node &, const Element &, const ID &);
    void clearAll(void);
  };

} // end of XC namespace

#endif
