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
//utils_python_interface.cxx


class_<XC::EnvelopeData, boost::noncopyable >("EnvelopeData", no_init);

class_<XC::Recorder, bases<EntCmd>, boost::noncopyable >("Recorder", no_init);

//class_<XC::AlgorithmIncrements , bases<XC::Recorder>, boost::noncopyable >("AlgorithmIncrements", no_init);

//class_<XC::DatastoreRecorder, bases<XC::Recorder>, boost::noncopyable >("DatastoreRecorder", no_init);

class_<XC::DomainRecorderBase, bases<XC::Recorder>, boost::noncopyable >("DomainRecorderBase", no_init);

//class_<XC::FilePlotter , bases<XC::Recorder>, boost::noncopyable >("FilePlotter", no_init);

class_<XC::PropRecorder, bases<XC::Recorder>, boost::noncopyable >("PropRecorder", no_init)
  .add_property("callbackRecord",&XC::PropRecorder::getCallbackRecord,&XC::PropRecorder::setCallbackRecord,"Assigns code to execute while recording.")
  .add_property("callbackRestart",&XC::PropRecorder::getCallbackRestart,&XC::PropRecorder::setCallbackRestart,"Assigns code to execute while restartingg.")
  .add_property("getLastCommitTag",&XC::PropRecorder::getLastCommitTag)
  .add_property("getLastTimeStamp",&XC::PropRecorder::getLastTimeStamp)
  .add_property("getCurrentTime",&XC::PropRecorder::getCurrentTime)
  .add_property("getCommittedTime",&XC::PropRecorder::getCommittedTime)
  .add_property("getCommitTag",&XC::PropRecorder::getCommitTag)
  .add_property("getNombreCombActual",&XC::PropRecorder::getNombreCombActual)
  ;

class_<XC::NodePropRecorder, bases<XC::PropRecorder>, boost::noncopyable >("NodePropRecorder", no_init)
  .def("setNodes",&XC::NodePropRecorder::setNodes,"Assigns nodes to the recorder.")
  ;

class_<XC::ElementPropRecorder, bases<XC::PropRecorder>, boost::noncopyable >("PropRecorder", no_init)
  .def("setElements",&XC::ElementPropRecorder::setElements,"Assigns elements to the recorder.")
  ;

// class_<XC::YsVisual , bases<XC::Recorder>, boost::noncopyable >("YsVisual", no_init);

// class_<XC::DamageRecorder, bases<XC::DomainRecorderBase>, boost::noncopyable >("DamageRecorder", no_init);

// class_<XC::GSA_Recorder, bases<XC::DomainRecorderBase>, boost::noncopyable >("GSA_Recorder", no_init);

 class_<XC::HandlerRecorder, bases<XC::DomainRecorderBase>, boost::noncopyable >("HandlerRecorder", no_init);

// class_<XC::MaxNodeDispRecorder, bases<XC::DomainRecorderBase>, boost::noncopyable >("MaxNodeDispRecorder", no_init);

// class_<XC::PatternRecorder, bases<XC::DomainRecorderBase>, boost::noncopyable >("PatternRecorder", no_init);

// class_<XC::DriftRecorder, bases<XC::HandlerRecorder>, boost::noncopyable >("DriftRecorder", no_init);

class_<XC::MeshCompRecorder, bases<XC::HandlerRecorder>, boost::noncopyable >("MeshCompRecorder", no_init);

class_<XC::ElementRecorderBase, bases<XC::MeshCompRecorder>, boost::noncopyable >("ElementRecorderBase", no_init);

class_<XC::NodeRecorderBase, bases<XC::MeshCompRecorder>, boost::noncopyable >("NodeRecorderBase", no_init);

class_<XC::NodeRecorder, bases<XC::NodeRecorderBase>, boost::noncopyable >("NodeRecorder", no_init);

class_<XC::EnvelopeNodeRecorder, bases<XC::NodeRecorderBase>, boost::noncopyable >("EnvelopeNodeRecorder", no_init);

class_<XC::ElementRecorder, bases<XC::ElementRecorderBase>, boost::noncopyable >("ElementRecorder", no_init);

class_<XC::EnvelopeElementRecorder, bases<XC::ElementRecorderBase>, boost::noncopyable >("EnvelopeElementRecorder", no_init);

class_<XC::ObjWithRecorders, bases<EntCmd>, boost::noncopyable >("ObjWithRecorders", no_init)
  .def("newRecorder",make_function(&XC::ObjWithRecorders::newRecorder,return_internal_reference<>()),"Creates a new recorder.")  
  .def("removeRecorders",&XC::ObjWithRecorders::removeRecorders,"Deletes all the recorders.")  
  ;


#include "response/python_interface.tcc"
