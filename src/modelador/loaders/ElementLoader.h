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
//ElementLoader.h

#ifndef ELEMENTLOADER_H
#define ELEMENTLOADER_H

#include "modelador/loaders/ProtoElementLoader.h"

namespace XC {

//!  \ingroup Ldrs
//! 
//! @brief Objeto encargado de agregar al dominio los elementos
//! finitos leídos desde archivo.
class ElementLoader: public ProtoElementLoader
  {
  public:
    class SeedElemLoader: public ProtoElementLoader
      {
        Element *semilla;
        SeedElemLoader(const SeedElemLoader &otro);
        SeedElemLoader &operator=(const SeedElemLoader &otro);
        void libera(void);
      protected:
        void agrega(Element *);
        bool procesa_comando(CmdStatus &);
      public:
        SeedElemLoader(Modelador *mdlr)
          : ProtoElementLoader(mdlr), semilla(NULL) {}
	Element *GetElementoSemilla(void)
          { return semilla; }
	const Element *GetElementoSemilla(void) const
          { return semilla; }
        int getDefaultTag(void) const;
        void clearAll(void);
        ~SeedElemLoader(void);
      };
  private:
    SeedElemLoader seed_elem_loader; //!< Elemento patrón para el mallado semi-automático.
  protected:
    virtual void agrega(Element *);
    bool procesa_comando(CmdStatus &status);
  public:
    ElementLoader(Modelador *);
    Element *getElement(int tag);

    void nuevo_elemento(Element *e);
    inline SeedElemLoader &getSeedElemLoader(void)
      { return seed_elem_loader; }
    const Element *get_elemento_semilla(void) const
      { return seed_elem_loader.GetElementoSemilla(); }

    virtual void Agrega(Element *);

    int getDefaultTag(void) const;
    void setDefaultTag(const int &tag);

    void clearAll(void);
    any_const_ptr GetProp(const std::string &cod) const;
  };

} // fin namespace XC

#endif
