# -*- coding: utf-8 -*-

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2016,LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import math
from materials import ShellInternalForces as sif

  

# Imprime los esfuerzos de los elementos contenidos en el conjunto que se pasa como parámetro.
def exportShellInternalForces(nmbComb, elems, fDesc,fConv= 1.0):
  internalForces= sif.ShellElementInternalForces()
  for e in elems:
    internalForces.setFromAverageInShellElement(e)
    strEsf= internalForces.getCSVString()
    fDesc.write(nmbComb+", "+str(e.tag)+", "+strEsf+'\n')

# Imprime los esfuerzos de los elementos contenidos en el conjunto que se pasa como parámetro.
def exportaEsfuerzosShellSet(preprocessor,nmbComb, st, fName):
  elems= st.getElements
  exportShellInternalForces(nmbComb,elems,fName)

# Imprime los esfuerzos de los elementos contenidos en el conjunto que se pasa como parámetro.
def exportBeamInternalForces(nmbComb, elems, fName):
  for e in elems:
    e.getResistingForce()
    N1= str(e.getN1)
    Qy1= str(e.getVy1)
    Qz1= str(e.getVz1)
    T1= str(e.getT1)
    My1= str(e.getMy1) # Momento en el extremo dorsal de la barra
    Mz1= str(e.getMz1) # Momento en el extremo dorsal de la barra
    fName.write(nmbComb+", "+str(e.tag*10+1)+",")
    fName.write(N1+", "+Qy1+", "+Qz1+", ")
    fName.write(T1+", "+My1+", "+Mz1+"\n")
    N2= str(e.getN2)
    Qy2= str(e.getVy2)
    Qz2= str(e.getVz2)
    T2= str(e.getT2)
    My2= str(e.getMy2) # Momento en el extremo frontal de la barra
    Mz2= str(e.getMz2) # Momento en el extremo frontal de la barra
    fName.write(nmbComb+", "+str(e.tag*10+2)+",")
    fName.write(N2+", "+Qy2+", "+Qz2+", ")
    fName.write(T2+", "+My2+", "+Mz2+"\n")
