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
//ProtoElementLoader.cc

#include "ProtoElementLoader.h"
#include "crea_elem.h"
#include "xc_utils/src/base/CmdStatus.h"

#include "domain/mesh/element/truss_beam_column/truss/CorotTruss.h"
#include "domain/mesh/element/truss_beam_column/truss/CorotTrussSection.h"
#include "domain/mesh/element/truss_beam_column/truss/Truss.h"
#include "domain/mesh/element/truss_beam_column/truss/TrussSection.h"
#include "domain/mesh/element/truss_beam_column/truss/Muelle.h"

#include "domain/mesh/element/truss_beam_column/beam2d/beam2d02.h"
#include "domain/mesh/element/truss_beam_column/beam2d/beam2d03.h"
#include "domain/mesh/element/truss_beam_column/beam2d/beam2d04.h"
#include "domain/mesh/element/truss_beam_column/beam3d/beam3d01.h"
#include "domain/mesh/element/truss_beam_column/beam3d/beam3d02.h"
#include "domain/mesh/element/truss_beam_column/beamWithHinges/BeamWithHinges2d.h"
#include "domain/mesh/element/truss_beam_column/beamWithHinges/BeamWithHinges3d.h"
#include "domain/mesh/element/truss_beam_column/elasticBeamColumn/ElasticBeam2d.h"
#include "domain/mesh/element/truss_beam_column/elasticBeamColumn/ElasticBeam3d.h"
#include "domain/mesh/element/truss_beam_column/forceBeamColumn/ForceBeamColumn2d.h"
#include "domain/mesh/element/truss_beam_column/forceBeamColumn/ForceBeamColumn3d.h"
#include "domain/mesh/element/truss_beam_column/dispBeamColumn/DispBeamColumn2d.h"
#include "domain/mesh/element/truss_beam_column/dispBeamColumn/DispBeamColumn3d.h"
#include "domain/mesh/element/truss_beam_column/nonlinearBeamColumn/element/NLBeamColumn2d.h"
#include "domain/mesh/element/truss_beam_column/nonlinearBeamColumn/element/NLBeamColumn3d.h"
#include "domain/mesh/element/truss_beam_column/updatedLagrangianBeamColumn/UpdatedLagrangianBeam2D.h"

#include "domain/mesh/element/plane/fourNodeQuad/ConstantPressureVolumeQuad.h"
#include "domain/mesh/element/plane/fourNodeQuad/EnhancedQuad.h"
#include "domain/mesh/element/plane/fourNodeQuad/FourNodeQuad.h"
#include "domain/mesh/element/plane/triangle/Tri31.h"
#include "domain/mesh/element/plane/fourNodeQuad/NineNodeMixedQuad.h"
#include "domain/mesh/element/plane/UP-ucsd/FourNodeQuadUP.h"
#include "domain/mesh/element/plane/UP-ucsd/NineFourNodeQuadUP.h"

#include "domain/mesh/element/volumen/brick/BbarBrick.h"
#include "domain/mesh/element/volumen/brick/Brick.h"
#include "domain/mesh/element/volumen/8nbrick/EightNodeBrick.h"
#include "domain/mesh/element/volumen/20nbrick/TwentyNodeBrick.h"
#include "domain/mesh/element/volumen/20nbrick/Twenty_Node_Brick.h"
#include "domain/mesh/element/volumen/27nbrick/TwentySevenNodeBrick.h"
#include "domain/mesh/element/volumen/TotalLagrangianFD20NodeBrick/TotalLagrangianFD20NodeBrick.h"
#include "domain/mesh/element/volumen/upU/EightNodeBrick_u_p_U.h"
#include "domain/mesh/element/volumen/upU/TwentyNodeBrick_u_p_U.h"
#include "domain/mesh/element/volumen/UP-ucsd/BrickUP.h"
#include "domain/mesh/element/volumen/UP-ucsd/TwentyEightNodeBrickUP.h"

#include "domain/mesh/element/joint/BeamColumnJoint2d.h"
#include "domain/mesh/element/joint/BeamColumnJoint3d.h"
#include "domain/mesh/element/joint/Joint2D.h"
#include "domain/mesh/element/joint/Joint3D.h"

#include "domain/mesh/element/plane/shell/ShellMITC4.h"
#include "domain/mesh/element/plane/shell/CorotShellMITC4.h"
#include "domain/mesh/element/plane/shell/ShellNL.h"

