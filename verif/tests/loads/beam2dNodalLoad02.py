# -*- coding: utf-8 -*-
# home made test
''' Reference:  Expresiones de la flecha el el Prontuario de
Estructuras Metálicas del CEDEX. Apartado 3.3 Carga puntual sobre ménsula. '''

import xc_base
import geom
import xc
from solution import predefined_solutions
from model import predefined_spaces
from materials import typical_materials
from materials import section_properties
from materials import defSeccAggregation

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2015, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

E= 2e9 # Elastic modulus
L= 20 # Bar length.
h= 0.30 # Beam cross-section depth.
b= 0.2 # Beam cross-section width.
A= b*h # Cross section area.
I= b*h**3/12 # Inercia de la viga in inches a la cuarta
x= 0.5 # Abcisa relativa en la que se aplica la carga puntual.
P= 1e3 # Carga puntual.

# Problem type
prueba= xc.ProblemaEF()
preprocessor=  prueba.getPreprocessor   
nodes= preprocessor.getNodeLoader

sccPrueba= section_properties.RectangularSection("prueba",b,h)
matSccPrueba=typical_materials.MaterialData(name='matSec',E=E,nu=0.3,rho=2500)

modelSpace= predefined_spaces.StructuralMechanics2D(nodes)
# Definimos el material
defSeccAggregation.defSeccAggregation2d(preprocessor, sccPrueba,matSccPrueba)
nodes.defaultTag= 1 #First node number.
nod= nodes.newNodeXY(0,0)
nod= nodes.newNodeXY(L,0.0)

# Geometric transformation(s)
trfs= preprocessor.getTransfCooLoader
lin= trfs.newLinearCrdTransf2d("lin")

    
# Materials definition
scc= typical_materials.defElasticSection2d(preprocessor, "scc",A,E,I)


# Elements definition
elements= preprocessor.getElementLoader
elements.defaultTransformation= "lin"# Coordinate transformation for the new elements
elements.defaultMaterial= sccPrueba.sectionName
elements.defaultTag= 1 #Tag for next element.
beam2d= elements.newElement("force_beam_column_2d",xc.ID([1,2]))
    
# Constraints
constraints= preprocessor.getConstraintLoader
modelSpace.fixNode000(1)


# Loads definition
cargas= preprocessor.getLoadLoader
casos= cargas.getLoadPatterns
#Load modulation.
ts= casos.newTimeSeries("constant_ts","ts")
casos.currentTimeSeries= "ts"
#Load case definition
lp0= casos.newLoadPattern("default","0")
lp0.newNodalLoad(2,xc.Vector([0,-P,0]))
#We add the load case to domain.
casos.addToDomain("0")

# Solution
analisis= predefined_solutions.simple_static_linear(prueba)
result= analisis.analyze(1)


nod2= nodes.getNode(2)
delta= nod2.getDisp[1] 


deltaTeor= (-P*L**3/3/E/I)
ratio1= ((delta-deltaTeor)/deltaTeor)

''' 
print "delta= ",delta
print "deltaTeor= ",deltaTeor
print "ratio1= ",ratio1
 '''

import os
from miscUtils import LogMessages as lmsg
fname= os.path.basename(__file__)
if abs(ratio1)<5e-4:
  print "test ",fname,": ok."
else:
  lmsg.error(fname+' ERROR.')
