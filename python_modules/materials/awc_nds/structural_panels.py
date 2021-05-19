# -*- coding: utf-8 -*-
''' Properties of Plywood structural panels taken from:
    http://www.pfsteco.com/techtips/pdf/tt_plywooddesigncapacities
    table C.

    Oriented strand board panels according to document:
    "Panel design specification" Form No. D510C/Revised May 2012/0300

    Properties of LSL beams and headers taken from:
    LP SolidStart LSL Beam & Header Technical Guide

    Properties of LVL beams and headers taken from:
    LP SolidStart LVL Beam & Header Technical Guide
'''

from __future__ import division
from __future__ import print_function

from materials.awc_nds import AWCNDS_materials

__author__= "Luis C. Pérez Tato (LCPT) , Ana Ortega (AO_O) "
__copyright__= "Copyright 2020, LCPT, AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es, ana.ortega@ciccp.es "

PlywoodPanelSections= dict()

PlywoodPanelSections['3/8']= AWCNDS_materials.PlywoodPanelSection('3/8',b= 1.0, h= 0.375*AWCNDS_materials.in2meter, shear_constant= 3.00) #,  'CrossSectionalArea':4.500, 'MomentOfInertia':0.053, 'SectionModulus':0.281, 'StaticalMoment':0.211
PlywoodPanelSections['7/16']= AWCNDS_materials.PlywoodPanelSection('7/16',b= 1.0, h= 0.437*AWCNDS_materials.in2meter, shear_constant= 3.50) #,  'CrossSectionalArea':5.250, 'MomentOfInertia':0.084, 'SectionModulus':0.383, 'StaticalMoment':0.287
PlywoodPanelSections['15/32']= AWCNDS_materials.PlywoodPanelSection('15/32',b= 1.0, h= 0.469*AWCNDS_materials.in2meter, shear_constant= 3.75) #,  'CrossSectionalArea':5.625, 'MomentOfInertia':0.103, 'SectionModulus':0.440, 'StaticalMoment':0.330
PlywoodPanelSections['1/2']= AWCNDS_materials.PlywoodPanelSection('1/2',b= 1.0, h= 0.500*AWCNDS_materials.in2meter, shear_constant= 4.00) #,  'CrossSectionalArea':6.000, 'MomentOfInertia':0.125, 'SectionModulus':0.500, 'StaticalMoment':0.375
PlywoodPanelSections['19/32']= AWCNDS_materials.PlywoodPanelSection('19/32',b= 1.0, h= 0.594*AWCNDS_materials.in2meter, shear_constant= 4.75) #,  'CrossSectionalArea':7.125, 'MomentOfInertia':0.209, 'SectionModulus':0.705, 'StaticalMoment':0.529
PlywoodPanelSections['5/8']= AWCNDS_materials.PlywoodPanelSection('5/8',b= 1.0, h= 0.625*AWCNDS_materials.in2meter, shear_constant= 5.00) #,  'CrossSectionalArea':7.500, 'MomentOfInertia':0.244, 'SectionModulus':0.781, 'StaticalMoment':0.586
PlywoodPanelSections['23/32']= AWCNDS_materials.PlywoodPanelSection('23/32',b= 1.0, h= 0.719*AWCNDS_materials.in2meter, shear_constant= 5.75) #,  'CrossSectionalArea':8.625, 'MomentOfInertia':0.371, 'SectionModulus':1.033, 'StaticalMoment':0.775
PlywoodPanelSections['3/4']= AWCNDS_materials.PlywoodPanelSection('3/4',b= 1.0, h= 0.750*AWCNDS_materials.in2meter, shear_constant= 6.00) #,  'CrossSectionalArea':9.000, 'MomentOfInertia':0.422, 'SectionModulus':1.125, 'StaticalMoment':0.844
PlywoodPanelSections['7/8']= AWCNDS_materials.PlywoodPanelSection('7/8',b= 1.0, h= 0.875*AWCNDS_materials.in2meter, shear_constant= 7.00) #, 'CrossSectionalArea':10.500, 'MomentOfInertia':0.670, 'SectionModulus':1.531, 'StaticalMoment':1.148
PlywoodPanelSections['1']= AWCNDS_materials.PlywoodPanelSection('1',b= 1.0, h= 1.000*AWCNDS_materials.in2meter, shear_constant= 8.00) #,  'CrossSectionalArea':12.000, 'MomentOfInertia':1.000, 'SectionModulus':2.000, 'StaticalMoment':1.500
PlywoodPanelSections['1-1/8']= AWCNDS_materials.PlywoodPanelSection('1-1/8',b= 1.0, h= 1.125*AWCNDS_materials.in2meter, shear_constant= 9.00) #,  'CrossSectionalArea':13.500, 'MomentOfInertia':1.424, 'SectionModulus':2.531, 'StaticalMoment':1.898