#include "domain/mesh/element/zeroLength/ZeroLength.h"
#include "domain/mesh/element/zeroLength/ZeroLengthSection.h"
#include "domain/mesh/element/zeroLength/ZeroLengthContact2D.h"
#include "domain/mesh/element/zeroLength/ZeroLengthContact3D.h"

#include "preprocessor/Preprocessor.h"
#include "xc_utils/src/base/any_const_ptr.h"

//! @brief Constructor por defecto.
XC::ProtoElementLoader::ProtoElementLoader(Preprocessor *preprocessor)
  : Loader(preprocessor), nmb_mat("nil"), num_sec(3), dim_elem(0), nmb_transf("nil"), nmb_integ("lobatto"), dir(0)
 {}

//! @brief Devuelve una referencia al cargador de materiales.
XC::MaterialLoader &XC::ProtoElementLoader::get_material_loader(void) const
  { return preprocessor->getMaterialLoader(); }

//! @brief Devuelve un iterador al material que se especifica en nmb_mat.
XC::MaterialLoader::const_iterator XC::ProtoElementLoader::get_iter_material(void) const
  { return preprocessor->getMaterialLoader().find(nmb_mat); }

//! @brief Devuelve un apuntador al material que se especifica en nmb_mat.
XC::Material *XC::ProtoElementLoader::get_ptr_material(void) const
  {
    Material *retval= nullptr;
    MaterialLoader::const_iterator imat= get_iter_material();
    if(imat!= get_material_loader().end())
      retval= imat->second;
    else //if(verborrea>0)
      std::cerr << "ProtoElementLoader - material: '" 
                << nmb_mat << "' not found.\n";
    return retval;
  }

//! @brief Devuelve una referencia al cargador de integradores.
XC::BeamIntegratorLoader &XC::ProtoElementLoader::get_beam_integrator_loader(void) const
  { return preprocessor->getBeamIntegratorLoader(); }

//! @brief Devuelve un iterador al integrador que se especifica en nmb_integ.
XC::BeamIntegratorLoader::const_iterator XC::ProtoElementLoader::get_iter_beam_integrator(void) const
  { return preprocessor->getBeamIntegratorLoader().find(nmb_integ); }


//! @brief Devuelve un apuntador al integrador que se especifica en nmb_integ.
XC::BeamIntegration *XC::ProtoElementLoader::get_ptr_beam_integrator(void) const
  {
    BeamIntegration *retval= nullptr;
    BeamIntegratorLoader::const_iterator iInteg= get_iter_beam_integrator();
    if(iInteg!= get_beam_integrator_loader().end())
      retval= iInteg->second;
    else
      if(verborrea>0)
        std::cerr << "ProtoElementLoader - no se encontró el integrador: '" 
                  << nmb_integ << "'.\n";
    return retval;
  }

//! @brief Devuelve una referencia al cargador de transformaciones de coordenadas.
XC::TransfCooLoader &XC::ProtoElementLoader::get_transf_coo_loader(void) const
  { return preprocessor->getTransfCooLoader(); }

//! @brief Devuelve un iterador a la transformacion que se especifica en nmb_transf.
XC::TransfCooLoader::const_iterator XC::ProtoElementLoader::get_iter_transf_coo(void) const
  { return preprocessor->getTransfCooLoader().find(nmb_transf); }

//! @brief Devuelve un apuntador a la transformación que se especifica en nmb_transf.
XC::CrdTransf *XC::ProtoElementLoader::get_ptr_transf_coo(void) const
  {
    CrdTransf *retval= nullptr;
    TransfCooLoader::const_iterator itrf= get_iter_transf_coo();
    if(itrf!= this->get_transf_coo_loader().end())
      retval= itrf->second;
    else
      if(verborrea>0)
        std::cerr << "ProtoElementLoader::get_ptr_transf_coo - no se encontró la transformación de coordenadas: '" 
                  << nmb_transf << "'.\n";
    return retval;
  }
