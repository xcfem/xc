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
//ZeroLengthMaterials.h
                                                                        
                                                                        
#ifndef ZeroLengthMaterials_h
#define ZeroLengthMaterials_h

#include <material/uniaxial/DqUniaxialMaterial.h>

namespace XC {
class UniaxialMaterial;
class ZeroLength;
class MaterialLoader;

//! @ingroup MatUnx
//
//! @brief A ZeroLengthElement is defined by two nodes
//! with the same coordinate.
//! One or more material objects may be associated with the nodes to
//! provide a force displacement relationship.
//! ZeroLengthMaterials will work with 1d, 2d, or 3d material models.
class ZeroLengthMaterials: public DqUniaxialMaterial
  {
  private:
    // private methods
    void checkDirection(void);
    
    // Storage for uniaxial material models
    std::deque<int> direcciones; //!< array of directions 0-5 for 1d materials

  protected:
    MaterialLoader *get_material_loader(void);
    bool procesa_comando(CmdStatus &status);
  public:
    // Constructor for a single 1d material model
    ZeroLengthMaterials(ZeroLength *owner,UniaxialMaterial &theMaterial,int direction);
    ZeroLengthMaterials(ZeroLength *owner,UniaxialMaterial *theMaterial,int direction );
    // Constructor for a multiple 1d material models
    ZeroLengthMaterials(ZeroLength *owner,const DqUniaxialMaterial &theMaterial,const ID &direction);
    ZeroLengthMaterials(ZeroLength *owner=NULL);
    void push_back(const int &dir,const UniaxialMaterial *);
    void push_front(const int &dir,const UniaxialMaterial *);

    inline const int &getDir(const size_t i) const
      { return direcciones[i]; }
    inline int &getDir(const size_t i)
      { return direcciones[i]; }

    int sendSelf(CommParameters &);
    int recvSelf(const CommParameters &);
    void Print(std::ostream &s, int flag =0);

    void clear(void);    

    any_const_ptr GetProp(const std::string &cod) const;
  };
} // end of XC namespace

#endif




