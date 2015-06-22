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

// class_<XC::IntegratorVectors, bases<XC::MovableObject>, boost::noncopyable >("IntegratorVectors", no_init);

class_<XC::ProtoArcLength, bases<XC::StaticIntegrator>, boost::noncopyable >("ProtoArcLength", no_init);

class_<XC::ArcLengthBase, bases<XC::ProtoArcLength>, boost::noncopyable >("ArcLengthBase", no_init);

class_<XC::HSConstraint, bases<XC::ProtoArcLength>, boost::noncopyable >("HSConstraint", no_init);

class_<XC::ArcLength , bases<XC::ArcLengthBase>, boost::noncopyable >("ArcLength", no_init);

class_<XC::ArcLength1 , bases<XC::ArcLengthBase>, boost::noncopyable >("ArcLength1", no_init);

class_<XC::BaseControl , bases<XC::StaticIntegrator>, boost::noncopyable >("BaseControl", no_init);

class_<XC::DispBase, bases<XC::BaseControl>, boost::noncopyable >("DispBase", no_init);

class_<XC::DisplacementControl, bases<XC::DispBase>, boost::noncopyable >("DisplacementControl", no_init)
  .add_property("nod",&XC::DisplacementControl::getNodeTag,&XC::DisplacementControl::setNodeTag,"assign/retrieve node tag value.")
  .add_property("dof",&XC::DisplacementControl::getDof,&XC::DisplacementControl::setDof,"assign/retrieve dof index.")
  .add_property("dU1",&XC::DisplacementControl::getIncrement,&XC::DisplacementControl::setIncrement,"assign/retrieve increment value.")
  ;

class_<XC::MinUnbalDispNorm, bases<XC::DispBase>, boost::noncopyable >("MinUnbalDispNorm", no_init);

// class_<XC::DistributedDisplacementControl, bases<XC::DisplacementControl>, boost::noncopyable >("DistributedDisplacementControl", no_init);

class_<XC::LoadControl , bases<XC::BaseControl>, boost::noncopyable >("LoadControl", no_init)
  .add_property("dLambda1",&XC::LoadControl::getDeltaLambda,&XC::LoadControl::setDeltaLambda,"assign/retrieve dLambda value.")
  ;

class_<XC::LoadPath, bases<XC::StaticIntegrator>, boost::noncopyable >("LoadPath", no_init);




