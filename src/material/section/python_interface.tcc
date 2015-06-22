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

const XC::Vector &(XC::SectionForceDeformation::*getStressResultantVector)(void) const= &XC::SectionForceDeformation::getStressResultant;
class_<XC::SectionForceDeformation, XC::SectionForceDeformation *, bases<XC::Material>, boost::noncopyable >("SectionForceDeformation", no_init)
  .def("getStressResultantComponent",&XC::SectionForceDeformation::getStressResultantByName)
  .def("getStressResultant",make_function(getStressResultantVector, return_internal_reference<>()))
  .def("getSectionDeformationByName",&XC::SectionForceDeformation::getSectionDeformationByName)
  .def("getTangentStiffness", make_function(&XC::SectionForceDeformation::getSectionTangent, return_internal_reference<>()))
  .def("getInitialTangentStiffness", make_function(&XC::SectionForceDeformation::getInitialTangent, return_internal_reference<>()))
  .def("getFlexibility", make_function(&XC::SectionForceDeformation::getSectionFlexibility, return_internal_reference<>()))
  .def("getInitialFlexibility", make_function(&XC::SectionForceDeformation::getInitialFlexibility, return_internal_reference<>()))
  .def("getStrain",&XC::SectionForceDeformation::getStrain,"Returns strain at (y,z) position.")
   ;

class_<XC::Bidirectional, bases<XC::SectionForceDeformation>, boost::noncopyable >("Bidirectional", no_init);

//class_<XC::GenericSectionNd, bases<XC::SectionForceDeformation>, boost::noncopyable >("GenericSectionNd", no_init);

//class_<XC::Isolator2spring , bases<XC::SectionForceDeformation>, boost::noncopyable >("Isolator2spring", no_init);

//Semiplano2d (XC::SeccionBarraPrismatica::*getSemiplanoTraccionesRecta)(const Recta2d &,bool &)= &XC::SeccionBarraPrismatica::getSemiplanoTracciones;
//Semiplano2d (XC::SeccionBarraPrismatica::*getSemiplanoTracciones)(bool &)= &XC::SeccionBarraPrismatica::getSemiplanoTracciones;
class_<XC::SeccionBarraPrismatica, XC::SeccionBarraPrismatica *, bases<XC::SectionForceDeformation>, boost::noncopyable >("SeccionBarraPrismatica", no_init)
  .def("getCdgY",&XC::SeccionBarraPrismatica::getCdgY)
  .def("getCdgZ",&XC::SeccionBarraPrismatica::getCdgZ)
  .def("isSubjectedToAxialLoad",&XC::SeccionBarraPrismatica::hayAxil,"True if section is bended.")
  .def("isSubjectedToBending",&XC::SeccionBarraPrismatica::hayMomento,"True if section is bended.")
  .def("isSubjectedToShear",&XC::SeccionBarraPrismatica::hayCortante,"True if section is subjected to shear.")
  .def("isSubjectedToTorsion",&XC::SeccionBarraPrismatica::hayTorsor,"True if section is subjected to torsion.")
  .def("getCenterOfMass",&XC::SeccionBarraPrismatica::getCdg)
  .def("getInternalForcesAxis",&XC::SeccionBarraPrismatica::getEjeEsfuerzos)
  .def("getNeutralAxis",&XC::SeccionBarraPrismatica::getFibraNeutra,"Returns neutral axis.")
  .def("getPuntoSemiplanoTracciones",&XC::SeccionBarraPrismatica::getPuntoSemiplanoTracciones)
  .def("getPuntoSemiplanoCompresiones",&XC::SeccionBarraPrismatica::getPuntoSemiplanoCompresiones)
//.def("getTractionHalfPlane",getSemiplanoTracciones)
//  .def("getTractionHalfPlaneFromLine",getSemiplanoTraccionesRecta)
//  .def("getSemiplanoCompresiones",&XC::SeccionBarraPrismatica::getSemiplanoCompresiones)
//  .def("getSemiplanoCompresiones",&XC::SeccionBarraPrismatica::getSemiplanoCompresiones)
  ;

class_<XC::SectionAggregator , bases<XC::SeccionBarraPrismatica>, boost::noncopyable >("SectionAggregator", no_init)
  .def("getSection", make_function(&XC::SectionAggregator::getSection, return_internal_reference<>()))
  .def("setSection",&XC::SectionAggregator::setSection)
  .def("setAdditions",&XC::SectionAggregator::setAddtionsPyList)
  .def("getAdditions", make_function(&XC::SectionAggregator::getAdditions, return_internal_reference<>()))
  ;

class_<XC::AggregatorAdditions, bases<XC::DqUniaxialMaterial>, boost::noncopyable >("AggregatorAdditions", no_init);

class_<XC::GenericSection1d , bases<XC::SeccionBarraPrismatica>, boost::noncopyable >("GenericSection1d", no_init);

typedef std::vector<XC::SeccionBarraPrismatica *> v_sections;
class_<v_sections, boost::noncopyable>("v_sections")
  .def(vector_indexing_suite<v_sections>() )
  ;

class_<XC::VectorSeccionesBarraPrismatica, bases<EntCmd,v_sections>, boost::noncopyable >("VectorSeccionesBarraPrismatica", no_init);

class_<XC::KRSeccion, bases<EntCmd>, boost::noncopyable >("KRSeccion", no_init);


class_<XC::ResponseId, bases<XC::ID>, boost::noncopyable >("ResponseId", no_init);

class_<XC::RespPMz, bases<XC::ResponseId>, boost::noncopyable >("RespPMz", no_init);

class_<XC::RespPMzV, bases<XC::ResponseId>, boost::noncopyable >("RespPMzV", no_init);

class_<XC::RespVyP, bases<XC::ResponseId>, boost::noncopyable >("RespVyP", no_init);

class_<XC::RespPMzMy, bases<XC::ResponseId>, boost::noncopyable >("RespPMzMy", no_init);

class_<XC::RespPMzMyT, bases<XC::ResponseId>, boost::noncopyable >("RespPMzMyT", no_init);

class_<XC::RespPMzVyMyVzT, bases<XC::ResponseId>, boost::noncopyable >("RespPMzVyMyVzT", no_init);

class_<XC::RespFiberSectionShear3d, bases<XC::ResponseId>, boost::noncopyable >("RespFiberSectionShear3d", no_init);

class_<XC::RespPVyMz, bases<XC::ResponseId>, boost::noncopyable >("RespPVyMz", no_init);

class_<XC::RespMembraneMaterial, bases<XC::ResponseId>, boost::noncopyable >("RespMembraneMaterial", no_init);

class_<XC::RespPlateMaterial, bases<XC::ResponseId>, boost::noncopyable >("RespPlateMaterial", no_init);

class_<XC::RespShellMaterial, bases<XC::ResponseId>, boost::noncopyable >("RespShellMaterial", no_init);


#include "repres/python_interface.tcc"
#include "elastic_section/python_interface.tcc"
#include "plate_section/python_interface.tcc"