OSBPanelSections= dict()

OSBPanelSections['3/8']= AWCNDS_materials.OSBPanelSection('3/8',b= 1.0, h= 0.375*AWCNDS_materials.in2meter, shear_constant= 3.00)
OSBPanelSections['7/16']= AWCNDS_materials.OSBPanelSection('7/16',b= 1.0, h= 0.437*AWCNDS_materials.in2meter, shear_constant= 3.50)
OSBPanelSections['15/32']= AWCNDS_materials.OSBPanelSection('15/32',b= 1.0, h= 0.469*AWCNDS_materials.in2meter, shear_constant= 3.75)
OSBPanelSections['1/2']= AWCNDS_materials.OSBPanelSection('1/2',b= 1.0, h= 0.500*AWCNDS_materials.in2meter, shear_constant= 4.00)
OSBPanelSections['19/32']= AWCNDS_materials.OSBPanelSection('19/32',b= 1.0, h= 0.594*AWCNDS_materials.in2meter, shear_constant= 4.75)
OSBPanelSections['5/8']= AWCNDS_materials.OSBPanelSection('5/8',b= 1.0, h= 0.625*AWCNDS_materials.in2meter, shear_constant= 5.00)
OSBPanelSections['23/32']= AWCNDS_materials.OSBPanelSection('23/32',b= 1.0, h= 0.719*AWCNDS_materials.in2meter, shear_constant= 5.75)
OSBPanelSections['3/4']= AWCNDS_materials.OSBPanelSection('3/4',b= 1.0, h= 0.750*AWCNDS_materials.in2meter, shear_constant= 6.00)
OSBPanelSections['7/8']= AWCNDS_materials.OSBPanelSection('7/8',b= 1.0, h= 0.875*AWCNDS_materials.in2meter, shear_constant= 7.00)
OSBPanelSections['1']= AWCNDS_materials.OSBPanelSection('1',b= 1.0, h= 1.000*AWCNDS_materials.in2meter, shear_constant= 8.00)
OSBPanelSections['1-1/8']= AWCNDS_materials.OSBPanelSection('1-1/8',b= 1.0, h= 1.125*AWCNDS_materials.in2meter, shear_constant= 9.00)

LSL135HeaderSections= dict()

LSL135HeaderSections['3.5x4-3/8']= AWCNDS_materials.LSL_135E_HeaderSection('3.5x4-3/8',b= 3.5*AWCNDS_materials.in2meter, h= (4+3/8.0)*AWCNDS_materials.in2meter, Ms= 1817*AWCNDS_materials.pound2N*AWCNDS_materials.foot2meter, Vs= 4185*AWCNDS_materials.pound2N, linearDensity= 4.7*AWCNDS_materials.pound2kg)
LSL135HeaderSections['1.5x5-1/2']= AWCNDS_materials.LSL_135E_HeaderSection('1.5x5-1/2',b= 1.5*AWCNDS_materials.in2meter, h= 5.5*AWCNDS_materials.in2meter, Ms= 1197*AWCNDS_materials.pound2N*AWCNDS_materials.foot2meter, Vs= 2255*AWCNDS_materials.pound2N, linearDensity= 2.6*AWCNDS_materials.pound2kg)
LSL135HeaderSections['3.5x5-1/2']= AWCNDS_materials.LSL_135E_HeaderSection('3.5x5-1/2',b= 3.5*AWCNDS_materials.in2meter, h= 5.5*AWCNDS_materials.in2meter, Ms= 2794*AWCNDS_materials.pound2N*AWCNDS_materials.foot2meter, Vs= 5262*AWCNDS_materials.pound2N, linearDensity= 5.9*AWCNDS_materials.pound2kg)
LSL135HeaderSections['1.5x7-1/4']= AWCNDS_materials.LSL_135E_HeaderSection('1.5x7-1/4',b= 1.5*AWCNDS_materials.in2meter, h= 7.25*AWCNDS_materials.in2meter, Ms= 2013*AWCNDS_materials.pound2N*AWCNDS_materials.foot2meter, Vs= 2972*AWCNDS_materials.pound2N, linearDensity= 3.4*AWCNDS_materials.pound2kg)
LSL135HeaderSections['3.5x7-1/4']= AWCNDS_materials.LSL_135E_HeaderSection('3.5x7-1/4',b= 3.5*AWCNDS_materials.in2meter, h= 7.25*AWCNDS_materials.in2meter, Ms= 4696*AWCNDS_materials.pound2N*AWCNDS_materials.foot2meter, Vs= 6936*AWCNDS_materials.pound2N, linearDensity= 7.8*AWCNDS_materials.pound2kg)

LSL155HeaderSections= dict()

