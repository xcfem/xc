# -*- coding: utf-8 -*-

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2016,LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import math
from materials.sections import internal_forces
from miscUtils import LogMessages as lmsg

def exportInternalForces(nmbComb, elems, fDesc):
  '''Writes a comma separated values file with the element's internal forces.

  :param nmbComb: combination name.
  :param elems: element set.
  :param fDesc: file descriptor to write internal forces on.'''
  for e in elems:
    elementType= e.type()
    if('Shell' in elementType):
      internalForces= internal_forces.ShellMaterialInternalForces()
      internalForces.setFromAverageInShellElement(e)
      strEsf= internalForces.getCSVString()
      forcesOnNodes= internalForces.getWoodArmer()
      sz= len(forcesOnNodes)
      for i in range(0,sz):
        force= forcesOnNodes[i]
        outStr= nmbComb+", "+str(e.tag)+", "+str(i)+", "+force.getCSVString()+'\n'
        fDesc.write(outStr)
    elif('Beam2d' in elementType):
      e.getResistingForce()
      # Internal forces at the origin of the bar. 
      internalForces= internal_forces.CrossSectionInternalForces(e.getN1,e.getV1,0.0,0.0,0.0,e.getM1) 
      fDesc.write(nmbComb+", "+str(e.tag)+", 0, "+internalForces.getCSVString()+'\n')
      internalForces= internal_forces.CrossSectionInternalForces(e.getN2,e.getV2,0.0,0.0,0.0,e.getM2) # Internal forces at the end of the bar.
      fDesc.write(nmbComb+", "+str(e.tag)+", 1, "+internalForces.getCSVString()+'\n')
    elif('Beam' in elementType):
      e.getResistingForce()
      internalForces= internal_forces.CrossSectionInternalForces(e.getN1,e.getVy1,e.getVz1,e.getT1,e.getMy1,e.getMz1) # Internal forces at the origin of the bar.
      if e.hasProp('chiLT'):   #steel beam
        fDesc.write(nmbComb+", "+str(e.tag)+", 0, "+internalForces.getCSVString()+" , "+str(e.getProp('chiLT'))+'\n')
      else:
        fDesc.write(nmbComb+", "+str(e.tag)+", 0, "+internalForces.getCSVString()+'\n')
      internalForces= internal_forces.CrossSectionInternalForces(e.getN2,e.getVy2,e.getVz2,e.getT2,e.getMy2,e.getMz2) # Internal forces at the end of the bar.
      if e.hasProp('chiLT'):
        fDesc.write(nmbComb+", "+str(e.tag)+", 1, "+internalForces.getCSVString()+" , " + str(e.getProp('chiLT'))+'\n')
      else:
        fDesc.write(nmbComb+", "+str(e.tag)+", 1, "+internalForces.getCSVString()+'\n')
    elif('ZeroLength' in elementType):
      lmsg.warning("exportInternalForces for element type: '"+elementType+"' not implemented.")
    else:
      lmsg.error("exportInternalForces error; element type: '"+elementType+"' unknown.")
      

def exportShellInternalForces(nmbComb, elems, fDesc,fConv= 1.0):
  '''Writes a comma separated values file with the element's internal forces.'''
  errMsg= 'exportShellInternalForces deprecated use exportInternalForces'
  errMsg+= 'with apropriate arguments'
  lmsg.error(errMsg)
  internalForces= internal_forces.ShellMaterialInternalForces()
  for e in elems:
    internalForces.setFromAverageInShellElement(e)
    strEsf= internalForces.getCSVString()
    forcesOnNodes= internalForces.getWoodArmer()
    sz= len(forcesOnNodes)
    for i in range(0,sz):
      force= forcesOnNodes[i]
      outStr= nmbComb+", "+str(e.tag)+", "+str(i)+", "+force.getCSVString()+'\n'
      fDesc.write(outStr)

def exportaEsfuerzosShellSet(preprocessor,nmbComb, st, fName):
  '''Writes a comma separated values file with the element's internal forces.'''
  errMsg= 'exportaEsfuerzosShellSet deprecated use exportInternalForces'
  errMsg+= 'with apropriate arguments'
  lmsg.error(errMsg)
  elems= st.elements
  exportShellInternalForces(nmbComb,elems,fName)

def exportBeamInternalForces(nmbComb, elems, fName):
  '''Writes a comma separated values file with the element's internal forces.'''
  for e in elems:
    e.getResistingForce()
    internalForces= internal_forces.CrossSectionInternalForces(e.getN1,e.getVy1,e.getVz1,e.getT1,e.getMy1,e.getMz1) # Internal forces at the origin of the bar.
    fName.write(nmbComb+", "+str(e.tag*10+1)+","+internalForces.getCSVString())
    internalForces= internal_forces.CrossSectionInternalForces(e.getN2,e.getVy2,e.getVz2,e.getT2,e.getMy2,e.getMz2) # Internal forces at the end of the bar.
    fName.write(nmbComb+", "+str(e.tag*10+2)+","+internalForces.getCSVString())
