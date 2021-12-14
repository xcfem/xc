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
//BeamIntegratorHandler.cc

#include "BeamIntegratorHandler.h"
#include "domain/domain/Domain.h"

#include "domain/mesh/element/truss_beam_column/forceBeamColumn/beam_integration/BeamIntegration.h"
#include "domain/mesh/element/truss_beam_column/forceBeamColumn/beam_integration/LobattoBeamIntegration.h"
#include "domain/mesh/element/truss_beam_column/forceBeamColumn/beam_integration/TrapezoidalBeamIntegration.h"
#include "domain/mesh/element/truss_beam_column/forceBeamColumn/beam_integration/ChebyshevBeamIntegration.h"
#include "domain/mesh/element/truss_beam_column/forceBeamColumn/beam_integration/CompositeSimpsonBeamIntegration.h"
#include "domain/mesh/element/truss_beam_column/forceBeamColumn/beam_integration/FixedLocationBeamIntegration.h"
#include "domain/mesh/element/truss_beam_column/forceBeamColumn/beam_integration/LowOrderBeamIntegration.h"
#include "domain/mesh/element/truss_beam_column/forceBeamColumn/beam_integration/MidDistanceBeamIntegration.h"
#include "domain/mesh/element/truss_beam_column/forceBeamColumn/beam_integration/LegendreBeamIntegration.h"
#include "domain/mesh/element/truss_beam_column/forceBeamColumn/beam_integration/RadauBeamIntegration.h"
#include "domain/mesh/element/truss_beam_column/forceBeamColumn/beam_integration/NewtonCotesBeamIntegration.h"
#include "domain/mesh/element/truss_beam_column/forceBeamColumn/beam_integration/UserDefinedBeamIntegration.h"
#include "domain/mesh/element/truss_beam_column/forceBeamColumn/beam_integration/plastic_hinge_integration/HingeMidpointBeamIntegration.h"
#include "domain/mesh/element/truss_beam_column/forceBeamColumn/beam_integration/plastic_hinge_integration/HingeEndpointBeamIntegration.h"
#include "domain/mesh/element/truss_beam_column/forceBeamColumn/beam_integration/plastic_hinge_integration/HingeRadauBeamIntegration.h"
#include "domain/mesh/element/truss_beam_column/forceBeamColumn/beam_integration/plastic_hinge_integration/HingeRadauTwoBeamIntegration.h"
#include "domain/mesh/element/truss_beam_column/forceBeamColumn/beam_integration/plastic_hinge_integration/UserDefinedHingeIntegration.h"
#include "domain/mesh/element/truss_beam_column/forceBeamColumn/beam_integration/plastic_hinge_integration/DistHingeIntegration.h"




void XC::BeamIntegratorHandler::free_mem(void)
  {
    for(iterator i= begin();i!= end();i++)
      delete (*i).second;
    beam_integrators.erase(begin(),end());
  }

//! @brief Default constructor.
XC::BeamIntegratorHandler::BeamIntegratorHandler(Preprocessor *owr)
  : PrepHandler(owr)
  {
    //Default integrators.
    newBI("Lobatto","Lobatto");
    newBI("Legendre","Legendre");
    newBI("Radau","Radau");
    newBI("NewtonCotes","NewtonCotes");
  }

//! @brief Copy constructor.
XC::BeamIntegratorHandler::BeamIntegratorHandler(const BeamIntegratorHandler &other) : PrepHandler(other)
  { std::cerr << "This object must no be copied." << std::endl; }

//! @brief Assignment operator.
XC::BeamIntegratorHandler &XC::BeamIntegratorHandler::operator=(const BeamIntegratorHandler &other)
  {
    PrepHandler::operator=(other);
    std::cerr << "This object must not be assigned." << std::endl;
    return *this;
  }

XC::BeamIntegratorHandler::~BeamIntegratorHandler(void)
  { free_mem(); }

//! @brief Returns a reference to the beam integrator container.
const XC::BeamIntegratorHandler::map_beam_integrators &XC::BeamIntegratorHandler::Map(void) const
  { return beam_integrators; }

//! @brief Returns an iterator which points to principio de la lista.
XC::BeamIntegratorHandler::const_iterator XC::BeamIntegratorHandler::begin(void) const
  { return beam_integrators.begin(); }
//! @brief Returns an iterator apuntando después del final de la lista.
XC::BeamIntegratorHandler::const_iterator XC::BeamIntegratorHandler::end(void) const
  { return beam_integrators.end(); }
//! @brief Returns an iterator which points to principio de la lista.
XC::BeamIntegratorHandler::iterator XC::BeamIntegratorHandler::begin(void)
  { return beam_integrators.begin(); }
