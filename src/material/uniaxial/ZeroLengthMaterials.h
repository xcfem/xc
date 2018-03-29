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
//ZeroLengthMaterials.h
                                                                        
                                                                        
#ifndef ZeroLengthMaterials_h
#define ZeroLengthMaterials_h

#include <material/uniaxial/DqUniaxialMaterial.h>

namespace XC {
class UniaxialMaterial;
class ZeroLength;
class MaterialHandler;

//! @ingroup MatUnx
//
//! @brief A ZeroLengthElement is defined by two nodes
//! with the same coordinate.
//! One or more material objects may be associated with the nodes to
//! provide a force displacement relationship.
//! ZeroLengthMaterials will work with 1d, 2d, or 3d material models.
//! 
//! The force-deformation relationship for the element is given by a
//! pointer \p theMaterial to a {\bf UniaxialMaterial} model acting in
//! local \p direction.
//! The local \p direction is 1, 2, 3, for translation in the local x, y, z
//! axes or 4, 5, 6 for rotation about the local x, y, z axes. 
class ZeroLengthMaterials: public DqUniaxialMaterial
  {
  private:
    // private methods
    void checkDirection(void);
    
    // Storage for uniaxial material models
    std::deque<int> directions; //!< array of local directions 0-5 for 1d materials

  protected:
    MaterialHandler *get_material_handler(void);

  public:
    // Constructor for a single 1d material model
    ZeroLengthMaterials(ZeroLength *owner,UniaxialMaterial &theMaterial,int direction);
    ZeroLengthMaterials(ZeroLength *owner,UniaxialMaterial *theMaterial,int direction );
    // Constructor for a multiple 1d material models
    ZeroLengthMaterials(ZeroLength *owner,const DqUniaxialMaterial &theMaterial,const ID &direction);
    ZeroLengthMaterials(ZeroLength *owner= nullptr);
    void push_back(const int &dir,const UniaxialMaterial *);
    void push_front(const int &dir,const UniaxialMaterial *);

    inline const int &getDir(const size_t i) const
      { return directions[i]; }
    inline int &getDir(const size_t i)
      { return directions[i]; }

    int sendSelf(CommParameters &);
    int recvSelf(const CommParameters &);
    void Print(std::ostream &s, int flag =0);

    void clear(void);    


  };
} // end of XC namespace

#endif




