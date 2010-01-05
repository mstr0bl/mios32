// $Id$
/*
 * Header file for MBSID MIDI Parser
 *
 * ==========================================================================
 *
 *  Copyright (C) 2009 Thorsten Klose (tk@midibox.org)
 *  Licensed for personal non-commercial use only.
 *  All other rights reserved.
 * 
 * ==========================================================================
 */

#ifndef _SID_MIDI_B_H
#define _SID_MIDI_B_H


/////////////////////////////////////////////////////////////////////////////
// Global definitions
/////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////
// Global Types
/////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////
// Prototypes
/////////////////////////////////////////////////////////////////////////////

extern s32 SID_MIDI_B_Receive_Note(u8 sid, mios32_midi_package_t midi_package);
extern s32 SID_MIDI_B_Receive_CC(u8 sid, mios32_midi_package_t midi_package);


/////////////////////////////////////////////////////////////////////////////
// Export global variables
/////////////////////////////////////////////////////////////////////////////
  // go through four midi voices
  // 1 and 2 used to play the bassline/select the sequence
  // 3 and 4 used to transpose a sequence

#endif /* _SID_MIDI_B_H */