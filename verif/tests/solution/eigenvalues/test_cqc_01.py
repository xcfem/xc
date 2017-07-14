# -*- coding: utf-8 -*-
''' Test to verify the mode combinations
taken from example A87 of Solvia Verification Manual.
This exercise is based on example E26.8 of the 
book «Dynamics of Structures» by Clough, R. W., and Penzien, J. '''
import xc_base
import geom
import xc

from model import predefined_spaces
from solution import predefined_solutions
from materials import typical_materials
import math
import numpy

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2014, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

masaExtremo= 1e-2 # Masa en kg.
nodeMassMatrix= xc.Matrix([[masaExtremo,0,0,0,0,0],
                                         [0,masaExtremo,0,0,0,0],
                                         [0,0,masaExtremo,0,0,0],
                                         [0,0,0,0,0,0],
                                         [0,0,0,0,0,0],
                                         [0,0,0,0,0,0]])
EMat= 1 # Elastic modulus.
nuMat= 0 # Poisson's ratio.
GMat= EMat/(2.0*(1+nuMat)) # Shear modulus.

Iyy= 1 # Inercia a flexión eje y.
Izz= 1 # Inercia a flexión eje z.
Ir= 4/3.0 # Inercia a torsion.
area= 1e7 # Area de la sección.
Lx= 1
Ly= 1
Lz= 1


# Problem type
prueba= xc.ProblemaEF()
preprocessor=  prueba.getPreprocessor
nodes= preprocessor.getNodeLoader
modelSpace= predefined_spaces.StructuralMechanics3D(nodes)
nod0= nodes.newNodeIDXYZ(0,0,0,0)
nod1= nodes.newNodeXYZ(0,-Ly,0)
nod2= nodes.newNodeXYZ(0,-Ly,-Lz)
nod3= nodes.newNodeXYZ(Lx,-Ly,-Lz)
nod3.mass= nodeMassMatrix

constraints= preprocessor.getConstraintLoader
nod0.fix(xc.ID([0,1,2,3,4,5]),xc.Vector([0,0,0,0,0,0]))

# Materials definition
scc= typical_materials.defElasticSection3d(preprocessor, "scc",area,EMat,GMat,Izz,Iyy,Ir)

# Geometric transformation(s)
linX= preprocessor.getTransfCooLoader.newLinearCrdTransf3d("linX")
linX.xzVector= xc.Vector([1,0,0])
linY= preprocessor.getTransfCooLoader.newLinearCrdTransf3d("linY")
linY.xzVector= xc.Vector([0,1,0])

# Elements definition
elements= preprocessor.getElementLoader
elements.defaultTransformation= "linX"
elements.defaultMaterial= "scc"
beam3d= elements.newElement("elastic_beam_3d",xc.ID([0,1]))
beam3d= elements.newElement("elastic_beam_3d",xc.ID([1,2]))
elements.defaultTransformation= "linY"
beam3d= elements.newElement("elastic_beam_3d",xc.ID([2,3]))


# Procedimiento de solución
solu= prueba.getSoluProc
solCtrl= solu.getSoluControl


solModels= solCtrl.getModelWrapperContainer
sm= solModels.newModelWrapper("sm")


cHandler= sm.newConstraintHandler("transformation_constraint_handler")

numberer= sm.newNumberer("default_numberer")
numberer.useAlgorithm("rcm")

solMethods= solCtrl.getSoluMethodContainer
smt= solMethods.newSoluMethod("smt","sm")
solAlgo= smt.newSolutionAlgorithm("frequency_soln_algo")
integ= smt.newIntegrator("eigen_integrator",xc.Vector([1.0,1,1.0,1.0]))

soe= smt.newSystemOfEqn("full_gen_eigen_soe")
solver= soe.newSolver("full_gen_eigen_solver")

analysis= solu.newAnalysis("modal_analysis","smt","")
analOk= analysis.analyze(3)
periodos= analysis.getPeriods()
angularFrequencies= analysis.getAngularFrequencies()
aceleraciones= [2.27,2.45,6.98]
crossCQCCoefficients= analysis.getCQCModalCrossCorrelationCoefficients(xc.Vector([0.05,0.05,0.05]))


from misc import matrixUtils
eigNod3= nod3.getNormalizedEigenvectors
eigenvectors= matrixUtils.matrixToNumpyArray(eigNod3)
modos= eigenvectors[0:3,0:3] #eigenvectors.getCaja(0,0,2,2)
modo1= modos[:,0] #.getCol(1)
modo2= modos[:,1] #.getCol(2)
modo3= modos[:,2] #.getCol(3)

factoresParticipacionModalX= nod3.getModalParticipationFactorsForGdls([0])
factoresDistribucion= nod3.getDistributionFactors
A1= matrixUtils.vectorToNumpyArray(nod3.getMaxModalDisplacementForGdls(1,aceleraciones[0],[0]))
maxDispMod1= A1[0:3] #getCaja(A1,1,1,3,1)
A2= matrixUtils.vectorToNumpyArray(nod3.getMaxModalDisplacementForGdls(2,aceleraciones[1],[0]))
maxDispMod2= A2[0:3] #getCaja(A2,1,1,3,1)
A3= matrixUtils.vectorToNumpyArray(nod3.getMaxModalDisplacementForGdls(3,aceleraciones[2],[0]))
maxDispMod3= A3[0:3] #getCaja(A3,1,1,3,1)


