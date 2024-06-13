//----------------------------------------------------------------------------
//  XC program; finite element analysis code
//  for structural analysis and design.
//
//  Copyright (C)  Luis C. Pérez Tato
//
//  This program derives from OpenSees <http://opensees.berkeley.edu>
//  developed by the  «Pacific earthquake engineering research center».
//
//  Except for the restrictions that may arise from the copyright
//  of the original program (see copyright_opensees.txt)
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
/* ****************************************************************** **
**    OpenSees - Open System for Earthquake Engineering Simulation    **
**          Pacific Earthquake Engineering Research Center            **
**                                                                    **
**                                                                    **
** (C) Copyright 1999, The Regents of the University of California    **
** All Rights Reserved.                                               **
**                                                                    **
** Commercial use of this program without express permission of the   **
** University of California, Berkeley, is strictly prohibited.  See   **
** file 'COPYRIGHT'  in main directory for information on usage and   **
** redistribution,  and for a DISCLAIMER OF ALL WARRANTIES.           **
**                                                                    **
** Developed by:                                                      **
**   Frank McKenna (fmckenna@ce.berkeley.edu)                         **
**   Gregory L. Fenves (fenves@ce.berkeley.edu)                       **
**   Filip C. Filippou (filippou@ce.berkeley.edu)                     **
**                                                                    **
** ****************************************************************** */
                                                                        
// $Revision: 1.4 $
// $Date: 2004/11/13 08:08:14 $
// $Source: /usr/local/cvs/OpenSees/SRC/analysis/algorithm/RecorderContainer.cpp,v $
                                                                        
                                                                        
#include "RecorderContainer.h"
#include <utility/recorder/Recorder.h>
#include <utility/recorder/DamageRecorder.h>
#include <utility/recorder/DatastoreRecorder.h>
#include <utility/recorder/DriftRecorder.h>
#include <utility/recorder/ElementRecorder.h>
#include <utility/recorder/EnvelopeElementRecorder.h>
#include <utility/recorder/EnvelopeNodeRecorder.h>
#include <utility/recorder/FilePlotter.h>
#include <utility/recorder/NodeRecorder.h>
#include <utility/recorder/GSA_Recorder.h>
#include <utility/recorder/MaxNodeDispRecorder.h>
#include <utility/recorder/PatternRecorder.h>
#include <utility/recorder/NodePropRecorder.h>
#include <utility/recorder/ElementPropRecorder.h>
#include "utility/utils/misc_utils/colormod.h"

XC::RecorderContainer::RecorderContainer(DataOutputHandler::map_output_handlers *oh)
  : theRecorders(), output_handlers(oh) {}


//! @brief Read a Recorder object from file.
XC::Recorder *XC::RecorderContainer::newRecorder(const std::string &cod, DataOutputHandler *output_handler)
  {
    Recorder *retval= nullptr;
    if((cod == "data_store_recorder") or (cod== "XC::DatastoreRecorder"))
      {
        DatastoreRecorder *tmp= new DatastoreRecorder();
        retval= tmp;
      }
    else if((cod == "drift_recorder") or (cod== "XC::DriftRecorder"))
      {
        DriftRecorder *tmp= new DriftRecorder();
        if(output_handler)
          tmp->SetOutputHandler(output_handler);
        else
          std::cerr << Color::red << "RecorderContainer::" << __FUNCTION__
		    << "; new recorder: " << cod 
                    << " no output handler."
		    << Color::def << std::endl;
        retval= tmp;
      }
    else if((cod == "element_recorder") or (cod== "XC::ElementRecorder"))
      {
        ElementRecorder *tmp= new ElementRecorder();
        if(output_handler)
          tmp->SetOutputHandler(output_handler);
        else
          std::cerr << Color::red << "RecorderContainer::" << __FUNCTION__
		    << "; new recorder: " << cod 
                    << " no output handler."
		    << Color::def << std::endl;
        retval= tmp;
      }
    else if((cod == "envelope_element_recorder") or (cod== "XC::EnvelopeElementRecorder"))
      {
        EnvelopeElementRecorder *tmp= new EnvelopeElementRecorder();
        if(output_handler)
          tmp->SetOutputHandler(output_handler);
        else
          std::cerr << Color::red << "RecorderContainer::" << __FUNCTION__
		    << "; new recorder: " << cod 
                    << " no output handler."
		    << Color::def << std::endl;
        retval= tmp;
      }
    else if((cod == "envelope_node_recorder") or (cod== "XC::EnvelopeNodeRecorder"))
      {
        EnvelopeNodeRecorder *tmp= new EnvelopeNodeRecorder();
        if(output_handler)
          tmp->SetOutputHandler(output_handler);
        else
          std::cerr << Color::red << "RecorderContainer::" << __FUNCTION__
		    << "; new recorder: " << cod 
                    << " no output handler."
		    << Color::def << std::endl;
        retval= tmp;
      }
    else if((cod == "max_node_disp_recorder") or (cod== "XC::MaxNodeDispRecorder"))
      {
        MaxNodeDispRecorder *tmp= new MaxNodeDispRecorder();
        retval= tmp;
      }
    else if((cod == "node_recorder") or (cod== "XC::NodeRecorder"))
      {
        NodeRecorder *tmp= new NodeRecorder();
        if(output_handler)
          tmp->SetOutputHandler(output_handler);
        else
          std::cerr << Color::red << "RecorderContainer::" << __FUNCTION__
		    << "; new recorder: " << cod 
                    << " no output handler."
		    << Color::def << std::endl;
        retval= tmp;
      }
    else if((cod == "pattern_recorder") or (cod== "XC::PatternRecorder"))
      {
        PatternRecorder *tmp= new PatternRecorder();
        retval= tmp;
      }
    else if((cod == "node_prop_recorder") or (cod== "XC::NodePropRecorder"))
      {
        NodePropRecorder *tmp= new NodePropRecorder(get_domain_ptr());
        retval= tmp;
      }
    else if((cod == "element_prop_recorder") or (cod== "XC::ElementPropRecorder"))
      {
        ElementPropRecorder *tmp= new ElementPropRecorder(get_domain_ptr());
        retval= tmp;
      }
    else
      std::cerr << Color::red << "RecorderContainer::" << __FUNCTION__
		<< "; recorder type: '" << cod
                << "' unknown."
		<< Color::def << std::endl;
    if(retval)
      addRecorder(*retval);
    return retval;
  }

