# -*- coding: utf-8 -*-
# home made test.

import xc_base
import geom
import xc
from postprocess import med_xc_vars
from model import predefined_spaces
from materials import typical_materials

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2014, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

E= 30e6 # Young modulus (psi)
nu= 0.3 # Poisson's ratio
rho= 0.0 # Densidad

# Problem type
spaceDim= -1
meshDim= -1
cooNames= None
unitsNames= None
coordenadas= None
numberOfCellTypes= None
cellTypes= None
cellTypeNames= None
numberOfCellsOfType= None
numNodesTypeCell= None
conectividad= None
numNodeGroups= None
numElementGroups= None

prueba= xc.ProblemaEF()
prueba.logFileName= "/tmp/borrar.log" # Para no imprimir mensajes de advertencia
preprocessor=  prueba.getPreprocessor
nodes= preprocessor.getNodeLoader
modelSpace= predefined_spaces.SolidMechanics2D(nodes)


nodes.defaultTag= 101
nodes.newNodeXY(0,0)
nodes.defaultTag= 102 
nod2= nodes.newNodeXY(2,0)
nod2.setTrialDisp(xc.Vector([0.002,0.003]))
nodes.defaultTag= 103
nod3= nodes.newNodeXY(2,1) 
nod3.setTrialDisp(xc.Vector([0.006,0.0032]))
nodes.defaultTag= 104
nod4= nodes.newNodeXY(0,1)


elast2d= typical_materials.defElasticIsotropicPlaneStress(preprocessor, "elast2d",E,nu,rho)
# Elements definition
elements= preprocessor.getElementLoader
elements.defaultMaterial= "elast2d"
quad4n= elements.newElement("quad4n",xc.ID([101,102,103,104]))
quad4n.commitState()

ele0= elements.getElement(0)
detJ= ele0.detJ(0.0,0.0)
sgMed1= ele0.getPhysicalProperties.getCommittedAvgStress[0]
sgMed2= ele0.getPhysicalProperties.getCommittedAvgStress[1]
sgMed3= ele0.getPhysicalProperties.getCommittedAvgStress[2]

fields=  prueba.getFields

disp= fields.newField("disp")
disp.setName= "total"
disp.componentNames= ["UX","UY"]
disp.componentDescriptions= ["x disp.","y disp"]
disp.componentUnits= ["in","in"]
disp.componentsProperty= "self.getDisp"


areas= fields.newField("areas")
areas.setName= "total"
areas.definedOnElements()
areas.componentNames= ["A"]
areas.componentDescriptions= ["Element area"]
areas.componentUnits= ["in2"]
areas.componentsProperty= "self.getArea(True)"

avgStress= fields.newField("avgStress")
avgStress.setName= "total"
avgStress.definedOnGaussPoints()
avgStress.componentNames= ["avgStress"]
avgStress.componentDescriptions= ["Average stress"]
avgStress.componentUnits= ["lbs/in2"]
avgStress.componentsProperty= "self.getPhysicalProperties.getCommittedStress"

mesh_output= xc.MEDMeshing(prueba)
vertices= mesh_output.vertices
vertices.coordinateNames= ["X","Y","Z"]
vertices.coordinateUnits= ["in","in","in"]
mesh_output.meshName= "prueba01"
mesh_output.write("/tmp/test_exporta_med01.med")

med_import= xc.MEDMesh()
med_import.meshName= "prueba01"
med_import.read("/tmp/test_exporta_med01.med")
spaceDim= med_import.getSpaceDimension()
meshDim= med_import.getMeshDimension()
cooNames= med_import.getCoordinatesNames()
unitsNames= med_import.getCoordinatesUnits()
coordenadas= med_import.getCoordinates()
numberOfCellTypes= med_import.getNumberOfCellTypes()
cellTypes= med_import.getCellTypes()
cellTypeNames= med_import.getCellTypeNames
numberOfCellsOfType= med_import.getNumCellsOfType(cellTypes[0])
numNodesTypeCell= cellTypes[0]%100
conectividad= med_import.getConnectivityCellsOfType(cellTypes[0])
numNodeGroups= med_import.getNumberOfGroups(xc.MED_NODE)
numElementGroups= med_import.getNumberOfGroups(xc.MED_CELL)



''' 
print "spaceDim= ",spaceDim
print "meshDim= ",meshDim
print "cooNames= ",cooNames
print "unitsNames= ",unitsNames
print "coordenadas= ",coordenadas
print "numberOfCellTypes= ",numberOfCellTypes
print "cellTypes= ",cellTypes
print "cellTypeNames= ",cellTypeNames
print "numberOfCellsOfType(",cellTypes[0],")= ",numberOfCellsOfType
print "numNodesTypeCell(",cellTypes[0],")= ",numNodesTypeCell
print "conectividad= ",conectividad
print "numNodeGroups= ",numNodeGroups
print "numElementGroups= ",numElementGroups
   '''

ratio1= spaceDim-3
ratio2= meshDim-2

import os
from miscUtils import LogMessages as lmsg
fname= os.path.basename(__file__)
if((ratio1<1e-15) & (ratio2<1e-15)):
  print "test ",fname,": ok."
else:
  lmsg.error(fname+' ERROR.')

