# -*- coding: utf-8 -*-
'''OpenSees style commands. Useful when translating OpenSees tcl files.'''

from __future__ import division

__author__= " Ana Ortega (AO_O), Luis C. Pérez Tato (LCPT) "
__copyright__= "Copyright 2017, AO_O, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "ana.ortega@ciccp.es, l.pereztato@ciccp.es "

# Impone movimiento nulo en todos los DOF of the node.
def fix(constraintHandler, idNode, dofs):
    ''' Simulates OpenSees fix command.

    :param constraintHandler: constraint handler.
    :param idNode: identifier of the node to constraint.
    :param dofs: degrees of freedom to fix (0: free, 1 fixed).
    '''
    for i, gdl in enumerate(dofs):
        if gdl!=0:
            constraintHandler.newSPConstraint(idNode,i,0.0) # Node id: idNode
