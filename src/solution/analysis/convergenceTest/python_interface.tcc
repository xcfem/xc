//----------------------------------------------------------------------------
//  XC program; finite element analysis code
//  for structural analysis and design.
//
//  Copyright (C)  Luis Claudio Pérez Tato
//
//  XC is free software: you can redistribute it and/or modify 
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or 
//  (at your option) any later version.
//
//  This software is distributed in the hope that it will be useful, but 
//  WITHOUT ANY WARRANTY; without even the implied warranty of 
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details. 
//
//
// You should have received a copy of the GNU General Public License 
// along with this program.
// If not, see <http://www.gnu.org/licenses/>.
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
