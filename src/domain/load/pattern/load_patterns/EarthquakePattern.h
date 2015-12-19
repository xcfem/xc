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
                                                                        
// $Revision: 1.5 $
// $Date: 2005/02/17 01:15:27 $
// $Source: /usr/local/cvs/OpenSees/SRC/domain/load/pattern/EarthquakePattern.h,v $
                                                                        
                                                                        
#ifndef EarthquakePattern_h
#define EarthquakePattern_h

// File: ~/domain/load/pattern/EarthquakePattern.h
//
// Written: fmk 11/98
// Revised:
//
// Purpose: This file contains the class definition for EarthquakePattern.
// EarthquakePattern is an abstract class.

#include "EQBasePattern.h"

namespace XC {
class GroundMotion;
class Vector;

//! @ingroup LPatterns
//
//! @brief Pauta de cargas correspondiente a un terremoto.
class EarthquakePattern : public EQBasePattern
  {
  private:
    Vector *uDotG; //!< Velocidades.
    Vector *uDotDotG; //!< Aceleraciones.
    double currentTime;

// AddingSensitivity:BEGIN //////////////////////////////////////////
    int parameterID;
// AddingSensitivity:END ///////////////////////////////////////////
    EarthquakePattern(const EarthquakePattern &otro);
    EarthquakePattern &operator=(const EarthquakePattern &);
  protected:
    void libera(void);
    void alloc(const size_t &);
    void copia(const Vector *,const Vector *);
    int addMotion(GroundMotion &);
    int sendData(CommParameters &);
    int recvData(const CommParameters &);
  public:
    EarthquakePattern(int tag, int classTag);
    virtual ~EarthquakePattern(void);

    virtual void applyLoad(double time);
    virtual bool addSP_Constraint(SP_Constraint *);
    virtual bool addNodalLoad(NodalLoad *);
    virtual bool addElementalLoad(ElementalLoad *);

    // methods for o/p
    virtual int sendSelf(CommParameters &) =0;
    virtual int recvSelf(const CommParameters &) =0;

    virtual void Print(std::ostream &s, int flag =0);        

    // AddingSensitivity:BEGIN //////////////////////////////////////////
    virtual void applyLoadSensitivity(double pseudoTime = 0.0);
    virtual int  setParameter(const std::vector<std::string> &argv, Parameter &param);
    virtual int  updateParameter(int parameterID, Information &info);
    virtual int  activateParameter(int parameterID);
    // AddingSensitivity:END ///////////////////////////////////////////
    any_const_ptr GetProp(const std::string &cod) const;
  };
} // end of XC namespace

#endif