//! @brief Procesa los comandos que se emplean para definir
//! los elementos del modelo de elementos finitos. Interpreta
//! los siguientes comandos:
//!
//! - truss[tag]: Define un elemento de tipo barra articulada (Truss). Si no
//!   se especifica el identificador (tag) se le asignará uno por defecto.
//! - truss_section[tag]: Define un elemento de tipo barra articulada (TrussSection) con material de
//!   tipo sección. Si no se especifica el identificador (tag) se le asignará uno por defecto.
//! - corot_truss[tag]: Define un elemento de tipo barra articulada (CorotTruss) con formulación
//!   corrotacional. Si no se especifica el identificador (tag) se le asignará uno por defecto.
//! - corot_truss_section[tag]: Define un elemento de tipo barra articulada (CorotTrussSection)
//!   con formulación  corrotacional y material de tipo sección. Si no se especifica el
//!   identificador (tag) se le asignará uno por defecto.
//! - beam2d_02[tag]: Define un elemento de tipo barra (beam2d02) para problemas planos. Si no
//!   se especifica el identificador (tag) se le asignará uno por defecto.
//! - beam2d_03[tag]: Define un elemento de tipo barra (beam2d03) para problemas planos. Si no
//!   se especifica el identificador (tag) se le asignará uno por defecto.
//! - beam2d_04[tag]: Define un elemento de tipo barra (beam2d04) para problemas planos. Si no
//!   se especifica el identificador (tag) se le asignará uno por defecto.
//! - beam3d_01[tag]: Define un elemento de tipo barra (beam3d01) para problemas tridimensionales. Si no
//!   se especifica el identificador (tag) se le asignará uno por defecto.
//! - beam3d_02[tag]: Define un elemento de tipo barra (beam2d02) para problemas tridimensionales. Si no
//!   se especifica el identificador (tag) se le asignará uno por defecto.
//! - elastic_beam2d[tag]: Define un elemento de tipo barra (ElasticBeam2d) para problemas planos. Si no
//!   se especifica el identificador (tag) se le asignará uno por defecto.
//! - elastic_beam3d[tag]: Define un elemento de tipo barra (ElasticBeam3d) para problemas tridimensionales. Si no
//!   se especifica el identificador (tag) se le asignará uno por defecto.
//! - force_beam_column_2d[tag]: Define un elemento de tipo barra (ForceBeamColumn2d) no lineal,
//!   para problemas planos. Si no se especifica el identificador (tag) se le asignará uno por defecto.
//! - force_beam_column_3d[tag]: Define un elemento de tipo barra (ForceBeamColumn3d) no lineal,
//!   para problemas tridimensionales. Si no se especifica el identificador (tag) se le asignará uno por defecto.
//! - shell_mitc4[tag]: Define un elemento de tipo shell (ShellMITC4),
//! - corot_shell_mitc4[tag]: Define un elemento de tipo shell con formulación corrotacional (CorotShellMITC4),
//! - shell_nl[tag]: Define un elemento de tipo shell (ShellNL),
//!   para problemas de láminas. Si no se especifica el identificador (tag) se le asignará uno por defecto.
//! - quad4n[tag]: Define un elemento cuadrilátero de cuatro nodos (FourNodeQuad),
//! - tri31[tag]: Define un elemento triangular de tres nodos y un sólo punto de gauss (Tri31),
//!   para problemas planos. Si no se especifica el identificador (tag) se le asignará uno por defecto.
//! - brick[tag]: Define un elemento hexaédrico de ocho nodos (Brick),
//!   para análisis de sólidos. Si no se especifica el identificador (tag) se le asignará uno por defecto.
//! - zero_length[tag]: Define un elemento de dimensión cero (ZeroLength).
//!   Si no se especifica el identificador (tag) se le asignará uno por defecto.
//! - zero_length_section[tag]: Define un elemento de dimensión cero (ZeroLengthSection) con
//!   material de tipo sección. Si no se especifica el identificador (tag) se le asignará uno por defecto.
XC::Element *XC::ProtoElementLoader::crea_elemento(const std::string &cmd,int tag_elem)
  {
    Element *retval= nullptr;
    if(cmd == "truss")
      retval= nuevo_elemento_dim_gen_mat<Truss>(tag_elem, dim_elem, get_ptr_material());
    else if(cmd == "truss_section")
      retval= nuevo_elemento_dim_gen_mat<TrussSection>(tag_elem, dim_elem, get_ptr_material());
    else if(cmd == "corot_truss")
      retval= nuevo_elemento_dim_gen_mat<CorotTruss>(tag_elem, dim_elem, get_ptr_material());
    else if(cmd == "corot_truss_section")
      retval= nuevo_elemento_dim_gen_mat<CorotTrussSection>(tag_elem, dim_elem, get_ptr_material());
    else if(cmd == "muelle")
      retval= nuevo_elemento_dim_gen_mat<Muelle>(tag_elem, dim_elem, get_ptr_material());
    else if(cmd == "beam2d_02")
      retval= nuevo_elemento<beam2d02>(tag_elem);
    else if(cmd == "beam2d_03")
      retval= nuevo_elemento<beam2d03>(tag_elem);
    else if(cmd == "beam2d_04")
      retval= nuevo_elemento<beam2d04>(tag_elem);
    else if(cmd == "beam3d_01")
      retval= nuevo_elemento<beam3d01>(tag_elem);
    else if(cmd == "beam3d_02")
      retval= nuevo_elemento<beam3d02>(tag_elem);
    else if(cmd == "elastic_beam_2d")
      retval= nuevo_elemento_mat_crd<ElasticBeam2d>(tag_elem, get_ptr_material(), get_ptr_transf_coo());
    else if(cmd == "elastic_beam_3d")
      retval= nuevo_elemento_mat_crd<ElasticBeam3d>(tag_elem, get_ptr_material(), get_ptr_transf_coo());
    else if(cmd == "beam_with_hinges_2d")
      retval= nuevo_elemento_gen_mat_crd<BeamWithHinges2d>(tag_elem, get_ptr_material(), get_ptr_transf_coo());
    else if(cmd == "beam_with_hinges_3d")
      retval= nuevo_elemento_gen_mat_crd<BeamWithHinges3d>(tag_elem, get_ptr_material(), get_ptr_transf_coo());
    else if(cmd == "disp_beam_column_2d")
      retval= nuevo_elemento_dim_gen_mat_crd<DispBeamColumn2d>(tag_elem, dim_elem, get_ptr_material(), get_ptr_transf_coo());
    else if(cmd == "disp_beam_column_3d")
      retval= nuevo_elemento_dim_gen_mat_crd<DispBeamColumn3d>(tag_elem, dim_elem, get_ptr_material(), get_ptr_transf_coo());
    else if(cmd == "nl_beam_column_2d")
      retval= nuevo_elemento_ns_gen_mat_crd<NLBeamColumn2d>(tag_elem, get_ptr_material(), num_sec, get_ptr_transf_coo());
    else if(cmd == "nl_beam_column_3d")
      retval= nuevo_elemento_ns_gen_mat_crd<NLBeamColumn3d>(tag_elem, get_ptr_material(), num_sec, get_ptr_transf_coo());
    else if(cmd == "force_beam_column_2d")
      retval= nuevo_elemento_ns_gen_mat_crd_integ<ForceBeamColumn2d>(tag_elem, get_ptr_material(), num_sec, get_ptr_transf_coo(),get_ptr_beam_integrator());
    else if(cmd == "force_beam_column_3d")
      retval= nuevo_elemento_ns_gen_mat_crd_integ<ForceBeamColumn3d>(tag_elem, get_ptr_material(), num_sec, get_ptr_transf_coo(),get_ptr_beam_integrator());
    else if(cmd == "zero_length")
      retval= nuevo_elemento_dim_gen_mat_dir<ZeroLength>(tag_elem, dim_elem, get_ptr_material(),dir);
    else if(cmd == "zero_length_contact_2d")
      retval= nuevo_elemento<ZeroLengthContact2D>(tag_elem);
    else if(cmd == "zero_length_contact_3d")
      retval= nuevo_elemento<ZeroLengthContact3D>(tag_elem);
    else if(cmd == "zero_length_section")
      retval= nuevo_elemento_dim_gen_mat<ZeroLengthSection>(tag_elem, dim_elem, get_ptr_material());
    else if(cmd == "shell_mitc4")
      {
        retval= nuevo_elemento_mat<ShellMITC4,SectionForceDeformation>(tag_elem, get_ptr_material());
        if(!retval)
          std::cerr << "Error en ProtoElementLoader::nuevo_elemento; el material: '"
                    << nmb_mat << "' no es del tipo adecuado." << std::endl;
      }
    else if(cmd == "corot_shell_mitc4")
      {
        retval= nuevo_elemento_mat<CorotShellMITC4,SectionForceDeformation>(tag_elem, get_ptr_material());
        if(!retval)
          std::cerr << "Error en ProtoElementLoader::nuevo_elemento; el material: '"
                    << nmb_mat << "' no es del tipo adecuado." << std::endl;
      }
    else if(cmd == "shell_nl")
      {
        retval= nuevo_elemento_mat<ShellNL,SectionForceDeformation>(tag_elem, get_ptr_material());
        if(!retval)
          std::cerr << "Error en ProtoElementLoader::nuevo_elemento; el material: '"
                    << nmb_mat << "' no es del tipo adecuado." << std::endl;
      }
    else if(cmd == "quad4n")
      {
        retval= nuevo_elemento_mat<FourNodeQuad,NDMaterial>(tag_elem, get_ptr_material());
        if(!retval)
          std::cerr << "Error en ProtoElementLoader::nuevo_elemento; el material: '"
                    << nmb_mat << "' no es del tipo adecuado." << std::endl;
      }
    else if(cmd == "tri31")
      {
        retval= nuevo_elemento_mat<Tri31,NDMaterial>(tag_elem, get_ptr_material());
        if(!retval)
          std::cerr << "Error en ProtoElementLoader::nuevo_elemento; el material: '"
                    << nmb_mat << "' no es del tipo adecuado." << std::endl;
      }
    else if(cmd == "brick")
      {
        retval= nuevo_elemento_mat<Brick,NDMaterial>(tag_elem, get_ptr_material());
        if(!retval)
          std::cerr << "Error en ProtoElementLoader::nuevo_elemento; el material: '"
                    << nmb_mat << "' no es del tipo adecuado." << std::endl;
      }
    else
      std::cerr << "Element type: " << cmd << " unknown." << std::endl;
    return retval;
  }

