# -*- coding: utf-8 -*-
'''Laterally-loaded pile foundation.

Static pushover of a single pile, modeled as a beam on a nonlinear Winkler 
foundation. Lateral soil response is described by p-y springs. Vertical soil 
response described by t-z and q-z springs.

Based on the example 14.4.1 of OpenSees (see https://openseespydoc.readthedocs.io/en/stable/src/pile.html) by Pavan Chigullapally from University of Auckland, Auckland.
'''

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2026, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

silent= True

import math
import xc
from model import predefined_spaces
from model.sets import sets_mng 
from materials import soil_structure_interaction as ssi
from materials import typical_materials
from geotechnics.foundations import pile
from geotechnics.foundations import pytzqz_functions as pytzqzf
from solution import predefined_solutions
from actions.quake import ground_motion_utils as gmu
from scipy.constants import g

# Pile geometry and mesh
L1 = 1.0 # length of pile head (above ground surface) (m)
L2 = 20.0 # length of embedded pile (below ground surface) (m)
pileDiameter = 1.0 # pile diameter
eleSize= 0.25 # pile element length 

# Define FE problem.
feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor
nodeHandler= preprocessor.getNodeHandler
modelSpace= predefined_spaces.StructuralMechanics3D(nodeHandler)

# K-points.
p1= modelSpace.newKPoint(0.0, 0.0, 0.0) # pile tip.
p2= modelSpace.newKPoint(0.0, 0.0, L2)
p3= modelSpace.newKPoint(0.0, 0.0, L1+L2) # pile head.

# Lines.
l1= modelSpace.newLine(p1, p2)
l2= modelSpace.newLine(p2, p3)

# Create mesh.
## Pile section.
E = 25e9
nu= 0.3
G= E/(2*(1+nu))
A = 0.785
Iz = 0.049
Iy = 0.049
J = 0.098
sectionProperties= xc.CrossSectionProperties3d()
sectionProperties.A= A; sectionProperties.E= E; sectionProperties.G= G
sectionProperties.Iz= Iz; sectionProperties.Iy= Iy; sectionProperties.J= J
pileSection= typical_materials.defElasticSectionFromMechProp3d(preprocessor, "section",sectionProperties)
# Coordinate transfomration.
lin= modelSpace.newLinearCrdTransf("lin",xc.Vector([0,-1,0]))
## Create elements.
modelSpace.setDefaultCoordTransf(lin)
modelSpace.setDefaultMaterial(pileSection)
modelSpace.newSeedElement("ElasticBeam3d")
for l in [l1, l2]:
    l.setElemSize(eleSize)
    l.genMesh(xc.meshDir.I)
pileSet= modelSpace.defSet('pileSet', lines= [l1, l2])
pileSet.fillDownwards()

# Soil properties
soilRho= 17e3/g # soil density kg/m3
soilPhi= math.radians(36.0) # soil internal friction angle (radians)
soilG= 150000000.0 # soil shear modulus at pile tip (Pa).

# select pult definition method for p-y curves
# API (default) --> 1
# Brinch Hansen --> 2
puSwitch= 1 

# variation in coefficent of subgrade reaction with depth for p-y curves
# API linear variation (default)   --> 1
# modified API parabolic variation --> 2
kSwitch= 1

# effect of ground water on subgrade reaction modulus for p-y curves
# above gwt --> 1
# below gwt --> 2
gwtSwitch = 1

## Create springs along the pile shaft.
tributaryLengths= sets_mng.get_tributary_lengths(xcSet= l1, initialGeometry= True)

modelSpace.setElementDimension(3)
springNodes= list()
springElements= list()
for n in l1.nodes:
    nPos= n.getInitialPos3d
    pyDepth= L2-nPos.z
    tributaryLength=  tributaryLengths[n.tag]
    pult, y50= pytzqzf.get_py_param(pyDepth, soilRho, soilPhi, pileDiameter, tributaryLength, puSwitch, kSwitch, gwtSwitch)
    pyMatName= 'pyS_'+str(n.tag)
    pyMat= ssi.def_pysimple1_material(preprocessor,
                                      matName= pyMatName,
                                      soilType= 2, # Sand.
                                      pult= pult,
                                      Y50= y50,
                                      c= 0.0,
                                      Cd= 0.0)

    sigV= soilRho*g*pyDepth # Vertical stress.
    tult, z50= pytzqzf.get_tz_param(soilPhi, pileDiameter, sigV, tributaryLength)
    tzMatName= 'tzS:'+str(n.tag)
    tzMat= ssi.def_tzsimple1_material(preprocessor,
                                      matName= tzMatName,
                                      tzType= 2, # Mosher relation.
                                      tult= tult, # Ultimate skin friction capacity.
                                      z50= z50, # 50% capacity reached at 0.05 units of displacement
                                      c= 0.0)
    # Create spring.
    fixNode= modelSpace.duplicateNode(n)
    modelSpace.fixNode('000_000', fixNode.tag)
    modelSpace.setDefaultMaterial(pyMat)
    pileShaftSpring= modelSpace.newElement("ZeroLength", [fixNode.tag, n.tag])
    pileShaftSpring.clearMaterials()
    pileShaftSpring.setMaterial(0, pyMat.name)
    pileShaftSpring.setMaterial(1, pyMat.name)
    pileShaftSpring.setMaterial(2, tzMat.name)
    springNodes.append(fixNode)
    springElements.append(pileShaftSpring)

