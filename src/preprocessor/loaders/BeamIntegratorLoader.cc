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
    integradores.erase(begin(),end());
  }

//! @brief Constructor por defecto.
XC::BeamIntegratorLoader::BeamIntegratorLoader(Preprocessor *owr)
  : Loader(owr)
  {
    LobattoBeamIntegration *ptr1= new LobattoBeamIntegration();
    integradores["lobatto"]= ptr1;
    LegendreBeamIntegration *ptr2= new LegendreBeamIntegration();
    integradores["legendre"]= ptr2;
    RadauBeamIntegration *ptr3= new RadauBeamIntegration();
    integradores["radau"]= ptr3;
    NewtonCotesBeamIntegration *ptr4= new NewtonCotesBeamIntegration();
    integradores["newtoncotes"]= ptr4;
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

//! @brief Devuelve una referencia al mapa de integradores.
const XC::BeamIntegratorLoader::map_integradores &XC::BeamIntegratorLoader::Map(void) const
  { return integradores; }

//! @brief Devuelve un iterador apuntando al principio de la lista.
XC::BeamIntegratorLoader::const_iterator XC::BeamIntegratorLoader::begin(void) const
  { return integradores.begin(); }
//! @brief Devuelve un iterador apuntando después del final de la lista.
XC::BeamIntegratorLoader::const_iterator XC::BeamIntegratorLoader::end(void) const
  { return integradores.end(); }
//! @brief Devuelve un iterador apuntando al principio de la lista.
XC::BeamIntegratorLoader::iterator XC::BeamIntegratorLoader::begin(void)
  { return integradores.begin(); }
//! @brief Devuelve un iterador apuntando después del final de la lista.
XC::BeamIntegratorLoader::iterator XC::BeamIntegratorLoader::end(void)
  { return integradores.end(); }

//! @brief Si encuentra el BeamIntegrator cuyo nombre se pasa como parámetro devuelve un iterador apuntando al mismo.
XC::BeamIntegratorLoader::const_iterator XC::BeamIntegratorLoader::find(const std::string &nmb) const
  { return integradores.find(nmb); }

//! @brief Si encuentra el BeamIntegrator cuyo nombre se pasa como parámetro devuelve un iterador apuntando al mismo.
XC::BeamIntegratorLoader::iterator XC::BeamIntegratorLoader::find(const std::string &nmb)
  { return integradores.find(nmb); }

//! @brief Si encuentra el BeamIntegrator cuyo nombre se pasa como parámetro devuelve un puntero al mismo,
//! en otro caso devuelve nullptr.
XC::BeamIntegration *XC::BeamIntegratorLoader::find_ptr(const std::string &nmb)
  {
    XC::BeamIntegratorLoader::iterator i= find(nmb);
    if(i!= integradores.end())
      return (*i).second;
    else
      return nullptr; 
  }

//! @brief Si encuentra el BeamIntegrator cuyo nombre se pasa como parámetro devuelve un puntero al mismo,
//! en otro caso devuelve nullptr.
const XC::BeamIntegration *XC::BeamIntegratorLoader::find_ptr(const std::string &nmb) const
  {
    XC::BeamIntegratorLoader::const_iterator i= find(nmb);
    if(i!= integradores.end())
      return (*i).second;
    else
      return nullptr; 
  }