//! @brief Crea un nuevo elemento.
XC::Element *XC::ProtoElementLoader::nuevoElemento(const std::string &tipo,const ID &iNodos)
  {
    const int tag_elem= getDefaultTag();
    Element *retval= preprocessor->GetDominio()->getElement(tag_elem);
    if(!retval) //no existe (efectivament es nuevo).
      {
        retval= crea_elemento(tipo,tag_elem);
        if(retval)
          {
            retval->setIdNodos(iNodos);
            agrega(retval);
          }
      }
    else
      std::cerr << "ProtoElementLoader::nuevoElemento; ERROR el elemento: "
                << tag_elem << " ya existe.\n";
    return retval;
  }

//! @brief Especifica nombre del material que se asignará por defecto a los nuevos elementos.
void XC::ProtoElementLoader::setDefaultMaterial(const std::string &nmb)
  { nmb_mat= nmb; }

//! @brief Devuelve el nombre del material que se asignará por defecto a los nuevos elementos.
const std::string &XC::ProtoElementLoader::getDefaultMaterial(void) const
  { return nmb_mat; }

//! @brief Especifica el número de secciones que
//! se asignará por defecto a los nuevos elementos que lo requieran.
void XC::ProtoElementLoader::setNumSections(const int &ns)
  { num_sec= ns; }

