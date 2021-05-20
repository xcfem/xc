      SUBROUTINE SKYSSF (LN,NX,A,D,X,DGPNT,BLOCK,NBLOCK,
     &                   FNAME,FUNIT,INFO)
C
C***********************************************************************
C*                                                                     *
C*  - PURPOSE :                                                        *
C*    SKYSSF performs a forward substitution in (L)*(D)*(L')*(X)=(Y)   *
C*                                                                     *
C*  - ARGUMENTS :                                                      *
C*    LN     (sii) : leading dimension of (X)                          *
C*    NX     (sii) : number of right-hand sides                        *
C*    A      (arb) : the factor (L) stored in a skyline pattern        *
C*    D      (ari) : reciprocal of the coefficients of (D)             *
C*    X      (arb) : right-hand sides, LNxNX array                     *
C*    DGPNT  (aii) : addresses of the diagonal coefficients of (L)     *
C*    BLOCK  (aii) : blocking information                              *
C*    NBLOCK (sii) : number of blocks                                  *
C*    FNAME  (ssi) : file name associated with (L)                     *
C*    FUNIT  (sii) : file unit associated with (A), which is           *
C*                   taken into account only if FNAME is               *
C*                   different from "INCORE"                           *
C*    INFO   (sio) : information on the execution of SKYSSF            *
C*                   = 0, normal exit                                  *
C*                   = 1, IO error                                     *
C*                                                                     *
C*  - BLAS kernel :                                                    *
C*    DDOT                                                             *
C*                                                                     *
C***********************************************************************
C
C==== arguments ========================================================
C
      INTEGER          BLOCK(3,*),DGPNT(*),INFO,FUNIT,LN,NBLOCK,NX
      DOUBLE PRECISION A(*),D(*),X(LN,NX)
      CHARACTER*(*)    FNAME
C
C==== local variables ==================================================
C
      INTEGER          IA,IBLOCK,IX,J,JDIAG,JMAX,JMIN,K,NCFB
C
C==== BLAS kernel ======================================================
C
      DOUBLE PRECISION DDOT
C
C**** executable statements ********************************************
C
      INFO = 0 
C
      IF ( FNAME.NE.'INCORE' .AND. FNAME.NE.'incore' )
     &REWIND (UNIT=FUNIT,ERR=1)
C
C.... loop on all blocks ...............................................
C
      DO 010 IBLOCK = 1,NBLOCK
C
         IA   = 1
C
C....... define the limits of the current block ........................
C
	 JMIN = BLOCK(1,IBLOCK)
	 JMAX = BLOCK(2,IBLOCK)
         NCFB = DGPNT(JMAX)
C
         IF ( FNAME.NE.'INCORE' .AND. FNAME.NE.'incore' )
     &   READ (UNIT=FUNIT,ERR=1) (A(K),K=1,NCFB)
C
C....... subtract the computed terms from the right-hand sides .........
C
         DO 020 J = JMIN,JMAX
            JDIAG = DGPNT(J)
            IF ( IA .LT. JDIAG ) THEN
               IX = J - JDIAG + IA
               DO 030 K = 1,NX
                  X(J,K) = X(J,K) - DDOT(JDIAG-IA,A(IA),1,X(IX,K),1)
  030          CONTINUE
            END IF
            D(J) = A(JDIAG)
            IA = JDIAG + 1
  020    CONTINUE
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
C**** end of SKYSSF ****************************************************
C
      END
