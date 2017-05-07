# -*- coding: utf-8 -*-

# home made test
l= 1e-7 # Distance between nodes

import xc_base
import geom
import xc
from solution import predefined_solutions
from model import predefined_spaces
from model import fix_node_3dof
from materials import typical_materials
from materials import materialGraphics 
from materials.ehe import EHE_concrete
import matplotlib.pyplot as plt
import numpy as np

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2015, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

# Model definition
prueba= xc.ProblemaEF()
preprocessor=  prueba.getPreprocessor
nodes= preprocessor.getNodeLoader
nodes.dimEspace= 1
nodes.numGdls= 1

nodes.defaultTag= 1 #First node number.
nod= nodes.newNodeXY(1,0)
nod= nodes.newNodeXY(1.0+l,0)

# Materials definition
concrAux=EHE_concrete.HA25
concr=typical_materials.defConcrete02(preprocessor=preprocessor,name='concr25',epsc0=concrAux.epsilon0(),fpc=concrAux.fmaxK(),fpcu=0.85*concrAux.fmaxK(),epscu=concrAux.epsilonU(),ratioSlope=0.1,ft=concrAux.fctk(),Ets=concrAux.E0()/19.0)


# Elements definition
elementos= preprocessor.getElementLoader
elementos.defaultMaterial='concr25'
elementos.dimElem= 1
elementos.defaultTag= 1
elem1= elementos.newElement("zero_length",xc.ID([1,2]))

''' 
print "dimensión: ",getDim
print "filas trf: ",getTrf.nrows
print "cols. trf: ",getTrf.ncols
print "trf[0]: ","getTrf[0,0]"," ","getTrf[0,1]"," ","getTrf[0,2]"
print "trf[1]: ","getTrf[1,0]"," ","getTrf[1,1]"," ","getTrf[1,2]"
print "trf[2]: ","getTrf[2,0]"," ","getTrf[2,1]"," ","getTrf[2,2]"
   '''

    
# Constraints
constraints= preprocessor.getConstraintLoader
spc= constraints.newSPConstraint(1,0,0.0) # Nodo 1


# Loads definition
cargas= preprocessor.getLoadLoader
casos= cargas.getLoadPatterns
#Load modulation.
ts= casos.newTimeSeries("constant_ts","ts")
casos.currentTimeSeries= "ts"
#Load case definition
lp0= casos.newLoadPattern("default","0")
#casos.currentLoadPattern= "0"
#We add the load case to domain.
casos.addToDomain("0")

# Solution
analisis= predefined_solutions.simple_static_modified_newton(prueba)
dom= preprocessor.getDomain

strainData= [-1.55637e-3,-1.4037e-3,-1.28474e-3,-1.18796e-3,-1.09365e-3,-1.01316e-3,-0.93755e-3,-0.86304e-3,-0.796515e-3,-0.732951e-3,-0.672066e-3,-0.611906e-3,-0.556246e-3,-0.502408e-3,-0.450266e-3,-0.399699e-3,-0.3506e-3,-0.30244e-3,-0.25618e-3,-0.211025e-3,-0.166922e-3,-0.123819e-3,-0.0816646e-3,-0.0404e-3,0e-3,0.04e-3,0.06e-3,0.068e-3,0.0721905e-3]
stressData= [-2.37699e7,-22.7776e6,-21.8025e6,-20.8787e6,-19.8658e6,-18.9134e6,-17.945e6,-16.9208e6,-15.9476e6,-14.9662e6,-13.9787e6,-12.9575e6,-11.9723e6,-10.9826e6,-9.98952e6,-8.99399e6,-7.99676e6,-6.98931e6,-5.99432e6,-4.9973e6,-3.99891e6,-2.99966e6,-1.99993e6,-0.999799e6,0e6,1e6,1.5e6,1.7e6,1.79499e6]
stressInput= [-24e6,-23e6,-22e6,-21e6,-20e6,-19e6,-18e6,-17e6,-16e6,-15e6,-14e6,-13e6,-12e6,-11e6,-10e6,-9e6,-8e6,-7e6,-6e6,-5e6,-4e6,-3e6,-2e6,-1e6,0.0,1e6,1.5e6,1.7e6,1.8e6]
calculatedStresses= []
calculatedStrains= []

for stress in stressInput:
  dom.revertToStart()
  lp0.clearLoads()
  nl= lp0.newNodalLoad(2,xc.Vector([stress]))
  result= analisis.analyze(1)
  calculatedStresses.append(-elem1.getResistingForce()[0])
  calculatedStrains.append(elem1.getMaterials()[0].getStrain())

residStresses= (np.array(calculatedStresses) - np.array(stressData))
residStrains= (np.array(calculatedStrains) - np.array(strainData))

ratio1= np.linalg.norm(residStresses)/-concrAux.fmaxK()
ratio2= np.linalg.norm(residStrains)/3.5e-3



''' 
print 'E0= ', concr.getInitialTangent()/1e9, 2*concr.fpc/concr.epsc0/1e9
print 'epsc0= ', concr.epsc0
print 'fmaxK= ', concr.fpc/1e6
print 'epscu= ', concr.epscu
print 'ft= ', concr.ft/1e6
print 'Ets= ', concr.Ets/1e9, concr.getInitialTangent()/19e9
print calculatedStresses
print 'error in stress calculation: ', residStresses
print calculatedStrains
print residStrains
print "ratio1= ",ratio1
print "ratio2= ",ratio2
retval= materialGraphics.UniaxialMaterialDiagramGraphic(epsMin=-0.0035,epsMax=0.0016,title=' concrete02 characteristic stress-strain diagram')
retval.setupGraphic(plt,concr)
plt.plot(strainData, stressData, '-')
retval.savefig(plt,'concrete02_diagram.jpeg')
   '''
  
import os
from miscUtils import LogMessages as lmsg
fname= os.path.basename(__file__)
if((ratio1<1e-5) and (ratio2<1e-5)) :
  print "test ",fname,": ok."
else:
  lmsg.error(fname+' ERROR.')
