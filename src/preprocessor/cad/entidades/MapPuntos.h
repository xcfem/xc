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
//MapPuntos.h

#ifndef MAPPUNTOS_H
#define MAPPUNTOS_H

#include "MapEnt.h"
#include <map>

class Pos3d;
class Recta3d;
class Plano3d;
class Vector3d;

namespace XC {

class Preprocessor;
class Cad;
class Pnt;
class SetEstruct;
class TrfGeom;

//! @ingroup Cad
//
//! @brief Contenedor de puntos del modelo.
class MapPuntos: public MapEnt<Pnt>
  {
  private:
    void UpdateSets(Pnt *) const;
  protected:


    Pnt *Crea(void);
  public:
    MapPuntos(Cad *cad= NULL);

    Vector3d getVector(const Indice &i,const Indice &j) const;
    Recta3d getRecta(const Indice &i,const Indice &j) const;
    Plano3d getPlano(const Indice &i,const Indice &j,const Indice &k) const;

    Pnt *Nuevo(void);
    Pnt *Nuevo(const Pos3d &);
    Pnt *Nuevo(const size_t &,const Pos3d &);

    Pnt *Copia(const Pnt *,const Vector3d &v);
    void Copia(const std::vector<MapPuntos::Indice> &);
    void Transforma(const TrfGeom &trf,const std::vector<MapPuntos::Indice> &);
    double Dist(const Indice &i,const Indice &j) const;


  };

} //end of XC namespace
#endif