//! @brief Destructor.
XC::RecorderContainer::~RecorderContainer(void)
  {
    for(recorders_list::iterator i= theRecorders.begin();i!= theRecorders.end(); i++)
      delete *i; 
    theRecorders.erase(theRecorders.begin(),theRecorders.end());
  }

//! @brief Adds a recorder.
//!
//! To add a recorder object \p theRecorder to the
//! SolutionAlgorithm. returns \f$0\f$.
int XC::RecorderContainer::addRecorder(Recorder &theRecorder)
  {
    theRecorders.push_back(&theRecorder);
    return 0;
  }

//! @brief To invoke {\em record(cTag, timeStamp)} on any Recorder objects
//! which have been added.
int XC::RecorderContainer::record(int cTag, double timeStamp)
  {
    for(recorders_list::iterator i= theRecorders.begin();i!= theRecorders.end(); i++)
      (*i)->record(cTag, timeStamp);
    return 0;
  }

//! @brief To invoke {\em restart()} on any Recorder objects
//! which have been added.
void XC::RecorderContainer::restart(void)
  {
    for(recorders_list::iterator i= theRecorders.begin();i!= theRecorders.end(); i++)
      (*i)->restart();
  }

//! @brief Remove the recorders.
int XC::RecorderContainer::removeRecorders(void)
  {
    for(recorders_list::iterator i= theRecorders.begin();i!= theRecorders.end(); i++)
      delete *i; 
    theRecorders.erase(theRecorders.begin(),theRecorders.end());
    return 0;
  }

//! @brief Asigna el domain a los recorders.
void XC::RecorderContainer::setLinks(Domain *ptr_dom)
  {
    if(ptr_dom)
      {
        for(recorders_list::iterator i= theRecorders.begin();i!= theRecorders.end(); i++)
          {
            DomainRecorderBase *tmp= dynamic_cast<DomainRecorderBase *>(*i);
            if(tmp)
              tmp->setDomain(*ptr_dom);
          }
      }
    else
      std::cerr << Color::red << "RecorderContainer::" << __FUNCTION__
		<< "; domain not set."
		<< Color::def << std::endl;
  }

//! @brief Set the outputhandlers container.
void XC::RecorderContainer::SetOutputHandlers(DataOutputHandler::map_output_handlers *oh)
  { output_handlers= oh; }

int XC::RecorderContainer::sendData(Communicator &comm)
  {
    //Position 1 and 2 are reserved for members of
    //this class.
    return 0;
  }

int XC::RecorderContainer::recvData(const Communicator &comm)
  {
    //Position 1 and 2 are reserved for members of
    //this class.
    return 0;
  }

//! @brief Return a Python dictionary with the object members values.
boost::python::dict XC::RecorderContainer::getPyDict(void) const
  {
    boost::python::dict retval;
    boost::python::list recorder_lst;
    for(recorders_list::const_iterator i= theRecorders.begin();i!= theRecorders.end(); i++)
      {
	DomainRecorderBase *tmp= dynamic_cast<DomainRecorderBase *>(*i);
	if(tmp)
	  recorder_lst.append(tmp->getPyDict());
      }
    retval["values"]= recorder_lst;
    return retval;
  }

//! @brief Set the values of the object members from a Python dictionary.
void XC::RecorderContainer::setPyDict(const boost::python::dict &d)
  {
    const boost::python::list recorder_lst= boost::python::extract<boost::python::list>(d["values"]);
    const size_t sz= len(recorder_lst);
    for(size_t i= 0; i<sz; i++)
      {
	boost::python::dict tmp= boost::python::extract<boost::python::dict>(recorder_lst[i]);
	const std::string className= boost::python::extract<std::string>(tmp["className"]);
	Recorder *newR= this->newRecorder(className);
	if(!newR) // failed.
	  std::cerr << Color::red << "RecorderContainer::" << __FUNCTION__
		    << "; can't read recorder of type: '" << className
		    << "'."
		    << Color::def << std::endl;
      }
  }
