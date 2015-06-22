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

#ifndef python_interface_h
#define python_interface_h

#include <boost/python/module.hpp>
#include <boost/python/def.hpp>
#include <boost/python/class.hpp>
#include <boost/python/suite/indexing/vector_indexing_suite.hpp>
#include <boost/python/suite/indexing/map_indexing_suite.hpp>
#include <boost/python/docstring_options.hpp>
#include <boost/python/implicit.hpp>
#include "utility/actor/objectBroker/all_includes.h"
#include "xc_utils/src/geom/d1/Recta2d.h"
#include "xc_utils/src/geom/d2/Semiplano2d.h"
#include "xc_utils/src/geom/d2/Rejilla2d.h"
#include "xc_utils/src/geom/d2/poligonos2d/Cuadrilatero2d.h"
#include "xc_utils/src/geom/d1/Segmento2d.h"
#include "xc_utils/src/geom/pos_vec/SVD3d.h"
#include "xc_utils/src/geom/pos_vec/TritrizPos3d.h"


#include <domain/domain/single/SingleDomNodIter.h>

#include "xc_utils/src/geom/pos_vec/Vector3d.h"

//Physical properties.
typedef XC::PhysicalProperties<XC::UniaxialMaterial> PhysicalProperties_UMat;
typedef PhysicalProperties_UMat::material_vector material_vector_UMat;

typedef XC::PhysicalProperties<XC::SectionForceDeformation> PhysicalProperties_SectionFDMat;
typedef PhysicalProperties_SectionFDMat::material_vector material_vector_SectionFDMat;

typedef XC::PhysicalProperties<XC::NDMaterial> PhysicalProperties_NDMat;
typedef PhysicalProperties_NDMat::material_vector material_vector_NDMat;


//Elements.
// Plane elements.
typedef XC::ElemWithMaterial<3,XC::SolidMech2D> ElemWithMaterial3N_Mech2D;
typedef XC::ElemPlano<3,XC::SolidMech2D> ElemPlano3N_Mech2D;
typedef XC::TriBase3N<XC::SolidMech2D> TriBase3N_Mech2D;


typedef XC::ElemWithMaterial<4,XC::SolidMech2D> ElemWithMaterial4N_Mech2D;
typedef XC::ElemPlano<4,XC::SolidMech2D> ElemPlano4N_Mech2D;
typedef XC::QuadBase4N<XC::SolidMech2D> QuadBase4N_Mech2D;

//Shell
typedef XC::ElemWithMaterial<4,XC::SectionFDPhysicalProperties> ElemWithMaterial4N_SFD;
typedef XC::ElemPlano<4,XC::SectionFDPhysicalProperties> ElemPlano4N_SFD;


#endif



