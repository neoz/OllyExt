#pragma once


void ntGlobalFlagPreHook( bool protect );
void ntGlobalFlagPostHook( bool protect );
void ntGlobalFlagReset( void );
void ntGlobalFlagApply( bool protect );
