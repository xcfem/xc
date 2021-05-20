      SUBROUTINE SKYSF2 (A,D,DGPNT,JMIN,JMAX)
C
C***********************************************************************
C*                                                                     *
C*  - PURPOSE :                                                        *
C*    SKYSF2 factors (A) into (L)*(D)*(L') in a standard way           *
C*                                                                     *
C*  - ARGUMENTS :                                                      *
C*    A     (arb) : the coefficients of the matrix (A)                 *
C*    D     (aio) : reciprocal of the entries in (D)                   *
C*    DGPNT (aii) : addresses of the diagonal coefficients of (L)      *
C*    JMIN  (sii) : first column of (A)                                *
C*    JMAX  (sii) : last  column of (A)                                *
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
      INTEGER          DGPNT(*),JMAX,JMIN
      DOUBLE PRECISION A(*),D(*)
C
C==== local variables ==================================================
C
      INTEGER          I,II,IJ,ILEN,J,JJ,JLEN,LGAP,MLEN
      DOUBLE PRECISION PROD,SUM
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
      JJ   = 0
C
C.... loop on the columns of (A) .......................................
C
      DO 010 J = JMIN,JMAX
C
         SUM  = 0.0D0
	 JLEN = DGPNT(J) - JJ  
	 JJ   = DGPNT(J)
	 II   = J - JLEN
C
C....... factorize the j-th column .....................................
C
	 DO 030 IJ = JJ-JLEN+1,JJ-1
	    II = II + 1
	    IF ( D(II) .NE. 0.0D0 ) THEN
               PROD  = A(IJ)*D(II)
               SUM   = SUM + PROD*A(IJ)
               A(IJ) = PROD
            END IF
  030    CONTINUE
C
         A(JJ) = A(JJ) - SUM
C
C....... compute the reciprocal of the pivot ...........................
C
         IF ( A(JJ) .NE. 0.0D0 ) THEN
            D(J) = 1.0D0/A(JJ)
         ELSE
            D(J) = 0.0D0
         END IF
C
         A(JJ) = D(J)
C
	 II = JJ
C
C....... update the remaining columns ..................................
C
         DO 020 I = J+1,JMAX
	    ILEN = DGPNT(I) - II
	    II   = DGPNT(I)
	    LGAP = I - J
            MLEN = MIN0(JLEN-1,ILEN-LGAP-1)    
	    IF ( MLEN .GT. 0 ) THEN
	       IJ = II - LGAP
	       A(IJ) = A(IJ) - DDOT(MLEN,A(JJ-MLEN),1,A(IJ-MLEN),1)
            END IF
  020    CONTINUE
C
  010 CONTINUE
C
      RETURN
C
C**** end of SKYSF2 ****************************************************
C
      END
