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

class_<XC::ShellCrdTransf3dBase, bases<EntCmd>, boost::noncopyable >("ShellCrdTransf3dBase", no_init)
  .add_property("getG1Vector", make_function(&XC::ShellCrdTransf3dBase::G1, return_internal_reference<>() ))
  .add_property("getG2Vector", make_function(&XC::ShellCrdTransf3dBase::G2, return_internal_reference<>() ))
  .add_property("getG3Vector", make_function(&XC::ShellCrdTransf3dBase::G3, return_internal_reference<>() ))
  .add_property("getVPosCentroide", make_function(&XC::ShellCrdTransf3dBase::getVPosCentroide, return_internal_reference<>() ))
  .add_property("getPlane", &XC::ShellCrdTransf3dBase::getPlane)
  .def("update", &XC::ShellCrdTransf3dBase::update)
  .def("getBasicTrialDisp", &XC::ShellCrdTransf3dBase::getBasicTrialDisp)
  .def("getBasicTrialVel", &XC::ShellCrdTransf3dBase::getBasicTrialVel)
  .def("getBasicTrialAccel", &XC::ShellCrdTransf3dBase::getBasicTrialAccel)
  .def("getProj", &XC::ShellCrdTransf3dBase::getProj)
  .add_property("getLocalReference", &XC::ShellCrdTransf3dBase::getLocalReference)
  .def("getLocalCoordinates", &XC::ShellCrdTransf3dBase::getLocalCoordinates)
  .def("getNaturalCoordinates", &XC::ShellCrdTransf3dBase::getNaturalCoordinates)
   ;

class_<XC::ShellLinearCrdTransf3d, bases<XC::ShellCrdTransf3dBase>, boost::noncopyable >("ShellLinearCrdTransf3d", no_init)
   ;

class_<XC::ShellCorotCrdTransf3d, bases<XC::ShellCrdTransf3dBase>, boost::noncopyable >("ShellCorotCrdTransf3d", no_init)
  .def("getR0",&XC::ShellCorotCrdTransf3d::getR0)
  .def("getR",&XC::ShellCorotCrdTransf3d::getR)
  .def("getR0T",&XC::ShellCorotCrdTransf3d::getR0T)
   ;


typedef XC::QuadBase4N<XC::SectionFDPhysicalProperties> QuadBase4N_SFD;
class_<QuadBase4N_SFD, bases<ElemPlano4N_SFD>, boost::noncopyable >("QuadBase4N_SFD", no_init)
  .def("getPerimeter", &QuadBase4N_SFD::getPerimetro, "Returns element's perimeter.")
  .def("getArea", &QuadBase4N_SFD::getArea, "Returns element's area.")
   ;


XC::ShellCrdTransf3dBase *(XC::ShellMITC4Base::*getCoordTransfRef)(void)= &XC::ShellMITC4::getCoordTransf;
XC::Vector (XC::ShellMITC4Base::*getParticlePos3dInterpolationFactors)(const XC::ParticlePos3d &) const= &XC::ShellMITC4::getInterpolationFactors;
XC::Vector (XC::ShellMITC4Base::*getPos3dInterpolationFactors)(const Pos3d &) const= &XC::ShellMITC4::getInterpolationFactors;
XC::Vector (XC::ShellMITC4Base::*getParticlePos3dInterpolatedDisplacements)(const XC::ParticlePos3d &) const= &XC::ShellMITC4::getInterpolationFactors;
XC::Vector (XC::ShellMITC4Base::*getPos3dInterpolatedDisplacements)(const Pos3d &) const= &XC::ShellMITC4::getInterpolationFactors;
class_<XC::ShellMITC4Base, bases<QuadBase4N_SFD>, boost::noncopyable >("ShellMITC4Base", no_init)
    .add_property("getCoordTransf", make_function( getCoordTransfRef, return_internal_reference<>() ))
    .def("computeBasis", &XC::ShellMITC4Base::computeBasis,"Compute local coordinates and basis")
    .def("getLocalCoordinatesOfNode", &XC::ShellMITC4Base::getLocalCoordinatesOfNode,"Returns local coordinates of node i.")
    .def("getNaturalCoordinates", &XC::ShellMITC4Base::getNaturalCoordinates,"Returns natural coordinates of point.")
    .def("getMeanInternalForce",&XC::ShellMITC4Base::getMeanInternalForce)
    .def("getMeanInternalDeformation",&XC::ShellMITC4Base::getMeanInternalDeformation)
    .def("vector3dUniformLoadGlobal",&XC::ShellMITC4Base::getMeanInternalForce)
    .def("vector3dUniformLoadLocal",&XC::ShellMITC4Base::vector3dUniformLoadLocal)
    .def("vector3dUniformLoadGlobal",&XC::ShellMITC4Base::vector3dUniformLoadGlobal)
    .def("strainLoad",&XC::ShellMITC4Base::strainLoad)
    .def("getParticlePos3dInterpolationFactors",getParticlePos3dInterpolationFactors)
    .def("getPos3dInterpolationFactors",getPos3dInterpolationFactors)
    .def("getParticlePos3dInterpolatedDisplacements",getParticlePos3dInterpolatedDisplacements)
    .def("getPos3dInterpolatedDisplacements",getPos3dInterpolatedDisplacements)
   ;

class_<XC::ShellMITC4, bases<XC::ShellMITC4Base>, boost::noncopyable >("ShellMITC4", no_init)
   ;

class_<XC::CorotShellMITC4, bases<XC::ShellMITC4Base>, boost::noncopyable >("CorotShellMITC4", no_init)
   ;