springElementSet= modelSpace.defSet('springElementSet', elements= springElements)
springElementSet.fillDownwards()

## Create spring at the pile tip.
### q-z spring material
    
tipNode= p1.getNode()
### Fix vertical rotation of tip node.
modelSpace.fixNode('FFF_FF0', tipNode.tag)
### Compute tip node depth.
tipNodePos= tipNode.getInitialPos3d
tipNodeDepth= L2-tipNodePos.z
### vertical effective stress at pile tip, no water table (depth is embedded pile length)
sigVq= soilRho*g * tipNodeDepth # (N)
qult, z50q = pytzqzf.get_qz_param(phi= soilPhi, b= pileDiameter, sigV= sigVq, G= soilG)
# op.uniaxialMaterial('QzSimple1', 101, 2, qult, z50q) #, 0.0, 0.0
# op.uniaxialMaterial('TzSimple1', 101, 2, qult, z50q, 0.0)
qzMat= ssi.def_qzsimple1_material(preprocessor,
                                  matName= 'qzS1_'+str(tipNode.tag),
                                  qzType= 2, # 2 = Sand (Vijayvergiya, 1977 behavior).
                                  qult= qult, # Ultimate tip capacity.
                                  z50= z50q, # 50% capacity reached at 0.005 units of displacement
                                  suction= 0.0, # No suction during tension/uplift
                                  c= 0.0) # No viscous damping for this static test
pileTipFixNode= modelSpace.duplicateNode(tipNode)
modelSpace.fixNode('000_000', pileTipFixNode.tag)
modelSpace.setDefaultMaterial(qzMat)
modelSpace.setElementDimension(3)
pileTipSpring= modelSpace.newElement("ZeroLength", [pileTipFixNode.tag, tipNode.tag])
# Vertical direction.
pileTipSpring.setupVectors(xc.Vector([0,0,1]),xc.Vector([0,-1,0]))
pileTipSpring.clearMaterials()
pileTipSpring.setMaterial(0, qzMat.name)

# Create recorders.
domain= modelSpace.getDomain()

## Record displacements at pile nodes
displacements= dict()
pileNodeTags= pileSet.getNodeTags()
for tag in pileNodeTags:
    displacements[tag]= list()
dispRecorder= domain.newRecorder("node_prop_recorder", None)
dispRecorder.setNodes(xc.ID(pileNodeTags))
dispRecorder.callbackRecord= "displacements[self.tag].append((self.getDomain.getTimeTracker.getCurrentTime, self.getDisp.getList()))"
## Record reaction force in the p-y springs
reactions= dict()
springNodeTags= [n.tag for n in springNodes]
for tag in springNodeTags:
    reactions[tag]= list()
reacRecorder= domain.newRecorder("node_prop_recorder", None)
reacRecorder.setNodes(xc.ID(springNodeTags))
reacRecorder.callbackRecord= "self.getDomain.calculateNodalReactions(False, 1e-2); reactions[self.tag].append((self.getDomain.getTimeTracker.getCurrentTime, self.getReaction.getList()))"
# Record element forces in pile elements
internal_forces= dict()
pileElementTags= list(l1.getElementTags())
for tag in pileElementTags:
    internal_forces[tag]= list()
iForcesRecorder= domain.newRecorder("element_prop_recorder",None)
iForcesRecorder.setElements(xc.ID(pileElementTags))
iForcesRecorder.callbackRecord= "internal_forces[self.tag].append((self.getDomain.getTimeTracker.getCurrentTime, self.getNodeResistingForceIncInertia(1).getList()))"

# record information at specified increments
timeStep = 0.5

#----------------------------------------------------------
#  create the loading
#----------------------------------------------------------

domain.currentTime= 10.0 

## Apply point load at the uppermost pile node in the x-direction
factors = [0.0, 0.0, 1.0, 1.0]
time = [0.0, 10.0, 20.0, 10000.0]
ts1= modelSpace.newTimeSeries('ts1', "path_time_ts", setCurrent= True)
ts1.path= xc.Vector(factors)
ts1.time= xc.Vector(time)
lp1= modelSpace.newLoadPattern(name= 'lp1')
pileHeadNode= p3.getNode()
lp1.newNodalLoad(pileHeadNode.tag, xc.Vector([3500.0e3, 0.0, 0.0, 0.0, 0.0, 0.0]))
modelSpace.addLoadCaseToDomain(lp1.name)

# Solution procedure.
solProc= predefined_solutions.TransformationNewtonRaphsonBandGen(feProblem, convergenceTestTol= 1e-7, convTestType= 'norm_disp_incr_conv_test', maxNumIter= 100, printFlag= 0)
solProc.setup()
solProc.setDeltaLambda(.05)

