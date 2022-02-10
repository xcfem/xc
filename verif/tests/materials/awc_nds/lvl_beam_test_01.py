# -*- coding: utf-8 -*-
''' Example from page 5 of the "LVL 2900F b -2.0E Floor Beam Quick 
    Reference Tables" LP SolidStart LVL Technical Guide 2900F b -2.0E
    2014.
'''

from __future__ import print_function
from __future__ import division

__author__= "Luis Claudio Pérez Tato (LCPT)"
__copyright__= "Copyright 2022, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import xc_base
import geom
import xc
from materials.awc_nds import structural_panels
from rough_calculations import ng_simple_beam as sb

# Loads
from actions import load_cases as lcm

# Units
inchToMeter= 2.54/100.0
footToMeter= 0.3048
poundToN= 4.44822
psiToPa= 6894.76
psfToPa= 47.88026

span= 15*footToMeter
carriedSpan= 16/2.0*footToMeter

deadLoad= 15*psfToPa*carriedSpan
liveLoad= 40*psfToPa*carriedSpan

# Materials LVL 2900Fb (page 4 of the PDF document from "SolidStart")
beamSection= structural_panels.LVLHeaderSections['3.5x11-1/4']


# Section properties.

## Area
A= beamSection.A()
refA= 39.3*inchToMeter**2
ratio1= abs(A-refA)/refA

## Elastic section modulus.
Sz= beamSection.getElasticSectionModulusZ()
refSz= 73.8*inchToMeter**3
ratio2= abs(Sz-refSz)/refSz

## Inertia
Iz= beamSection.Iz()
refIz= 416*inchToMeter**4
ratio3= abs(Iz-refIz)/refIz

# Analysis model
beam= sb.SimpleBeam(E= beamSection.wood.E, I= Iz,l= span)

# Serviceability limit states.

## Live-load deflection:
liveLoadDeflection= beam.getDeflectionUnderUniformLoad(liveLoad, span/2.0)
refLiveLoadDeflection= 5*liveLoad*span**4/384.0/beamSection.wood.E/Iz
ratio4= abs(liveLoadDeflection-refLiveLoadDeflection)/refLiveLoadDeflection

# Ultimate limit state.
totalLoad= deadLoad+liveLoad
refTotalLoad= 440*poundToN/footToMeter
ratio5= abs(totalLoad-refTotalLoad)/refTotalLoad


## Shear
Vmax= totalLoad*span/2.0
Vu= beamSection.Vs
refVu= 7481.0*poundToN
ratio6= (Vu-refVu)/refVu
refVu2= A*beamSection.wood.Fv/1.5
ratio7= (Vu-refVu2)/refVu2
shearCF= Vmax/Vu


## Bending
Mmax= totalLoad*span**2/8.0
Mu= beamSection.Ms
refMu= 17970.0*poundToN*footToMeter
ratio8= (Mu-refMu)/refMu
refMu2= beamSection.getFb()*Sz
ratio9= (Mu-refMu2)/refMu2
bendingCF= Mmax/Mu

'''
print('A= ', A)
print('refA= ', refA)
print('ratio1=', ratio1)
print('Sz= ', Sz)
print('refSz= ', refSz)
print('ratio2=', ratio2)
print('Iz= ', Iz)
print('refIz= ', refIz)
print('ratio3=', ratio3)
print('liveLoadDeflection= ', liveLoadDeflection)
print('refLiveLoadDeflection= ', refLiveLoadDeflection)
print('ratio4=', ratio4)
print('ratio5=', ratio5)

print('Vmax= ', Vmax/1e3, ' kN Vu= ', Vu/1e3, ' kN; F= ',Vmax/Vu)
print('refVu= ', refVu/1e3, ' kN')
print('ratio6=', ratio6)
print('refVu2= ', refVu2/1e3, ' kN')
print('ratio7=', ratio7)
print('shear efficiency CF= '+str(shearCF))

print('ratio8=', ratio8)
print('Mmax= ', Mmax/1e3, ' kN.m Mu= ', Mu/1e3, ' kN.m; F= ',Mmax/Mu)
print('refMu2= ', refMu2/1e3, ' kN.m')
print('ratio9=', ratio9)
print('bending efficiency CF= '+str(bendingCF))
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if ((abs(ratio1)<1e-2) and (abs(ratio2)<1e-3) and (abs(ratio3)<1e-2) and (abs(ratio4)<1e-12) and (abs(ratio5)<1e-6) and (abs(ratio6)<1e-12) and (abs(ratio7)<1e-4) and (abs(ratio8)<1e-12) and (abs(ratio9)<1e-5)):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
