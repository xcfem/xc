from __future__ import division
from __future__ import print_function

__author__= "Ana Ortega (AO_O) and Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2016, AO_O and LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "ana.Ortega@ciccp.es" "l.pereztato@ciccp.es" 

import math
import xc
from actions.earth_pressure import earth_pressure as ep
'''Lateral earth pressure on a retaining wall due to a point 
    load acting in vertical direction on the backfill.'''

vref_cuadCos45=0.5

Qload=1e3 #value of the point load
loadAppPnt=xc.Vector([1,1,10]) #load application point
zBaseWall=0 #global Z coordinate of the base of the wall
distWall=1 # horizontal distance between the wall and the point load
vdir=xc.Vector([0,-1,0])
loadDef=ep.PointVerticalLoadOnBackfill(xcSet=None,Qload=Qload, loadAppPnt=loadAppPnt,zBaseWall=zBaseWall,distWall=distWall,vdir=vdir)
psi1=loadDef.getAngPsi(0,0)
psi01=loadDef.getAngPsi(1,0)
psi2=loadDef.getAngPsi(2,0)
pres1_z5=loadDef.getPressure(0,0,5)
pres0_z5=loadDef.getPressure(1,0,5) # point of maximum pressure for z=5
pres2_z5=loadDef.getPressure(2,0,5)
pres1_z2=loadDef.getPressure(0,0,2)
pres0_z2=loadDef.getPressure(1,0,2) # point of maximum pressure for z=5
pres2_z2=loadDef.getPressure(2,0,2)


loadDef.vdir=xc.Vector([-1,0,0])
psi3=loadDef.getAngPsi(0,0)
psi02=loadDef.getAngPsi(0,1)
psi4=loadDef.getAngPsi(0,2)

loadAppPnt=xc.Vector([-1,-1,10])
vdir=xc.Vector([0,1,0])
loadDef=ep.PointVerticalLoadOnBackfill(xcSet=None,Qload=Qload, loadAppPnt=loadAppPnt,zBaseWall=zBaseWall,distWall=distWall,vdir=vdir)
psi5=loadDef.getAngPsi(0,0)
psi03=loadDef.getAngPsi(-1,0)
psi6=loadDef.getAngPsi(-2,0)
loadDef.vdir=xc.Vector([1,0,0])
psi7=loadDef.getAngPsi(0,0)
psi8=loadDef.getAngPsi(0,-2)

OK01=(math.cos(psi1)**2-vref_cuadCos45)<1e-4 and (math.cos(psi2)**2-vref_cuadCos45)<1e-4 and (math.cos(psi3)**2-vref_cuadCos45)<1e-4 and (math.cos(psi4)**2-vref_cuadCos45)<1e-4 and (math.cos(psi5)**2-vref_cuadCos45)<1e-4 and (math.cos(psi6)**2-vref_cuadCos45)<1e-4 and (math.cos(psi7)**2-vref_cuadCos45)<1e-4 and (math.cos(psi8)**2-vref_cuadCos45)<1e-4

OK02=(math.cos(psi01)**2-1)<1e-5 and (math.cos(psi02)**2-1)<1e-5 and (math.cos(psi03)**2-1)<1e-5

OK3=(pres1_z5-5.078)<1e-3 and (pres0_z5-10.1565)<1e-3 and (pres2_z5-5.078)<1e-3 and (pres1_z2-1.75)<1e-3 and (pres0_z2-3.5)<1e-3 and (pres2_z2-1.75)<1e-3 
import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if OK01 and OK02:
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
