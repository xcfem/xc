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
//NDAdaptorMaterial.h

#ifndef NDAdaptorMaterial_h
#define NDAdaptorMaterial_h

#include <utility/matrix/Vector.h>
#include <material/nD/NDMaterial.h>

namespace XC {
//! @ingroup NDMat
//
//! @brief ??.
class NDAdaptorMaterial: public NDMaterial
  {
  protected:
    double Tstrain22;
    double Cstrain22;

    NDMaterial *theMaterial;
    Vector strain;
  protected:
    int sendData(CommParameters &);
    int recvData(const CommParameters &);
  public:
    NDAdaptorMaterial(int classTag,int tag, NDMaterial &theMat, int strain_size);
    NDAdaptorMaterial(int classTag,int tag, int strain_size);
    NDAdaptorMaterial(int classTag, int strain_size);
    virtual ~NDAdaptorMaterial(void);

    const Vector& getStrain(void);
    double getRho(void) const;

    int commitState(void);
    int revertToLastCommit(void);
    int revertToStart(void);

    void Print(std::ostream &s, int flag);

  };
} // end of XC namespace

#endif