# Los valores teóricos estan tomados deel ejemplo E26.8 del libro: Clough, R. W., and Penzien, J., Dynamics of Structures
angularFrequenciesTeor= xc.Vector([4.59,4.83,14.56])
periodosTeor= xc.Vector([2*math.pi/4.59,2*math.pi/4.83,2*math.pi/14.56])
ratio0= (angularFrequencies-angularFrequenciesTeor).Norm()
frecuenciasTeor= [4.59/2/math.pi,4.83/2/math.pi,14.56/2/math.pi]
ratio1= (periodos-periodosTeor).Norm()
modosTeor= numpy.matrix([[-0.731,0.271,-1],
                         [0.232,1,0.242],
                         [-1,0.036,0.787]])
# modo1Teor= getCol(modosTeor,1)
# modo2Teor= getCol(modosTeor,2)
# modo3Teor= getCol(modosTeor,3)
resta= (modos-modosTeor)
ratio2= numpy.linalg.norm(resta)
# Estos coeficientes del CQC están tomados del manual de Solvia
crossCQCCoefficientsTeor= xc.Matrix([[1,0.79280,0.005705],[0.79280,1,0.006383],[0.005705,0.006383,1]])
ratio3= (crossCQCCoefficients-crossCQCCoefficientsTeor).Norm()
factoresParticipacionModalXTeor= xc.Vector([-.731/1.588,.271/1.075,-1/1.678])
ratio4= (factoresParticipacionModalX-factoresParticipacionModalXTeor).Norm()
''' 
maxDispMod1Teor= modo1Teor*factoresParticipacionModalXTeor[0]*aceleraciones[0]/sqr(angularFrequenciesTeor[0])
maxDispMod2Teor= modo2Teor*factoresParticipacionModalXTeor[1]*aceleraciones[1]/sqr(angularFrequenciesTeor[1])
maxDispMod3Teor= modo3Teor*factoresParticipacionModalXTeor[2]*aceleraciones[2]/sqr(angularFrequenciesTeor[2])
maxDispMod1Teor= [0.119,-0.038,0.162]*0.3048
maxDispMod2Teor= [0.039,0.143,0.005]*0.3048
maxDispMod3Teor= [0.064,-0.016,-0.055]*0.3048
   '''
# Estos coeficientes del desplazamiento están tomados del manual de Solvia
maxDispMod1Teor= numpy.matrix([[36.202e-3],[-11.549e-3],[49.548e-3]])
maxDispMod2Teor= numpy.matrix([[7.123e-3],[26.38e-3],[0.945e-3]])
maxDispMod3Teor= numpy.matrix([[19.625e-3],[-4.746e-3],[-15.445e-3]])
ratioM1= numpy.linalg.norm(maxDispMod1-maxDispMod1Teor)
ratioM2= numpy.linalg.norm(maxDispMod2-maxDispMod2Teor)
ratioM3= numpy.linalg.norm(maxDispMod3-maxDispMod3Teor)

maxDispMod= maxDispMod1,maxDispMod2,maxDispMod3
maxDispCQC= [0.0,0.0,0.0]
maxDispCQCTeor= xc.Vector([46.53e-3,19.18e-3,52.53e-3])
nMod= 3
i= 0
j= 0
k= 0

for i in range(0,nMod):
  for j in range(0,nMod):
    maxDispModI= maxDispMod[i]
    maxDispModJ= maxDispMod[j]
    cqcCoeff= crossCQCCoefficients(i,j)
    for k in range(0,3):
      maxDispCQC[k]= maxDispCQC[k]+cqcCoeff*maxDispModI[k]*maxDispModJ[k]


for k in range(0,3):
  maxDispCQC[k]= math.sqrt(maxDispCQC[k])

maxDispCQC= xc.Vector(maxDispCQC)
ratioDispCQC= (maxDispCQC-maxDispCQCTeor).Norm()


''' 
print "angular frequencies: ",angularFrequencies
print "angular frequencies (teor.): ",angularFrequenciesTeor
print "ratio0= ",ratio0
print "periodos: ",periodos
print "periodosTeor: ",periodosTeor
print "ratio1= ",ratio1
print "modos: ",modos
print "modosTeor: ",modosTeor
print "resta: ",resta
print "ratio2= ",ratio2
print "coeficientes para CQC: ",crossCQCCoefficients
print "coeficientes CQC: ",crossCQCCoefficients
print "coeficientes CQC ejemplo: ",crossCQCCoefficientsTeor
print "ratio3= ",ratio3
print "factoresParticipacionModalX: ",factoresParticipacionModalX
print "factoresParticipacionModalXTeor: ",factoresParticipacionModalXTeor
print "ratio4= ",ratio4
print "********** desplazamiento máximo mode 1: ",maxDispMod1*1000
print "********** desplazamiento máximo mode 1 (ejemplo): ",maxDispMod1Teor*1000
print "ratioM1= ",ratioM1
print "********** desplazamiento máximo mode 2: ",maxDispMod2*1000
print "********** desplazamiento máximo mode 2 (ejemplo): ",maxDispMod2Teor*1000
print "ratioM2= ",ratioM2
print "********** desplazamiento máximo mode 3: ",maxDispMod3*1000
print "********** desplazamiento máximo mode 3 (ejemplo): ",maxDispMod3Teor*1000
print "ratioM3= ",ratioM3
#print "maxDispCQC= ",maxDispCQC*1e3
#print "maxDispCQCTeor= ",maxDispCQCTeor*1e3
print "ratioDispCQC= ",ratioDispCQC
   '''

import os
from miscUtils import LogMessages as lmsg
fname= os.path.basename(__file__)
if( (ratio1<1e-3) & (ratio2<1e-2) & (ratio3<1e-5) & (ratio4<1e-3) & (ratioM1<1e-6) & (ratioM2<1e-6) & (ratioM3<1e-6) & (ratioDispCQC<1e-5) ): 
  print "test ",fname,": ok."
else:
  lmsg.error(fname+' ERROR.')

