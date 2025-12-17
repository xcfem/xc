# -*- coding: utf-8 -*-
''' Generation of a strut-and-tie model for a threee-pile pile cap that 
    transfer loads from the piles to the piles. 

    Compute the angles of the radial struts and set its area according
    to this angle.
'''

from __future__ import division
from __future__ import print_function

__author__= "Luis Claudio Pérez Tato (LCPT)"
__copyright__= "Copyright 2025, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import os
import math
import geom
import xc
from model import predefined_spaces
from materials.ec2 import EC2_materials
from materials.sections.fiber_section import def_simple_RC_section
from materials.sections.fiber_section import def_column_RC_section
from misc_utils import log_messages as lmsg
from solution import predefined_solutions
from model.mesh import strut_and_tie_utils
from misc_utils import log_messages as lmsg

# Problem type.
feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor
nodes= preprocessor.getNodeHandler
modelSpace= predefined_spaces.StructuralMechanics3D(nodes)

radius= 2.0
angles= [math.radians(-30), math.pi/2.0, math.radians(210)]
pileTopPoints= list()
for a in angles: 
    pt= geom.Pos3d(radius*math.cos(a), radius*math.sin(a), 0.0)
    pileTopPoints.append(pt)
    
d= 1.995
pierHeight= 2
pierDiameter= 1.25
pierEffectiveDiameter= 0.85*pierDiameter
pileLength= 5.0
pileDiameter= 1.25

## Define mesh.
### Top of the pier.
n0= modelSpace.newNode(0.0, 0.0, d+pierHeight) # pier top.
n3= modelSpace.newNode(0.0, 0.0, d) # pier bottom.
### Bottom of the pile cap.
pileTopNodes= list()
pileBottomNodes= list()
for pt in pileTopPoints:
    pileTopNodes.append(modelSpace.newNode(pt.x, pt.y, pt.z))
    pileBottomNodes.append(modelSpace.newNode(pt.x, pt.y, pt.z-pileLength))

pileCap= strut_and_tie_utils.PileCap3Piles(pierBottomNode= n3, pileTopNodeA= pileTopNodes[0], pileTopNodeB= pileTopNodes[1], pileTopNodeC= pileTopNodes[2],  pierEffectiveDiameter= pierEffectiveDiameter)

# Define materials. 
concrete= EC2_materials.C30
reinfSteel= EC2_materials.S500B
## Define pier material.
pierRCSection= def_column_RC_section.RCCircularSection(name= 'pierRCSection', sectionDescr= 'pier section', concrType= concrete, reinfSteelType= reinfSteel, Rext= pierDiameter)
xcPierSectionMaterial= pierRCSection.defElasticShearSection3d(preprocessor)
## Define piles material.
pilesRCSection= def_column_RC_section.RCCircularSection(name= 'pilesRCSection', sectionDescr= 'piles section', concrType= concrete, reinfSteelType= reinfSteel, Rext= pileDiameter/2.0)
pilesMaterial= pilesRCSection.defElasticShearSection3d(preprocessor, reductionFactor= 1.0)

# Define pile cap.
pierArea= pierRCSection.getAc()
pileArea= pilesRCSection.getAc()
strutArea= min(pierArea, pileArea)/math.sqrt(2)
diam32Area= math.pi*(32e-3/2.0)**2
diam25Area= math.pi*(25e-3/2.0)**2
diam16Area= math.pi*(16e-3/2.0)**2
pileCap.createStrutAndTieModel(modelSpace, strutArea= strutArea, concrete= concrete, pileTiesArea= 24*diam25Area, radialTiesArea= 10*diam16Area, bottomChordsTiesArea= 10*diam32Area, topChordsTiesArea= 10*diam25Area, shearTiesArea= 10*diam16Area, reinfSteel= reinfSteel, xcPierSectionMaterial= xcPierSectionMaterial)

# Recompute areas of the radial struts.
radialStruts= pileCap.getRadialStrutList()
KVector= geom.Vector3d(0, 0, 1)
for rs in radialStruts:
    iVector= geom.Vector3d(rs.getIVector3d(True))
    angle= iVector.getAngle(KVector)
    if(angle>math.pi/2.0):
        angle= math.pi-angle
    # oldArea= rs.sectionArea
    newArea= min(pierArea, pileArea)*math.cos(angle)
    rs.sectionArea= newArea