LSL155HeaderSections['1.5x5-1/2']= AWCNDS_materials.LSL_155E_HeaderSection('1.5x5-1/2', b= 1.5*AWCNDS_materials.in2meter, h= 5.5*AWCNDS_materials.in2meter, Ms= 1633*AWCNDS_materials.pound2N*AWCNDS_materials.foot2meter, Vs= 2255*AWCNDS_materials.pound2N, linearDensity= 2.6*AWCNDS_materials.pound2kg)
LSL155HeaderSections['1.5x7-1/4']= AWCNDS_materials.LSL_155E_HeaderSection('1.5x7-1/4', b= 1.5*AWCNDS_materials.in2meter, h= 7.25*AWCNDS_materials.in2meter, Ms= 2745*AWCNDS_materials.pound2N*AWCNDS_materials.foot2meter, Vs= 2973*AWCNDS_materials.pound2N, linearDensity= 3.5*AWCNDS_materials.pound2kg)
LSL155HeaderSections['1.5x9-1/4']= AWCNDS_materials.LSL_155E_HeaderSection('1.5x9-1/4', b= 1.5*AWCNDS_materials.in2meter, h= 9.25*AWCNDS_materials.in2meter, Ms= 4340*AWCNDS_materials.pound2N*AWCNDS_materials.foot2meter, Vs= 3793*AWCNDS_materials.pound2N, linearDensity= 4.4*AWCNDS_materials.pound2kg)
LSL155HeaderSections['1.5x9-1/2']= AWCNDS_materials.LSL_155E_HeaderSection('1.5x9-1/2', b= 1.5*AWCNDS_materials.in2meter, h= 9.5*AWCNDS_materials.in2meter, Ms= 4563*AWCNDS_materials.pound2N*AWCNDS_materials.foot2meter, Vs= 3895*AWCNDS_materials.pound2N, linearDensity= 4.6*AWCNDS_materials.pound2kg)
LSL155HeaderSections['1.5x11-1/4']= AWCNDS_materials.LSL_155E_HeaderSection('1.5x11-1/4', b= 1.5*AWCNDS_materials.in2meter, h= 11.25*AWCNDS_materials.in2meter, Ms= 6271*AWCNDS_materials.pound2N*AWCNDS_materials.foot2meter, Vs= 4613*AWCNDS_materials.pound2N, linearDensity= 5.4*AWCNDS_materials.pound2kg)
LSL155HeaderSections['1.5x11-7/8']= AWCNDS_materials.LSL_155E_HeaderSection('1.5x11-7/8', b= 1.5*AWCNDS_materials.in2meter, h= 11.875*AWCNDS_materials.in2meter, Ms= 6942*AWCNDS_materials.pound2N*AWCNDS_materials.foot2meter, Vs= 4869*AWCNDS_materials.pound2N, linearDensity= 5.7*AWCNDS_materials.pound2kg)

LSL155HeaderSections['1.75x5-1/2']= AWCNDS_materials.LSL_155E_HeaderSection('1.75x5-1/2', b= 1.75*AWCNDS_materials.in2meter, h= 5.5*AWCNDS_materials.in2meter, Ms= 1905*AWCNDS_materials.pound2N*AWCNDS_materials.foot2meter, Vs= 2631*AWCNDS_materials.pound2N, linearDensity= 3.1*AWCNDS_materials.pound2kg)
LSL155HeaderSections['1.75x7-1/4']= AWCNDS_materials.LSL_155E_HeaderSection('1.75x7-1/4', b= 1.75*AWCNDS_materials.in2meter, h= 7.25*AWCNDS_materials.in2meter, Ms= 3203*AWCNDS_materials.pound2N*AWCNDS_materials.foot2meter, Vs= 3468*AWCNDS_materials.pound2N, linearDensity= 4.1*AWCNDS_materials.pound2kg)
LSL155HeaderSections['1.75x9-1/4']= AWCNDS_materials.LSL_155E_HeaderSection('1.75x9-1/4', b= 1.75*AWCNDS_materials.in2meter, h= 9.25*AWCNDS_materials.in2meter, Ms= 5064*AWCNDS_materials.pound2N*AWCNDS_materials.foot2meter, Vs= 4425*AWCNDS_materials.pound2N, linearDensity= 5.2*AWCNDS_materials.pound2kg)
LSL155HeaderSections['1.75x9-1/2']= AWCNDS_materials.LSL_155E_HeaderSection('1.75x9-1/2', b= 1.75*AWCNDS_materials.in2meter, h= 9.5*AWCNDS_materials.in2meter, Ms= 5324*AWCNDS_materials.pound2N*AWCNDS_materials.foot2meter, Vs= 4544*AWCNDS_materials.pound2N, linearDensity= 5.3*AWCNDS_materials.pound2kg)

