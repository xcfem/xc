# -*- coding: utf-8 -*-
''' Code to keep backwards compatibility.'''


__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AO_O)"
__copyright__= "Copyright 2024, LCPT and AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es" "ana.Ortega@ciccp.es"


import sys
from misc_utils import log_messages as lmsg

if sys.version_info < (3,9):
    
    def removeprefix(self: str, prefix: str) -> str:
        """ Backport of `removeprefix` from PEP-616 (Python 3.9+) """

        if self.startswith(prefix):
            return self[len(prefix):]
        else:
            return self


    def removesuffix(self: str, suffix: str) -> str:
        """ Backport of`removesuffix` from PEP-616 (Python 3.9+) """

        if self.endswith(suffix):
            return self[:-len(suffix)]
        else:
            return self
