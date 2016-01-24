//----------------------------------------------------------------------------
//  programa XC; cálculo mediante el método de los elementos finitos orientado
//  a la solución de problemas estructurales.
//
//  Copyright (C)  Luis Claudio Pérez Tato
//
//  El programa deriva del denominado OpenSees <http://opensees.berkeley.edu>
//  desarrollado por el «Pacific earthquake engineering research center».
//
//  Salvo las restricciones que puedan derivarse del copyright del
//  programa original (ver archivo copyright_opensees.txt) este
//  software es libre: usted puede redistribuirlo y/o modificarlo 
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
//BeamIntegratorLoader.cc

#include "BeamIntegratorLoader.h"
#include "domain/domain/Domain.h"

#include "domain/mesh/element/truss_beam_column/forceBeamColumn/beam_integration/BeamIntegration.h"
#include "domain/mesh/element/truss_beam_column/forceBeamColumn/beam_integration/LobattoBeamIntegration.h"
#include "domain/mesh/element/truss_beam_column/forceBeamColumn/beam_integration/TrapezoidalBeamIntegration.h"
#include "domain/mesh/element/truss_beam_column/forceBeamColumn/beam_integration/CompositeSimpsonBeamIntegration.h"
#include "domain/mesh/element/truss_beam_column/forceBeamColumn/beam_integration/FixedLocationBeamIntegration.h"
#include "domain/mesh/element/truss_beam_column/forceBeamColumn/beam_integration/LowOrderBeamIntegration.h"
#include "domain/mesh/element/truss_beam_column/forceBeamColumn/beam_integration/MidDistanceBeamIntegration.h"
#include "domain/mesh/element/truss_beam_column/forceBeamColumn/beam_integration/LegendreBeamIntegration.h"
#include "domain/mesh/element/truss_beam_column/forceBeamColumn/beam_integration/RadauBeamIntegration.h"
#include "domain/mesh/element/truss_beam_column/forceBeamColumn/beam_integration/NewtonCotesBeamIntegration.h"
#include "domain/mesh/element/truss_beam_column/forceBeamColumn/beam_integration/UserDefinedBeamIntegration.h"
#include "domain/mesh/element/truss_beam_column/forceBeamColumn/beam_integration/HingeMidpointBeamIntegration.h"
#include "domain/mesh/element/truss_beam_column/forceBeamColumn/beam_integration/HingeEndpointBeamIntegration.h"
#include "domain/mesh/element/truss_beam_column/forceBeamColumn/beam_integration/HingeRadauBeamIntegration.h"
#include "domain/mesh/element/truss_beam_column/forceBeamColumn/beam_integration/HingeRadauTwoBeamIntegration.h"
#include "domain/mesh/element/truss_beam_column/forceBeamColumn/beam_integration/UserDefinedHingeIntegration.h"
#include "domain/mesh/element/truss_beam_column/forceBeamColumn/beam_integration/DistHingeIntegration.h"

#include "xc_utils/src/base/any_const_ptr.h"
#include "xc_utils/src/base/utils_any.h"

void XC::BeamIntegratorLoader::libera(void)
  {
    for(iterator i= begin();i!= end();i++)
      delete (*i).second;
    beam_integrators.erase(begin(),end());
  }

//! @brief Constructor por defecto.
XC::BeamIntegratorLoader::BeamIntegratorLoader(Preprocessor *owr)
  : Loader(owr)
  {
    //Default integrators.
    newBI("Lobatto","Lobatto");
    newBI("Legendre","Legendre");
    newBI("Radau","Radau");
    newBI("NewtonCotes","NewtonCotes");
  }

//! @brief Constructor de copia.
XC::BeamIntegratorLoader::BeamIntegratorLoader(const BeamIntegratorLoader &otro) : Loader(otro)
  { std::cerr << "No se debe copiar este objeto." << std::endl; }

