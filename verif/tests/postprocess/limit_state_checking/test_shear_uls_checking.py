# -*- coding: utf-8 -*-

'''Limit state controller for shear.'''

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2015, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import math
import xc_base
import geom
import xc
from solution import predefined_solutions
from model import predefined_spaces
from materials import typical_materials
from materials.ehe import EHE_materials
from materials.ehe import EHE_limit_state_checking
from materials.sections import section_properties
from actions import combinations as combs
from postprocess import limit_state_data as lsd
from postprocess import RC_material_distribution
from materials.sections.fiber_section import defSimpleRCSection
import sys

#Hide logging messages from modules.
sysstdout = sys.stdout
logFile= open('/tmp/test.log', 'a')
sys.stdout = sys.stderr = logFile

# Geometry
L= 1.0 # Bar length (m)

feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor
nodes= preprocessor.getNodeLoader

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

elements= preprocessor.getElementLoader
elements.defaultTransformation= "lin"
elements.defaultMaterial= section.name
e1= elements.newElement("ElasticBeam3d",xc.ID([n1.tag,n2.tag]));
e2= elements.newElement("ElasticBeam3d",xc.ID([n2.tag,n3.tag]));

#Constraints.
modelSpace.fixNode000_000(n1.tag)

#Loads.
Fx= -400e3 # Axial force for shear checking.
Fz= 1e3 # Bending moment force for shear checking.
Fy= 1e5 # Bending moment force for shear checking.
cargas= preprocessor.getLoadLoader
casos= cargas.getLoadPatterns
#Load modulation.
ts= casos.newTimeSeries("constant_ts","ts")
casos.currentTimeSeries= "ts"
#Load case definition
lp0= casos.newLoadPattern("default","lp0")
lp0.newNodalLoad(n3.tag,xc.Vector([Fx,Fy,Fz,0,0,0]))
#We add the load case to domain.
casos.addToDomain(lp0.getName())

# # Solution
# analisis= predefined_solutions.simple_static_linear(feProblem)
# result= analisis.analyze(1)

# Load combinations
combContainer= combs.CombContainer()
combContainer.ULS.perm.add('allLoads', '1.0*lp0')
totalSet= preprocessor.getSets.getSet('total')
lsd.LimitStateData.internal_forces_results_directory= '/tmp/'
lsd.shearResistance.saveAll(feProblem,combContainer,totalSet) 

# Define available sections for the elements (spatial distribution of RC sections).
# It refers to the reinforced concrete sections associated with the element
# (i.e. for shell elements we typically define two RC sections, one for each
# main direction; in the case of beam elements the most common way is to define
# RC sections in the front and back ends of the elements)
reinfConcreteSectionDistribution= RC_material_distribution.RCMaterialDistribution()
sections= reinfConcreteSectionDistribution.sectionDefinition #creates an RC sections container

#Generic layers (rows of rebars). Other instance variables that we can define
#for MainReinfLayers are coverLat and nRebars.If we define nRebars that
#value overrides the rebarsSpacing
barArea= 4e-4
barDiameter= math.sqrt(barArea)/math.pi

reinfLayer= defSimpleRCSection.MainReinfLayer(rebarsDiam= barDiameter,areaRebar= barArea,rebarsSpacing=0.075,width=0.25,nominalCover=0.050)

#instances of defSimpleRCSection.RecordRCSlabBeamSection that defines the
#variables that make up THE TWO reinforced concrete sections in the two
#reinforcement directions of a slab or the front and back ending sections
#of a beam element
reinfSteel= EHE_materials.B500S
beamRCsect= defSimpleRCSection.RecordRCSlabBeamSection(name='beamRCsect',sectionDescr='beam section',concrType=concr, reinfSteelType=reinfSteel,width= sectionGeometry.b,depth= sectionGeometry.h)
beamRCsect.lstRCSects[0].positvRebarRows=[reinfLayer]
beamRCsect.lstRCSects[0].negatvRebarRows=[reinfLayer]
beamRCsect.lstRCSects[1].positvRebarRows=[reinfLayer]
beamRCsect.lstRCSects[1].negatvRebarRows=[reinfLayer]
sections.append(beamRCsect)

# Spatial distribution of reinforced concrete
# sections (assign RC sections to elements).
reinfConcreteSectionDistribution.assign(elemSet=totalSet.getElements,setRCSects=beamRCsect)

#Checking shear.
lsd.shearResistance.controller= EHE_limit_state_checking.ShearController(limitStateLabel= lsd.shearResistance.label)
lsd.shearResistance.controller.analysisToPerform= predefined_solutions.simple_newton_raphson


(FEcheckedModel,meanFCs)= reinfConcreteSectionDistribution.runChecking(lsd.shearResistance,outputFileName='/tmp/resVerif', matDiagType="d",threeDim= True)  

#print "mean FCs: ", meanFCs

meanFC0Teor= 0.89306075607898694
ratio1= abs(meanFCs[0]-meanFC0Teor)/meanFC0Teor
meanFC1Teor= 0.97448959156755022
ratio2= abs(meanFCs[1]-meanFC1Teor)/meanFC1Teor

'''
print "meanFCs[0]= ", meanFCs[0]
print "ratio1= ",ratio1
print "meanFCs[1]= ", meanFCs[1]
print "ratio2= ",ratio2
'''

# Show logging messages.
sys.stdout = sysstdout
import os
fname= os.path.basename(__file__)
if (ratio1<0.01) & (ratio2<0.01):
  print "test ",fname,": ok."
else:
  lmsg.error(fname+' ERROR.')
