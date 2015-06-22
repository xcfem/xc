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

class_<XC::EigenAlgorithm, bases<XC::SolutionAlgorithm>, boost::noncopyable >("EigenAlgorithm", no_init);

class_<XC::FrequencyAlgo, bases<XC::EigenAlgorithm>, boost::noncopyable >("FrequencyAlgo", no_init);

class_<XC::LinearBucklingAlgo, bases<XC::EigenAlgorithm>, boost::noncopyable >("LinearBucklingAlgo", no_init);

class_<XC::StandardEigenAlgo, bases<XC::EigenAlgorithm>, boost::noncopyable >("StandardEigenAlgo", no_init);

