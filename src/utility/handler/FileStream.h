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
                                                                        
// $Revision: 1.3 $
// $Date: 2004/11/13 00:53:14 $
// $Source: /usr/local/cvs/OpenSees/SRC/handler/FileStream.h,v $

#ifndef _FileStream
#define _FileStream

#include <utility/handler/OPS_Stream.h>

#include <fstream>
#include <string>

namespace XC {
class FileStream : public OPS_Stream
  {
  private:
    std::ofstream theFile;
    int fileOpen;
    std::string fileName;
  public:
    FileStream(void);
    virtual ~FileStream(void);


    //int setFile(const char *fileName, openMode mode = OVERWRITE);
    int setFile(const std::string &fileName, openMode mode = OVERWRITE);
    int open(void);
    int close(void);

    int setPrecision(int precision);
    int setFloatField(floatField);
    int precision(int precision) {return 0;};
    int width(int width) {return 0;};
    inline const std::string &getFileName(void) const
      {return fileName;}
    std::ostream &stream(void)
      { return theFile; }
   
    OPS_Stream& write(const char *s, int n);
    OPS_Stream& write(const unsigned char *s, int n);
    OPS_Stream& write(const signed char *s, int n);
    OPS_Stream& write(const void *s, int n);
    OPS_Stream& operator<<(char c);
    OPS_Stream& operator<<(unsigned char c);
    OPS_Stream& operator<<(signed char c);
    OPS_Stream& operator<<(const char *s);
    OPS_Stream& operator<<(const std::string &s);
    OPS_Stream& operator<<(const unsigned char *s);
    OPS_Stream& operator<<(const signed char *s);
    OPS_Stream& operator<<(const void *p);
    OPS_Stream& operator<<(int n);
    OPS_Stream& operator<<(unsigned int n);
    OPS_Stream& operator<<(long n);
    OPS_Stream& operator<<(unsigned long n);
    OPS_Stream& operator<<(short n);
    OPS_Stream& operator<<(unsigned short n);
    OPS_Stream& operator<<(bool b);
    OPS_Stream& operator<<(double n);
    OPS_Stream& operator<<(float n);
    // OPS_Stream& operator<<(__omanip func);
    // OPS_Stream& operator<<(__manip func);
    // OPS_Stream& operator<<(streambuf*);
    // OPS_Stream& ends(OPS_Stream& outs);
    // OPS_Stream& flush(OPS_Stream& outs);
    // OPS_Stream& (OPS_Stream& outs);
  };
} // fin namespace XC

#endif
