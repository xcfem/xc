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
//utils_python_interface.cxx

#include "python_interface.h"


void export_material_section(void)
  {
    using namespace boost::python;
    docstring_options doc_options;

#include "material/section/python_interface.tcc"

typedef std::vector<XC::SectionForceDeformation *> vectorSectionForceDeformation;
class_<vectorSectionForceDeformation,boost::noncopyable>("vectorSectionForceDeformation")
  .def(vector_indexing_suite<vectorSectionForceDeformation>() )
  ;

    class_<material_vector_SectionFDMat,bases<vectorSectionForceDeformation,EntCmd>,boost::noncopyable>("MaterialVectorSectionFDMat", no_init)
        .def("commitState", &material_vector_SectionFDMat::commitState,"Commits materials state.")
        .def("revertToLastCommit", &material_vector_SectionFDMat::revertToLastCommit,"Devuelve el estado del material al último consumado.")
        .def("revertToStart", &material_vector_SectionFDMat::revertToStart,"Devuelve el material a su estado inicial.")
        .def("getGeneralizedStresses", &material_vector_SectionFDMat::getGeneralizedStresses)
        .def("getGeneralizedStrains", &material_vector_SectionFDMat::getGeneralizedStrains)
      //.def("getMeanGeneralizedStress", &material_vector_SectionFDMat::getMeanGeneralizedStress)
      //.def("getMeanGeneralizedStrain", &material_vector_SectionFDMat::getMeanGeneralizedStrain)
        .def("getMeanGeneralizedStressByName", &material_vector_SectionFDMat::getMeanGeneralizedStressByName)
        .def("getMeanGeneralizedStrainByName", &material_vector_SectionFDMat::getMeanGeneralizedStrainByName)
  ;



  }

