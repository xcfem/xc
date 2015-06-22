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
//UniaxialFiber.h

#ifndef UniaxialFiber_h
#define UniaxialFiber_h

#include "Fiber.h"

namespace XC {
class UniaxialMaterial;
class MaterialLoader;

//! \ingroup MATSCCFibras
//
//! @brief Representa una fibra de material uniaxial.
class UniaxialFiber: public Fiber
  {
    void libera(void);
    void alloc(const UniaxialMaterial &theMat);
  protected:
    UniaxialMaterial *theMaterial; //!< pointer to a material
    double area; //!< area of the fiber 

    int sendData(CommParameters &);
    int recvData(const CommParameters &);
    bool procesa_comando(CmdStatus &status);
  public:
    UniaxialFiber(int classTag);
    UniaxialFiber(int tag, int classTag,double Area);
    UniaxialFiber(int tag, int classTag,const UniaxialMaterial &,const double &);
    UniaxialFiber(int tag, int classTag,const MaterialLoader &,const std::string &,const double &);
    UniaxialFiber(const UniaxialFiber &);
    UniaxialFiber &operator=(const UniaxialFiber &);
    ~UniaxialFiber(void);

    void setMaterial(const UniaxialMaterial *theMat);
    void setMaterial(const MaterialLoader &ldr,const std::string &nmbMat);

    int commitState(void);
    int revertToLastCommit(void);    
    int revertToStart(void);
    
    inline UniaxialMaterial *getMaterial(void)
      { return theMaterial; }
    inline const UniaxialMaterial *getMaterial(void) const
      { return theMaterial; }
    inline double getArea(void) const
      { return area; }
  };
} // fin namespace XC


#endif






