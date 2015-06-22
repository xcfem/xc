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

class_<XC::nDarray, boost::noncopyable >("nDarray", no_init);

class_<XC::BJmatrix , bases<XC::nDarray>, boost::noncopyable >("BJmatrix", no_init);

class_<XC::BJtensor, bases<XC::nDarray>, boost::noncopyable >("BJtensor", no_init);

class_<XC::Cosseratstraintensor , bases<XC::BJtensor>, boost::noncopyable >("Cosseratstraintensor", no_init);

class_<XC::Cosseratstresstensor , bases<XC::BJtensor>, boost::noncopyable >("Cosseratstresstensor", no_init);

class_<XC::straintensor , bases<XC::BJtensor>, boost::noncopyable >("straintensor", no_init);

class_<XC::stresstensor , bases<XC::BJtensor>, boost::noncopyable >("stresstensor", no_init);

