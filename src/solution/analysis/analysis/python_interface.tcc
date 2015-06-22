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
