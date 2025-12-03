# -*- coding: utf-8 -*-
''' Home made test. '''
__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2022, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import os
import math
from materials.ec3 import EC3_materials
from materials.api import API_materials
from materials.ehe import EHE_materials
from geotechnics.foundations import guia_micropilotes_oc as guia
import xc
import geom
from model import predefined_spaces

# Material
## Steel pipe.
pipeSteel= API_materials.M80
gammaM0= 1.10
pipeSteel.gammaM= gammaM0
pipeShape= EC3_materials.MicropileTubeShape(steel= pipeSteel, name= 'MP_88.9x7.5')
## Mortar
mortar= EHE_materials.HA25
## Rebar
rebar= EHE_materials.rebar32_B500S

# Micropile section.
# (ascii art from ascii.co.uk)
# 
#              =-=   
#           =   _   = 
#          =  /   \  =
#          |    o    |
#          =  \ _ /  =
#           =       = 
#              = =    
#      
## soilAggressivity: soil aggresivity descriptor
##                   ('unalteredNatural', 'pollutedNatural', 'aggresiveNatural',
##                    'backfill', 'aggresiveBackfill') according to table 2.4.
soilAggressivity= 'unalteredNatural'
## designLife: design service life of the micropile (in years).
designLife= 50
## Fe: influence of the execution technique taken from table 3.5.
Fe= 1.5
## Fuc: reduction factor of the cross-section area due to the  type of the
##      union taken from table 3.4.
Fuc= 0.5
## Define soil horizons.
soilHorizons= guia.SandySoilLayers(soilProfile= [[-100.0, 'medium']], waterTableZ= -100.0, groundLevel= -1.0)
## Define micropile.
micropile= guia.Micropile(pileSet= None, pileDiam= 152.4e-3, soilLayers= soilHorizons, mortarMaterial= mortar, pipeSection= pipeShape, axialRebar= rebar, pileType= 'endBearing', soilAggressivity= soilAggressivity, designLife= designLife, Fe= Fe, Fuc= Fuc)

# Problem definition
feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor
nodes= preprocessor.getNodeHandler
modelSpace= predefined_spaces.StructuralMechanics2D(nodes)

## Finite element material
xcSection= micropile.defElasticShearSection2d(preprocessor)
modelSpace.setDefaultMaterial(xcSection)
## Problem geometry
points= preprocessor.getMultiBlockTopology.getPoints
pTop= points.newPoint(geom.Pos3d(0,0,0))
pBottom= points.newPoint(geom.Pos3d(0,-10.0,0))
lines= preprocessor.getMultiBlockTopology.getLines
l= lines.newLine(pTop.tag,pBottom.tag)
l.nDiv= 20
### Geometric transformations
lin= modelSpace.newLinearCrdTransf("lin")
modelSpace.setDefaultCoordTransf(lin)
## Mesh generation
modelSpace.newSeedElement('ElasticBeam2d')
l.genMesh(xc.meshDir.I)

### Constraints.
micropile.pileSet= l
micropile.generateSpringsPile(alphaKh_x=1,alphaKh_y=1,alphaKv_z=1)
bottomTipNode= pBottom.getNode()

# Graphic stuff.
from postprocess import output_handler
oh= output_handler.OutputHandler(modelSpace)
# oh.displayNodeValueDiagram(itemToDisp= 'k_x', caption= 'Spring stiffness x direction', defaultDirection= 'X', defaultValue= 0.0)
fname= os.path.basename(__file__)
outputFileName= '/tmp/'+fname.replace('.py', '.jpeg')
oh.displayNodeValueDiagram(itemToDisp= 'k_x', caption= 'Spring stiffness x direction', defaultDirection= 'X', defaultValue= 0.0, fileName= outputFileName)

# Check that file exists
testOK= os.path.isfile(outputFileName)

from misc_utils import log_messages as lmsg
if testOK:
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')


