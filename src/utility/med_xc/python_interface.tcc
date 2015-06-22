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

boost::python::scope().attr("MED_NODE")= MED_EN::MED_NODE;
boost::python::scope().attr("MED_CELL")= MED_EN::MED_CELL;
boost::python::scope().attr("MED_ALL_ENTITIES")= MED_EN::MED_ALL_ENTITIES;

class_<XC::MEDObject, bases<EntCmd>, boost::noncopyable >("MEDObject", no_init);

class_<XC::MEDBaseInfo, bases<XC::MEDObject>, boost::noncopyable >("MEDBaseInfo", no_init)
  .def("clear",&XC::MEDBaseInfo::clear)
  ;

class_<XC::MEDCellBaseInfo, bases<XC::MEDBaseInfo>, boost::noncopyable >("MEDCellBaseInfo", no_init);

class_<XC::MEDCellInfo, bases<XC::MEDCellBaseInfo>, boost::noncopyable >("MEDCellInfo", no_init);

class_<XC::MEDGaussModel, bases<XC::MEDObject>, boost::noncopyable >("MEDGaussModel", no_init);

class_<XC::MEDGroupInfo, bases<XC::MEDCellBaseInfo>, boost::noncopyable >("MEDGroupInfo", no_init);

class_<XC::MEDMesh, bases<XC::MEDObject>, boost::noncopyable >("MEDMesh")
  .add_property("meshName",&XC::MEDMesh::getMeshName,&XC::MEDMesh::setMeshName,"Mesh name.")
  .def("read",&XC::MEDMesh::read,"Reads the mesh from the MED file.")
  .def("getSpaceDimension",&XC::MEDMesh::getSpaceDimension)
  .def("getMeshDimension",&XC::MEDMesh::getMeshDimension)
  .def("getNumberOfNodes",&XC::MEDMesh::getNumberOfNodes)
  .def("getCoordinatesNames",&XC::MEDMesh::getCoordinatesNames)
  .def("getCoordinatesUnits",&XC::MEDMesh::getCoordinatesUnits)
  .def("getCoordinates",&XC::MEDMesh::getCoordinates)
  .def("getNumberOfCellTypes",&XC::MEDMesh::getNumberOfCellTypes)
  .def("getCellTypes",&XC::MEDMesh::getCellTypes)
  .def("getCellTypeNames",&XC::MEDMesh::getCellTypeNames)
  .def("getNumCellsOfType",&XC::MEDMesh::getNumCellsOfType)
  .def("getConnectivityCellsOfType",&XC::MEDMesh::getConnectivityCellsOfType)
  .def("getNumberOfGroups",&XC::MEDMesh::getNumberOfGroups)
  .def("getNumberOfFamilies",&XC::MEDMesh::getNumberOfFamilies)
  ;

class_<XC::MEDMeshing, bases<XC::MEDObject>, boost::noncopyable >("MEDMeshing", init<const XC::ProblemaEF &>())
  .add_property("meshName",&XC::MEDMeshing::getMeshName,&XC::MEDMeshing::setMeshName,"Mesh name.")
  .add_property("vertices",make_function(&XC::MEDMeshing::getVertices,return_internal_reference<>()),&XC::MEDMeshing::setVertices,"Access to mesh vertices.")
  .add_property("cells",make_function(&XC::MEDMeshing::getCells,return_internal_reference<>()),&XC::MEDMeshing::setCells,"Access to mesh cells.")
  .def("write",&XC::MEDMeshing::write,"Write the mesh to the MED file.")
  .def("clear",&XC::MEDMeshing::clear, "Clear objets.")
  ;

class_<XC::MEDVertexInfo, bases<XC::MEDBaseInfo>, boost::noncopyable >("MEDVertexInfo", no_init)
  .add_property("coordinateNames", make_function( &XC::MEDVertexInfo::getCoordinateNames, return_value_policy<return_by_value>() ), &XC::MEDVertexInfo::setCoordinateNames, "Set coordinate names (X,Y,Z), (r,s,t),...")
  ;

typedef XC::MEDTFieldInfo<int> MEDFieldInfoInt;
class_<MEDFieldInfoInt, bases<XC::MEDObject>, boost::noncopyable >("MEDFieldInfoInt", no_init)
  .def("setValueIJ",&MEDFieldInfoInt::setValueIJ,"Assigns value at i,) position")
  .def("setValueIJK",&MEDFieldInfoInt::setValueIJK,"Assigns value at i,),k position")
  .def("write",&MEDFieldInfoInt::write,"Write the field to the MED file.")
  ;

class_<XC::MEDIntFieldInfo, bases<MEDFieldInfoInt>, boost::noncopyable >("MEDIntFieldInfo", no_init);

typedef XC::MEDTFieldInfo<double> MEDFieldInfoDbl;
class_<MEDFieldInfoDbl, bases<XC::MEDObject>, boost::noncopyable >("MEDFieldInfoDbl", no_init)
  .def("setValueIJ",&MEDFieldInfoDbl::setValueIJ,"Assigns value at i,) position")
  .def("setValueIJK",&MEDFieldInfoDbl::setValueIJK,"Assigns value at i,),k position")
  .def("write",&MEDFieldInfoDbl::write,"Write the field to the MED file.")
  ;

class_<XC::MEDDblFieldInfo, bases<MEDFieldInfoDbl>, boost::noncopyable >("MEDDblFieldInfo", no_init)
  ;