//! @brief Returns an iterator apuntando después del final de la lista.
XC::BeamIntegratorHandler::iterator XC::BeamIntegratorHandler::end(void)
  { return beam_integrators.end(); }

//! @brief Si encuentra el BeamIntegrator which name is being passed as parameter returns an iterator which points to mismo.
XC::BeamIntegratorHandler::const_iterator XC::BeamIntegratorHandler::find(const std::string &nmb) const
  { return beam_integrators.find(nmb); }

//! @brief Si encuentra el BeamIntegrator which name is being passed as parameter returns an iterator which points to mismo.
XC::BeamIntegratorHandler::iterator XC::BeamIntegratorHandler::find(const std::string &nmb)
  { return beam_integrators.find(nmb); }

//! @brief Si encuentra el BeamIntegrator which name is being passed as parameter returns a pointer al mismo,
//! otherwise it returns nullptr.
XC::BeamIntegration *XC::BeamIntegratorHandler::find_ptr(const std::string &nmb)
  {
    XC::BeamIntegratorHandler::iterator i= find(nmb);
    if(i!= beam_integrators.end())
      return (*i).second;
    else
      return nullptr; 
  }

//! @brief Si encuentra el BeamIntegrator which name is being passed as parameter returns a pointer al mismo,
//! otherwise it returns nullptr.
const XC::BeamIntegration *XC::BeamIntegratorHandler::find_ptr(const std::string &nmb) const
  {
    XC::BeamIntegratorHandler::const_iterator i= find(nmb);
    if(i!= beam_integrators.end())
      return (*i).second;
    else
      return nullptr; 
  }

XC::BeamIntegration *load_beam_integration(const std::string &cmd)
  {
    XC::BeamIntegration *retval= nullptr;
    if(cmd == "Lobatto")
      retval= new XC::LobattoBeamIntegration();
    else if(cmd == "Legendre")
      retval= new XC::LegendreBeamIntegration();
    else if(cmd == "Radau")
      retval= new XC::RadauBeamIntegration();
    else if(cmd == "NewtonCotes")
      retval= new XC::NewtonCotesBeamIntegration();
    else if(cmd == "Trapezoidal")
      retval= new XC::TrapezoidalBeamIntegration();
    else if(cmd == "Chebyshev")
      retval= new XC::ChebyshevBeamIntegration();
    else if(cmd == "CompositeSimpson")
      retval= new XC::CompositeSimpsonBeamIntegration();
    else if(cmd == "UserDefined")
      retval= new XC::UserDefinedBeamIntegration();
    else if(cmd == "FixedLocation")
      retval= new XC::FixedLocationBeamIntegration();
    else if(cmd == "LowOrder")
      retval= new XC::LowOrderBeamIntegration();
    else if(cmd == "MidDistance")
      retval= new XC::MidDistanceBeamIntegration();
    else if(cmd == "HingeMidpoint")
      retval= new XC::HingeMidpointBeamIntegration();
    else if(cmd == "HingeRadau")
      retval= new XC::HingeRadauBeamIntegration();
    else if(cmd == "HingeRadauTwo")
      retval= new XC::HingeRadauTwoBeamIntegration();
    else if(cmd == "HingeEndpoint")
      retval= new XC::HingeEndpointBeamIntegration();
    else
      std::cerr << "Unknown integration type: " << cmd << std::endl;
    return retval;
  }

//! @brief Defines a new beam integrator framework.
XC::BeamIntegration *XC::BeamIntegratorHandler::newBI(const std::string &cmd,const std::string &cod)
  {
    XC::BeamIntegration *retval= load_beam_integration(cmd);
    if(retval)
      {
        retval->set_owner(this);
        if(find(cod)!=end()) //Integrator exists.
          {
	    std::clog << "BeamIntegratorHandler::newBI; ¡warning! redefining: '"
                      << cod << "'." << std::endl;
            delete beam_integrators[cod];
          }
        beam_integrators[cod]= retval;
      }
    return retval;

    if(!retval)
      std::cerr << getClassName() << "::" << __FUNCTION__
		<< "; integration scheme: " << cmd << " unknown." << std::endl;
    return retval;
  }

//! @brief True if integrator named nmb exists.
bool XC::BeamIntegratorHandler::exists(const std::string &nmb) const
  { return (find(nmb)!=end()); }

//! @brief Returns a reference to the material named as in the parameter.
XC::BeamIntegration &XC::BeamIntegratorHandler::get(const std::string &nmb)
  {
    BeamIntegration *retval= find_ptr(nmb);
    assert(retval);
    return *retval;
  }
