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
//TrfGeom.cc

#include "TrfGeom.h"
#include "xc_utils/src/geom/pos_vec/MatrizPos3d.h"
#include "xc_utils/src/geom/pos_vec/TritrizPos3d.h"
#include "xc_utils/src/base/CmdStatus.h"

//! @brief Lee un objeto TrfGeom desde el archivo de entrada.
bool XC::TrfGeom::procesa_comando(CmdStatus &status)
  {
    const std::string cmd= status.Cmd();
    if(verborrea>2)
      std::clog << "(TrfGeom) Procesando comando: " << cmd << std::endl;

    return EntMdlrBase::procesa_comando(status);
  }

const MatrizPos3d &XC::TrfGeom::Transforma(const MatrizPos3d &m) const
  {
    static MatrizPos3d retval;
    retval= m;
    const size_t nfilas= retval.getNumFilas();
    const size_t ncols= retval.getNumCols();
    for(size_t j= 1;j<=nfilas;j++)
      for(size_t k= 1;k<=ncols;k++)
        retval(j,k)= Transforma(retval(j,k));
    return retval;
  }
const TritrizPos3d &XC::TrfGeom::Transforma(const TritrizPos3d &m) const
  {
    static TritrizPos3d retval;
    retval= m;
    const size_t ncapas= retval.GetCapas();
    const size_t nfilas= retval.getNumFilas();
    const size_t ncols= retval.getNumCols();
    for(size_t i= 1;i<=ncapas;i++)
      for(size_t j= 1;j<=nfilas;j++)
        for(size_t k= 1;k<=ncols;k++)
          retval(i,j,k)= Transforma(retval(i,j,k));
    return retval;
  }
