# -*- coding: utf-8 -*-
# home made test.

import xc_base
import geom
import xc
from postprocess import med_xc_vars
from model import predefined_spaces
from materials import typical_materials

E= 30e6 # Young modulus (psi)
nu= 0.3 # Coeficiente de Poison
rho= 0.0 # Densidad

# Problem type
spaceDim= -1
meshDim= -1
cooNames= None
unitsNames= None
coordenadas= None
numeroTiposCelda= None
tiposCelda= None
nombresTiposCelda= None
numeroCeldasTipo= None
numNodosCeldaTipo= None
conectividad= None
numGruposNodos= None
numGruposElementos= None

prueba= xc.ProblemaEF()
prueba.logFileName= "/tmp/borrar.log" # Para no imprimir mensajes de advertencia
mdlr= prueba.getModelador
nodos= mdlr.getNodeLoader
predefined_spaces.gdls_elasticidad2D(nodos)


nodos.defaultTag= 101
nodos.newNodeXY(0,0)
nodos.defaultTag= 102 
nod2= nodos.newNodeXY(2,0)
nod2.setTrialDisp(xc.Vector([0.002,0.003]))
nodos.defaultTag= 103
nod3= nodos.newNodeXY(2,1) 
nod3.setTrialDisp(xc.Vector([0.006,0.0032]))
nodos.defaultTag= 104
nod4= nodos.newNodeXY(0,1)


elast2d= typical_materials.defElasticIsotropicPlaneStress(mdlr,"elast2d",E,nu,rho)
# Elements definition
elementos= mdlr.getElementLoader
elementos.defaultMaterial= "elast2d"
quad4n= elementos.newElement("quad4n",xc.ID([101,102,103,104]))
quad4n.commitState()

ele0= elementos.getElement(0)
detJ= ele0.detJ(0.0,0.0)
sgMed1= ele0.getPhysicalProperties.getCommittedAvgStress[0]
sgMed2= ele0.getPhysicalProperties.getCommittedAvgStress[1]
sgMed3= ele0.getPhysicalProperties.getCommittedAvgStress[2]

fields=  prueba.getFields

disp= fields.newField("disp")
disp.setName= "total"
disp.componentNames= ["UX","UY"]
disp.componentDescriptions= ["Mov. según x","Mov. según y"]
disp.componentUnits= ["in","in"]
disp.componentsProperty= "self.getDisp"


areas= fields.newField("areas")
areas.setName= "total"
areas.definedOnElements()
areas.componentNames= ["A"]
areas.componentDescriptions= ["Area elemento"]
areas.componentUnits= ["in2"]
areas.componentsProperty= "self.getArea(True)"

avgStress= fields.newField("avgStress")
avgStress.setName= "total"
avgStress.definedOnGaussPoints()
avgStress.componentNames= ["avgStress"]
avgStress.componentDescriptions= ["Tensión media"]
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
numeroTiposCelda= med_import.getNumberOfCellTypes()
tiposCelda= med_import.getCellTypes()
nombresTiposCelda= med_import.getCellTypeNames
numeroCeldasTipo= med_import.getNumCellsOfType(tiposCelda[0])
numNodosCeldaTipo= tiposCelda[0]%100
conectividad= med_import.getConnectivityCellsOfType(tiposCelda[0])
numGruposNodos= med_import.getNumberOfGroups(xc.MED_NODE)
numGruposElementos= med_import.getNumberOfGroups(xc.MED_CELL)



''' 
print "spaceDim= ",spaceDim
print "meshDim= ",meshDim
print "cooNames= ",cooNames
print "unitsNames= ",unitsNames
print "coordenadas= ",coordenadas
print "numeroTiposCelda= ",numeroTiposCelda
print "tiposCelda= ",tiposCelda
print "nombresTiposCelda= ",nombresTiposCelda
print "numeroCeldasTipo(",tiposCelda[0],")= ",numeroCeldasTipo
print "numeroNodosCeldaTipo(",tiposCelda[0],")= ",numNodosCeldaTipo
print "conectividad= ",conectividad
print "numGruposNodos= ",numGruposNodos
print "numGruposElementos= ",numGruposElementos
   '''

ratio1= spaceDim-3
ratio2= meshDim-2

import os
fname= os.path.basename(__file__)
if((ratio1<1e-15) & (ratio2<1e-15)):
  print "test ",fname,": ok."
else:
  print "test ",fname,": ERROR."

