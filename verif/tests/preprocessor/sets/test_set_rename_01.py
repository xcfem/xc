# -*- coding: utf-8 -*-
''' Test set renaming.'''
from __future__ import print_function

import xc_base
import geom
import xc
import math

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2014, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

FEcase= xc.FEProblem()
prep=FEcase.getPreprocessor

s1= prep.getSets.defSet("S1")

tmp= prep.getSets.getSet('S1')
oldName= tmp.name

s1.name= 'Lola'

tmp= prep.getSets.getSet('Lola')
newName= tmp.name

#print(' set names: ', prep.getSets.getSetsNames())

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)

if (oldName=='S1') and (newName=='Lola'):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
