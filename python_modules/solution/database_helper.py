# -*- coding: utf-8 -*-
''' Code to make database handling easier.'''

from __future__ import print_function
from __future__ import division

__author__= "Luis C. Pérez Tato (LCPT) Ana Ortega (AO_O)"
__copyright__= "Copyright 2016, LCPT, AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com, ana.ortega.ort@gmail.com"

import os
import sys
from misc_utils import log_messages as lmsg

def saveCombination(prb, nmbComb, tagComb, pth,db):
    fileName= pth+nmbComb+".db"
    os.system("rm -f "+fileName)
    db= prb.newDatabase("BerkeleyDB",fileName)
    db.save(tagComb)

class DatabaseHelperSolve:
    previousName= ""
    tagPrevia= -1
    db= None
    def __init__(self,db):
        self.previousName= ""
        self.tagPrevia= .1
        self.db= db
    def helpSolve(self,comb):
        previa= comb.getCombPrevia()
        if(previa!=None):
            self.previousName= previa.getName
            self.tagPrevia= previa.tag
            self.restore()  
    def restore(self):
        if(self.tagPrevia>0):
            self.db.restore(self.tagPrevia*100)
    def solveComb(self,comb,solutionProcedure):
        solutionProcedure.resetLoadCase()
        self.helpSolve(comb)
        ''' 
        print("previousName= ",previousName)
        print("tag= ",comb.tag)
        print("tagPrevia= ",tagPrevia)
        print("descomp previa= ",getComponentsCombPrevia)
        print("resto sobre previa= ",getComponentsRestoSobrePrevia)
        '''
        comb.addToDomain()
        analOk= solutionProcedure.solve()
        if(analOk!=0):
            className= type(self).__name__
            methodName= sys._getframe(0).f_code.co_name
            lmsg.error(className+'.'+methodName+'; can\'t solve.')        
        self.db.save(comb.tag*100)
        comb.removeFromDomain()
    
