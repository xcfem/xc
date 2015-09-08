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
//python_interface.tcc

class_<XC::Information, boost::noncopyable >("Information", no_init);


//class_<XC::ElemPos, bases<XC::KDTreePos>, boost::noncopyable >("ElemPos", no_init);

class_<XC::ParticlePos3d>("ParticlePos3d")
  .def(init<Pos2d>())
  .def(init<Pos3d>())
  .add_property("r", make_function( &XC::ParticlePos3d::r_coordinate, return_value_policy<return_by_value>()), &XC::ParticlePos3d::set_r_coordinate, "Returns r coordinate.")
  .add_property("s", make_function( &XC::ParticlePos3d::s_coordinate, return_value_policy<return_by_value>()), &XC::ParticlePos3d::set_s_coordinate, "Returns s coordinate.")
  .add_property("t", make_function( &XC::ParticlePos3d::t_coordinate, return_value_policy<return_by_value>()), &XC::ParticlePos3d::set_t_coordinate, "Returns t coordinate.")
  .def(self_ns::str(self_ns::self))
  ;

class_<XC::RayleighDampingFactors, bases<EntCmd>, boost::noncopyable >("RayleighDampingFactors", no_init);

#include "fvectors/python_interface.tcc"
#include "physical_properties/python_interface.tcc"

XC::Node *(XC::NodePtrs::*getNearestNodePtr)(const Pos3d &,bool geomInicial)= &XC::NodePtrs::getNearestNode;
class_<XC::NodePtrs, bases<EntCmd>, boost::noncopyable >("NodePtrs", no_init)
  .def("__getitem__",&XC::NodePtrs::getNodePtr, return_value_policy<reference_existing_object>())
  .def("__len__",&XC::NodePtrs::size)
  .def("getNearestNode",make_function(getNearestNodePtr, return_internal_reference<>() ),"Returns nearest node.")
  ;

class_<XC::NodePtrsWithIDs, bases<XC::NodePtrs>, boost::noncopyable >("NodePtrsWithIDs", no_init)
  .add_property("getExternalNodes",make_function(&XC::NodePtrsWithIDs::getExternalNodes, return_internal_reference<>() ),"Returns tags of external nodes.")
  ;

XC::NodePtrsWithIDs &(XC::Element::*getNodePtrsRef)(void)= &XC::Element::getNodePtrs;
const XC::Vector &(XC::Element::*getResistingForceRef)(void) const= &XC::Element::getResistingForce;
class_<XC::Element, XC::Element *,bases<XC::MeshComponent>, boost::noncopyable >("Element", no_init)
  .add_property("getNodes", make_function( getNodePtrsRef, return_internal_reference<>() ))
  .add_property("getIdxNodes",&XC::Element::getIdxNodes)
.add_property("getDimension",&XC::Element::getDimension,"Returns element's dimension (point: 0, line: 1, surface: 2 or volume: 3).")
  .def("commitState", &XC::Element::commitState,"Consuma el estado del elemento.")
  .def("revertToLastCommit", &XC::Element::revertToLastCommit,"Devuelve el estado del elemento al último consumado.")
  .def("revertToStart", &XC::Element::revertToStart,"Devuelve el elemento a su estado inicial.")
  .def("getNumDOF", &XC::Element::getNumDOF,"Devuelve el número de grados de libertad del elemento.")
  .def("getResistingForce",make_function(getResistingForceRef, return_internal_reference<>() ),"Calculates element's resisting force.")
  .def("setDeadSRF",XC::Element::setDeadSRF,"Assigns Stress Reduction Factor for element deactivation.")
  .add_property("getVtkCellType",&XC::Element::getVtkCellType,"Returns cell type for Vtk graphics.")
  .def("getMEDCellType",&XC::Element::getVtkCellType,"Returns cell type for MED file writing.")
  .def("getPosCentroid",make_function(&XC::Element::getPosCdg, return_internal_reference<>() ),"Returns centroid's position.")
  .def("getCooCentroid",make_function(&XC::Element::getCooCdg, return_internal_reference<>() ),"Returns centroid's coordinates.")
  .def("getPoints",&XC::Element::getPuntos,"Returns a uniform grid of points over the element.")
  .def("resetTributarias",&XC::Element::resetTributarias)
  .def("vuelcaTributarias",&XC::Element::vuelcaTributarias)
  .def("calculaLongsTributarias",&XC::Element::calculaLongsTributarias)
  .def("getLongTributaria",&XC::Element::getLongTributaria)
  .def("getLongTributariaByTag",&XC::Element::getLongTributariaByTag)
  .def("calculaAreasTributarias",&XC::Element::calculaAreasTributarias)
  .def("getAreaTributaria",&XC::Element::getAreaTributaria)
  .def("getAreaTributariaByTag",&XC::Element::getAreaTributariaByTag)
  .def("calculaVolsTributarios",&XC::Element::calculaVolsTributarios)
  .def("getVolTributario",&XC::Element::getVolTributario)
  .def("getVolTributarioByTag",&XC::Element::getVolTributarioByTag)
  .def("getMaxCooNod",&XC::Element::MaxCooNod)
  .def("getMinCooNod",&XC::Element::MinCooNod)
   ;

