# -*- coding: utf-8 -*-
''' Check behaviour when the same material is defined in two different FE
    problems.'''

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2026, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import xc
from model import predefined_spaces
from materials.ec2 import EC2_materials

C20= EC2_materials.EC2Concrete(nmbConcrete= 'C20', fck= -20e3, gammaC= 1.5, alphacc= 1.0) 
C30= EC2_materials.EC2Concrete(nmbConcrete= 'C30', fck= -30e3, gammaC= 1.5, alphacc= 1.0) 
    
# FE problem A definition.
feProblemA= xc.FEProblem()
preprocessorA=  feProblemA.getPreprocessor

diagDC30A= C30.defDiagD(preprocessorA)
tagC30ProblemA= C30.getMaterialDiagramD().tag

# FE problem B definition.
feProblemB= xc.FEProblem()
preprocessorB=  feProblemB.getPreprocessor

diagDC20B= C20.defDiagD(preprocessorB)
diagDC30B= C30.defDiagD(preprocessorB)
tagC30ProblemB= C30.getMaterialDiagramD().tag

testOK= (tagC30ProblemB!=tagC30ProblemA)

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if testOK:
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
