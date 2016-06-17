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
//NMPointCloud.cc

#include "NMPointCloud.h"


//! @brief Constructor.
XC::NMPointCloud::NMPointCloud(const double &u)
  : NMPointCloudBase(u), GeomObj::list_Pos2d()
  {
    lastInserted= nullptr;
  }

void XC::NMPointCloud::clear(void)
  {
    GeomObj::list_Pos2d::clear();
    NMPointCloudBase::clear();
    lastInserted= nullptr;
  }

//! @brief Inserta en la lista de esfuerzos being passed as parameter
//! la resultante de tensiones normales en la sección.
const Pos2d *XC::NMPointCloud::append(const Pos2d &NM)
  {
    if(lastInserted)
      {
        if(dist(NM,*lastInserted)>umbral)
          lastInserted= Agrega(NM);
      }
    else
      lastInserted= Agrega(NM);
    return lastInserted;  
  }
