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
//utils_python_interface.cxx


class_<XC::EnvelopeData, boost::noncopyable >("EnvelopeData", no_init);

class_<XC::Recorder, bases<CommandEntity>, boost::noncopyable >("Recorder", no_init);

//class_<XC::AlgorithmIncrements , bases<XC::Recorder>, boost::noncopyable >("AlgorithmIncrements", no_init);

//class_<XC::DatastoreRecorder, bases<XC::Recorder>, boost::noncopyable >("DatastoreRecorder", no_init);

class_<XC::DomainRecorderBase, bases<XC::Recorder>, boost::noncopyable >("DomainRecorderBase", no_init);

//class_<XC::FilePlotter , bases<XC::Recorder>, boost::noncopyable >("FilePlotter", no_init);

class_<XC::PropRecorder, bases<XC::Recorder>, boost::noncopyable >("PropRecorder", no_init)
  .add_property("callbackSetup",&XC::PropRecorder::getCallbackSetup,&XC::PropRecorder::setCallbackSetup,"Assigns code to execute to setup recording.")
  .add_property("callbackRecord",&XC::PropRecorder::getCallbackRecord,&XC::PropRecorder::setCallbackRecord,"Assigns code to execute while recording.")
  .add_property("callbackRestart",&XC::PropRecorder::getCallbackRestart,&XC::PropRecorder::setCallbackRestart,"Assigns code to execute while restartingg.")
  .add_property("getLastCommitTag",&XC::PropRecorder::getLastCommitTag)
  .add_property("getLastTimeStamp",&XC::PropRecorder::getLastTimeStamp)
  .add_property("getCurrentTime",&XC::PropRecorder::getCurrentTime)
  .add_property("getCommittedTime",&XC::PropRecorder::getCommittedTime)
  .add_property("getCommitTag",&XC::PropRecorder::getCommitTag)
  .add_property("getCurrentCombinationName",&XC::PropRecorder::getCurrentCombinationName)
  .add_property("getDomain", make_function( &XC::PropRecorder::getDomain, return_internal_reference<>() ),"Returns a reference to the domain.")
  ;

class_<XC::NodePropRecorder, bases<XC::PropRecorder>, boost::noncopyable >("NodePropRecorder", no_init)
  .def("setNodes",&XC::NodePropRecorder::setNodes,"Assigns nodes to the recorder.")
  ;

class_<XC::ElementPropRecorder, bases<XC::PropRecorder>, boost::noncopyable >("ElementPropRecorder", no_init)
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

class_<XC::RecorderContainer, boost::noncopyable >("RecorderContainer", no_init)
  .def("newRecorder",make_function(&XC::RecorderContainer::newRecorder,return_internal_reference<>()),"Creates a new recorder.")  
  .def("removeRecorders",&XC::RecorderContainer::removeRecorders,"Deletes all the recorders.")  
  ;
class_<XC::ObjWithRecorders, bases<CommandEntity,XC::RecorderContainer>, boost::noncopyable >("ObjWithRecorders", no_init)
  ;


#include "response/python_interface.tcc"
