# -*- coding: utf-8 -*-
from __future__ import print_function
''' Interaction diagram computation. Home made test. '''


import xc_base
import geom
import xc
from materials.ehe import EHE_materials

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (A_OO)"
__copyright__= "Copyright 2015, LCPT and AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com ana.ortega.ort@gmal.com"


FCs= []

feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor
# Define materials
materialHandler= preprocessor.getMaterialHandler
diagInteg= materialHandler.newInteractionDiagram("diagInteg")
diagInteg.readFrom("/tmp/interaction_diagram_test_02.dat")

FCs.append(diagInteg.getCapacityFactor(geom.Pos3d(2185.5e3,0,0)))
FCs.append(diagInteg.getCapacityFactor(geom.Pos3d(1595.8e3,0,186.5e3)))
FCs.append(diagInteg.getCapacityFactor(geom.Pos3d(1006.2e3,0,380.2e3)))
FCs.append(diagInteg.getCapacityFactor(geom.Pos3d(416.6e3,0,565.9e3)))
FCs.append(diagInteg.getCapacityFactor(geom.Pos3d(-173e3,0,739.3e3)))
FCs.append(diagInteg.getCapacityFactor(geom.Pos3d(-762.6e3,0,870.8e3)))
FCs.append(diagInteg.getCapacityFactor(geom.Pos3d(-1352.2e3,0,962.2e3)))
FCs.append(diagInteg.getCapacityFactor(geom.Pos3d(-1941.9e3,0,1004.1e3)))
FCs.append(diagInteg.getCapacityFactor(geom.Pos3d(-2531.5e3,0,1021.9e3)))
FCs.append(diagInteg.getCapacityFactor(geom.Pos3d(-3121.1e3,0,950.7e3)))
FCs.append(diagInteg.getCapacityFactor(geom.Pos3d(-3710.7e3,0,872.1e3)))
FCs.append(diagInteg.getCapacityFactor(geom.Pos3d(-4300.3e3,0,782.8e3)))
FCs.append(diagInteg.getCapacityFactor(geom.Pos3d(-4889.9e3,0,678.1e3)))
FCs.append(diagInteg.getCapacityFactor(geom.Pos3d(-5479.6e3,0,551.8e3)))
FCs.append(diagInteg.getCapacityFactor(geom.Pos3d(-6069.2e3,0,394.6e3)))
FCs.append(diagInteg.getCapacityFactor(geom.Pos3d(-6658.8e3,0,230.7e3)))
FCs.append(diagInteg.getCapacityFactor(geom.Pos3d(-7248.4e3,0,0.0)))


import os
os.system("rm -f /tmp/interaction_diagram02.dat")

szFCs= float(len(FCs))
sumQuad= 0
for f in FCs:
  sumQuad+= f**2


ratio1= abs(sumQuad-szFCs)/szFCs

''' 
print("FCs= ",FCs)
print("sumQuad= ",sumQuad)
print("ratio1= ",ratio1)
 '''

from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if(abs(ratio1)<0.02):
  print('test '+fname+': ok.')
else:
  lmsg.error(fname+' ERROR.')
