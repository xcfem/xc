# -*- coding: utf-8 -*-
from __future__ import division
''' Base classes and functions for structural members
    (beams, columns, ...).'''

__author__= "Ana Ortega (AO_O) and Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2022, AO_O and LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= " ana.Ortega.Ort@gmail.com, l.pereztato@gmail.com"

from misc_utils import log_messages as lmsg

class Member(object):
    '''Base class for structural members.
    
    :ivar name: object name.
    :ivar lstLines: ordered list of lines that make up the member 
          (defaults to None).
    :ivar elemSet: elements along the member.
    '''
    def __init__(self, name, lstLines=None):
        '''Constructor.

        :param name: object name.
        :param lstLines: ordered list of lines that make up the member 
                        (defaults to None).
        '''
        self.name= name
        self.lstLines= lstLines
        self.elemSet= None # elements along the member.
        
    def getPreprocessor(self):
        ''' Return the XC preprocessor.'''
        retval= None
        if self.lstLines:
            retval=self.lstLines[0].getPreprocessor
        else:
            lmsg.error('No lines.')
        return retval

    def getMemberGeometry(self):
        ''' Return the lines and points along the member.'''
        lstLn= None
        lstP3d= None
        if(self.lstLines):
            lstLn= self.lstLines
            lstP3d= gu.lstP3d_from_lstLns(lstLn)
        else:
            lmsg.warning('Incomplete member definition: list of lines required')
        return lstLn, lstP3d

    def getLength(self):
        ''' Return the member length.'''
        return self.pline.getLength()
    
    def createElementSet(self):
        '''Create the attributes 'length' and 'elemSet' that 
        represent the length of the member and the set of elements 
        included in it.'''
        lstLn, lstP3d= self.getMemberGeometry()
        
        prep= self.getPreprocessor()
        # set of elements included in the member
        setName= self.name+'Set'
        if(prep.getSets.exists(setName)):
            lmsg.error('set: '+setName+ ' already exists.')
        s= prep.getSets.defSet(setName)
        self.elemSet= s.elements
        for l in lstLn:
            for e in l.elements:
                self.elemSet.append(e)

