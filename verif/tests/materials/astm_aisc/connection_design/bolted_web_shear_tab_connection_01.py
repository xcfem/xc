# -*- coding: utf-8 -*-
''' Trivial getShearTabCore verification test.'''

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2015, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import xc
from model import predefined_spaces
from materials.astm_aisc import ASTM_materials # steel

# Problem type
feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor
nodes= preprocessor.getNodeHandler
modelSpace= predefined_spaces.StructuralMechanics2D(nodes)

#Materials
## Steel material
steel= ASTM_materials.A36
steel.gammaM= 1.00
## Profile geometry
shape= ASTM_materials.WShape(steel,'W10X39')
xcSection= shape.defElasticShearSection2d(preprocessor)

# Geometry.
n1= modelSpace.newNodeXY(0.0,0.0)
n2= modelSpace.newNodeXY(1.0,0.0)

# Element defininion
trfs= preprocessor.getTransfCooHandler
lin= trfs.newLinearCrdTransf2d("lin")
elements= preprocessor.getElementHandler
elements.defaultTransformation= lin.name
elements.defaultMaterial= xcSection.name
beam2d= elements.newElement("ElasticBeam2d",xc.ID([n1.tag,n2.tag]))

# Connected member.
beam2d.setProp('crossSection', shape)
member= ASTM_materials.ConnectedMember(beam2d)

# Return a minimal shear tab. The
# length of the plate is just enough to accomodate the bolts
# and its width is chosen as three inches.

# :param boltSteel: steel type of the bolts that connect the plate with
#                   the flange.
# :param plateSteel: steel type of the bolted plate.
# :param shearEfficiency: ratio between the design shear and 
#                         the shear strength.
webShearTab= member.getShearTabCore(boltSteel= ASTM_materials.A490, plateSteel= ASTM_materials.A490, shearEfficiency= 0.35)

width= webShearTab.getWidth()
ratio1= abs(width-75e-3)/75e-3
length= webShearTab.getLength()
ratio2= abs(length-150e-3)/150e-3
thickness= webShearTab.getThickness()
ratio3= abs(thickness-13e-3)/13e-3
boltArray= webShearTab.boltArray
numberOfBolts= boltArray.getNumberOfBolts()
ratio4= abs(numberOfBolts-2)/2
boltDiameter= boltArray.bolt.diameter
ratio5= abs(boltDiameter-22e-3)/22e-3

'''
print('shear tab width: ', width*1e3, ' mm')
print('ratio1= ', ratio1)
print('shear tab length: ', length*1e3, ' mm')
print('ratio2= ', ratio2)
print('shear tab thickness: ', thickness*1e3, ' mm')
print('ratio3= ', ratio3)
print('number of bolts: ', numberOfBolts)
print('ratio4= ', ratio4)
print('bolt diameter: ', boltDiameter)
print('ratio5= ', ratio5)
'''

from misc_utils import log_messages as lmsg
import os
fname= os.path.basename(__file__)
if (abs(ratio1)<1e-9) & (abs(ratio2)<1e-9) & (abs(ratio3)<1e-9) & (abs(ratio4)<1e-9) & (abs(ratio5)<1e-9):
    print('test: '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
