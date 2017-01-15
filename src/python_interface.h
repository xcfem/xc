//----------------------------------------------------------------------------
//  XC program; finite element analysis code
//  for structural analysis and design.
//
//  Copyright (C)  Luis Claudio Pérez Tato
//
//  This program derives from OpenSees <http://opensees.berkeley.edu>
//  developed by the  «Pacific earthquake engineering research center».
//
//  Except for the restrictions that may arise from the copyright
//  of the original program (see copyright_opensees.txt)
//  XC is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or 
//  (at your option) any later version.
//
//  This software is distributed in the hope that it will be useful, but 
//  WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details. 
//
//
// You should have received a copy of the GNU General Public License 
// along with this program.
// If not, see <http://www.gnu.org/licenses/>.
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
#include "xc_utils/src/geom/d2/BND2d.h"
#include "xc_utils/src/geom/d2/poligonos2d/Cuadrilatero2d.h"
#include "xc_utils/src/geom/d1/Segmento2d.h"
#include "xc_utils/src/geom/d1/Polilinea3d.h"
#include "xc_utils/src/geom/pos_vec/SVD3d.h"
#include "xc_utils/src/geom/pos_vec/TritrizPos3d.h"
#include "xc_utils/src/gnu_gts/TriangleMap.h"


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



