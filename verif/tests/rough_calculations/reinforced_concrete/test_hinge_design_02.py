# -*- coding: utf-8 -*-
''' Design of hinges under general loading.

   Based on the article:
   Markic T, Kaufmann W.
   Modeling and design of concrete hinges under general loading. 
   Structural Concrete. 2023;24(4):5122–49. 
   https://doi.org/10.1002/suco.202201110
'''

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2025, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import math
from materials import concrete_hinge
from materials.ec2 import EC2_materials
import scipy.optimize as optimize

class ConcreteHinge(concrete_hinge.ConcreteHinge):
    ''' Concrete hinge design according to the article:

       Markic T, Kaufmann W.
       Modeling and design of concrete hinges under general loading. 
       Structural Concrete. 2023;24(4):5122–49. 
       https://doi.org/10.1002/suco.202201110

    :ivar concrete: concrete material.
    :ivar reinfSteel: steel material.
    ;ivar phi: concrete internal friction angle.
    '''

    def fsy(self):
        ''' Return the yield stress of the reinforcing steel.'''
        # return self.reinfSteel.fyd()
        return self.reinfSteel.fyk
    

    def fc(self):
        ''' Return the concrete compressive strength.'''
        # return self.concrete.fcd()
        return -self.concrete.fck


        

concrete= EC2_materials.C40
steel= EC2_materials.S500C

# Figure 11 concrete hinge.
d= 1.0
# d1= 0.0
# d1= 0.1*d
# d1= 0.2*d
d1= .3*d
ht= min(0.2*d1, .02)
hf= ht+.05
As= 11e-4
figure11Hinge= ConcreteHinge(concrete= concrete, reinfSteel= steel, d1= d1, d= d, b1= 1.0, b= 1.0, ht= ht, hf= hf, reinforcement= [(As, (0,0), math.radians(30))])

#Nx= 0.5*-2e6
Nx= 1.0*-2e6
# Nx= 1.5*-2e6
Vyu= figure11Hinge.getShearStrength(Nx= Nx, simplifiedModel= False)
VyuRef= 3.06e6 # See leftmost diagram on figure 11
ratio= abs(Vyu-VyuRef)/VyuRef

'''
print('Vyu= ', Vyu/1e6)
print('VyuRef= ', VyuRef/1e6)
print('ratio= ', ratio)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (abs(ratio)<.001):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
