//----------------------------------------------------------------------------
//  programa XC; cálculo mediante el método de los elementos finitos orientado
//  a la solución de problemas estructurales.
//
//  Copyright (C)  Luis Claudio Pérez Tato
//
//  Este software es libre: usted puede redistribuirlo y/o modificarlo 
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
//MEDIntFieldInfo.h
//Información sobre las celdas de la malla (para exportar archivos a «salome»).

#ifndef MEDINTFIELDINFO_H
#define MEDINTFIELDINFO_H

#include "MEDTFieldInfo.h"

namespace XC {
class Set;
class MEDGroupInfo;

//! @ingroup MED
//
//! @brief Información sobre un campo definido sobre un
//! subconjunto de la malla.
class MEDIntFieldInfo: public MEDTFieldInfo<int>
  {
  protected:
    friend class MEDMeshing;
    MEDIntFieldInfo(const FieldInfo &,MEDGroupInfo *);


    void populateOnNodes(const Set &,const FieldInfo &);
    void populateOnElements(const Set &,const FieldInfo &);
    void populateOnGaussPoints(const Set &,const FieldInfo &);
  };
} // end of XC namespace
#endif
