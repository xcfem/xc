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
** file 'COPYRIGHT'  in XC::main directory for information on usage and   **
** redistribution,  and for a DISCLAIMER OF ALL WARRANTIES.           **
**                                                                    **
** Developed by:                                                      **
**   Frank McKenna (fmckenna@ce.berkeley.edu)                         **
**   Gregory L. Fenves (fenves@ce.berkeley.edu)                       **
**   Filip C. Filippou (filippou@ce.berkeley.edu)                     **
**                                                                    **
** ****************************************************************** */
                                                                        
// $Revision: 1.5 $
// $Date: 2004/11/24 22:37:26 $
// $Source: /usr/local/cvs/OpenSees/SRC/handler/StandardStream.cpp,v $

#include <utility/handler/StandardStream.h>
#include <iostream>
#include <iomanip>
using std::cerr;
using std::ios;
using std::setiosflags;

XC::StandardStream::StandardStream()
  :fileOpen(0) {}
XC::StandardStream::~StandardStream()
  {
    if(fileOpen == 1)
      theFile.close();
  }

int XC::StandardStream::setFile(const char *fileName, openMode mode)
{
  if (fileOpen == 1) {
    theFile.close();
    fileOpen = 0;
  }

  if (mode == OVERWRITE) 
    theFile.open(fileName, ios::out);
  else
    theFile.open(fileName, ios::out| ios::app);

  if (theFile.bad()) {
    std::cerr << "WARNING - XC::StandardStream::setFile()";
    std::cerr << " - could not open file " << fileName << std::endl;

    return -1;
  } else
    fileOpen = 1;

  return 0;
}


int 
XC::StandardStream::setPrecision(int prec)
{
  cerr << std::setprecision(prec);

  if (fileOpen != 0)
    theFile << std::setprecision(prec);

  return 0;
}

int 
XC::StandardStream::setFloatField(floatField field)
{
  if (field == FIXEDD) {
	  cerr << setiosflags(ios::fixed);
    if (fileOpen != 0)
      theFile << setiosflags(ios::fixed);
  }
  else if (field == SCIENTIFIC) {
    cerr << setiosflags(ios::scientific);
    if (fileOpen != 0)
      theFile << setiosflags(ios::scientific);
  }

  return 0;
}



 XC::OPS_Stream& 
XC::StandardStream::write(const char *s,int n)
{
  cerr.write(s, n);

  if (fileOpen != 0)
    theFile.write(s, n);
  
  return *this;
}

 XC::OPS_Stream& 
XC::StandardStream::write(const unsigned char*s, int n)
{
  cerr.write((const char *) s, n);

  if (fileOpen != 0)
    theFile.write((const char *) s, n);

  return *this;
}
 XC::OPS_Stream& 
XC::StandardStream::write(const signed char*s, int n)
{
  cerr.write((const char *)s, n);

  if (fileOpen != 0)
    theFile.write((const char *) s, n);

  return *this;
}
 XC::OPS_Stream& 
XC::StandardStream::write(const void *s, int n)
{
  cerr.write((const char *)s, n);

  if (fileOpen != 0)
   theFile.write((const char *) s, n);

  return *this;
}
 XC::OPS_Stream& 
XC::StandardStream::operator<<(char c)
{
  cerr << c;

  if (fileOpen != 0)
    theFile << c;

 return *this;
}
 XC::OPS_Stream& 
XC::StandardStream::operator<<(unsigned char c)
{
  cerr << c;

  if (fileOpen != 0)
    theFile << c;

 return *this;
}
 XC::OPS_Stream& 
XC::StandardStream::operator<<(signed char c)
{

  cerr << c;

  if (fileOpen != 0)
    theFile << c;

  return *this;
}
 XC::OPS_Stream& 
XC::StandardStream::operator<<(const char *s)
{
  // note that we do the flush so that a "/n" before
  // a crash will cause a flush() - similar to what 
  cerr << s;
  cerr.flush();

  if (fileOpen != 0) {
    theFile << s;
    theFile.flush();
  }

  return *this;
}

 XC::OPS_Stream& 
XC::StandardStream::operator<<(const unsigned char *s)
{
  cerr << s;

  if (fileOpen != 0)
    theFile << s;

  return *this;
}
 XC::OPS_Stream& 
XC::StandardStream::operator<<(const signed char *s)
{
  cerr << s;

  if (fileOpen != 0)
    theFile << s;

  return *this;
}
 XC::OPS_Stream& 
XC::StandardStream::operator<<(const void *p)
{
/*
//  cerr << p;

  if (fileOpen != 0)
    theFile << p;
*/
  return *this;
}
 XC::OPS_Stream& 
XC::StandardStream::operator<<(int n)
{
  cerr <<  1.0*n;

  if (fileOpen != 0)
    theFile << 1.0*n;

  return *this;
}

 XC::OPS_Stream& 
XC::StandardStream::operator<<(unsigned int n)
{
  cerr << 1.0*n;

  if (fileOpen != 0)
    theFile << 1.0*n;

  return *this;
}
 XC::OPS_Stream& 
XC::StandardStream::operator<<(long n)
{
/*
cerr << n;

if (fileOpen != 0)
  theFile << n;
*/
  return *this;
}
 XC::OPS_Stream& 
XC::StandardStream::operator<<(unsigned long n)
{
/*
  cerr << n;

  if (fileOpen != 0)
    theFile << n;
*/
  return *this;
}
 XC::OPS_Stream& 
XC::StandardStream::operator<<(short n)
{
/*
  cerr << n;

  if (fileOpen != 0)
    theFile << n;
*/
  return *this;
}
 XC::OPS_Stream& 
XC::StandardStream::operator<<(unsigned short n)
{
/*
  cerr << n;

  if (fileOpen != 0)
    theFile << n;
*/
return *this;
}

 XC::OPS_Stream& 
XC::StandardStream::operator<<(bool b)
{
/*
  cerr << b;

  if (fileOpen != 0)
    theFile << b;
*/
 return *this;
}
 XC::OPS_Stream& 
XC::StandardStream::operator<<(double n)
{
  cerr << n;

  if (fileOpen != 0)
    theFile << n;

 return *this;
}
 XC::OPS_Stream& 
XC::StandardStream::operator<<(float n)
{
  cerr << n;

  if (fileOpen != 0)
    theFile << n;

 return *this;
}
