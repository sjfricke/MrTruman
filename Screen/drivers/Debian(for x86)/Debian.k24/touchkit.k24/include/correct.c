
#ifndef _CORRECT_C_
#define _CORRECT_C_

#ifdef __MSDOS__
	#define _FAR_ far
#else
	#define _FAR_
#endif

void CorrectPoint( Mapping _FAR_ *pMapping, int _FAR_ *iCx, int _FAR_ *iCy )
{
	#define _STATIC_
	_STATIC_ long	lCeX, lCeY;
	_STATIC_ long	lOffsetX[2];
	_STATIC_ long	lOffsetY[2];
	_STATIC_ long	lSpecX[2];
	_STATIC_ long	lSpecY[2];
	_STATIC_ long	lTemp, lTemp1;
	_STATIC_ long	lTempX, lTempY;
	_STATIC_ long	lDen;
	_STATIC_ long	lNewX, lNewY;
	#define SPEC 1843
	#define ffff(X,Y)							\
	(((X)-pMapping->uiLLX)*lOffsetY[1] - ((Y)-pMapping->uiLLY)*lOffsetX[1])

	lCeX	=	 pMapping->uiLLX;
	lCeY	=	 pMapping->uiLLY;
	lOffsetX[1] = pMapping->uiURX - pMapping->uiLLX;
	lOffsetY[1] = pMapping->uiURY - pMapping->uiLLY;
	lTemp = ffff( pMapping->uiULX, pMapping->uiULY );
	lTempX = *iCx; /* - lCeX; */
	lTempY = *iCy; /* - lCeY; */
	lTemp1 = ffff( lTempX, lTempY );
	lTempX -= lCeX;
	lTempY -= lCeY;
	if( lTemp >= 0 )
	{
		if( lTemp1 >= 0 )
		{
			lOffsetX[0] = pMapping->uiULX - pMapping->uiLLX;
			lOffsetY[0] = pMapping->uiULY - pMapping->uiLLY;
			lSpecX[0]=0;
			lSpecX[1]=SPEC;
			lSpecY[0]=SPEC;
			lSpecY[1]=SPEC;
		}else{
			lOffsetX[0] = pMapping->uiLRX - pMapping->uiLLX;
			lOffsetY[0] = pMapping->uiLRY - pMapping->uiLLY;
			lSpecX[0]=SPEC;
			lSpecX[1]=SPEC;
			lSpecY[0]=0;
			lSpecY[1]=SPEC;
			}
	}else if( lTemp1 < 0 )
	{
		lOffsetX[0] = pMapping->uiULX - pMapping->uiLLX;
		lOffsetY[0] = pMapping->uiULY - pMapping->uiLLY;
		lSpecX[0]=0;
		lSpecX[1]=SPEC;
		lSpecY[0]=SPEC;
		lSpecY[1]=SPEC;
	}else{
		lOffsetX[0] = pMapping->uiLRX - pMapping->uiLLX;
		lOffsetY[0] = pMapping->uiLRY - pMapping->uiLLY;
		lSpecX[0]=SPEC;
		lSpecX[1]=SPEC;
		lSpecY[0]=0;
		lSpecY[1]=SPEC;

	}
	lDen = ( lOffsetX[0] * lOffsetY[1] - lOffsetX[1] * lOffsetY[0] ) / 4;

	if( lDen != 0 )
	{
		lNewX = ( ( lOffsetY[1] * lSpecX[0] - lOffsetY[0] * lSpecX[1]) / 4 * lTempX +
				  ( lOffsetX[0] * lSpecX[1] - lOffsetX[1] * lSpecX[0]) / 4 * lTempY ) / lDen + 102; 
		lNewY = ( ( lOffsetY[1] * lSpecY[0] - lOffsetY[0] * lSpecY[1]) / 4 * lTempX +
				( lOffsetX[0] * lSpecY[1] - lOffsetX[1] * lSpecY[0]) / 4 * lTempY ) / lDen + 102; 		
	}
	else
	{
		lNewX = 0;
		lNewY = 0;
	}
	
/*	
	if( lNewX < 0 ) 
		lNewX = 0;
	if( lNewX > MAX_DIMENSION )
		lNewX = MAX_DIMENSION;
	if( lNewY < 0 ) 
		lNewY = 0;
	if( lNewY > MAX_DIMENSION )
		lNewY = MAX_DIMENSION;
*/
	*iCx = ( int )lNewX;
	*iCy = ( int )lNewY;
}

#if defined(_25PtCal) /* { */

