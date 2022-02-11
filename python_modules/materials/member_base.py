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
from model.geometry import geom_utils as gu

import xc_base
import geom
import xc

class MemberConnection(object):
    '''Member connection

       :ivar rotI: fixity of the rotation at member start.
       :ivar transI: fixity of the translation at member start.
       :ivar rotJ: fixity of the rotation at member end.
       :ivar transJ: fixity of the translation at member end.
    '''
    def __init__(self,rotI='free',transI='fixed',rotJ= 'free',transJ= 'fixed'):
        '''Constructor.'''
        self.rotI= rotI
        self.transI= transI
        self.rotJ= rotJ
        self.transJ= transJ

    def getEffectiveBucklingLengthCoefficientRecommended(self):
        '''Return the effective length factor
           according to table C-A-7.1 or AISC specification
           and NDS 2018 appendix G'''
        if(self.rotI=='fixed'):
            if(self.rotJ=='fixed'):
                if(self.transJ=='fixed'):
                    retval= .65 # Theoretical .5
                else: # self.transJ=='free'
                    retval= 1.2 #Theoretical 1.0
            else: # self.rotJ=='free'
                if(self.transJ== 'fixed'):
                    retval= .8 # Theoretical .7
                else: # self.transJ=='free'
                    retval= 2.1 # Theoretical 2.0
        else: # self.rotI=='free'
             if(self.rotJ=='fixed'):
                 if(self.transJ=='free'):
                     retval= 2.0 # Theoretical 2.0
                 else:
                     retval= 0.8 # Theoretical .7
             else: # self.rotJ=='free'
                 if(self.transI=='fixed' and self.transJ=='fixed'):
                     retval= 1.0 # Theoretical 1.0
                 else:
                     retval= 1e6 # Stiffness matrix singular
        return retval

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

    def createRecorder(self, recorderType:str, calcSet= None):
        '''Install recorder for verification of ULS criterion.

        :param recorderType: type of the recorder to install.
        :param calcSet: set of elements to be checked (defaults to 'None' which 
                        means that this set will be created elsewhere). In not
                        'None' the member elements will be appended to this set.
        '''
        # Create recorder.
        domain= self.getPreprocessor().getDomain
        recorder= domain.newRecorder(recorderType,None)
        if(not self.elemSet):
            self.createElementSet()
        eleTags= list()
        for e in self.elemSet:
            eleTags.append(e.tag)
            e.setProp('ULSControlRecorder',recorder)
        idEleTags= xc.ID(eleTags)
        recorder.setElements(idEleTags)
        # Append the member elements into calcSet
        if(calcSet):
            calcSet.extend(self.elemSet)
        return recorder