LSL155HeaderSections['1.75x11-7/8']= AWCNDS_materials.LSL_155E_HeaderSection('1.75x11-7/8', b= 1.75*AWCNDS_materials.in2meter, h= 11.875*AWCNDS_materials.in2meter, Ms= 8099*AWCNDS_materials.pound2N*AWCNDS_materials.foot2meter, Vs= 5680*AWCNDS_materials.pound2N, linearDensity= 6.6*AWCNDS_materials.pound2kg)
LSL155HeaderSections['1.75x14']= AWCNDS_materials.LSL_155E_HeaderSection('1.75x14', b= 1.75*AWCNDS_materials.in2meter, h= 14*AWCNDS_materials.in2meter, Ms= 11037*AWCNDS_materials.pound2N*AWCNDS_materials.foot2meter, Vs= 6697*AWCNDS_materials.pound2N, linearDensity= 7.8*AWCNDS_materials.pound2kg)
LSL155HeaderSections['1.75x16']= AWCNDS_materials.LSL_155E_HeaderSection('1.75x16', b= 1.75*AWCNDS_materials.in2meter, h= 16*AWCNDS_materials.in2meter, Ms= 14186*AWCNDS_materials.pound2N*AWCNDS_materials.foot2meter, Vs= 7653*AWCNDS_materials.pound2N, linearDensity= 8.9*AWCNDS_materials.pound2kg)
LSL155HeaderSections['1.75x18']= AWCNDS_materials.LSL_155E_HeaderSection('1.75x18', b= 1.75*AWCNDS_materials.in2meter, h= 18*AWCNDS_materials.in2meter, Ms= 17702*AWCNDS_materials.pound2N*AWCNDS_materials.foot2meter, Vs= 8610*AWCNDS_materials.pound2N, linearDensity= 10.1*AWCNDS_materials.pound2kg)

LSL155HeaderSections['3.5x5-1/2']= AWCNDS_materials.LSL_155E_HeaderSection('3.5x5-1/2', b= 3.5*AWCNDS_materials.in2meter, h= 5.5*AWCNDS_materials.in2meter, Ms= 3811*AWCNDS_materials.pound2N*AWCNDS_materials.foot2meter, Vs= 5262*AWCNDS_materials.pound2N, linearDensity= 6.1*AWCNDS_materials.pound2kg)
LSL155HeaderSections['3.5x7-1/4']= AWCNDS_materials.LSL_155E_HeaderSection('3.5x7-1/4', b= 3.5*AWCNDS_materials.in2meter, h= 7.25*AWCNDS_materials.in2meter, Ms= 6406*AWCNDS_materials.pound2N*AWCNDS_materials.foot2meter, Vs= 6936*AWCNDS_materials.pound2N, linearDensity= 8.1*AWCNDS_materials.pound2kg)
LSL155HeaderSections['3.5x9-1/4']= AWCNDS_materials.LSL_155E_HeaderSection('3.5x9-1/4', b= 3.5*AWCNDS_materials.in2meter, h= 9.25*AWCNDS_materials.in2meter, Ms= 10127*AWCNDS_materials.pound2N*AWCNDS_materials.foot2meter, Vs= 8849*AWCNDS_materials.pound2N, linearDensity= 10.3*AWCNDS_materials.pound2kg)
LSL155HeaderSections['3.5x9-1/2']= AWCNDS_materials.LSL_155E_HeaderSection('3.5x9-1/2', b= 3.5*AWCNDS_materials.in2meter, h= 9.5*AWCNDS_materials.in2meter, Ms= 10648*AWCNDS_materials.pound2N*AWCNDS_materials.foot2meter, Vs= 9088*AWCNDS_materials.pound2N, linearDensity= 10.6*AWCNDS_materials.pound2kg)

LSL155HeaderSections['3.5x11-7/8']= AWCNDS_materials.LSL_155E_HeaderSection('3.5x11-7/8', b= 3.5*AWCNDS_materials.in2meter, h= 11.875*AWCNDS_materials.in2meter, Ms= 16198*AWCNDS_materials.pound2N*AWCNDS_materials.foot2meter, Vs= 11360*AWCNDS_materials.pound2N, linearDensity= 13.3*AWCNDS_materials.pound2kg)
LSL155HeaderSections['3.5x14']= AWCNDS_materials.LSL_155E_HeaderSection('3.5x14', b= 3.5*AWCNDS_materials.in2meter, h= 14*AWCNDS_materials.in2meter, Ms= 22073*AWCNDS_materials.pound2N*AWCNDS_materials.foot2meter, Vs= 13393*AWCNDS_materials.pound2N, linearDensity= 15.7*AWCNDS_materials.pound2kg)
LSL155HeaderSections['3.5x16']= AWCNDS_materials.LSL_155E_HeaderSection('3.5x16', b= 3.5*AWCNDS_materials.in2meter, h= 16*AWCNDS_materials.in2meter, Ms= 28372*AWCNDS_materials.pound2N*AWCNDS_materials.foot2meter, Vs= 15307*AWCNDS_materials.pound2N, linearDensity= 17.9*AWCNDS_materials.pound2kg)
LSL155HeaderSections['3.5x18']= AWCNDS_materials.LSL_155E_HeaderSection('3.5x18', b= 3.5*AWCNDS_materials.in2meter, h= 18*AWCNDS_materials.in2meter, Ms= 35405*AWCNDS_materials.pound2N*AWCNDS_materials.foot2meter, Vs= 17220*AWCNDS_materials.pound2N, linearDensity= 20.1*AWCNDS_materials.pound2kg)

