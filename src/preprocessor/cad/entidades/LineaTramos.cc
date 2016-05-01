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
//LineaTramos.cc

#include "LineaTramos.h"
#include "Pnt.h"
#include "xc_utils/src/geom/d3/BND3d.h"
#include "xc_utils/src/geom/pos_vec/MatrizPos3d.h"

#include "domain/mesh/node/Node.h"
#include "domain/mesh/element/Element.h"

//! @brief Constructor.
XC::LineaTramos::LineaTramos(Preprocessor *m,const size_t &nd)
  : Linea(m,nd), longs(nd,1.0){}

//! @brief Constructor virtual.
XC::SetEstruct *XC::LineaTramos::getCopy(void) const
  { return new LineaTramos(*this); }

void XC::LineaTramos::setLongs(const XC::Vector &ls)
  {
    longs= vector_to_std_vector(ls);
    SetNDiv(longs.size());
  }

//! @brief Devuelve ndiv+1 posiciones a lo largo de la línea espaciadas
//! según las longitudes que contenga el miembro longs.
MatrizPos3d XC::LineaTramos::get_posiciones(void) const
  {
    static MatrizPos3d retval;
    if(P1() && P2())
      {
	std::vector<GEOM_FT> tmp(NDiv(),0);
        for(size_t i= 0;i<NDiv();i++)
          tmp[i]= double_to_FT(longs[i]);
        retval= MatrizPos3d(P1()->GetPos(),P2()->GetPos(),tmp);
      }
    else
      std::cerr << "LineaTramos::get_posiciones; la línea no esté definida." << std::endl;
    return retval;
  }

