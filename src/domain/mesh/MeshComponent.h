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
//MeshComponent.h                                                         
                                                                        
#ifndef MeshComponent_h
#define MeshComponent_h

#include "domain/component/ContinuaReprComponent.h"
#include "xc_basic/src/texto/LabelContainer.h"

class Pos3d;

namespace XC {
class Matrix;

//! @brief Clase base para nodos y elementos.
class MeshComponent: public ContinuaReprComponent
  {
  private:
    void check_matrices(const std::deque<Matrix> &matrices,const int &ndof) const;
    void nueva_matriz(std::deque<Matrix> &matrices,const int &ndof) const;
  protected:
    mutable int index;
    LabelContainer etiquetas;

    void setup_matrices(std::deque<Matrix> &matrices,const int &ndof) const;
    int sendIdsEtiquetas(int posDbTag,CommParameters &);
    int recvIdsEtiquetas(int posDbTag,const CommParameters &);
    int sendData(CommParameters &);
    int recvData(const CommParameters &);
    bool procesa_comando(CmdStatus &status);
  public:
    // constructors
    MeshComponent(int classTag);
    MeshComponent(int tag, int classTag);

    virtual any_const_ptr GetProp(const std::string &cod) const;
  };

} // fin namespace XC

#endif