LSL155HeaderSections['5.25x5-1/2']= AWCNDS_materials.LSL_155E_HeaderSection('5.25x5-1/2', b= 5.25*AWCNDS_materials.in2meter, h= 5.5*AWCNDS_materials.in2meter, Ms= 5716*AWCNDS_materials.pound2N*AWCNDS_materials.foot2meter, Vs= 7893*AWCNDS_materials.pound2N, linearDensity= 9.2*AWCNDS_materials.pound2kg)
LSL155HeaderSections['5.25x7-1/4']= AWCNDS_materials.LSL_155E_HeaderSection('5.25x7-1/4', b= 5.25*AWCNDS_materials.in2meter, h= 7.25*AWCNDS_materials.in2meter, Ms= 9609*AWCNDS_materials.pound2N*AWCNDS_materials.foot2meter, Vs= 10404*AWCNDS_materials.pound2N, linearDensity= 12.2*AWCNDS_materials.pound2kg)
LSL155HeaderSections['5.25x9-1/4']= AWCNDS_materials.LSL_155E_HeaderSection('5.25x9-1/4', b= 5.25*AWCNDS_materials.in2meter, h= 9.25*AWCNDS_materials.in2meter, Ms= 15191*AWCNDS_materials.pound2N*AWCNDS_materials.foot2meter, Vs= 13274*AWCNDS_materials.pound2N, linearDensity= 15.5*AWCNDS_materials.pound2kg)
LSL155HeaderSections['5.25x9-1/2']= AWCNDS_materials.LSL_155E_HeaderSection('5.25x9-1/2', b= 5.25*AWCNDS_materials.in2meter, h= 9.5*AWCNDS_materials.in2meter, Ms= 15972*AWCNDS_materials.pound2N*AWCNDS_materials.foot2meter, Vs= 13633*AWCNDS_materials.pound2N, linearDensity= 15.9*AWCNDS_materials.pound2kg)

LSL155HeaderSections['5.25x11-7/8']= AWCNDS_materials.LSL_155E_HeaderSection('5.25x11-7/8', b= 5.25*AWCNDS_materials.in2meter, h= 11.875*AWCNDS_materials.in2meter, Ms= 24297*AWCNDS_materials.pound2N*AWCNDS_materials.foot2meter, Vs= 17041*AWCNDS_materials.pound2N, linearDensity= 19.9*AWCNDS_materials.pound2kg)
LSL155HeaderSections['5.25x14']= AWCNDS_materials.LSL_155E_HeaderSection('5.25x14', b= 5.25*AWCNDS_materials.in2meter, h= 14*AWCNDS_materials.in2meter, Ms= 33110*AWCNDS_materials.pound2N*AWCNDS_materials.foot2meter, Vs= 20090*AWCNDS_materials.pound2N, linearDensity= 23.5*AWCNDS_materials.pound2kg)
LSL155HeaderSections['5.25x16']= AWCNDS_materials.LSL_155E_HeaderSection('5.25x16', b= 5.25*AWCNDS_materials.in2meter, h= 16*AWCNDS_materials.in2meter, Ms= 42558*AWCNDS_materials.pound2N*AWCNDS_materials.foot2meter, Vs= 22960*AWCNDS_materials.pound2N, linearDensity= 26.8*AWCNDS_materials.pound2kg)
LSL155HeaderSections['5.25x18']= AWCNDS_materials.LSL_155E_HeaderSection('5.25x18', b= 5.25*AWCNDS_materials.in2meter, h= 18*AWCNDS_materials.in2meter, Ms= 53107*AWCNDS_materials.pound2N*AWCNDS_materials.foot2meter, Vs= 25830*AWCNDS_materials.pound2N, linearDensity= 30.2*AWCNDS_materials.pound2kg)

LVLHeaderSections= dict()

