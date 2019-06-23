# -*- coding: utf-8 -*-

from __future__ import division
from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2016, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import scipy.interpolate
from materials.aci import ACI_materials
import geom
from materials import limit_state_checking_base as lsc
from postprocess import control_vars as cv
from rough_calculations import ng_simple_bending_reinforcement
import math
import xc_base
import geom
from materials.sections.fiber_section import fiber_sets
from materials.sections import stressCalc as sc
from miscUtils import LogMessages as lmsg

class RebarController(object):
    '''Control of some parameters as development lenght 
       minimum reinforcement and so on.

       :ivar psi_t: reinforcement location factor; "concrete below" is 
                    taken as the depth from the rebar center to the bottom 
                    of the concrete section.
                    * 1.3 for concrete below ≥ 12 inches 
                    * 1.0 for concrete below < 12 inches 
       :ivar psi_e: coating factor 
                    * 1.5 for epoxy coated bars with cover (to center of bar)
                          less than 3db (spacing is not calculated)
                    * 1.2 for all other epoxy coated bars 
                    * 1.0 for uncoated bars
       :ivar Lambda: lightweight aggregate concrete factor 
                    * 0.75 for lightweight concrete 
                    * 1.0 for normal-weight concrete
       :ivar Ktr: transverse reinforcement index. Factor that represents the 
                  contribution of confining reinforcement across potential 
                  splitting planes; is conservatively assumed to be zero.
       :ivar concreteCover: the distance from center of a bar or wire to 
                            nearest concrete surface.
       :ivar spacing: center-to-center spacing of bars or wires being 
                      developed, in.
    '''

    def __init__(self, psi_t= 1.3, psi_e= 1.0, concreteCover= 35e-3, spacing= 150e-3):
        '''Constructor.'''
        self.psi_t= psi_t
        self.psi_e= psi_e
        self.concreteCover= concreteCover
        self.spacing= spacing

    def getKtr(self, n= 1, Atr= 0.0):
        '''Return the transverse reinforcing index according to
           clause 25.4.2.3 of ACI 318-14.

        :param n: number of bars or wires being developed or
                  lap spliced along the plane of splitting.
        :param Atr: total cross-sectional area of all transverse reinforcement
                    within spacing s that crosses the potential plane of
                    splitting through the reinforcement being developed.
        '''
        Atr_inch2= Atr*(1.0/0.0254)**2
        return (40.0*Atr/self.spacing/n)*0.0254 #To meters.

    def getConfinementTerm(self, phi, n= 1, Atr= 0.0):
        '''Return the confinement term as defined in clause 25.4.2.3 
           of ACI 318-14.

        :param phi: nominal diameter of bar, wire, or prestressing strand.
        :param n: number of bars or wires being developed or
                  lap spliced along the plane of splitting.
        :param Atr: total cross-sectional area of all transverse reinforcement
                    within spacing s that crosses the potential plane of
                    splitting through the reinforcement being developed.
        '''
        Ktr= self.getKtr(n,Atr)
        return (self.concreteCover+Ktr)/phi
    def getBasicAnchorageLength(self, concrete, phi, steel):
        """Returns anchorage length in tension according to ACI-318 25.4.2.3.

        :param concrete: concrete material.
        :param phi: nominal diameter of bar, wire, or prestressing strand.
        :param fck: concrete characteristic compressive strength.
        :param steel: reinforcement steel.
        """
        psi_t_psi_e= min(self.psi_t*self.psi_e,1.7)
        psi_t_psi_e_psi_s= psi_t_psi_e*ACI_materials.getPsi_sFromDiameter(phi)
        #Clause 25.4.1.4:
        l= min(concrete.getLambdaSqrtFck(),concrete.Lambda*concrete.toPascal*100.0)
        retval= 3.0/40.0*(steel.fyk/l)
        retval*= psi_t_psi_e_psi_s/self.getConfinementTerm(phi)
        retval*= phi
        return max(retval,12*0.0254) #Clause 25.4.2.1b
