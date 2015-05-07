#pragma once


extern std::hash_map<std::string, DWORD> g_asmLabels;
extern DWORD g_asmCodeBase;
extern BYTE* g_asmBuffer;


#define ASSEMBLER_FLUSH() \
	g_asmLabels.clear();

#define ASSEMBLER_SET_CODEBASE( asmCodeBase ) \
	g_asmCodeBase = asmCodeBase;

#define ASSEMBLER_SET_TARGET_BUFFER( buffer ) \
	g_asmBuffer = buffer;

#define ASSEMBLER_SET_LABEL( name, localEip ) \
	g_asmLabels[name] = localEip;

#define ASSEMBLER_GET_LABEL( name ) \
	g_asmLabels[name];

#define ASSEMBLE( cmd, localEip ) \
{ \
	DWORD codeLen = 0; \
	wchar_t assemblyError[TEXTLEN] = { 0 }; \
	codeLen = Assemble( (cmd), g_asmCodeBase + localEip, &g_asmBuffer[localEip], MAXCMDSIZE, 0, assemblyError ); \
	PLUGIN_CHECK_ERROR( codeLen, \
		ASSEMBLER_ERROR_HANDLER(), \
		UNABLE_TO_ASSEMBLE_CODE, (cmd), g_asmCodeBase + localEip, assemblyError ); \
}

#define ASSEMBLE_AND_STEP( cmd, localEip ) \
{ \
	DWORD codeLen = 0; \
	wchar_t assemblyError[TEXTLEN] = { 0 }; \
	codeLen = Assemble( (cmd), g_asmCodeBase + localEip, &g_asmBuffer[localEip], MAXCMDSIZE, 0, assemblyError ); \
	PLUGIN_CHECK_ERROR( codeLen, \
		ASSEMBLER_ERROR_HANDLER(), \
		UNABLE_TO_ASSEMBLE_CODE, (cmd), g_asmCodeBase + localEip, assemblyError ); \
	localEip += codeLen; \
}

#define ASSEMBLER_ADD_BYTE_AND_STEP( b, localEip ) \
{ \
	g_asmBuffer[localEip] = (BYTE)(b); \
	++localEip; \
}

#define ASSEMBLER_ADD_STRING_AND_STEP( str, localEip ) \
{ \
	DWORD strLen = strlen( (str) ); \
	PLUGIN_CHECK_ERROR( strLen, \
		ASSEMBLER_ERROR_HANDLER(), \
		UNABLE_TO_ADD_STRING, (str), g_asmCodeBase + localEip ); \
	memcpy( (void*)&g_asmBuffer[localEip], (str), strLen + 1 ); \
	localEip += ( strLen + 1 ); \
}

#define ASSEMBLER_ADD_WSTRING_AND_STEP( str, localEip ) \
{ \
	DWORD strLen = wcslen( (str) ); \
	PLUGIN_CHECK_ERROR( strLen, \
		ASSEMBLER_ERROR_HANDLER(), \
		UNABLE_TO_ADD_STRING, (str), g_asmCodeBase + localEip ); \
	wmemcpy( (wchar_t*)&g_asmBuffer[localEip], (str), strLen + 1 ); \
	localEip += ( ( strLen << 1 ) + 2 ); \
}

#define ASSEMBLER_ADD_DWROD_AND_STEP( data, localEip ) \
{ \
	*(DWORD*)&g_asmBuffer[localEip] = (DWORD)(data); \
	localEip += sizeof( DWORD ); \
}

#define ASSEMBLER_ADD_ARRAY_AND_STEP( array, len, localEip ) \
{ \
	memcpy( &g_asmBuffer[localEip], (array), (len) ); \
	localEip += (len); \
}