LVLHeaderSections['1.75x7-1/4']= AWCNDS_materials.LVL_2900Fb2E_HeaderSection('1.75x7-1/4', b= 1.75*AWCNDS_materials.in2meter, h= 7.25*AWCNDS_materials.in2meter, Ms= 3918*AWCNDS_materials.pound2N*AWCNDS_materials.foot2meter, Vs= 2411*AWCNDS_materials.pound2N, linearDensity= 3.6*AWCNDS_materials.pound2kg)
LVLHeaderSections['1.75x9-1/4']= AWCNDS_materials.LVL_2900Fb2E_HeaderSection('1.75x9-1/4', b= 1.75*AWCNDS_materials.in2meter, h= 9.25*AWCNDS_materials.in2meter, Ms= 6208*AWCNDS_materials.pound2N*AWCNDS_materials.foot2meter, Vs= 3076*AWCNDS_materials.pound2N, linearDensity= 4.6*AWCNDS_materials.pound2kg)
LVLHeaderSections['1.75x9-1/2']= AWCNDS_materials.LVL_2900Fb2E_HeaderSection('1.75x9-1/2', b= 1.75*AWCNDS_materials.in2meter, h= 9.5*AWCNDS_materials.in2meter, Ms= 6529*AWCNDS_materials.pound2N*AWCNDS_materials.foot2meter, Vs= 3159*AWCNDS_materials.pound2N, linearDensity= 4.8*AWCNDS_materials.pound2kg)
LVLHeaderSections['1.75x11-1/4']= AWCNDS_materials.LVL_2900Fb2E_HeaderSection('1.75x11-1/4', b= 1.75*AWCNDS_materials.in2meter, h= 11.25*AWCNDS_materials.in2meter, Ms= 8985*AWCNDS_materials.pound2N*AWCNDS_materials.foot2meter, Vs= 3741*AWCNDS_materials.pound2N, linearDensity= 5.6*AWCNDS_materials.pound2kg)
LVLHeaderSections['1.75x11-7/8']= AWCNDS_materials.LVL_2900Fb2E_HeaderSection('1.75x11-7/8', b= 1.75*AWCNDS_materials.in2meter, h= 11.875*AWCNDS_materials.in2meter, Ms= 9951*AWCNDS_materials.pound2N*AWCNDS_materials.foot2meter, Vs= 3948*AWCNDS_materials.pound2N, linearDensity= 5.9*AWCNDS_materials.pound2kg)
LVLHeaderSections['1.75x14']= AWCNDS_materials.LVL_2900Fb2E_HeaderSection('1.75x14', b= 1.75*AWCNDS_materials.in2meter, h= 14*AWCNDS_materials.in2meter, Ms= 13514*AWCNDS_materials.pound2N*AWCNDS_materials.foot2meter, Vs= 4655*AWCNDS_materials.pound2N, linearDensity= 7*AWCNDS_materials.pound2kg)
LVLHeaderSections['1.75x16']= AWCNDS_materials.LVL_2900Fb2E_HeaderSection('1.75x16', b= 1.75*AWCNDS_materials.in2meter, h= 16*AWCNDS_materials.in2meter, Ms= 17318*AWCNDS_materials.pound2N*AWCNDS_materials.foot2meter, Vs= 5320*AWCNDS_materials.pound2N, linearDensity= 8*AWCNDS_materials.pound2kg)
LVLHeaderSections['1.75x18']= AWCNDS_materials.LVL_2900Fb2E_HeaderSection('1.75x18', b= 1.75*AWCNDS_materials.in2meter, h= 18*AWCNDS_materials.in2meter, Ms= 21552*AWCNDS_materials.pound2N*AWCNDS_materials.foot2meter, Vs= 5985*AWCNDS_materials.pound2N, linearDensity= 9*AWCNDS_materials.pound2kg)

