// -*-c++-*-
//----------------------------------------------------------------------------
//  XC program; finite element analysis code
//  for structural analysis and design.
//
//  Copyright (C)  Luis C. Pérez Tato
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
#include "utility/geom/d1/Line2d.h"
#include "utility/geom/d2/HalfPlane2d.h"
#include "utility/geom/d2/Grid2d.h"
#include "utility/geom/d2/BND2d.h"
#include "utility/geom/d2/2d_polygons/Quadrilateral2d.h"
#include "utility/geom/d1/Segment2d.h"
#include "utility/geom/d1/Polyline3d.h"
#include "utility/geom/pos_vec/SlidingVectorsSystem2d.h"
#include "utility/geom/pos_vec/SlidingVectorsSystem3d.h"
#include "utility/geom/pos_vec/Pos3dArray3d.h"
#include "utility/gnu_gts/TriangleMap.h"

#include "domain/domain/single/SingleDomNodIter.h"

#include "utility/geom/pos_vec/Vector3d.h"
#include "utility/geom/coo_sys/ref_sys/PrincipalAxes2D.h"
#include "utility/geom/coo_sys/ref_sys/PrincipalAxes3D.h"

//Physical properties.
typedef XC::PhysicalProperties<XC::UniaxialMaterial> PhysicalProperties_UMat;
typedef PhysicalProperties_UMat::material_vector material_vector_UMat;

typedef XC::PhysicalProperties<XC::SectionForceDeformation> PhysicalProperties_SectionFDMat;
typedef PhysicalProperties_SectionFDMat::material_vector material_vector_SectionFDMat;

typedef XC::PhysicalProperties<XC::ElasticSection1d> PhysicalProperties_ElasticSection1d;
typedef PhysicalProperties_ElasticSection1d::material_vector material_vector_ElasticSection1d;
typedef XC::ElasticSectionPhysicalProperties<XC::ElasticSection1d> ElasticSectionPhysicalProperties_ElasticSection1d;

typedef XC::PhysicalProperties<XC::ElasticSection2d> PhysicalProperties_ElasticSection2d;
typedef PhysicalProperties_ElasticSection2d::material_vector material_vector_ElasticSection2d;
typedef XC::ElasticSectionPhysicalProperties<XC::ElasticSection2d> ElasticSectionPhysicalProperties_ElasticSection2d;

typedef XC::PhysicalProperties<XC::ElasticSection3d> PhysicalProperties_ElasticSection3d;
typedef PhysicalProperties_ElasticSection3d::material_vector material_vector_ElasticSection3d;
typedef XC::ElasticSectionPhysicalProperties<XC::ElasticSection3d> ElasticSectionPhysicalProperties_ElasticSection3d;

typedef XC::PhysicalProperties<XC::NDMaterial> PhysicalProperties_NDMat;
typedef PhysicalProperties_NDMat::material_vector material_vector_NDMat;


//Elements.
// Two nodes.
typedef XC::ElementBase<2> ElementBase2N;
// Three nodes.
typedef XC::ElementBase<3> ElementBase3N;
typedef XC::ElemWithMaterial<3,XC::SolidMech2D> ElemWithMaterial3N_Mech2D;
typedef XC::PlaneElement<3,XC::SolidMech2D> PlaneElement3N_Mech2D;
typedef XC::TriBase3N<XC::SolidMech2D> TriBase3N_Mech2D;


// Four nodes.
typedef XC::ElementBase<4> ElementBase4N;
typedef XC::ElemWithMaterial<4,XC::SolidMech2D> ElemWithMaterial4N_Mech2D;
typedef XC::PlaneElement<4,XC::SolidMech2D> PlaneElement4N_Mech2D;
typedef XC::QuadBase4N<XC::SolidMech2D> QuadBase4N_Mech2D;

// Eigth nodes.
typedef XC::ElementBase<8> ElementBase8N;

// Nine nodes.
typedef XC::ElementBase<9> ElementBase9N;
typedef XC::ElemWithMaterial<9,XC::SolidMech2D> ElemWithMaterial9N_Mech2D;

// Twenty nodes.
typedef XC::ElementBase<20> ElementBase20N;

// Twenty seven nodes.
typedef XC::ElementBase<27> ElementBase27N;

//Shell
typedef XC::ElemWithMaterial<4,XC::SectionFDPhysicalProperties> ElemWithMaterial4N_SFD;
typedef XC::PlaneElement<4,XC::SectionFDPhysicalProperties> PlaneElement4N_SFD;
typedef XC::ElemWithMaterial<9,XC::SectionFDPhysicalProperties> ElemWithMaterial9N_SFD;
typedef XC::PlaneElement<9,XC::SectionFDPhysicalProperties> PlaneElement9N_SFD;

#endif



