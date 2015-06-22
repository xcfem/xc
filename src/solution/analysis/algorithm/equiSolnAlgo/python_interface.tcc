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

class_<XC::EquiSolnAlgo, bases<XC::SolutionAlgorithm>, boost::noncopyable >("EquiSolnAlgo", no_init);

class_<XC::EquiSolnConvAlgo, bases<XC::EquiSolnAlgo>, boost::noncopyable >("EquiSolnConvAlgo", no_init);

class_<XC::BFBRoydenBase, bases<XC::EquiSolnConvAlgo>, boost::noncopyable >("BFBRoydenBase", no_init);

class_<XC::BFGS, bases<XC::BFBRoydenBase>, boost::noncopyable >("BFGS", no_init);

class_<XC::Broyden, bases<XC::BFBRoydenBase>, boost::noncopyable >("Broyden", no_init);

class_<XC::KrylovNewton, bases<XC::EquiSolnAlgo>, boost::noncopyable >("KrylovNewton", no_init);

class_<XC::Linear, bases<XC::EquiSolnAlgo>, boost::noncopyable >("Linear", no_init);

class_<XC::NewtonBased, bases<XC::EquiSolnAlgo>, boost::noncopyable >("NewtonBased", no_init);

class_<XC::ModifiedNewton, bases<XC::NewtonBased>, boost::noncopyable >("ModifiedNewton", no_init);

class_<XC::NewtonLineSearch, bases<XC::EquiSolnAlgo>, boost::noncopyable >("NewtonLineSearch", no_init);

class_<XC::NewtonRaphson, bases<XC::NewtonBased>, boost::noncopyable >("NewtonRaphson", no_init);

class_<XC::PeriodicNewton, bases<XC::NewtonBased>, boost::noncopyable >("PeriodicNewton", no_init);

#include "lineSearch/python_interface.tcc"
