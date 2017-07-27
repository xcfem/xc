# -*- coding: utf-8 -*-
''' Test made from the example 7.2 of the book: 
    Introducción Al Estudio Del Elemento Finito en Ingeniería. 
    Segunda Edición. Tirupathi R. Chandrupatla, Ashok D. Belegundu. 
    Pearson Educación, 1999
    isbn={9789701702604},
    url={https://books.google.ch/books?id=auQwAAAACAAJ},
'''

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2015, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import xc_base
import geom
import xc
from solution import predefined_solutions
from model import predefined_spaces
from materials import typical_materials

E= 30e6 # Young modulus (psi)
nu= 0.3 # Coeficiente de Poison
rho= 0.0 # Densidad

# Problem type
prueba= xc.ProblemaEF()
prueba.logFileName= "/tmp/borrar.log" # Don't pring warnings
preprocessor=  prueba.getPreprocessor
nodes= preprocessor.getNodeLoader
modelSpace= predefined_spaces.SolidMechanics2D(nodes)


n1= nodes.newNodeIDXY(1,0,0)
n2= nodes.newNodeIDXY(2,2,0)
n2.setTrialDisp(xc.Vector([0.002,0.003]))
n3= nodes.newNodeIDXY(3,2,1)
n3.setTrialDisp(xc.Vector([0.006,0.0032]))
n4= nodes.newNodeIDXY(4,0,1)


elast2d= typical_materials.defElasticIsotropicPlaneStress(preprocessor, "elast2d",E,nu,rho)
# Elements definition
elements= preprocessor.getElementLoader
elements.defaultMaterial= "elast2d"
quad4n= elements.newElement("quad4n",xc.ID([1,2,3,4]))

preprocessor.getDomain.commit()
detJ= quad4n.detJ(0.0,0.0)
avgStress= quad4n.getPhysicalProperties.getCommittedAvgStress
sgMed1= avgStress[0]
sgMed2= avgStress[1]
sgMed3= avgStress[2]

ratio1= (detJ-0.5)/0.5
ratio2= (sgMed1-66920)/66920
ratio3= (sgMed2-23080)/23080
ratio4= (sgMed3-40960)/40960

''' 
print "detJ= ",(detJ)
print "sg1= ",(sgMed1)
print "sg2= ",(sgMed2)
print "sg3= ",(sgMed3)
print "ratio1= ",(ratio1)
print "ratio2= ",(ratio2)
print "ratio3= ",(ratio3)
print "ratio4= ",(ratio4)
 '''

import os
from miscUtils import LogMessages as lmsg
fname= os.path.basename(__file__)
if (abs(ratio1)<1e-9) & (abs(ratio2)<1e-3) & (abs(ratio3)<1e-3) & (abs(ratio4)<1e-3):
  print "test ",fname,": ok."
else:
  lmsg.error(fname+' ERROR.')
