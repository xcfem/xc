# -*- coding: utf-8 -*-
from __future__ import print_function
'''Verification of the rotate method for an 3D elastic section.'''

import xc_base
import geom
import xc
from materials import typical_materials
import math

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2015, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

Iz= 4
Iy= 1
ratio0= 10
ratio1= 10
ratio2= 10
ratio3= 10
feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor   
# Define materials
scc= typical_materials.defElasticSection3d(preprocessor, "scc",1,1,1,Iz,Iy,4)
scc.sectionProperties.rotate(math.radians(45))

I1= scc.sectionProperties.getI1()
I2= scc.sectionProperties.getI2()
I1axis= scc.sectionProperties.getAxis1VDir()
I2axis= scc.sectionProperties.getAxis2VDir()
strongAxis= scc.sectionProperties.getVDirStrongAxis()
weakAxis= scc.sectionProperties.getVDirWeakAxis()

strongAxisTeor= geom.Vector2d(math.sqrt(2)/2,math.sqrt(2)/2)
ratio0= (strongAxis-strongAxisTeor).getModulus()
ratio1= (I1axis-strongAxisTeor).getModulus()
weakAxisTeor= geom.Vector2d(-math.sqrt(2)/2,math.sqrt(2)/2)
ratio2= (weakAxis-weakAxisTeor).getModulus()
ratio3= (I2axis-weakAxisTeor).getModulus()

''' 
print("Iz: ",Iz)
print("I1: ",I1)
print("Axis 1: ",I1axis)
print("Strong axis: ",strongAxis)
print("Iy: ",Iy)
print("I2: ",I2)
print("Axis 2: ",I2axis)
print("Weak axis: ",weakAxis)
print("ratio0= ",ratio0)
print("ratio1= ",ratio1)
print("ratio2= ",ratio2)
print("ratio3= ",ratio3)
   '''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if((ratio0<1e-15) & (ratio1<1e-15) & (ratio2<1e-15) & (ratio3<1e-15)):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