//! @brief Devuelve el número de secciones que
//! se asignará por defecto a los nuevos elementos que lo requieran.
int XC::ProtoElementLoader::getNumSections(void) const
  { return num_sec; }

//! @brief Especifica la dimensión que
//! se asignará por defecto a los nuevos elementos que la requieran.
void XC::ProtoElementLoader::setDimElem(const int &dim)
  { dim_elem= dim; }

//! @brief Devuelve la dimensión que
//! se asignará por defecto a los nuevos elementos que la requieran.
int XC::ProtoElementLoader::getDimElem(void) const
  { return dim_elem; }

//! @brief Especifica nombre de la transformación que
//! se asignará por defecto a los nuevos elementos que la requieran.
void XC::ProtoElementLoader::setDefaultTransf(const std::string &nmb)
  { nmb_transf= nmb; }

//! @brief Devuelve el nombre de la transformación que se asignará por defecto
//! a los nuevos elementos que la requieran.
const std::string &XC::ProtoElementLoader::getDefaultTransf(void) const
  { return nmb_transf; }

//! @brief Especifica nombre del integrador que se asignará por
//! defecto a los nuevos elementos que lo requieran.
void XC::ProtoElementLoader::setDefaultIntegrator(const std::string &nmb)
  { nmb_integ= nmb; }

//! @brief Devuelve el nombre del integrador que se
//! asignará por defecto a los nuevos elementos que la requieran.
const std::string &XC::ProtoElementLoader::getDefaultIntegrator(void) const
  { return nmb_integ; }