#define NUM_OF_ROL 4
static int m_i25PtCoeffTempX[2][2];/*[Col][Row]*/
static int m_i25PtCoeffTempY[2][2];/*[Col][Row]*/
static void Update25PtCoefficient( LinearCoef _FAR_ *pCoef, int byCol, int byRow )
{	int	byCountCol, byCountRow;
	
	for( byCountCol = 0; byCountCol < 2; byCountCol++ )
	{	for( byCountRow = 0; byCountRow < 2; byCountRow++ )
		{	m_i25PtCoeffTempX[ byCountCol ][ byCountRow ] = 
				pCoef->delta[0][ byCol + byCountCol ][ byRow + byCountRow ];
			m_i25PtCoeffTempY[ byCountCol ][ byCountRow ] = 
				pCoef->delta[1][ byCol + byCountCol ][ byRow + byCountRow ];
		}
	}		
}

static void Update25PtCorrectData(
	short sXOffset, short sYOffset, int _FAR_ *iCx, int _FAR_ *iCy )
{
	short sDeltaX, sDeltaY;
	long lTemp;
	
	sDeltaX = ( short )( ( ( long )( m_i25PtCoeffTempX[1][0] - m_i25PtCoeffTempX[0][0] ) * sXOffset ) / 1024 );
	sDeltaX += ( short )( ( ( long )( m_i25PtCoeffTempX[0][1] - m_i25PtCoeffTempX[0][0] ) * sYOffset ) / 1024 );
	lTemp = ( long )( m_i25PtCoeffTempX[0][0] + m_i25PtCoeffTempX[1][1] - m_i25PtCoeffTempX[0][1] - m_i25PtCoeffTempX[1][0] ) * sYOffset / 1024;
	lTemp = lTemp * sXOffset / 1024;
	sDeltaX += m_i25PtCoeffTempX[0][0] + ( short )lTemp; 
	
	sDeltaY = ( short )( ( ( long )( m_i25PtCoeffTempY[1][0] - m_i25PtCoeffTempY[0][0] ) * sXOffset ) / 1024 );
	sDeltaY += ( short )( ( ( long )( m_i25PtCoeffTempY[0][1] - m_i25PtCoeffTempY[0][0] ) * sYOffset ) / 1024 );
	lTemp = ( long )( m_i25PtCoeffTempY[0][0] + m_i25PtCoeffTempY[1][1] - m_i25PtCoeffTempY[0][1] - m_i25PtCoeffTempY[1][0] ) * sYOffset / 1024;
	lTemp = lTemp * sXOffset / 1024;
	sDeltaY += m_i25PtCoeffTempY[0][0] + ( short )lTemp; 
	/*
	//==============================================================================================================================================
	// X = Xraw - sDeltaX
	// Y = Yraw - sDeltaY
	//==============================================================================================================================================
	*/
	*iCx = *iCx - sDeltaX;
	*iCy = *iCy - sDeltaY;
}


void Calibration25Pt( LinearCoef _FAR_ *pCoef, int _FAR_ *iCx, int _FAR_ *iCy )
{
	int	sXOffset, sYOffset;
	int	byCol, byRow;
	static int sSPAN[ 4 ] = { 409, 512, 512, 410 };
	#define	COL_MARGIN 102

	/*
	*iCx = *iCx / 32;
	*iCy = *iCy / 32;
	*/
	sXOffset = *iCx - COL_MARGIN;
	for( byCol = 0; byCol < NUM_OF_ROL - 1; byCol++ )
	{
		if( sXOffset <= sSPAN[ byCol ] )
			break;
		sXOffset -= sSPAN[ byCol ];
	}
	/*
	if( NUM_OF_ROL == byCol )
	{
		byCol--;
		sXOffset += sSPAN[ byCol ];
	}
	*/
	sYOffset = *iCy - COL_MARGIN;
	for( byRow = 0; byRow < NUM_OF_ROL - 1; byRow++ )
	{
		if( sYOffset <= sSPAN[ byRow ] )
			break;
		sYOffset -= sSPAN[ byRow ];
	}
	/*
	if( NUM_OF_ROL == byRow )
	{
		byRow--;
		sYOffset += sSPAN[ byRow ];
	}
	*/
	if( byCol == 1 || byCol == 2 )
		sXOffset *= 2;
	else
		sXOffset = ( sXOffset * 2 ) + ( sXOffset / 2 );
	
	if( byRow == 1 || byRow == 2 )
		sYOffset *= 2;
	else
		sYOffset = ( sYOffset * 2 ) + ( sYOffset / 2 );
		
	Update25PtCoefficient( pCoef, byCol, byRow );
	Update25PtCorrectData( sXOffset, sYOffset, iCx, iCy );
	
	/*
	*iCx = *iCx * 32;
	*iCy = *iCy * 32;
	*/

	#undef COL_MARGIN
}

#undef NUM_OF_ROL

#endif /* } */

#endif



