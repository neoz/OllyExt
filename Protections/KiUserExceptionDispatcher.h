#pragma once


extern DWORD g_drxRegistersChangedAddress;
extern DWORD g_drxRegistersAddress;


void kiUserExceptionDispatcherReset( void );
void kiUserExceptionDispatcherApply( bool protectDRX, bool hideDRX );
