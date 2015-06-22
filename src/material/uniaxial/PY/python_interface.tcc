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

 //class_<XC::InternalParamsA, bases<XC::MovableObject>, boost::noncopyable >("InternalParamsA", no_init);

 //class_<XC::InternalParamsIn, bases<XC::InternalParamsA>, boost::noncopyable >("InternalParamsIn", no_init);

 //class_<XC::InternalParamsLR, bases<XC::InternalParamsA>, boost::noncopyable >("InternalParamsLR", no_init);

 //class_<XC::InternalParamsLRIn, bases<XC::InternalParamsLR>, boost::noncopyable >("InternalParamsLRIn", no_init);

class_<XC::PYBase, bases<XC::UniaxialMaterial>, boost::noncopyable >("PYBase", no_init);

class_<XC::PQyzBase, bases<XC::PYBase>, boost::noncopyable >("PQyzBase", no_init);

class_<XC::TzSimple1, bases<XC::PYBase>, boost::noncopyable >("TzSimple1", no_init);

class_<XC::TzLiq1 , bases<XC::TzSimple1>, boost::noncopyable >("TzLiq1", no_init);

class_<XC::PySimple1, bases<XC::PQyzBase>, boost::noncopyable >("PySimple1", no_init);

class_<XC::QzSimple1 , bases<XC::PQyzBase>, boost::noncopyable >("QzSimple1", no_init);

class_<XC::PyLiq1 , bases<XC::PySimple1>, boost::noncopyable >("PyLiq1", no_init);

//class_<XC::PySimple1Gen, bases<XC::Simple1GenBase>, boost::noncopyable >("PySimple1Gen", no_init);

//class_<XC::TzSimple1Gen, bases<XC::Simple1GenBase>, boost::noncopyable >("TzSimple1Gen", no_init);

