# -*- coding: utf-8 -*-
# Home made test

import xc_base
import geom
import xc
from model import predefined_spaces
from materials import typical_materials
from materials.ehe import EHE_limit_state_checking

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AO_O)"
__copyright__= "Copyright 2015, LCPT and AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

Ec= 20e9
sideLength= 1.0


feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor


concrete= typical_materials.defElasticMaterial(preprocessor, 'concrete',Ec)
sectionGeometryTest= preprocessor.getMaterialHandler.newSectionGeometry("sectionGeometryTest")
regions= sectionGeometryTest.getRegions
concrete= regions.newQuadRegion('concrete')
concrete.pMin= geom.Pos2d(0.0,0.0)
concrete.pMax= geom.Pos2d(1.0,1.0)

pt= EHE_limit_state_checking.computeEffectiveHollowSectionParameters(sectionGeometryTest,0.5,0.03)


ratio1= (pt.A()-1)
ratio2= (pt.u()-4)/4
ratio3= (pt.he()-0.25)/0.25
ratio4= (pt.Ae()-(0.75**2))/(0.75**2)
ratio5= (pt.ue()-3)/3

''' 
    print "A= ",A
    print "u= ",u
    print "he= ",he
    print "Ae= ",Ae
    print "ue= ",ue
    print "ratio1= ",ratio1
    print "ratio2= ",ratio2
    print "ratio3= ",ratio3
    print "ratio4= ",ratio4
    print "ratio5= ",ratio5
   '''


import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if((ratio1<1e-15) & (ratio2<1e-15) & (ratio3<1e-15) & (ratio4<1e-15) & (ratio5<1e-15)):
  print "test ",fname,": ok."
else:
  lmsg.error(fname+' ERROR.')