LVLHeaderSections['3.5x7-1/4']= AWCNDS_materials.LVL_2900Fb2E_HeaderSection('3.5x7-1/4', b= 3.5*AWCNDS_materials.in2meter, h= 7.25*AWCNDS_materials.in2meter, Ms= 7837*AWCNDS_materials.pound2N*AWCNDS_materials.foot2meter, Vs= 4821*AWCNDS_materials.pound2N, linearDensity= 7.3*AWCNDS_materials.pound2kg)
LVLHeaderSections['3.5x9-1/4']= AWCNDS_materials.LVL_2900Fb2E_HeaderSection('3.5x9-1/4', b= 3.5*AWCNDS_materials.in2meter, h= 9.25*AWCNDS_materials.in2meter, Ms= 12416*AWCNDS_materials.pound2N*AWCNDS_materials.foot2meter, Vs= 6151*AWCNDS_materials.pound2N, linearDensity= 9.3*AWCNDS_materials.pound2kg)
LVLHeaderSections['3.5x9-1/2']= AWCNDS_materials.LVL_2900Fb2E_HeaderSection('3.5x9-1/2', b= 3.5*AWCNDS_materials.in2meter, h= 9.5*AWCNDS_materials.in2meter, Ms= 13057*AWCNDS_materials.pound2N*AWCNDS_materials.foot2meter, Vs= 6318*AWCNDS_materials.pound2N, linearDensity= 9.5*AWCNDS_materials.pound2kg)
LVLHeaderSections['3.5x11-1/4']= AWCNDS_materials.LVL_2900Fb2E_HeaderSection('3.5x11-1/4', b= 3.5*AWCNDS_materials.in2meter, h= 11.25*AWCNDS_materials.in2meter, Ms= 17970*AWCNDS_materials.pound2N*AWCNDS_materials.foot2meter, Vs= 7481*AWCNDS_materials.pound2N, linearDensity= 11.3*AWCNDS_materials.pound2kg)
LVLHeaderSections['3.5x11-7/8']= AWCNDS_materials.LVL_2900Fb2E_HeaderSection('3.5x11-7/8', b= 3.5*AWCNDS_materials.in2meter, h= 11.875*AWCNDS_materials.in2meter, Ms= 19902*AWCNDS_materials.pound2N*AWCNDS_materials.foot2meter, Vs= 7897*AWCNDS_materials.pound2N, linearDensity= 11.9*AWCNDS_materials.pound2kg)
LVLHeaderSections['3.5x14']= AWCNDS_materials.LVL_2900Fb2E_HeaderSection('3.5x14', b= 3.5*AWCNDS_materials.in2meter, h= 14*AWCNDS_materials.in2meter, Ms= 27029*AWCNDS_materials.pound2N*AWCNDS_materials.foot2meter, Vs= 9310*AWCNDS_materials.pound2N, linearDensity= 14*AWCNDS_materials.pound2kg)
LVLHeaderSections['3.5x16']= AWCNDS_materials.LVL_2900Fb2E_HeaderSection('3.5x16', b= 3.5*AWCNDS_materials.in2meter, h= 16*AWCNDS_materials.in2meter, Ms= 34636*AWCNDS_materials.pound2N*AWCNDS_materials.foot2meter, Vs= 10640*AWCNDS_materials.pound2N, linearDensity= 16*AWCNDS_materials.pound2kg)
LVLHeaderSections['3.5x18']= AWCNDS_materials.LVL_2900Fb2E_HeaderSection('3.5x18', b= 3.5*AWCNDS_materials.in2meter, h= 18*AWCNDS_materials.in2meter, Ms= 43105*AWCNDS_materials.pound2N*AWCNDS_materials.foot2meter, Vs= 11970*AWCNDS_materials.pound2N, linearDensity= 18*AWCNDS_materials.pound2kg)

LVLHeaderSections['5.25x7-1/4']= AWCNDS_materials.LVL_2900Fb2E_HeaderSection('5.25x7-1/4', b= 5.25*AWCNDS_materials.in2meter, h= 7.25*AWCNDS_materials.in2meter, Ms= 11755*AWCNDS_materials.pound2N*AWCNDS_materials.foot2meter, Vs= 7232*AWCNDS_materials.pound2N, linearDensity= 10.9*AWCNDS_materials.pound2kg)
LVLHeaderSections['5.25x9-1/4']= AWCNDS_materials.LVL_2900Fb2E_HeaderSection('5.25x9-1/4', b= 5.25*AWCNDS_materials.in2meter, h= 9.25*AWCNDS_materials.in2meter, Ms= 18624*AWCNDS_materials.pound2N*AWCNDS_materials.foot2meter, Vs= 9227*AWCNDS_materials.pound2N, linearDensity= 13.9*AWCNDS_materials.pound2kg)
LVLHeaderSections['5.25x9-1/2']= AWCNDS_materials.LVL_2900Fb2E_HeaderSection('5.25x9-1/2', b= 5.25*AWCNDS_materials.in2meter, h= 9.5*AWCNDS_materials.in2meter, Ms= 19586*AWCNDS_materials.pound2N*AWCNDS_materials.foot2meter, Vs= 9476*AWCNDS_materials.pound2N, linearDensity= 14.3*AWCNDS_materials.pound2kg)
LVLHeaderSections['5.25x11-1/4']= AWCNDS_materials.LVL_2900Fb2E_HeaderSection('5.25x11-1/4', b= 5.25*AWCNDS_materials.in2meter, h= 11.25*AWCNDS_materials.in2meter, Ms= 26955*AWCNDS_materials.pound2N*AWCNDS_materials.foot2meter, Vs= 11222*AWCNDS_materials.pound2N, linearDensity= 16.9*AWCNDS_materials.pound2kg)
LVLHeaderSections['5.25x11-7/8']= AWCNDS_materials.LVL_2900Fb2E_HeaderSection('5.25x11-7/8', b= 5.25*AWCNDS_materials.in2meter, h= 11.875*AWCNDS_materials.in2meter, Ms= 29854*AWCNDS_materials.pound2N*AWCNDS_materials.foot2meter, Vs= 11845*AWCNDS_materials.pound2N, linearDensity= 17.8*AWCNDS_materials.pound2kg)
LVLHeaderSections['5.25x14']= AWCNDS_materials.LVL_2900Fb2E_HeaderSection('5.25x14', b= 5.25*AWCNDS_materials.in2meter, h= 14*AWCNDS_materials.in2meter, Ms= 40543*AWCNDS_materials.pound2N*AWCNDS_materials.foot2meter, Vs= 13965*AWCNDS_materials.pound2N, linearDensity= 21*AWCNDS_materials.pound2kg)
LVLHeaderSections['5.25x16']= AWCNDS_materials.LVL_2900Fb2E_HeaderSection('5.25x16', b= 5.25*AWCNDS_materials.in2meter, h= 16*AWCNDS_materials.in2meter, Ms= 51954*AWCNDS_materials.pound2N*AWCNDS_materials.foot2meter, Vs= 15960*AWCNDS_materials.pound2N, linearDensity= 24*AWCNDS_materials.pound2kg)
LVLHeaderSections['5.25x18']= AWCNDS_materials.LVL_2900Fb2E_HeaderSection('5.25x18', b= 5.25*AWCNDS_materials.in2meter, h= 18*AWCNDS_materials.in2meter, Ms= 64657*AWCNDS_materials.pound2N*AWCNDS_materials.foot2meter, Vs= 17955*AWCNDS_materials.pound2N, linearDensity= 27*AWCNDS_materials.pound2kg)