class_<XC::ElementIter, boost::noncopyable >("ElementIter", no_init)
  .def("next", &XC::ElementIter::operator(), return_internal_reference<>(),"Returns next node.")
   ;


class_<XC::ElementBase<2>, bases<XC::Element >, boost::noncopyable >("ElementBase2N", no_init);

class_<XC::ElementBase<3>, bases<XC::Element >, boost::noncopyable >("ElementBase3N", no_init);

class_<XC::ElementBase<4>, bases<XC::Element >, boost::noncopyable >("ElementBase4N", no_init);

class_<XC::ElementBase<8>, bases<XC::Element >, boost::noncopyable >("ElementBase8N", no_init);

class_<XC::ElementBase<20>, bases<XC::Element >, boost::noncopyable >("ElementBase20N", no_init);

class_<XC::ElementBase<27>, bases<XC::Element >, boost::noncopyable >("ElementBase27N", no_init);

class_<XC::Element0D , bases<XC::ElementBase<2>>, boost::noncopyable >("Element0D", no_init)
  .def("setupVectors",&XC::Element0D::setUpVectors)
  .add_property("getIVector",make_function(&XC::Element0D::getX, return_internal_reference<>() ),"Returns i vector.")
  .add_property("getJVector",make_function(&XC::Element0D::getY, return_internal_reference<>() ),"Returns j vector.")
  .add_property("getKVector",make_function(&XC::Element0D::getZ, return_internal_reference<>() ),"Returns k vector.")
  ;


XC::CrdTransf *(XC::Element1D::*getCrdTransf)(void)= &XC::Element1D::getCoordTransf;
class_<XC::Element1D, bases<XC::ElementBase<2> >, boost::noncopyable >("Element1D", no_init)
    .add_property("getCoordTransf", make_function( getCrdTransf, return_internal_reference<>() ))
    .def("getMEDCellType", &XC::Element1D::getMEDCellType,"Devuelve el tipo de celda que le corresponde al elemento en un archivo MED.")
    .def("vector2dUniformLoadGlobal", &XC::Element1D::vector2dUniformLoadGlobal,"Element's load.")
    .def("vector2dUniformLoadLocal", &XC::Element1D::vector2dUniformLoadLocal,"Element's load.")
    .def("vector2dPointByRelDistLoadGlobal", &XC::Element1D::vector2dPointByRelDistLoadGlobal,"Element's load.")
    .def("vector2dPointByRelDistLoadLocal", &XC::Element1D::vector2dPointByRelDistLoadLocal,"Element's load.")
    .def("vector2dPointLoadGlobal", &XC::Element1D::vector2dPointLoadGlobal,"Element's load.")
    .def("vector2dPointLoadLocal", &XC::Element1D::vector2dPointLoadLocal,"Element's load.")
    .def("vector3dUniformLoadGlobal", &XC::Element1D::vector3dUniformLoadGlobal,"Element's load.")
    .def("vector3dUniformLoadLocal", &XC::Element1D::vector3dUniformLoadLocal,"Element's load.")
    .def("vector3dPointByRelDistLoadGlobal", &XC::Element1D::vector3dPointByRelDistLoadGlobal,"Element's load.")
    .def("vector3dPointByRelDistLoadLocal", &XC::Element1D::vector3dPointByRelDistLoadLocal,"Element's load.")
    .def("vector3dPointLoadGlobal", &XC::Element1D::vector3dPointLoadGlobal,"Element's load.")
    .def("vector3dPointLoadLocal", &XC::Element1D::vector3dPointLoadLocal,"Element's load.")
    .def("strainLoad", &XC::Element1D::strainLoad,"Element's load.")
    .def("getCooPuntos",make_function(&XC::Element1D::getCooPuntos, return_internal_reference<>()) ,"Points over element's length.")
       ;




#include "truss_beam_column/python_interface.tcc"
#include "coordTransformation/python_interface.tcc"


#include "plane/python_interface.tcc"
//#include "frictionBearing/python_interface.tcc"
#include "zeroLength/python_interface.tcc"