### Define pier.
#### Define pier elements.
lin= modelSpace.newLinearCrdTransf("lin", xzVector= xc.Vector([1,0,0]))
modelSpace.setDefaultCoordTransf(lin)
modelSpace.setDefaultMaterial(xcPierSectionMaterial)
modelSpace.newElement('ElasticBeam3d', [n0.tag, n3.tag])

### Define piles.
#### Set material.
modelSpace.setDefaultMaterial(pilesMaterial)
for nTop, nBottom in zip(pileTopNodes, pileBottomNodes):
    modelSpace.newElement('ElasticBeam3d', [nTop.tag, nBottom.tag])

## Define constraints
for n in pileBottomNodes:
    modelSpace.fixNode('000_000', n.tag)
    
# Load definition.
F= 3000e3
lp0= modelSpace.newLoadPattern(name= '0')
lp0.newNodalLoad(n0.tag,xc.Vector([0, 0, -F, 0, 0, 0]))
# We add the load case to domain.
modelSpace.addLoadCaseToDomain(lp0.name)

# Solution
analysis= predefined_solutions.penalty_newton_raphson(feProblem, maxNumIter= 30, printFlag= 0)
result= analysis.analyze(1)
if(result!=0):
    lmsg.error("Can't solve for load case: "+str(lp0.name))
    info= None
    solver= analysis.linearSOE.solver
    if(solver.hasProp("info")):
        info= solver.getProp("info")
    unconstrainedNode= modelSpace.locateEquationNumber(eqNumber= info-1)
    lmsg.error('unconstrained node id: ', unconstrainedNode.tag)
    lmsg.error('unconstrained node position: ', unconstrainedNode.getInitialPos3d)
    exit(1)
# Check the sign of the computed stresses.
stressesOK= pileCap.checkStressesSign()
if(not stressesOK):
    lmsg.error('Computed stresses are wrong.')
    exit(1)

# Check results.
modelSpace.calculateNodalReactions()
R0a= pileBottomNodes[0].getReaction
R0b= pileBottomNodes[1].getReaction
R0c= pileBottomNodes[2].getReaction
ratio01= abs(R0a[0]+R0b[0]+R0c[0])/F
ratio02= abs(R0a[1]+R0b[1]+R0c[1])/F
ratio03= abs(R0a[2]+R0b[2]+R0c[2]-F)/F
F_3= F/3.0
ratio04= math.sqrt((R0a[2]-F_3)**2+(R0b[2]-F_3)**2+(R0c[2]-F_3)**2)/F
R0= R0a+R0b+R0c

# Compute stresses on the radial struts.
stressesOnRadialStruts= list()
for rs in radialStruts:
    area= rs.sectionArea
    stress= rs.getN()/area
    if(abs(stress)>1e3): # if not zero.
        stressesOnRadialStruts.append(stress)
refStressesOnRadialStruts= [-1230761.229314036]*3
stressError= 0.0
for stress, refStress in zip(stressesOnRadialStruts, refStressesOnRadialStruts):
    stressError+= (stress-refStress)**2
stressError= math.sqrt(stressError)
testOK= (stressError<1e-5)


tol= 1e-10
testOK= testOK and ((abs(ratio01)<tol) and (abs(ratio02)<tol) and (abs(ratio03)<tol) and (abs(ratio04)<tol))

'''
# Report results.
print('\nR0a= ', R0a*1/1e3, 'kN')
print('R0b= ', R0b*1/1e3, 'kN')
print('R0c= ', R0c*1/1e3, 'kN')
print('R0= ', R0*1/1e3, 'kN')
print('ratio01= ', ratio01)
print('ratio02= ', ratio02)
print('ratio02= ', ratio03)
print('ratio04= ', ratio04)
print(stressesOnRadialStruts, stressError)
'''

import os
fname= os.path.basename(__file__)
if testOK:
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')

# # Graphic stuff.
# from postprocess import output_handler
# oh= output_handler.OutputHandler(modelSpace)
# # oh.displayBlocks()
# oh.displayFEMesh()
# oh.displayLoads()
# oh.displayReactions()
# oh.displayIntForcDiag('N')
# oh.displayIntForcDiag('My')
# oh.displayIntForcDiag('Mz')
# oh.displayDispRot(itemToDisp='uY')

# # DXF output
# fname= os.path.basename(__file__)
# from import_export import mesh_entities
# xcTotalSet= modelSpace.getTotalSet()
# me= mesh_entities.MeshData()
# me.readFromXCSet(xcSet= xcTotalSet)
# outputFileName= fname.replace('.py', '.dxf')
# me.writeDxfFile(outputFileName)
