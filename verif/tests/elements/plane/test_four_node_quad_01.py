# -*- coding: utf-8 -*-
''' Test tomado del libro:
    Introdución al estudio del elemento finito en Ingeniería. Segunda edición.
    Tirupathi R. Chandrupatla
    Ashok D. Belegundu.
    Prentice Hall, México 1999
    Ejemplo 7.2 '''

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
prueba.logFileName= "/tmp/borrar.log" # Para no imprimir mensajes de advertencia
mdlr= prueba.getModelador
nodos= mdlr.getNodeLoader
predefined_spaces.gdls_elasticidad2D(nodos)


n1= nodos.newNodeIDXY(1,0,0)
n2= nodos.newNodeIDXY(2,2,0)
n2.setTrialDisp(xc.Vector([0.002,0.003]))
n3= nodos.newNodeIDXY(3,2,1)
n3.setTrialDisp(xc.Vector([0.006,0.0032]))
n4= nodos.newNodeIDXY(4,0,1)


elast2d= typical_materials.defElasticIsotropicPlaneStress(mdlr,"elast2d",E,nu,rho)
# Elements definition
elementos= mdlr.getElementLoader
elementos.defaultMaterial= "elast2d"
quad4n= elementos.newElement("quad4n",xc.ID([1,2,3,4]))

mdlr.getDomain.commit()
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
fname= os.path.basename(__file__)
if (abs(ratio1)<1e-9) & (abs(ratio2)<1e-3) & (abs(ratio3)<1e-3) & (abs(ratio4)<1e-3):
  print "test ",fname,": ok."
else:
  print "test ",fname,": ERROR."
