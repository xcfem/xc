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

class_<XC::ReinfBar, XC::ReinfBar *, bases<XC::DiscretBase>, boost::noncopyable >("ReinfBar", no_init)

  .add_property("diameter",&XC::ReinfBar::getDiameter,&XC::ReinfBar::setDiameter,"Bar diameter.")
  .add_property("area",&XC::ReinfBar::getArea,&XC::ReinfBar::setArea,"Bar area.")
  .add_property("pos",make_function(&XC::ReinfBar::getPosition,return_internal_reference<>()),&XC::ReinfBar::setPosition,"Position vector of bar centroid.")
  .add_property("getPos2d",&XC::ReinfBar::getPos2d,"Pos2d of bar centroid.")

  .add_property("getIx",&XC::ReinfBar::Ix,"Moment of inertia about x axis.")
  .add_property("getIy",&XC::ReinfBar::Iy,"Moment of inertia about y axis.")
  .add_property("getIz",&XC::ReinfBar::Iz,"Moment of inertia about z axis.")
  .add_property("getPyz",&XC::ReinfBar::Pyz,"Moment of inertia about z axis.")
  .add_property("getThetaP",&XC::ReinfBar::Theta_p,"Angle of principal inertia axis with reference axis.")
  .add_property("getIAxisDirA",make_function(&XC::ReinfBar::DirEjeI_a,return_internal_reference<>()),"Principal inertia axis direction A.")
  .add_property("getIAxisDirB",make_function(&XC::ReinfBar::DirEjeI_b,return_internal_reference<>()),"Principal inertia axis direction B.")
  .add_property("getI1",&XC::ReinfBar::getI1,"First principal moment of inertia.")
  .add_property("getI2",&XC::ReinfBar::getI2,"Second principal moment of inertia.")
  //.def("getI",&XC::ReinfBar::getI,"(i,j) component of inertia tensor.")
  //.def("getI",&XC::ReinfBar::getI,"(i,j,o) component of inertia tensor about point o.")
  //.def("getI",&XC::ReinfBar::getI,"(O,e) inertia .")
  .def("getIO",&XC::ReinfBar::getIO,"(o) inertia .")
  //.def("getI",&XC::ReinfBar::getI,"inertia tensor.")
  //.def("getI",&XC::ReinfBar::getI,"(o) inertia tensor about o.")
  ;

typedef std::vector<XC::ReinfBar *> vector_ptr_reinf_bar;
class_<vector_ptr_reinf_bar, boost::noncopyable>("vector_ptr_reinf_bar")
  .def(vector_indexing_suite<vector_ptr_reinf_bar>() )
  ;

class_<XC::VectorReinfBar, bases<XC::SeccionInerte,vector_ptr_reinf_bar>, boost::noncopyable >("VectorReinfBar", no_init);
