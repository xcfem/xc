# -*- coding: utf-8 -*-
'''Limit state controller for shear. Home made test.'''

from __future__ import print_function
from __future__ import division

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AO_O)"
__copyright__= "Copyright 2015, LCPT and AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com ana.ortega@ciccp.es"

import math
import xc
from solution import predefined_solutions
from model import predefined_spaces
from materials.ehe import EHE_materials
from materials.ehe import EHE_limit_state_checking
from materials.sections import section_properties
from actions import combinations as combs
from postprocess import limit_state_data as lsd
from postprocess import RC_material_distribution
from materials.sections.fiber_section import def_simple_RC_section
from postprocess import element_section_map
from postprocess.config import default_config
import os
import logging

from misc_utils import log_messages as lmsg

#Hide INFO messages from modules.
rootLogger = logging.getLogger()
rootLogger.setLevel(logging.ERROR)


# Geometry
L= 1.0 # Bar length (m)

feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor
nodes= preprocessor.getNodeHandler

# Materials
sectionGeometry= section_properties.RectangularSection("test",b=.3,h=.4)
concr= EHE_materials.HA25
concr.alfacc=0.85    #f_maxd= 0.85*fcd concrete long term compressive strength factor (normally alfacc=1)
section= concr.defElasticShearSection3d(preprocessor, sectionGeometry)

# Problem type
modelSpace= predefined_spaces.StructuralMechanics3D(nodes)

#Mesh.
n1= nodes.newNodeXYZ(0,0.0,0.0)
n2= nodes.newNodeXYZ(L/2.0,0.0,0.0)
n3= nodes.newNodeXYZ(L,0.0,0.0)

lin= modelSpace.newLinearCrdTransf("lin",xc.Vector([0,1,0]))

elements= preprocessor.getElementHandler
elements.defaultTransformation= lin.name
elements.defaultMaterial= section.name
e1= elements.newElement("ElasticBeam3d",xc.ID([n1.tag,n2.tag]))
e2= elements.newElement("ElasticBeam3d",xc.ID([n2.tag,n3.tag]))

#Constraints.
modelSpace.fixNode000_000(n1.tag)

#Loads.
Fx= -400e3 # Axial force for shear checking.
Fz= 1e3 # Bending moment force for shear checking.
Fy= 1e5 # Bending moment force for shear checking.
loadHandler= preprocessor.getLoadHandler
lPatterns= loadHandler.getLoadPatterns
#Load modulation.
ts= lPatterns.newTimeSeries("constant_ts","ts")
lPatterns.currentTimeSeries= "ts"
#Load case definition
lp0= lPatterns.newLoadPattern("default","lp0")
lp0.newNodalLoad(n3.tag,xc.Vector([Fx,Fy,Fz,0,0,0]))
#We add the load case to domain.
lPatterns.addToDomain(lp0.getName())

# # Solution
# analysis= predefined_solutions.simple_static_linear(feProblem)
# result= analysis.analyze(1)

# Load combinations
combContainer= combs.CombContainer()
combContainer.ULS.perm.add('allLoads', '1.0*lp0')
totalSet= preprocessor.getSets.getSet('total')
cfg= default_config.get_temporary_env_config()
lsd.LimitStateData.envConfig= cfg
lsd.shearResistance.analyzeLoadCombinations(combContainer,totalSet) 

# Define available sections for the elements (spatial distribution of RC sections).
# It refers to the reinforced concrete sections associated with the element
# (i.e. for shell elements we typically define two RC sections, one for each
# main direction; in the case of beam elements the most common way is to define
# RC sections in the front and back ends of the elements)
reinfConcreteSectionDistribution= RC_material_distribution.RCMaterialDistribution()
sections= reinfConcreteSectionDistribution.sectionDefinition # get the sections container

#Generic layers (rows of rebars). Other instance variables that we can define
#for ReinfRows are coverLat and nRebars.If we define nRebars that
#value overrides the rebarsSpacing
barArea= 4e-4

reinfLayer= def_simple_RC_section.ReinfRow(areaRebar= barArea,rebarsSpacing=0.075,width=0.25,nominalCover=0.050)

#instances of element_section_map.RCSlabBeamSection that defines the
#variables that make up THE TWO reinforced concrete sections in the two
#reinforcement directions of a slab or the front and back ending sections
#of a beam element
reinfSteel= EHE_materials.B500S
beamRCsect= element_section_map.RCSlabBeamSection(name='beamRCsect',sectionDescr='beam section',concrType=concr, reinfSteelType=reinfSteel,width= sectionGeometry.b,depth= sectionGeometry.h)
beamRCsect.dir1PositvRebarRows= def_simple_RC_section.LongReinfLayers([reinfLayer])
beamRCsect.dir1NegatvRebarRows= def_simple_RC_section.LongReinfLayers([reinfLayer])
beamRCsect.dir2PositvRebarRows= def_simple_RC_section.LongReinfLayers([reinfLayer])
beamRCsect.dir2NegatvRebarRows= def_simple_RC_section.LongReinfLayers([reinfLayer])
sections.append(beamRCsect)

# Spatial distribution of reinforced concrete
# sections (assign RC sections to elements).
reinfConcreteSectionDistribution.assign(elemSet=totalSet.getElements,setRCSects=beamRCsect)

# Checking shear.
## Limit state to check.
limitState= lsd.shearResistance
## Build controller.
controller= EHE_limit_state_checking.ShearController(limitStateLabel= limitState.label)
controller.analysisToPerform= predefined_solutions.plain_newton_raphson
## Perform checking.
meanCFs= limitState.check(setCalc= None, crossSections= reinfConcreteSectionDistribution, listFile='N',calcMeanCF='Y',threeDim= True, controller= controller)

#print("mean FCs: ", meanCFs)

# Check results.
refMeanFC0= 0.9010407567556409
ratio1= abs(meanCFs[0]-refMeanFC0)/refMeanFC0
refMeanFC1= 1.0284811082983671
ratio2= abs(meanCFs[1]-refMeanFC1)/refMeanFC1

'''
print("meanCFs[0]= ", meanCFs[0])
print("ratio1= ",ratio1)
print("meanCFs[1]= ", meanCFs[1])
print("ratio2= ",ratio2)
'''

# Show logging messages.
cfg.cleandirs()  # Clean after yourself.
fname= os.path.basename(__file__)
if (ratio1<0.01) & (ratio2<0.01):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
