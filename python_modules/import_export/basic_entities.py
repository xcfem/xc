# -*- coding: utf-8 -*-

'''Basic entities for data exchange.'''

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2016, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

from misc_utils import log_messages as lmsg

class ComponentSupportRecord:
  '''Constraints for x,y,z,rx,ry,rz displacements of a node/kPoint.'''
  def __init__(self,typ= 'Rigid', k= 0.0):
    self.typ= typ #Free","Rigid", "Flexible"
    self.k= k
  def getTypeCode(self):
    if(self.typ=='Free'):
      return '0'
    elif(self.typ=='Rigid'):
      return '1'
    elif(self.typ=='Flexible'):
      return '2'
    else:
      lmsg.error('ComponentSupportRecord::getTypeCode; error: unknown type: '+ self.typ)
      return '0'
  def __str__(self):
    retval= self.typ
    if(self.typ == 'Flexible'):
      retval+= '('+str(self.k)+')'
    return retval

class SupportRecord(object):
  ''' Constraints for displacements'''
  def __init__(self, id, xComp= ComponentSupportRecord(), yComp= ComponentSupportRecord(), zComp= ComponentSupportRecord(), rxComp= ComponentSupportRecord('Free'), ryComp= ComponentSupportRecord('Free'), rzComp= ComponentSupportRecord('Free')):
    self.id= id
    self.typ= 'Standard'
    self.xComp= xComp
    self.yComp= yComp
    self.zComp= zComp
    self.rxComp= rxComp
    self.ryComp= ryComp
    self.rzComp= rzComp
  def setupFromComponentLabels(self,componentLabels):
    self.xComp= ComponentSupportRecord(componentLabels[0])
    self.yComp= ComponentSupportRecord(componentLabels[1])
    self.zComp= ComponentSupportRecord(componentLabels[2])
    self.rxComp= ComponentSupportRecord(componentLabels[3])
    self.ryComp= ComponentSupportRecord(componentLabels[4])
    self.rzComp= ComponentSupportRecord(componentLabels[5])
  def getStrConstraints(self):
    return 'type: ' + self.typ + ' x: ' + str(self.xComp)+ ' y: ' + str(self.yComp) + ' z: ' + str(self.zComp)+ ' rx: ' + str(self.rxComp)+ ' ry: ' + str(self.ryComp) + ' rz: ' + str(self.rzComp)
    
  def __str__(self):
    return str(self.id) + ' nodeId: ' + str(self.nodeId) + ' ' + self.getStrConstraints()
