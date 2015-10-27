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
//ProtoElementLoader.h

#ifndef PROTOELEMENTLOADER_H
#define PROTOELEMENTLOADER_H

#include "Loader.h"
#include "MaterialLoader.h"
#include "TransfCooLoader.h"
#include "BeamIntegratorLoader.h"
#include "xc_utils/src/base/CmdStatus.h"
#include <map>

namespace XC {
class Element;

//!  \ingroup Ldrs
//! 
//! @brief Objeto encargado de leer elementos finitos desde el archivo de entrada.
//!
//! Se emplea tanto para cargar directamente los elementos del dominio como
//! para cargar la definición del elemento semilla que se empleará en las rutinas
//! de mallado semi-automático. Con este fin se redefine el método agrega en las
//! clases hijas.
class ProtoElementLoader: public Loader
  {
    Element *crea_elemento(const std::string &cmd,int tag_elem);

    std::string nmb_mat; //!< En su caso, nombre del material para el nuevo elemento.
    int num_sec; //!< En su caso, número de secciones (puntos de integración) en elementos lineales.
    int dim_elem; //!< Dimensión, en su caso, para el nuevo elemento. 
    std::string nmb_transf; //!< En su caso, nombre de la transformación para el nuevo elemento.
    std::string nmb_integ; //!< En su caso, nombre del integrador para el nuevo elemento.
    int dir; //!< En su caso (elementos de longitud cero), dirección en la que actúa el material del elemento.
  protected:
    virtual void agrega(Element *)= 0;
    MaterialLoader &get_material_loader(void) const;
    MaterialLoader::const_iterator get_iter_material(void) const;
    Material *get_ptr_material(void) const;
    TransfCooLoader &get_transf_coo_loader(void) const;
    TransfCooLoader::const_iterator get_iter_transf_coo(void) const;
    CrdTransf *get_ptr_transf_coo(void) const;
    BeamIntegratorLoader &get_beam_integrator_loader(void) const;
    BeamIntegratorLoader::const_iterator get_iter_beam_integrator(void) const;
    BeamIntegration *get_ptr_beam_integrator(void) const;

    friend class Preprocessor;

    void ejecuta_bloque_for_each(CmdStatus &,const std::string &);
  public:
    ProtoElementLoader(Preprocessor *preprocessor);
    virtual int getDefaultTag(void) const= 0;

    void setDefaultMaterial(const std::string &);
    const std::string &getDefaultMaterial(void) const;
    void setNumSections(const int &);
    int getNumSections(void) const;
    void setDimElem(const int &);
    int getDimElem(void) const;
    void setDefaultTransf(const std::string &);
    const std::string &getDefaultTransf(void) const;
    void setDefaultIntegrator(const std::string &);
    const std::string &getDefaultIntegrator(void) const;

    Element *nuevoElemento(const std::string &,const ID &);

  };

} // fin namespace XC

#endif
