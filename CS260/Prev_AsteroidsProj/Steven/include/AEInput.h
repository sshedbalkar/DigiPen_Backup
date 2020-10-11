// ---------------------------------------------------------------------------
// Project Name		:	Alpha Engine
// File Name		:	AEInput.h
// Author			:	Sun Tjen Fam
// Creation Date	:	2008/01/31
// Purpose			:	declaration for input stuff
// History			:
// - 2008/01/31		:	- initial implementation
// ---------------------------------------------------------------------------

#ifndef AE_INPUT_H
#define AE_INPUT_H

// ---------------------------------------------------------------------------
// Defines/Enums

// ---------------------------------------------------------------------------
// Externs

// ---------------------------------------------------------------------------
// class definitions

// ---------------------------------------------------------------------------
// Function prototypes

// ---------------------------------------------------------------------------

#ifdef __cplusplus

extern "C"
{
#endif

// ---------------------------------------------------------------------------


AE_API int AEInputInit();
AE_API void AEInputReset();
AE_API void AEInputUpdate();
AE_API void AEInputExit();

AE_API u8 AEInputCheckCurr		(u8 key);
AE_API u8 AEInputCheckPrev		(u8 key);
AE_API u8 AEInputCheckTriggered(u8 key);
AE_API u8 AEInputCheckReleased	(u8 key);
// ---------------------------------------------------------------------------

#ifdef __cplusplus 
}
#endif

// ---------------------------------------------------------------------------

#endif // AE_INPUT_H

