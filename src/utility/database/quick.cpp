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
                                                                        
// $Revision: 1.2 $
// $Date: 2003/02/14 23:00:54 $
// $Source: /usr/local/cvs/OpenSees/SRC/database/quick.cpp,v $
                                                                        
                                                                        
int XC::FileDatastore::sendID(int dataTag, int commitTag, const ID &theID, ChannelAddress *theAddress)
{

  // we first ensure that the ID is not too big
  int idSize = theID.Size();
  if (idSize >= maxIDsize) {
    std::cerr << "XC::FileDatastore::sendID() - the database does not deal with IDs of this size ";
    std::cerr << idSize << std::endl;
  }


  // open a file if not already opened
  if (ids[idSize] == 0) {
    char fileName[70];
    char intName[10];
    strcpy(fileName, dataBase);
    itoa(idSize, intName);
    strcat(fileName,".IDs.");
    strcat(fileName,intName);
    ids[idSize] = this->openFile(fileName);    
    int loc = ids[idSize]->tellg();
    if (loc == -1) loc = 0;
    std::cerr << "LOCATION: " << loc << std::endl;
    fileEnds.ids[idSize] = loc;
  }

  // we now found the location in the file to write the data
  fstream *theStream = ids[idSize];
  int fileEnd = fileEnds.ids[idSize];
  int stepSize = (2 + idSize)*sizeof(int);

  theStream->seekg(0);
  bool found = false;
  int pos =0;

  while ((pos < fileEnd) && (found == false)) {
    theStream->read((char *)&idBuffer, stepSize);
    if ((idBuffer.dbTag == dataTag) && (idBuffer.commitTag == commitTag)) 
      found = true;
    else {
      pos += stepSize;
    }
  }

  // we now place the data to be sent into our buffer
  idBuffer.dbTag = dataTag;
  idBuffer.commitTag = commitTag;
  for (int i=0; i<idSize; i++)
    idBuffer.data[i] = theID(i);

  // we now write the data
  if (found == true)
    theStream->seekp(pos);

  theStream->write((char *)&idBuffer, stepSize);

  // update the size of file if we have added to eof
  if (fileEnd <= pos)
    fileEnds.ids[idSize] += stepSize;

  return 0;
}		       

int XC::FileDatastore::recvID(int dataTag, int commitTag, ID &theID, ChannelAddress *theAddress)    
{
  // we first check ID not too big
  int idSize = theID.Size();
  if (idSize >= maxIDsize) {
    std::cerr << "XC::FileDatastore::recvID() - the database does not deal with IDs";
    std::cerr << " of this size "  << idSize << std::endl;
    return -1;
  }
  
  // open the file - if not already opened
  if (ids[idSize] == 0) {
    char fileName[70];
    char intName[10];
    strcpy(fileName, dataBase);
    itoa(idSize, intName);
    strcat(fileName,".IDs.");
    strcat(fileName,intName);
    ids[idSize] = this->openFile(fileName);    
    int loc = ids[idSize]->tellg();
    if (loc == -1) loc = 0;
    fileEnds.ids[idSize] = loc;
  }

  // we now read in the data unti we reach eof or find the data
  int stepSize = (2 + idSize)*sizeof(int);
  fstream *theStream = ids[idSize];

  theStream->seekg(0);
  bool found = false;
  int pos =0; 
  int fileEnd = fileEnds.ids[idSize];

  while ((pos < fileEnd) && (found == false)) {
    theStream->read((char *)&idBuffer, stepSize);
    if ((idBuffer.dbTag == dataTag) && (idBuffer.commitTag == commitTag))
      found = true;
    pos += stepSize;
  }
  
  if (found == false) {
    std::cerr << "XC::FileDatastore::recvID() - failed to find data for ID of size ";
    std::cerr << idSize << std::endl;
    return -1;
  }

  // we now place the received data into the ID 
  idBuffer.dbTag = dataTag;
  idBuffer.commitTag = commitTag;
  for (int i=0; i<idSize; i++)
    theID(i) = idBuffer.data[i];

  return 0;
}		       
