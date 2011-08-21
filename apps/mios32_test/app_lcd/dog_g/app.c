// $Id$
/*
 * Demo application for 8xDOGM128 GLCDs
 *
 * ==========================================================================
 *
 *  Copyright (C) 2008 Thorsten Klose (tk@midibox.org)
 *  Licensed for personal non-commercial use only.
 *  All other rights reserved.
 * 
 * ==========================================================================
 */

/////////////////////////////////////////////////////////////////////////////
// Include files
/////////////////////////////////////////////////////////////////////////////

#include <mios32.h>
#include "app.h"

#include <glcd_font.h>


/////////////////////////////////////////////////////////////////////////////
// This hook is called after startup to initialize the application
/////////////////////////////////////////////////////////////////////////////
void APP_Init(void)
{
  MIOS32_BOARD_LED_Init(0xffffffff); // initialize all LEDs
}


/////////////////////////////////////////////////////////////////////////////
// This task is running endless in background
/////////////////////////////////////////////////////////////////////////////
void APP_Background(void)
{
  // print configured LCD parameters
  MIOS32_MIDI_SendDebugMessage("\n");
  MIOS32_MIDI_SendDebugMessage("\n");
  MIOS32_MIDI_SendDebugMessage("GLCD_DOG Demo started.");
  MIOS32_MIDI_SendDebugMessage("Configured LCD Parameters in MIOS32 Bootloader Info Range:\n");
  MIOS32_MIDI_SendDebugMessage("lcd_type: 0x%02x (%s)\n", mios32_lcd_parameters.lcd_type, MIOS32_LCD_LcdTypeName(mios32_lcd_parameters.lcd_type));
  MIOS32_MIDI_SendDebugMessage("num_x:    %4d\n", mios32_lcd_parameters.num_x);
  MIOS32_MIDI_SendDebugMessage("num_y:    %4d\n", mios32_lcd_parameters.num_y);
  MIOS32_MIDI_SendDebugMessage("width:    %4d\n", mios32_lcd_parameters.width);
  MIOS32_MIDI_SendDebugMessage("height:   %4d\n", mios32_lcd_parameters.height);

  if( mios32_lcd_parameters.lcd_type != MIOS32_LCD_TYPE_GLCD_DOG ) {
    // print warning if correct LCD hasn't been selected
    MIOS32_MIDI_SendDebugMessage("WARNING: your core module hasn't been configured for the GLCD_DOG!\n");
    MIOS32_MIDI_SendDebugMessage("Please do this with the bootloader update application!\n");
  }


  // print static screen
  MIOS32_LCD_FontInit((u8 *)GLCD_FONT_NORMAL);

  // clear LCD
  MIOS32_LCD_Clear();

  // endless loop - LED will flicker on each iteration
  while( 1 ) {
    // toggle the state of all LEDs (allows to measure the execution speed with a scope)
    MIOS32_BOARD_LED_Set(0xffffffff, ~MIOS32_BOARD_LED_Get());

    // X/Y "position" of displays (see also comments in $MIOS32_PATH/modules/app_lcd/dog_g/README.txt)
    const u8 lcd_x[8] = {0, 1, 2, 0, 1, 2, 0, 1}; // CS#0..7
    const u8 lcd_y[8] = {0, 0, 0, 1, 1, 1, 2, 2};

    u8 i;
    for(i=0; i<8; ++i) {
      u16 x_offset = 128*lcd_x[i];
      u16 y_offset = 8*8*lcd_y[i];

      // print text
      MIOS32_LCD_FontInit((u8 *)GLCD_FONT_NORMAL);
      MIOS32_LCD_GCursorSet(x_offset + 16, y_offset + 1*8);
      MIOS32_LCD_PrintFormattedString("  DOGM128 #%d", i+1);

      MIOS32_LCD_GCursorSet(x_offset + 16, y_offset + 2*8);
      MIOS32_LCD_PrintString("  powered by  ");

      MIOS32_LCD_FontInit((u8 *)GLCD_FONT_BIG);
      MIOS32_LCD_GCursorSet(x_offset + 16, y_offset + 4*8);
      MIOS32_LCD_PrintString("MIOS32");
    }
  }
}


/////////////////////////////////////////////////////////////////////////////
// This hook is called when a MIDI package has been received
/////////////////////////////////////////////////////////////////////////////
void APP_MIDI_NotifyPackage(mios32_midi_port_t port, mios32_midi_package_t midi_package)
{
}


/////////////////////////////////////////////////////////////////////////////
// This hook is called before the shift register chain is scanned
/////////////////////////////////////////////////////////////////////////////
void APP_SRIO_ServicePrepare(void)
{
}


/////////////////////////////////////////////////////////////////////////////
// This hook is called after the shift register chain has been scanned
/////////////////////////////////////////////////////////////////////////////
void APP_SRIO_ServiceFinish(void)
{
}


/////////////////////////////////////////////////////////////////////////////
// This hook is called when a button has been toggled
// pin_value is 1 when button released, and 0 when button pressed
/////////////////////////////////////////////////////////////////////////////
void APP_DIN_NotifyToggle(u32 pin, u32 pin_value)
{
}


/////////////////////////////////////////////////////////////////////////////
// This hook is called when an encoder has been moved
// incrementer is positive when encoder has been turned clockwise, else
// it is negative
/////////////////////////////////////////////////////////////////////////////
void APP_ENC_NotifyChange(u32 encoder, s32 incrementer)
{
}


/////////////////////////////////////////////////////////////////////////////
// This hook is called when a pot has been moved
/////////////////////////////////////////////////////////////////////////////
void APP_AIN_NotifyChange(u32 pin, u32 pin_value)
{
}
