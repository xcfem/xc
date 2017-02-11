//----------------------------------------------------------------------------
//  XC program; finite element analysis code
//  for structural analysis and design.
//
//  Copyright (C)  Luis Claudio Pérez Tato
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
//ElementLoader.h

#ifndef ELEMENTLOADER_H
#define ELEMENTLOADER_H

#include "preprocessor/loaders/ProtoElementLoader.h"

namespace XC {

//!  \ingroup Ldrs
//! 
//! @brief Objeto encargado de agregar al domain los elementos
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
      public:
        SeedElemLoader(Preprocessor *preprocessor)
          : ProtoElementLoader(preprocessor), semilla(NULL) {}
	Element *GetElementoSemilla(void)
          { return semilla; }
	const Element *GetElementoSemilla(void) const
          { return semilla; }
        int getDefaultTag(void) const;
        void clearAll(void);
        ~SeedElemLoader(void);
      };
  private:
    SeedElemLoader seed_elem_loader; //!< Seed element for meshing.
  protected:
    virtual void agrega(Element *);
  public:
    ElementLoader(Preprocessor *);
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
  };

} // end of XC namespace

#endif
