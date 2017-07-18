# -*- coding: utf-8 -*-
''' Test made from the the book: 
    Introducción Al Estudio Del Elemento Finito en Ingeniería. 
    Segunda Edición. Tirupathi R. Chandrupatla, Ashok D. Belegundu. 
    Pearson Educación, 1999
    isbn={9789701702604},
    url={https://books.google.ch/books?id=auQwAAAACAAJ},
'''
E= 2.1e6 # Young modulus of the steel en kg/cm2.
nu= 0.3 # Poisson's ratio.
h= 0.1 # Espesor.
dens= 1.33 # Densidad kg/m2.

# Problem type
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

fName= "/tmp/test_exporta_med02.med"
spaceDim= -1
meshDim= -1
cooNames= None
unitsNames= None
coordenadas= None
numberOfCellTypes= None
cellTypes= None
cellTypeNames= None
numberOfCellsOfType= None
numNodesCeldaTipo= None
conectividad= None
numNodeGroups= None
numElementGroups= None

prueba= xc.ProblemaEF()
prueba.logFileName= "/tmp/borrar.log" # Ignore warning messages
preprocessor=  prueba.getPreprocessor
nodes= preprocessor.getNodeLoader
modelSpace= predefined_spaces.StructuralMechanics3D(nodes)

nodes.newNodeIDXYZ(1,0,0,0)
nodes.newNodeIDXYZ(2,2,0,0)
nodes.newNodeIDXYZ(3,2,1,0)
nodes.newNodeIDXYZ(4,0,1,0)
nodes.newNodeIDXYZ(5,3,0,0)
nodes.newNodeIDXYZ(6,3,1,0)


# Materials definition
memb1= typical_materials.defElasticMembranePlateSection(preprocessor, "memb1",E,nu,dens,h)
# Elements definition
elements= preprocessor.getElementLoader
elements.defaultMaterial= "memb1"
elem= elements.newElement("shell_mitc4",xc.ID([1,2,3,4]))
elem= elements.newElement("shell_mitc4",xc.ID([2,5,6,3]))



fields=  prueba.getFields
disp= fields.newField("disp")
disp.setName= "total"
disp.componentNames= ["UX","UY","UZ","RX","RY","RZ"]
disp.componentDescriptions= ["x disp.","y disp.","z disp.","x rot.","y rot.","z rot."]
disp.componentUnits= ["in","in","in","rad","rad","rad"]
disp.componentsProperty= "self.getDisp"


mesh_output= xc.MEDMeshing(prueba)
vertices= mesh_output.vertices
vertices.coordinateNames= ["X","Y","Z"]
vertices.coordinateUnits= ["in","in","in"]
mesh_output.meshName= "prueba02"
mesh_output.write(fName)

med_import= xc.MEDMesh()
med_import.meshName= "prueba02"
med_import.read(fName)
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


import os
os.system("rm -f "+fName)


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

from miscUtils import LogMessages as lmsg
fname= os.path.basename(__file__)
if((ratio1<1e-15) & (ratio2<1e-15)):
  print "test ",fname,": ok."
else:
  lmsg.error(fname+' ERROR.')
