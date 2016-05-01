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
//MapUniformGrids.h

#ifndef MAPUniformGrids_H
#define MAPUniformGrids_H

#include "MapEnt.h"

class Recta3d;
class Plano3d;

namespace XC {

class UniformGrid;

//! @ingroup Cad
//
//! @brief Contenedor de puntos del modelo.
class MapUniformGrids: public MapEnt<UniformGrid>
  {
  private:
    void UpdateSets(UniformGrid *) const;
  protected:


    UniformGrid *Crea(void);
  public:
    MapUniformGrids(Cad *cad= NULL);

    Vector3d getVector(const Indice &i,const Indice &j) const;
    Recta3d getRecta(const Indice &i,const Indice &j) const;
    Plano3d getPlano(const Indice &i,const Indice &j,const Indice &k) const;

    UniformGrid *Nueva(void);
  };


//! @brief Crea una nueva superficie.


} //end of XC namespace
#endif