//! @brief Operador asignación.
XC::BeamIntegratorLoader &XC::BeamIntegratorLoader::operator=(const BeamIntegratorLoader &otro)
  {
    Loader::operator=(otro);
    std::cerr << "No se debe asignar este objeto." << std::endl;
    return *this;
  }

XC::BeamIntegratorLoader::~BeamIntegratorLoader(void)
  { libera(); }

//! @brief Devuelve una referencia al mapa de beam_integrators.
const XC::BeamIntegratorLoader::map_beam_integrators &XC::BeamIntegratorLoader::Map(void) const
  { return beam_integrators; }

//! @brief Devuelve un iterador apuntando al principio de la lista.
XC::BeamIntegratorLoader::const_iterator XC::BeamIntegratorLoader::begin(void) const
  { return beam_integrators.begin(); }
//! @brief Devuelve un iterador apuntando después del final de la lista.
XC::BeamIntegratorLoader::const_iterator XC::BeamIntegratorLoader::end(void) const
  { return beam_integrators.end(); }
//! @brief Devuelve un iterador apuntando al principio de la lista.
XC::BeamIntegratorLoader::iterator XC::BeamIntegratorLoader::begin(void)
  { return beam_integrators.begin(); }
//! @brief Devuelve un iterador apuntando después del final de la lista.
XC::BeamIntegratorLoader::iterator XC::BeamIntegratorLoader::end(void)
  { return beam_integrators.end(); }

//! @brief Si encuentra el BeamIntegrator cuyo nombre se pasa como parámetro devuelve un iterador apuntando al mismo.
XC::BeamIntegratorLoader::const_iterator XC::BeamIntegratorLoader::find(const std::string &nmb) const
  { return beam_integrators.find(nmb); }

//! @brief Si encuentra el BeamIntegrator cuyo nombre se pasa como parámetro devuelve un iterador apuntando al mismo.
XC::BeamIntegratorLoader::iterator XC::BeamIntegratorLoader::find(const std::string &nmb)
  { return beam_integrators.find(nmb); }

//! @brief Si encuentra el BeamIntegrator cuyo nombre se pasa como parámetro devuelve un puntero al mismo,
//! en otro caso devuelve nullptr.
XC::BeamIntegration *XC::BeamIntegratorLoader::find_ptr(const std::string &nmb)
  {
    XC::BeamIntegratorLoader::iterator i= find(nmb);
    if(i!= beam_integrators.end())
      return (*i).second;
    else
      return nullptr; 
  }

//! @brief Si encuentra el BeamIntegrator cuyo nombre se pasa como parámetro devuelve un puntero al mismo,
//! en otro caso devuelve nullptr.
const XC::BeamIntegration *XC::BeamIntegratorLoader::find_ptr(const std::string &nmb) const
  {
    XC::BeamIntegratorLoader::const_iterator i= find(nmb);
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

//! @brief Defines a new beam integrator scheme.
XC::BeamIntegration *XC::BeamIntegratorLoader::newBI(const std::string &cmd,const std::string &cod)
  {
    XC::BeamIntegration *retval= load_beam_integration(cmd);
    if(retval)
      {
        retval->set_owner(this);
        if(find(cod)!=end()) //Integrator exists.
          {
	    std::clog << "BeamIntegratorLoader::newBI; ¡warning! redefining: '"
                      << cod << "'." << std::endl;
            delete beam_integrators[cod];
          }
        beam_integrators[cod]= retval;
      }
    return retval;

    if(!retval)
      std::cerr << "Integration scheme: " << cmd << " unknown." << std::endl;
    return retval;
  }

//! @brief True if integrator named nmb exists.
bool XC::BeamIntegratorLoader::exists(const std::string &nmb) const
  { return (find(nmb)!=end()); }

//! @brief Returns a reference to the material named as in the parameter.
XC::BeamIntegration &XC::BeamIntegratorLoader::get(const std::string &nmb)
  {
    BeamIntegration *retval= find_ptr(nmb);
    assert(retval);
    return *retval;
  }
