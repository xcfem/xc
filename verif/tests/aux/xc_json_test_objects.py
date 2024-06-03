# -*- coding: utf-8 -*-

__author__= "Luis C. Pérez Tato (LCPT)"
__cppyright__= "Copyright 2024 LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import math
import xc
from materials.ehe import EHE_materials
from materials.sections.fiber_section import def_simple_RC_section
from materials.sections.fiber_section import def_column_RC_section
from materials.sections import RC_sections_container
from postprocess import element_section_map
from postprocess import RC_material_distribution
from model import predefined_spaces
from materials import typical_materials

reinfRow= def_simple_RC_section.ReinfRow(areaRebar= 0.01, width= 0.25, nRebars= 4, nominalCover= 0.035, nominalLatCover= 0.04)

eheConcrete= EHE_materials.HA30
eheSteel= EHE_materials.B500S

rectgRCSection= def_simple_RC_section.RCRectangularSection(name= 'rectgRCSection', sectionDescr= 'rectangular RC section', concrType= eheConcrete, reinfSteelType= eheSteel, width= 1.5, depth= 1.5)

circularRCSection= def_column_RC_section.RCCircularSection(name='circularRCSection', Rext= 0.35/2.0, concrType= eheConcrete, reinfSteelType= eheSteel)

areaFi10= EHE_materials.Fi10
areaFi16= EHE_materials.Fi16
fi10s75r30= def_simple_RC_section.ReinfRow(areaRebar= areaFi10,rebarsSpacing=0.075,width=0.25,nominalCover=0.030)
fi16s75r30= def_simple_RC_section.ReinfRow(areaRebar= areaFi16,rebarsSpacing=0.075,width=0.25,nominalCover=0.030)
beamRCsect=element_section_map.RCSlabBeamSection(name='beamRCsect',sectionDescr='beam section',concrType= eheConcrete, reinfSteelType= eheSteel, width= 0.25, depth= 0.5)
beamRCsect.dir1PositvRebarRows= def_simple_RC_section.LongReinfLayers([fi10s75r30])
beamRCsect.dir1NegatvRebarRows= def_simple_RC_section.LongReinfLayers([fi16s75r30])
beamRCsect.dir2PositvRebarRows= def_simple_RC_section.LongReinfLayers([fi10s75r30])
beamRCsect.dir2NegatvRebarRows= def_simple_RC_section.LongReinfLayers([fi16s75r30])

sectionContainer= RC_sections_container.SectionContainer()
sectionContainer.append(beamRCsect)

reinfConcreteSectionDistribution= RC_material_distribution.RCMaterialDistribution()
reinfConcreteSectionDistribution.sectionDefinition= sectionContainer

# Create elements to assign the sections to.
feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor   
nodes= preprocessor.getNodeHandler
## Problem type
modelSpace= predefined_spaces.StructuralMechanics3D(nodes)

## Problem geometry
n0= nodes.newNodeXYZ(0,0.0,0.0)
n1= nodes.newNodeXYZ(1,0.0,0.0)
n2= nodes.newNodeXYZ(0.0,1,0.0)
n3= nodes.newNodeXYZ(0.0,0.0,1)
## Geometric transformations
ltXbeam= modelSpace.newLinearCrdTransf("ltXbeam", xc.Vector([0,-1,0])) 
ltYbeam= modelSpace.newLinearCrdTransf("ltYbeam",xc.Vector([0,0,-1]))
ltZbeam= modelSpace.newLinearCrdTransf("ltZbeam",xc.Vector([1/math.sqrt(2),1/math.sqrt(2),0,1]))
IzElem=beamRCsect.lstRCSects[0].getIz_RClocalZax()
IyElem=beamRCsect.lstRCSects[0].getIy_RClocalYax()
JElem= beamRCsect.lstRCSects[0].getJTorsion()   
scc= typical_materials.defElasticSection3d(preprocessor=preprocessor, name= "scc", A= beamRCsect.lstRCSects[0].getAc(),E= beamRCsect.lstRCSects[0].getConcreteType().Ecm(),G= beamRCsect.lstRCSects[0].getConcreteType().Gcm(),Iz= IzElem,Iy= IyElem,J= JElem)
## Elements definition
elements= preprocessor.getElementHandler
elements.defaultMaterial= scc.name

elements.defaultTransformation= ltXbeam.name
beam3dX= elements.newElement("ElasticBeam3d",xc.ID([n0.tag,n1.tag]))
elements.defaultTransformation= ltYbeam.name
beam3dY= elements.newElement("ElasticBeam3d",xc.ID([n0.tag,n2.tag]))
elements.defaultTransformation= ltZbeam.name
beam3dZ= elements.newElement("ElasticBeam3d",xc.ID([n0.tag,n3.tag]))

xcTotalSet= modelSpace.getTotalSet()

reinfConcreteSectionDistribution.assign(elemSet= xcTotalSet.elements, setRCSects=beamRCsect)

data= {'a list': [1, 42, 3.141, 1337, 'help', u'€'],
        'a string': 'bla',
        'another dict': {'foo': 'bar',
                         'key': 'value',
                         'the answer': 42},
       'reinforcement_row': reinfRow,
       'ehe_concrete': eheConcrete,
       'ehe_steel': eheSteel,
       'rectangular_rc_section': rectgRCSection,
       'circular_rc_section': circularRCSection,
       'copy_rectangular_rc_section': rectgRCSection,
       'section_container':sectionContainer,
       'section_distribution': reinfConcreteSectionDistribution}
