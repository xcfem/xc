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
//DOF_GrpConstIter.h


#ifndef DOF_GrpConstIter_h
#define DOF_GrpConstIter_h

namespace XC {
class DOF_Group;
class TaggedObjectStorage;
class TaggedObjectIter;

//! @ingroup AnalisisModel
//
//! @brief Iterador sobre los grupos de grados de libertad.
class DOF_GrpConstIter 
  {
  private:
    TaggedObjectIter *myIter;
  public:
    DOF_GrpConstIter(TaggedObjectStorage *);

    virtual void reset(void);    
    virtual const DOF_Group *operator()(void);
  };
} // fin namespace XC

#endif

