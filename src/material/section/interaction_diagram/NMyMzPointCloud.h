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
//NMyMzPointCloud.h
//Internal forces point cloud container.


#ifndef NMYMZPOINTCLOUD_H
#define NMYMZPOINTCLOUD_H

#include "NMPointCloudBase.h"
#include "NMPointCloud.h"
#include "xc_utils/src/geom/pos_vec/Pos3d.h"
#include "xc_utils/src/geom/pos_vec/ListaPos3d.h"


namespace XC {


//! @ingroup MATSCCDiagInt
//
//! @brief (N,My,Mz) point set
class NMyMzPointCloud: public NMPointCloudBase, public GeomObj::list_Pos3d
  {
    const Pos3d *lastInserted;
  public:
    NMyMzPointCloud(const double &u=0.0);
    void clear(void);
    const Pos3d *append(const Pos3d &);
    NMPointCloud getNMy(void) const;
    NMPointCloud getNMz(void) const;
    NMPointCloud getNM(const double &) const;
  };

} // end of XC namespace

#endif
