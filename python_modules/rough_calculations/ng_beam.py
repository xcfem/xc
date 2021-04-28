# -*- coding: utf-8 -*-
# Base for beam formulas

from __future__ import division

class Beam(object):
    ''' Base class for beam formulas.

    :ivar E: elastic modulus.
    :ivar I: moment of inertia of the beam section.
    :ivar l: beam length.
    '''
    def __init__(self,E= 1.0, I= 1.0, l= 1.0):
        ''' Constructor.

        :param E: elastic modulus.
        :param I: moment of inertia of the beam section.
        :param l: beam length.
        '''
        self.E= E
        self.I= I
        self.l= l

    def EI(self):
        ''' Return the tangent stiffness.'''
        return self.E*self.I
