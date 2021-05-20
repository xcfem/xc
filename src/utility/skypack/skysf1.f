      SUBROUTINE SKYSF1 (A,B,DGPNT,BLOCK,NBLOCK,LBLOCK,FFUNIT,INFO) 
C
C***********************************************************************
C*                                                                     *
C*  - PURPOSE :                                                        *
C*    SKYSF1 updates (A) with the blocks already factorized            *
C*                                                                     *
C*  - ARGUMENTS :                                                      *
C*    A      (arb) : block being   factorized                          *
C*    B      (arw) : block already factorized                          *
C*    DGPNT  (aii) : addresses of the diagonal coefficients            *
C*    BLOCK  (aii) : blocking information                              *
C*    NBLOCK (sii) : number of blocks                                  *
C*    LBLOCK (sii) : index of the current block                        *
C*    FFUNIT (sii) : file unit for (B) if NBLOCK > 1                   *
C*    INFO   (sio) : information on the execution of SKYSF1            *
C*                   = 0, normal exit                                  *
C*                   = 1, IO error                                     *
C*                                                                     *
C*  - BLAS kernel :                                                    *
C*    DDOT                                                             *
C*                                                                     *
C*  - INTRINSIC FUNCTION :                                             *
C*    MIN0                                                             *
C*                                                                     *
C***********************************************************************
C
C==== arguments ========================================================
C
      INTEGER          BLOCK(3,*),DGPNT(*),FFUNIT,INFO,LBLOCK,NBLOCK
      DOUBLE PRECISION A(*),B(*)
C
C==== local variables ==================================================
C
      INTEGER          I,IBLOCK,II,IJ,ILEN,IMAX,IMIN,J,JJ,
     &                 JLEN,JMAX,JMIN,LGAP,MLEN
C
C==== BLAS kernel ======================================================
C
      DOUBLE PRECISION DDOT
C
C==== intrinsic function ===============================================
C
      INTEGER          MIN0
C
C**** executable statements ********************************************
C
      INFO = 0
C
      IF ( NBLOCK .GT. 1 ) REWIND (UNIT=FFUNIT,ERR=1)
C
C.... previous LBLOCK-1 factorized blocks ..............................
C
      DO 010 IBLOCK = 1,LBLOCK-1
C
         IF ( BLOCK(3,LBLOCK) .GE. BLOCK(2,IBLOCK) ) THEN 
C
C.......... skip this block ............................................
C
            READ (UNIT=FFUNIT,ERR=1)
C
         ELSE
C
C.......... define the limits of the blocks ............................
C
            II   = 0
	    IMIN = BLOCK(1,IBLOCK)
	    IMAX = BLOCK(2,IBLOCK) 
	    JMIN = BLOCK(1,LBLOCK)
	    JMAX = BLOCK(2,LBLOCK) 
C
            READ (UNIT=FFUNIT,ERR=1) (B(I),I=1,DGPNT(IMAX)) 
C
C.......... loop on the columns of (B) .................................
C
            DO 020 I = IMIN,IMAX
C
	       ILEN = DGPNT(I) - II
               II   = DGPNT(I)
	       JJ   = 0
C
C............. loop on the columns of (A) ..............................
C
	       DO 030 J = JMIN,JMAX
	          JLEN = DGPNT(J) - JJ  
		  JJ   = DGPNT(J)
	          LGAP = J - I
                  MLEN = MIN0(ILEN-1,JLEN-LGAP-1)    
	          IF ( MLEN .GE. 1 ) THEN
	             IJ = JJ - LGAP
	             A(IJ) = A(IJ)-DDOT(MLEN,B(II-MLEN),1,A(IJ-MLEN),1)
                  END IF
  030          CONTINUE
C
  020       CONTINUE
C
         END IF
C
  010 CONTINUE
C
      RETURN
C
C.... an IO error has occurred .........................................
C
    1 INFO = 1
C 
      RETURN
C
C**** end of SKYSF1 ****************************************************
C
      END
