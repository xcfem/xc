      SUBROUTINE SKYSSB (LN,NX,A,X,DGPNT,BLOCK,NBLOCK,FUNIT,INFO)
C
C***********************************************************************
C*                                                                     *
C*  - PURPOSE :                                                        *
C*    SKYSSB performs a backward substitution in (L)*(D)*(L')*(X)=(Y)  *
C*                                                                     *
C*  - ARGUMENTS :                                                      *
C*    LN     (sii) : leading dimension of (X)                          *
C*    NX     (sii) : number of right-hand sides                        *
C*    A      (arb) : the factor (L) stored in a skyline pattern        *
C*    X      (arb) : right-hand sides, LNxNX array                     *
C*    DGPNT  (aii) : addresses of the diagonal coefficients of (L)     *
C*    BLOCK  (aii) : blocking information                              *
C*    NBLOCK (sii) : number of blocks                                  *
C*    FUNIT  (sii) : file unit for (L) if NBLOCK > 1                   *
C*    INFO   (sio) : information on the execution of SKYSSB            *
C*                   = 0, normal exit                                  *
C*                   = 1, IO error                                     *
C*                                                                     *
C*  - BLAS kernel :                                                    *
C*    DAXPY                                                            *
C*                                                                     *
C***********************************************************************
C
C==== arguments ========================================================
C
      INTEGER          BLOCK(3,*),DGPNT(*),FUNIT,INFO,LN,NBLOCK,NX
      DOUBLE PRECISION A(*),X(LN,NX)
C
C==== local variables ==================================================
C
      INTEGER          IA,IBLOCK,IX,J,JDIAG,JMAX,JMIN,K
C
C**** executable statements ********************************************
C
      INFO = 0
C
C.... loop on all blocks ...............................................
C
      IF ( NBLOCK .GT. 1 ) BACKSPACE (UNIT=FUNIT,ERR=1)
C
      DO 010 IBLOCK = NBLOCK,1,-1
C
C....... define the limits of the current block ........................
C
	 JMIN = BLOCK(1,IBLOCK)
	 JMAX = BLOCK(2,IBLOCK)
C
C....... subtract the scaled columns from the right-hand sides .........
C
         DO 020 J = JMAX,JMIN,-1
	    IF ( J .EQ. JMIN ) THEN
	       IA = 1
	    ELSE
               IA = DGPNT(J-1) + 1
	    END IF
            JDIAG = DGPNT(J)
            IF ( IA .LT. JDIAG ) THEN
               IX = J - JDIAG + IA
               DO 030 K = 1,NX
                  CALL DAXPY (JDIAG-IA,-X(J,K),A(IA),1,X(IX,K),1)
  030          CONTINUE
            END IF
  020    CONTINUE
C
C....... read the next block ...........................................
C
         IF ( IBLOCK .GT. 1 ) THEN
            BACKSPACE (UNIT=FUNIT,ERR=1)
            READ (UNIT=FUNIT,ERR=1) (A(K),K=1,DGPNT(JMIN-1))
            BACKSPACE (UNIT=FUNIT,ERR=1)
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
C**** end of SKYSSB ****************************************************
C
      END
