# -*- coding: utf-8 -*-
'''OpenSees style commands. Useful when translating OpenSees tcl files.'''

from __future__ import division

__author__= " Ana Ortega (AO_O), Luis C. Pérez Tato (LCPT) "
__copyright__= "Copyright 2017, AO_O, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "ana.ortega@ciccp.es, l.pereztato@ciccp.es "

# Impone movimiento nulo en todos los DOF of the node.
def fix(constraintHandler, idNode, gdls):
    ''' Simulates OpenSees fix command.

    :param constraintHandler: constraint handler.
    :param idNode: identifier of the node to constraint.
    :param gdls: degrees of freedom to fix (0: free, 1 fixed).
    '''
    sz= len(gdls)
    for i, gdl in enumerate(gdls):
        if gdl!=0:
            constraintHandler.newSPConstraint(idNode,i,0.0) # Node id: idNode