LVLHeaderSections['7x7-1/4']= AWCNDS_materials.LVL_2900Fb2E_HeaderSection('7x7-1/4', b= 7*AWCNDS_materials.in2meter, h= 7.25*AWCNDS_materials.in2meter, Ms= 15673*AWCNDS_materials.pound2N*AWCNDS_materials.foot2meter, Vs= 9643*AWCNDS_materials.pound2N, linearDensity= 14.5*AWCNDS_materials.pound2kg)
LVLHeaderSections['7x9-1/4']= AWCNDS_materials.LVL_2900Fb2E_HeaderSection('7x9-1/4', b= 7*AWCNDS_materials.in2meter, h= 9.25*AWCNDS_materials.in2meter, Ms= 24832*AWCNDS_materials.pound2N*AWCNDS_materials.foot2meter, Vs= 12303*AWCNDS_materials.pound2N, linearDensity= 18.5*AWCNDS_materials.pound2kg)
LVLHeaderSections['7x9-1/2']= AWCNDS_materials.LVL_2900Fb2E_HeaderSection('7x9-1/2', b= 7*AWCNDS_materials.in2meter, h= 9.5*AWCNDS_materials.in2meter, Ms= 26115*AWCNDS_materials.pound2N*AWCNDS_materials.foot2meter, Vs= 12635*AWCNDS_materials.pound2N, linearDensity= 19*AWCNDS_materials.pound2kg)
LVLHeaderSections['7x11-1/4']= AWCNDS_materials.LVL_2900Fb2E_HeaderSection('7x11-1/4', b= 7*AWCNDS_materials.in2meter, h= 11.25*AWCNDS_materials.in2meter, Ms= 35940*AWCNDS_materials.pound2N*AWCNDS_materials.foot2meter, Vs= 14963*AWCNDS_materials.pound2N, linearDensity= 22.5*AWCNDS_materials.pound2kg)
LVLHeaderSections['7x11-7/8']= AWCNDS_materials.LVL_2900Fb2E_HeaderSection('7x11-7/8', b= 7*AWCNDS_materials.in2meter, h= 11.875*AWCNDS_materials.in2meter, Ms= 39805*AWCNDS_materials.pound2N*AWCNDS_materials.foot2meter, Vs= 15794*AWCNDS_materials.pound2N, linearDensity= 23.8*AWCNDS_materials.pound2kg)
LVLHeaderSections['7x14']= AWCNDS_materials.LVL_2900Fb2E_HeaderSection('7x14', b= 7*AWCNDS_materials.in2meter, h= 14*AWCNDS_materials.in2meter, Ms= 54057*AWCNDS_materials.pound2N*AWCNDS_materials.foot2meter, Vs= 18620*AWCNDS_materials.pound2N, linearDensity= 28*AWCNDS_materials.pound2kg)
LVLHeaderSections['7x16']= AWCNDS_materials.LVL_2900Fb2E_HeaderSection('7x16', b= 7*AWCNDS_materials.in2meter, h= 16*AWCNDS_materials.in2meter, Ms= 69272*AWCNDS_materials.pound2N*AWCNDS_materials.foot2meter, Vs= 21280*AWCNDS_materials.pound2N, linearDensity= 32*AWCNDS_materials.pound2kg)
LVLHeaderSections['7x18']= AWCNDS_materials.LVL_2900Fb2E_HeaderSection('7x18', b= 7*AWCNDS_materials.in2meter, h= 18*AWCNDS_materials.in2meter, Ms= 86209*AWCNDS_materials.pound2N*AWCNDS_materials.foot2meter, Vs= 23940*AWCNDS_materials.pound2N, linearDensity= 36.1*AWCNDS_materials.pound2kg)
