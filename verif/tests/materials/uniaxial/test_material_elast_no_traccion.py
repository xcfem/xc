# -*- coding: utf-8 -*-
# Home made test

E= 2.1e6 # Young modulus of the material.
Nsteps= 10 # number of analysis steps.
lStrain=[] 
lStress=[]
vStrainTeor= [-0.01,-0.008,-0.006,-0.004,-0.002,0,0.002,0.004,0.006,0.008,0.01]
vStressTeor= [E*-0.01,E*-0.008,E*-0.006,E*-0.004,E*-0.002,E*0,E*0,E*0,E*0,E*0,E*0]

import math
import xc_base
import geom
import xc
from solution import predefined_solutions
from model import predefined_spaces
from materials import typical_materials

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2014, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

# Model definition
feProblem= xc.FEProblem()
feProblem.logFileName= "/tmp/borrar.log" # Ignore warning messages
preprocessor=  feProblem.getPreprocessor
# Define materials
sIni= -0.01
sFin= 0.01
incr= ((sFin-sIni)/Nsteps)
ent= typical_materials.defElastNoTensMaterial(preprocessor, "ent",E)

j=sIni
while(j<=sFin):
  ent.setTrialStrain(j,0.0)
  ent.commitState()
  lStrain.append(ent.getStrain())
  lStress.append(ent.getStress())
  j= j+incr 


diff_strain= []
diff_stress= []
def substract(x,y): return x-y
diff_strain= map(substract,lStrain,vStrainTeor)
diff_stress= map(substract,lStress,vStressTeor)

ratio1= 0
for d in diff_strain:
  ratio1= ratio1+d**2
ratio3= math.sqrt(ratio1)
ratio2= 0
for d in diff_stress:
  ratio2= ratio2+d**2
ratio4= math.sqrt(ratio2)

''' 
print "lStrain= ",lStrain
print "lStress= ",lStress
print "vStrain= ",vStrain
print "vStress= ",vStress
print "ratio1= ",ratio1
print "ratio2= ",ratio2
 '''

import os
from miscUtils import LogMessages as lmsg
fname= os.path.basename(__file__)
if (ratio1 < 1e-15) & (ratio2 < 1e-15):
  print "test ",fname,": ok."
else:
  lmsg.error(fname+' ERROR.')
