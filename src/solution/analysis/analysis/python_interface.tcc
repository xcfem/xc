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

class_<XC::Analysis, bases<EntCmd>, boost::noncopyable >("Analysis", no_init)
  .add_property("getAnalysisResult", &XC::Analysis::getAnalysisResult)
  ;

class_<XC::StaticAnalysis, bases<XC::Analysis>, boost::noncopyable >("StaticAnalysis", no_init)
  .def("analyze", &XC::StaticAnalysis::analyze,"Lanza el análisis definido.")
  .def("initialize", &XC::StaticAnalysis::initialize,"Inicializa el análisis del problema.")
    ;

class_<XC::EigenAnalysis , bases<XC::Analysis>, boost::noncopyable >("EigenAnalysis", no_init)
  //Eigenvectors.
  .def("getEigenvector", make_function(&XC::EigenAnalysis::getEigenvector, return_internal_reference<>()) )
.def("getEigenvectors", &XC::EigenAnalysis::getEigenvectors)
  .def("getNormalizedEigenvector", &XC::EigenAnalysis::getNormalizedEigenvector)
  .def("getNormalizedEigenvectors",&XC::EigenAnalysis::getNormalizedEigenvectors)
  //Eigenvalues.
  .def("getEigenvalue", make_function(&XC::EigenAnalysis::getEigenvalue, return_value_policy<copy_const_reference>()) )
  .def("getPulsatance",&XC::EigenAnalysis::getPulsacion)
  .def("getPeriod",&XC::EigenAnalysis::getPeriodo)
  .def("getFrequency",&XC::EigenAnalysis::getFrecuencia)
  .def("getEigenvalues",&XC::EigenAnalysis::getEigenvalues)
  .def("getPulsatances",&XC::EigenAnalysis::getPulsaciones)
  .def("getPeriods",&XC::EigenAnalysis::getPeriodos)
  .def("getFrequencies",&XC::EigenAnalysis::getFrecuencias)
  .def("getNumModes",&XC::EigenAnalysis::getNumModes)
  //Modal participation factors.
  .def("getModalParticipationFactor",&XC::EigenAnalysis::getModalParticipationFactor)
  .def("getModalParticipationFactors",&XC::EigenAnalysis::getModalParticipationFactors)
  //Distribution factors.
  .def("getDistributionFactor",&XC::EigenAnalysis::getDistributionFactor)
  .def("getDistributionFactors",&XC::EigenAnalysis::getDistributionFactors)
  //Effective modal masses.
  .def("getEffectiveModalMass",&XC::EigenAnalysis::getEffectiveModalMass)
  .def("getEffectiveModalMasses",&XC::EigenAnalysis::getEffectiveModalMasses)
  .def("getTotalMass",&XC::EigenAnalysis::getTotalMass)
  //Static equivalent load.
  .def("getEquivalentStaticLoad",&XC::EigenAnalysis::getEquivalentStaticLoad)

  .def("analyze", &XC::EigenAnalysis::analyze,"Lanza el análisis definido.")
  ;

class_<XC::LinearBucklingAnalysis, bases<XC::StaticAnalysis>, boost::noncopyable >("LinearBucklingAnalysis", no_init)
  .add_property("numModes",&XC::LinearBucklingAnalysis::getNumModes,&XC::LinearBucklingAnalysis::setNumModes)
  .def("getEigenvalue", make_function(&XC::LinearBucklingAnalysis::getEigenvalue, return_value_policy<copy_const_reference>()) )
  ;

class_<XC::LinearBucklingEigenAnalysis, bases<XC::EigenAnalysis>, boost::noncopyable >("LinearBucklingEigenAnalysis", no_init)
  .def("getEigenvalue", make_function(&XC::LinearBucklingEigenAnalysis::getEigenvalue, return_value_policy<copy_const_reference>()) )
  ;

class_<XC::ModalAnalysis , bases<XC::EigenAnalysis>, boost::noncopyable >("ModalAnalysis", no_init)
  .add_property("spectrum", make_function(&XC::ModalAnalysis::getSpectrum,return_internal_reference<>()),&XC::ModalAnalysis::setSpectrum,"Response spectrum,") 
  .def("getCQCModalCrossCorrelationCoefficients",&XC::ModalAnalysis::getCQCModalCrossCorrelationCoefficients,"Returns CQC correlation coefficients.")
  ;


//class_<XC::SubdomainAnalysis, bases<XC::Analysis, XC::MovableObject>, boost::noncopyable >("SubdomainAnalysis", no_init);

class_<XC::TransientAnalysis, bases<XC::Analysis>, boost::noncopyable >("TransientAnalysis", no_init);

class_<XC::DirectIntegrationAnalysis, bases<XC::TransientAnalysis>, boost::noncopyable >("DirectIntegrationAnalysis", no_init);

class_<XC::VariableTimeStepDirectIntegrationAnalysis, bases<XC::DirectIntegrationAnalysis>, boost::noncopyable >("VariableTimeStepDirectIntegrationAnalysis", no_init);

#ifdef _PARALLEL_PROCESSING
class_<XC::DomainDecompositionAnalysis, bases<XC::Analysis, XC::MovableObject>, boost::noncopyable >("DomainDecompositionAnalysis", no_init);

class_<XC::StaticDomainDecompositionAnalysis, bases<XC::DomainDecompositionAnalysis>, boost::noncopyable >("StaticDomainDecompositionAnalysis", no_init);

class_<XC::TransientDomainDecompositionAnalysis, bases<XC::DomainDecompositionAnalysis>, boost::noncopyable >("TransientDomainDecompositionAnalysis", no_init);

//class_<XC::SubstructuringAnalysis, bases<XC::DomainDecompositionAnalysis>, boost::noncopyable >("SubstructuringAnalysis", no_init);

#endif
