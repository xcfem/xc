# -*- coding: utf-8 -*-
''' Test remove_accents function.'''

from __future__ import print_function
from __future__ import division

__author__= "Luis C. Pérez Tato (LCPT)"
__cppyright__= "Copyright 2020 LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import sys
from misc_utils import data_struct_utils

testList= ['abcdEFG', 'áéíóúÁÉÍÓÚüñÑç', 'Montrèal', u'über', '$12.89', u'Mère', '', u'noël', '889']
testResults= ['abcdEFG', 'aeiouAEIOUunNc', 'Montreal', 'uber', '$12.89', 'Mere', '', 'noel', '889']
if sys.version_info[0] >= 3:
    testResults= ['abcdEFG', 'aeiouAEIOUunNc', 'Montreal', 'uber', '$12.89', 'Mere', '', 'noel', '889']
error= False
results= list()
for s1,s2 in zip(testList,testResults):
    r= data_struct_utils.remove_accents(s1)
    if(r!=s2):
        error= True
    results.append(r)
    
'''
print(results)
print(testResults)
'''

import os
fname= os.path.basename(__file__)
if (not error):
  print("test "+fname+": ok.")
else:
  print("test "+fname+": ERROR.")
