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

class_<XC::Constraint, bases<XC::ContinuaReprComponent>, boost::noncopyable >("Constraint", no_init)
  .add_property("getNodeIdx",&XC::Constraint::getNodeIdx,"Returns node idxmfor Vtk graphics.")
  .add_property("getVtkCellType",&XC::Constraint::getVtkCellType,"Returns cell type for Vtk graphics.")
  ;

class_<XC::SP_Constraint, bases<XC::Constraint>, boost::noncopyable >("SPConstraint", no_init)
  ;

class_<XC::ImposedMotionBase , bases<XC::SP_Constraint>, boost::noncopyable >("ImposedMotionBase", no_init);

class_<XC::ImposedMotionSP , bases<XC::ImposedMotionBase>, boost::noncopyable >("ImposedMotionSP", no_init);

class_<XC::ImposedMotionSP1 , bases<XC::ImposedMotionBase>, boost::noncopyable >("ImposedMotionSP1", no_init);

class_<XC::MP_ConstraintBase, bases<XC::Constraint>, boost::noncopyable >("MP_ConstraintBase", no_init);


class_<XC::MP_Constraint, bases<XC::MP_ConstraintBase>, boost::noncopyable >("MP_Constraint", no_init);

class_<XC::EqualDOF, bases<XC::MP_Constraint>, boost::noncopyable >("EqualDOF", no_init);

class_<XC::MP_Joint2D , bases<XC::MP_Constraint>, boost::noncopyable >("MP_Joint2D", no_init);

class_<XC::MP_Joint3D, bases<XC::MP_Constraint>, boost::noncopyable >("MP_Joint3D", no_init);

class_<XC::RigidDiaphragm, bases<XC::MP_Constraint>, boost::noncopyable >("RigidDiaphragm", no_init);

class_<XC::RigidBase, bases<XC::MP_Constraint>, boost::noncopyable >("RigidBase", no_init);

class_<XC::RigidBeam, bases<XC::RigidBase>, boost::noncopyable >("RigidBeam", no_init);

class_<XC::RigidRod, bases<XC::RigidBase>, boost::noncopyable >("RigidRod", no_init);

class_<XC::MRMP_Constraint, bases<XC::MP_ConstraintBase>, boost::noncopyable >("MRMP_Constraint", no_init);

class_<XC::GlueNodeToElement, bases<XC::MRMP_Constraint>, boost::noncopyable >("GlueNodeToElement", no_init);

//class_<XC::MapCasosActivos, bases<EntCmd>, boost::noncopyable >("MapCasosActivos", no_init);

class_<XC::CondContorno, bases<XC::MeshComponentContainer>, boost::noncopyable >("CondContorno", no_init);
