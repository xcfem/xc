# -*- coding: utf-8 -*-
''' Verify computation of Mononobe-Okabe pressures on a finite element mesh.'''

from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2022, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import math
import geom
import xc
from model import predefined_spaces
from materials.ec2 import EC2_materials
from materials.sections.fiber_section import def_simple_RC_section
from actions import load_cases
from misc_utils import log_messages as lmsg
from actions.earth_pressure import earth_pressure

# =============================================================================
# =============================================================================
#  __                                                   
# |_  . _  ._|_ _     _ | _ __  _  _ _|_   __  _  _| _ |
# |   || | | |_(/_   (/_|(/_|||(/_| | |_   |||(_)(_|(/_|
#
# =============================================================================
# =============================================================================
# Problem type
feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor
nodes= preprocessor.getNodeHandler

modelSpace= predefined_spaces.StructuralMechanics3D(nodes)

# Define materials
## Materials.
concrete= EC2_materials.C30
steel= EC2_materials.S500B
## Geometry
h= 0.40
## RC section.
rcSection= def_simple_RC_section.RCRectangularSection(name='WallSection', width= 1.0, depth= h, concrType= concrete, reinfSteelType= steel)
dummySection= rcSection.defElasticMembranePlateSection(preprocessor) # Elastic membrane plate section.

# Problem geometry.
wallHeight= 4.5
wallLength= 10
## K-points.
points= preprocessor.getMultiBlockTopology.getPoints
pt1= points.newPoint(geom.Pos3d(0.0,0.0,0.0))
pt2= points.newPoint(geom.Pos3d(wallLength,0.0,0.0))
pt3= points.newPoint(geom.Pos3d(wallLength,0.0,wallHeight))
pt4= points.newPoint(geom.Pos3d(0.0,0.0,wallHeight))
## Surface.
surfaces= preprocessor.getMultiBlockTopology.getSurfaces
s= surfaces.newQuadSurfacePts(pt1.tag,pt2.tag,pt3.tag,pt4.tag)
s.nDivI= 30
s.nDivJ= 10

# Generate mesh.
seedElemHandler= preprocessor.getElementHandler.seedElemHandler
seedElemHandler.defaultMaterial= rcSection.name
elem= seedElemHandler.newElement("ShellMITC4")
s.genMesh(xc.meshDir.I)

# Constraints.
fixedNodes= list()
for n in s.nodes:
    pos= n.getInitialPos3d
    if(abs(pos.z)<1e-3):
        fixedNodes.append(n)
for n in fixedNodes:
    modelSpace.fixNode000_0FF(n.tag)

# Actions
# =============================================================================
# =============================================================================
#                                    _
#      |V| _  _  _  _  _ |_  _  --- / \|  _ |_  _
#      | |(_)| |(_)| |(_)|_)(/_     \_/|<(_||_)(/_
# =============================================================================
# =============================================================================
# Define load case.
loadCaseManager= load_cases.LoadCaseManager(preprocessor)
loadCaseNames= ['earthquake']
loadCaseManager.defineSimpleLoadCases(loadCaseNames)
cLC= loadCaseManager.setCurrentLoadCase('earthquake')

zGround= wallHeight # m z coordinate of the ground surface.
gammaSoil= 21e3 # N/m3 unit weight of soil (backfill)
H= wallHeight # m height of the structure.
kh= 0.086 # seismic coefficient of horizontal acceleration.
kv= kh/2.0 # seismic coefficient of vertical acceleration.
psi= math.radians(90) # back face inclination of the structure (<= PI/2)
phi= math.radians(30.0) # angle of internal friction of soil.
delta_ad= 0.0 # angle of friction soil - structure.
beta= 0.0 # slope inclination of backfill.
Kas= 1/3.0 # static earth pressure coefficient 

mononobeOkabe= earth_pressure.MononobeOkabePressureDistribution(zGround= zGround, gammaSoil= gammaSoil, H= H, kv= kv, kh= kh, psi= psi, phi= phi, delta_ad= delta_ad, beta= 0.0, Kas= Kas)

mononobeOkabe.appendLoadToCurrentLoadPattern(xcSet= s, vDir= xc.Vector([0,-1,0]), iCoo= 2)
modelSpace.addLoadCaseToDomain(cLC.name)

# Solution
result= modelSpace.analyze(calculateNodalReactions= True)
if(result!=0):
    lmsg.error('Can\'t solve.')
    
# Compute reaction
reaction= xc.Vector(6*[0.0])
refValue= xc.Vector([0, 144180, 0, -431457, 0, 0])
for n in fixedNodes:
    nReac= n.getReaction
    reaction+= nReac

# Check result.
err= (reaction-refValue).Norm()/refValue.Norm()

# print(reaction)
# print(err)

# # #########################################################
# # # Graphic stuff.
# from postprocess import output_handler
# oh= output_handler.OutputHandler(modelSpace)

# # oh.displayFEMesh()
# # oh.displayLocalAxes()
# oh.displayLoads()

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if(err<1e-5):
    print('test: '+fname+': ok.')
else:
    lmsg.error(fname+": ERROR.")
