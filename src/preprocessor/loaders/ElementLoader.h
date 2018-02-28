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
//! @brief Element creation manager.
class ElementLoader: public ProtoElementLoader
  {
  public:
    class SeedElemLoader: public ProtoElementLoader
      {
        Element *semilla;
        SeedElemLoader(const SeedElemLoader &otro);
        SeedElemLoader &operator=(const SeedElemLoader &otro);
        void free_mem(void);
      protected:
        void add(Element *);
      public:
        SeedElemLoader(Preprocessor *preprocessor)
          : ProtoElementLoader(preprocessor), semilla(nullptr) {}
	Element *GetSeedElement(void)
          { return semilla; }
	const Element *GetSeedElement(void) const
          { return semilla; }
        int getDefaultTag(void) const;
        void clearAll(void);
        ~SeedElemLoader(void);
      };
  private:
    SeedElemLoader seed_elem_loader; //!< Seed element for meshing.
  protected:
    virtual void add(Element *);
  public:
    ElementLoader(Preprocessor *);
    Element *getElement(int tag);

    void new_element(Element *e);
    inline SeedElemLoader &getSeedElemLoader(void)
      { return seed_elem_loader; }
    const Element *get_seed_element(void) const
      { return seed_elem_loader.GetSeedElement(); }

    virtual void Add(Element *);

    int getDefaultTag(void) const;
    void setDefaultTag(const int &tag);

    void clearAll(void);
  };

} // end of XC namespace

#endif
