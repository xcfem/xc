# -*- coding: utf-8 -*-
''' Micropile with "custom" soil reaction properties. Home made test.'''

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2022, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

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

# Define custom micropile class.
class Micropile(guia.Micropile):
    ''' Micropile class with custom soil reaction properties.'''
    def __init__(self, pileSet, pileDiam, unitCGroundLevel, mortarMaterial, pipeSection, axialRebar, pileType, soilAggressivity, designLife, Fe, Fuc):
        super().__init__(pileSet= pileSet, pileDiam= pileDiam, soilLayers= None, mortarMaterial= mortarMaterial, pipeSection= pipeSection, axialRebar= axialRebar, pileType= pileType, soilAggressivity= soilAggressivity, designLife= designLife, Fe= Fe, Fuc= Fuc)
        self.unitCGroundLevel= unitCGroundLevel
        
    def getLinearSpringsConstants3D(self, alphaKh_x= 1.0, alphaKh_y= 1.0, alphaKv_z= 1.0):
        '''Compute the spring contants that simulate the soils along the pile

        :param alphaKh_x: coefficient to be applied to the horizontal stiffness
                          of a single pile in X direction
        :param alphaKh_y: coefficient to be applied to the horizontal stiffness
                          of a single pile in Y direction
        :param alphaKh_Z: coefficient to be applied to the vertical stiffness of
                          a single pile in Z direction
        '''
        # Soil reaction modulus at each node.
        lstNodPile= self.getNodeZs()
        self.computeTributaryLengths(False)
        retval= dict()
        for (n,z) in lstNodPile:
            if(z>self.unitCGroundLevel):
                soilReactionModulus= 5000e3
            else:
                soilReactionModulus= -15000e3*z
            assert(soilReactionModulus>=0)
            tL= self.tributaryLengths[n.tag]
            springStiffness= soilReactionModulus*tL
            retval[n.tag]= [alphaKh_x*springStiffness, alphaKh_y*springStiffness, 1e-5]
        # Compute vertical stiffness on the tip.
        tipNode= lstNodPile[-1][0]
        tipNodeStiffness= retval[tipNode.tag]
        tipStiffness= self.getEstimatedTipReactionModulus()
        tipNodeStiffness[2]= alphaKv_z*tipStiffness
        return retval
    
        
## Define micropile.
micropile= Micropile(pileSet= None, pileDiam= 152.4e-3, unitCGroundLevel= -4, mortarMaterial= mortar, pipeSection= pipeShape, axialRebar= rebar, pileType= 'endBearing', soilAggressivity= soilAggressivity, designLife= designLife, Fe= Fe, Fuc= Fuc)

# Problem definition
feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor
nodes= preprocessor.getNodeHandler
modelSpace= predefined_spaces.StructuralMechanics3D(nodes)

## Finite element material
xcSection= micropile.defElasticShearSection3d(preprocessor)
## Problem geometry
points= preprocessor.getMultiBlockTopology.getPoints
pTop= points.newPoint(geom.Pos3d(0,0,0))
pBottom= points.newPoint(geom.Pos3d(0,0,-10.0))
lines= preprocessor.getMultiBlockTopology.getLines
l= lines.newLine(pTop.tag,pBottom.tag)
l.nDiv= 20
### Geometric transformations
lin= modelSpace.newLinearCrdTransf("lin",xc.Vector([0,1,0]))
## Mesh generation
seedElemHandler= preprocessor.getElementHandler.seedElemHandler
seedElemHandler.dimElem= 3 # Bars defined in a three dimensional space.
seedElemHandler.defaultMaterial= xcSection.name
seedElemHandler.defaultTransformation= lin.name
seedElem= seedElemHandler.newElement("ElasticBeam3d")
l.genMesh(xc.meshDir.I)

### Constraints.
micropile.pileSet= l
micropile.generateSpringsPile(alphaKh_x=1,alphaKh_y=1,alphaKv_z=1)
bottomTipNode= pBottom.getNode()
modelSpace.fixNode('FFF_FF0',bottomTipNode.tag)

### Loads
Fh= 10e3
N= -0.85*micropile.getNcRd()
topTipNode= pTop.getNode()
lp0= modelSpace.newLoadPattern(name= '0')
lp0.newNodalLoad(topTipNode.tag,xc.Vector([Fh,0,N,0,0,0]))

# We add the load case to domain.
modelSpace.addLoadCaseToDomain(lp0.name)

# Compute solution
result= modelSpace.analyze(calculateNodalReactions= True)

# Get bottom tip displacement.
bottomTipDisp= bottomTipNode.getDisp[2]
ratio1= abs(bottomTipDisp+0.010075333675765406)/0.010075333675765406
topTipDisp= topTipNode.getDisp
ratio2= (topTipDisp[0]-0.00480384)**2+topTipDisp[1]**2+(topTipDisp[2]+0.018579)**2

'''
print('bottom tip displacement: ', bottomTipDisp)
print('ratio1= ', ratio1)
print('top tip displacement: ', topTipDisp)
print('ratio2= ', ratio2)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if abs(ratio1)<1e-8 and abs(ratio2)<1e-8:
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')


# Graphic stuff.
dispSet= modelSpace.defSet('dispSet')
dispSet.lines.append(l)
dispSet.fillDownwards()
    
'''
from postprocess import output_handler
oh= output_handler.OutputHandler(modelSpace)
oh.displayFEMesh()
oh.displayNodeValueDiagram(itemToDisp= 'k_x', caption= 'Spring stiffness x direction', defaultDirection= 'X', defaultValue= 0.0)
oh.displayNodeValueDiagram(itemToDisp= 'k_y', caption= 'Spring stiffness y direction', defaultDirection= 'Y', defaultValue= 0.0)
oh.displayDispRot(itemToDisp='uX',defFScale= 2.0,setToDisplay=dispSet)
oh.displayDispRot(itemToDisp='uZ',defFScale= 2.0,setToDisplay=dispSet)
oh.displayIntForcDiag(itemToDisp='N',setToDisplay=dispSet)
oh.displayIntForcDiag(itemToDisp='Vy',setToDisplay=dispSet)
oh.displayIntForcDiag(itemToDisp='Mz',setToDisplay=dispSet)
'''
