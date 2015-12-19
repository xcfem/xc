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
/* ****************************************************************** **
**    OpenSees - Open System for Earthquake Engineering Simulation    **
**          Pacific Earthquake Engineering Research Center            **
**                                                                    **
**                                                                    **
** (C) Copyright 1999, The Regents of the University of California    **
** All Rights Reserved.                                               **
**                                                                    **
** Commercial use of this program without express permission of the   **
** University of California, Berkeley, is strictly prohibited.  See   **
** file 'COPYRIGHT'  in main directory for information on usage and   **
** redistribution,  and for a DISCLAIMER OF ALL WARRANTIES.           **
**                                                                    **
** Developed by:                                                      **
**   Frank McKenna (fmckenna@ce.berkeley.edu)                         **
**   Gregory L. Fenves (fenves@ce.berkeley.edu)                       **
**   Filip C. Filippou (filippou@ce.berkeley.edu)                     **
**                                                                    **
** ****************************************************************** */
                                                                        
// $Revision: 1.11 $
// $Date: 2003/02/25 23:32:47 $
// $Source: /usr/local/cvs/OpenSees/SRC/element/beamWithHinges/BeamWithHinges3d.h,v $

#ifndef BeamColumnWithSectionFDTrf3d_h
#define BeamColumnWithSectionFDTrf3d_h

#include <domain/mesh/element/truss_beam_column/BeamColumnWithSectionFD.h>

namespace XC {
class Node;

class CrdTransf;
class CrdTransf3d;

//! \ingroup ElemBarra
//
//! @brief Elemento barra con material de tipo SeccionBarraPrismatica para problemas tridimensionales.
class BeamColumnWithSectionFDTrf3d: public BeamColumnWithSectionFD
  {
    BeamColumnWithSectionFDTrf3d &operator=(const BeamColumnWithSectionFDTrf3d &);
  protected:
    CrdTransf3d *theCoordTransf;

    int sendData(CommParameters &cp);
    int recvData(const CommParameters &cp);

    void set_transf(const CrdTransf *trf);
    bool procesa_comando(CmdStatus &status);
  public:
    BeamColumnWithSectionFDTrf3d(int tag,int classTag,int numSec);
    BeamColumnWithSectionFDTrf3d(int tag,int classTag,int numSec,const Material *mat,const CrdTransf *trf);
    BeamColumnWithSectionFDTrf3d(int tag, int classTag,int numSec, int nodeI, int nodeJ,CrdTransf3d &coordTransf);
    BeamColumnWithSectionFDTrf3d(const BeamColumnWithSectionFDTrf3d &);
    ~BeamColumnWithSectionFDTrf3d(void);
    void initialize_trf(void);
    virtual CrdTransf *getCoordTransf(void);
    virtual const CrdTransf *getCoordTransf(void) const;

    Vector getVDirEjeFuerteLocales(const size_t &i) const;
    Vector getVDirEjeDebilLocales(const size_t &i) const;
    double getAnguloEjeFuerte(const size_t &i) const;
    double getAnguloEjeDebil(const size_t &i) const;
    const Vector &getVDirEjeFuerteGlobales(const size_t &i) const;
    const Vector &getVDirEjeDebilGlobales(const size_t &i) const;

    virtual any_const_ptr GetProp(const std::string &cod) const;
  };
} // end of XC namespace

#endif
