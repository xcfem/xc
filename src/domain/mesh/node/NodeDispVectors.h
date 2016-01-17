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
//NodeDispVectors
                                                                        
                                                                        
#ifndef NodeDispVectors_h
#define NodeDispVectors_h

#include "NodeVectors.h"

namespace XC {

//! \ingroup Nod
//
//! @brief Vectores para almacenar los valores de prueba
//! y los valores consumados («commited») del desplazamiento del nodo.
class NodeDispVectors: public NodeVectors
  {
  private:
    int createDisp(const size_t &);

    Vector *incrDisp;
    Vector *incrDeltaDisp;
  protected:
    void libera(void);
  public:
    // constructors
    NodeDispVectors(void);
    NodeDispVectors(const NodeDispVectors &otro);
    NodeDispVectors &operator=(const NodeDispVectors &otro);
    virtual ~NodeDispVectors(void);

    // public methods for obtaining committed and trial
    // response quantities of the node
    virtual const Vector &getIncrDisp(const size_t &) const;
    virtual const Vector &getIncrDeltaDisp(const size_t &) const;

    // public methods for updating the trial response quantities
    virtual int setTrialDispComponent(const size_t &nDOF,const double &value,const size_t &dof);
    virtual int setTrialDisp(const size_t &nDOF,const Vector &);
    virtual int incrTrialDisp(const size_t &nDOF,const Vector &);

    // public methods dealing with the committed state of the node
    virtual int commitState(const size_t &nDOF);
    virtual int revertToLastCommit(const size_t &nDOF);

    virtual void Print(std::ostream &s, int flag = 0);
  };

} // end of XC namespace

#endif

