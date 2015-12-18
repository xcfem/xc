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
//NodeVectors
                                                                        
                                                                        
#ifndef NodeVectors_h
#define NodeVectors_h

#include "utility/actor/actor/MovableObject.h"
#include "xc_utils/src/nucleo/EntCmd.h"

namespace XC {

class Vector;
class Channel;
class FEM_ObjectBroker;

//! \ingroup Nod
//
//! @brief Vectores para almacenar los valores de prueba («trial»)
//! y los valores consumados («commited») del desplazamiento, velocidad, etc.
//! del nodo.
class NodeVectors: public EntCmd, public MovableObject
  {
  protected:
    size_t numVectors; //!< número de vectores.
    Vector *commitData; //!< commited quantities
    Vector *trialData; //!< trial quantities
    
    double *data; //!< double array holding the displ.

    DbTagData &getDbTagData(void) const;
    int sendData(CommParameters &);
    int recvData(const CommParameters &);
    int createData(const size_t &);
    void libera(void);
    void copia(const NodeVectors &);
    bool procesa_comando(CmdStatus &status);
  public:
    // constructors
    NodeVectors(const size_t &nv= 2);
    NodeVectors(const NodeVectors &otro);        
    NodeVectors &operator=(const NodeVectors &otro);
    virtual ~NodeVectors(void);

    // public methods dealing with the DOF at the node
    size_t getVectorsSize(void) const;

    // public methods for obtaining committed and trial 
    // response quantities of the node
    virtual const Vector &getData(const size_t &) const;
    virtual const Vector &getTrialData(const size_t &) const;
    const Vector &getTrialData(void) const;
    const Vector &getCommitData(void) const;

    // public methods for updating the trial response quantities
    virtual int setTrialData(const size_t &nDOF,const double &value,const size_t &dof);
    virtual int setTrialData(const size_t &nDOF,const Vector &);    
    virtual int incrTrialData(const size_t &nDOF,const Vector &);    
    
    // public methods dealing with the committed state of the node
    virtual int commitState(const size_t &nDOF);
    virtual int revertToLastCommit(const size_t &nDOF);    
    virtual int revertToStart(const size_t &nDOF);        

    virtual int sendSelf(CommParameters &);
    virtual int recvSelf(const CommParameters &);
    any_const_ptr GetProp(const std::string &cod) const;
  };

} // end of XC namespace

#endif

