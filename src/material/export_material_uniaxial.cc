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

void export_material_uniaxial(void)
  {
    using namespace boost::python;
    docstring_options doc_options;

#include "uniaxial/python_interface.tcc"

typedef std::vector<XC::UniaxialMaterial *> vectorUniaxialMaterial;
class_<vectorUniaxialMaterial,boost::noncopyable>("vectorUniaxialMaterial")
  .def(vector_indexing_suite<vectorUniaxialMaterial>() )
  ;

class_<material_vector_UMat,bases<vectorUniaxialMaterial,EntCmd>,boost::noncopyable>("MaterialVectorUMat", no_init)
        .def("commitState", &material_vector_UMat::commitState,"Commits materials state.")
        .def("revertToLastCommit", &material_vector_UMat::revertToLastCommit,"Devuelve el estado del material al último consumado.")
        .def("revertToStart", &material_vector_UMat::revertToStart,"Devuelve el material a su estado inicial.")
      ;

  }