solProc.getAnalysis().analyze(201)

# Check results.
uXExtrema= gmu.MinMaxTracker()
for key in displacements:
    nodeDisplacements= displacements[key]
    for (t, u) in nodeDisplacements:
        uXExtrema.update(u[0], t, key)

fXExtrema= gmu.MinMaxTracker()
for key in reactions:
    nodeReactions= reactions[key]
    for (t, R) in nodeReactions:
        fXExtrema.update(R[0], t, key)
        
uXMinRef= -0.0031199247973825983
tUXMinRef= 20.05
uXMaxRef= 0.4686389531954688
tUXMaxRef= 20.05
ratio1= abs(uXExtrema.min-uXMinRef)/uXMinRef # Minimum displacement.
ratio2= abs(uXExtrema.t_min-tUXMinRef)/tUXMinRef
ratio3= abs(uXExtrema.max-uXMaxRef)/uXMaxRef
ratio4= abs(uXExtrema.t_max-tUXMaxRef)/tUXMaxRef

fXMinRef= -643.4969113194871e3
tFXMinRef= 20.05
fXMaxRef= 484.5114254240712e3
tFXMaxRef= 20.05
ratio5= abs(fXExtrema.min-fXMinRef)/fXMinRef
ratio6= abs(fXExtrema.t_min-tFXMinRef)/tFXMinRef
ratio7= abs(fXExtrema.max-fXMaxRef)/fXMaxRef
ratio8= abs(fXExtrema.t_max-tFXMaxRef)/tFXMaxRef

if(not silent):
    # print('Extreme node displacements are: ', uXExtrema.getDict())
    print('Check displacements.')
    print('  Minimum displacement: ', uXExtrema.min*1e3, 'mm')
    print('  Expected minimum displacement: ', uXMinRef*1e3, 'mm')
    print('  Minimum displacement; ratio1= ', ratio1)
    print('  Time when minimum displacement occurs: ', uXExtrema.t_min, 's')
    print('  Expected time when minimum displacement occurs: ', tUXMinRef, 's')
    print('  Time of minimum displacement; ratio2= ', ratio2)
    print('\n  Maximum displacement: ', uXExtrema.max*1e3, 'mm')
    print('  Expected maximum displacement: ', uXMaxRef*1e3, 'mm')
    print('  Maximum displacement; ratio3= ', ratio3)
    print('  Time when maximum displacement occurs: ', uXExtrema.t_max, 's')
    print('  Expected time when maximum displacement occurs: ', tUXMaxRef, 's')
    print('  Time of minimum displacement; ratio4= ', ratio4)
    # print('Extreme node reactions are: ', fXExtrema.getDict())    
    print('\nCheck reactions.')
    print('  Minimum reaction: ', fXExtrema.min/1e3, 'kN')
    print('  Expected minimum reaction: ', fXMinRef/1e3, 'kN')
    print('  Expected minimum reaction; ratio5= ', ratio5)
    print('  Time when minimum reaction occurs: ', fXExtrema.t_min, 's')
    print('  Expected time when minimum reaction occurs: ', tFXMinRef, 's')
    print('  Time of minimum reaction; ratio6= ', ratio6)
    print('\n  Maximum reaction: ', fXExtrema.max/1e3, 'kN')
    print('  Expected maximum reaction: ', fXMaxRef/1e3, 'kN')
    print('  Expected maximum reaction; ratio7= ', ratio7)
    print('  Time when maximum reaction occurs: ', fXExtrema.t_max, 's')
    print('  Expected time when maximum reaction occurs: ', tFXMaxRef, 's')
    print('  Time of maximum reaction; ratio8= ', ratio8)
    
import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if((abs(ratio1)<1e-12) and (abs(ratio2)<1e-12) and (abs(ratio3)<1e-12) and (abs(ratio4)<1e-12) and (abs(ratio5)<1e-12) and (abs(ratio6)<1e-12) and (abs(ratio7)<1e-12) and (abs(ratio8)<1e-12)):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
    
# Graphic stuff.
## Compute spring stiffness for each node.
for n in l1.nodes:
    elements= n.getConnectedElements(springElementSet)
    for e in elements:
        materials= e.getMaterials()
        kx= materials[0].getTangent()
        n.setProp('k_x', kx)
        ky= materials[1].getTangent()
        n.setProp('k_y', ky)
    
if(not silent):
    from postprocess import output_handler
    oh= output_handler.OutputHandler(modelSpace)
    # oh.displayBlocks()
    # oh.displayFEMesh()
    # oh.displayLocalAxes()
    oh.displayLoads()
    oh.displayDispRot(itemToDisp='uX', defFScale= 10.0)
    oh.displayDispRot(itemToDisp='uY', defFScale= 10.0)
    oh.displayReactions()
    oh.displayNodeValueDiagram(itemToDisp= 'k_x', caption= 'Spring stiffness x direction', defaultDirection= 'X', defaultValue= 0.0)
    oh.displayNodeValueDiagram(itemToDisp= 'k_y', caption= 'Spring stiffness y direction', defaultDirection= 'Y', defaultValue= 0.0)
