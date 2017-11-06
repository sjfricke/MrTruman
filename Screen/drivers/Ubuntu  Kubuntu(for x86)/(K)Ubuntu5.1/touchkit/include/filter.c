


VOID OnZTransfer(  int* iX, int* iY,BOOL bPenDown,  PFILTERCOEFF pFilterCoeff  )
{
	static long	ZTSUMX;
	static long	ZTSUMY;
	static	int	iZTransferTempX[ZTNum] ;
	static	int	iZTransferTempY[ZTNum] ;
	static	int	ZTransferIndex = -1;
	static long lCofZTranX[ZTNum ];
	static long lCofZTranY[ZTNum ];
	
	int	iTemp;
	long	lTempX;
	long	lTempY;
	int	ZTIndexTemp;
	if( !(pFilterCoeff->bFilterOnOff) )
		return;
	lTempX = 0;
	lTempY = 0;
	if( bPenDown )
	{
		if( -1 == ZTransferIndex  )
		{
			for( iTemp = 0; iTemp < ZTNum; iTemp++ )
			{
				iZTransferTempX[ iTemp ] = *iX;
				iZTransferTempY[ iTemp ] = *iY;
			}
			ZTIndexTemp = 0;
			ZTransferIndex = 1;
			ZTSUMX = 0;
			for( iTemp = 0; iTemp < ZTNum;iTemp++  )
			{
				lCofZTranX[iTemp] = pFilterCoeff-> lDefZTranCoeff[iTemp];
				lCofZTranY[iTemp] = pFilterCoeff-> lDefZTranCoeff[iTemp];
				ZTSUMX += pFilterCoeff-> lDefZTranCoeff[iTemp];
			}
			ZTSUMY  = ZTSUMX;
		}else
		{
			iZTransferTempX[ ZTransferIndex ] = *iX;
			iZTransferTempY[ ZTransferIndex ] = *iY;
			ZTIndexTemp = ZTransferIndex;
			ZTransferIndex++;
			if( ZTNum == ZTransferIndex )
			{
				ZTransferIndex = 0;
			}
			
		}
	}else
	{
		iZTransferTempX[ ZTransferIndex ] = *iX;
		iZTransferTempY[ ZTransferIndex ] = *iY;
		ZTIndexTemp =  ZTransferIndex;
		ZTransferIndex = -1;
	
	}
	if( abs(iZTransferTempX[ZTIndexTemp%ZTNum]-iZTransferTempX[ ( ZTIndexTemp +7 )%ZTNum ] ) > pFilterCoeff-> lThreshold )
	{
		if( pFilterCoeff-> lUpperBound > lCofZTranX[ 3 ] )
		{
			ZTSUMX++;
			lCofZTranX[ 3 ]++;
		}
	}else
	{
		if(  lCofZTranX[ 3 ] > pFilterCoeff-> lLowerBound )
		{
			ZTSUMX--;
			lCofZTranX[ 3 ]--;
		}

	}
	if( abs(iZTransferTempY[  ZTIndexTemp%ZTNum ] -iZTransferTempY[ ( ZTIndexTemp +7 )%ZTNum ] )  > pFilterCoeff-> lThreshold )
	{
		if( pFilterCoeff-> lUpperBound > lCofZTranY[ 3 ] )
		{
			ZTSUMY++;
			lCofZTranY[ 3 ]++;
		}
	}else
	{
		if(  lCofZTranY[ 3 ] > pFilterCoeff-> lLowerBound )
		{
			ZTSUMY--;
			lCofZTranY[ 3 ]--;
		}

	}
	
	for( iTemp = 0; iTemp < ZTNum;  iTemp ++ )
	{
		
		lTempX+=(long)iZTransferTempX[ (iTemp + 1 + ZTIndexTemp)%ZTNum ] * (long)lCofZTranX[ iTemp ];
		
		lTempY+=(long)iZTransferTempY[ (iTemp + 1 + ZTIndexTemp)%ZTNum ] * (long)lCofZTranY[ iTemp ];
	}
	*iX = (int) (lTempX / ZTSUMX);
	*iY = (int) (lTempY / ZTSUMY);
	if( ZTransferIndex != -1 )
	{
		iZTransferTempX[ ZTIndexTemp ] = *iX;
		iZTransferTempY[ ZTIndexTemp ] = *iY;

	}

}

