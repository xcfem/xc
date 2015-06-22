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

class_<XC::ConvergenceTest, bases<XC::MovableObject,EntCmd>, boost::noncopyable >("ConvergenceTest", no_init)
  .add_property("maxNumIter", &XC::ConvergenceTest::getMaxNumIter, &XC::ConvergenceTest::setMaxNumIter)
  .add_property("currentIter", &XC::ConvergenceTest::getCurrentIter, &XC::ConvergenceTest::setCurrentIter)
  .add_property("printFlag", &XC::ConvergenceTest::getPrintFlag, &XC::ConvergenceTest::setPrintFlag)
  .add_property("normType", &XC::ConvergenceTest::getNormType, &XC::ConvergenceTest::setNormType)
  ;


class_<XC::CTestFixedNumIter, bases<XC::ConvergenceTest>, boost::noncopyable >("CTestFixedNumIter", no_init);


class_<XC::ConvergenceTestTol, bases<XC::ConvergenceTest>, boost::noncopyable >("ConvergenceTestTol", no_init)
  .add_property("tol", &XC::ConvergenceTestTol::getTolerance, &XC::ConvergenceTestTol::setTolerance)
  ;

class_<XC::CTestEnergyIncr, bases<XC::ConvergenceTestTol>, boost::noncopyable >("CTestEnergyIncr", no_init);

class_<XC::CTestNormDispIncr, bases<XC::ConvergenceTestTol>, boost::noncopyable >("CTestNormDispIncr", no_init);

class_<XC::CTestNormUnbalance, bases<XC::ConvergenceTestTol>, boost::noncopyable >("CTestNormUnbalance", no_init);


class_<XC::ConvergenceTestNorm, bases<XC::ConvergenceTestTol>, boost::noncopyable >("ConvergenceTestNorm", no_init);

class_<XC::CTestRelativeEnergyIncr, bases<XC::ConvergenceTestNorm>, boost::noncopyable >("CTestRelativeEnergyIncr", no_init);

class_<XC::CTestRelativeNormDispIncr, bases<XC::ConvergenceTestNorm>, boost::noncopyable >("CTestRelativeNormDispIncr", no_init);

class_<XC::CTestRelativeNormUnbalance, bases<XC::ConvergenceTestNorm>, boost::noncopyable >("CTestRelativeNormUnbalance", no_init);

class_<XC::CTestRelativeTotalNormDispIncr, bases<XC::ConvergenceTestNorm>, boost::noncopyable >("CTestRelativeTotalNormDispIncr", no_init);
