#define DIA_MASK_X   15         // 0x000F
#define DIA_MASK_Y   240        // 0x00F0 
#define DIA_MASK_W   1792       // 0x0700 
#define DIA_MASK_H   28672      // 0x7000 

#define DIA_SHIFT_X   0
#define DIA_SHIFT_Y   4
#define DIA_SHIFT_W   8
#define DIA_SHIFT_H  12

#define DIA_X_FIX    0          // 0x0000 
#define DIA_Y_FIX    0          // 0x0000 
#define DIA_W_FIX    0          // 0x0000 
#define DIA_H_FIX    0          // 0x0000 

#define DIA_X_RATIO  7          // 0x0007 
#define DIA_Y_RATIO  112        // 0x0070 
#define DIA_W_RATIO  1792       // 0x0700 
#define DIA_H_RATIO  28672      // 0x7000 

#define DIA_X_RIGHT  8          // 0x0008 
#define DIA_Y_BOTTOM 128        // 0x0080 

#define DIA_X_PLUS_DW 1         // 0x0001 
#define DIA_Y_PLUS_DH 16        // 0x0010 
#define DIA_W_PLUS_DW 256       // 0x0100 
#define DIA_H_PLUS_DH 4096      // 0x1000 

#define DIA_X_PLUS_1_2_DW 2     // 0x0002 
#define DIA_Y_PLUS_1_2_DH 32    // 0x0020 
#define DIA_W_PLUS_1_2_DW 512   // 0x0200 
#define DIA_H_PLUS_1_2_DH 8192  // 0x2000 

#define DIA_X_PLUS_1_3_DW 3     // 0x0003 
#define DIA_Y_PLUS_1_3_DH 48    // 0x0030 
#define DIA_W_PLUS_1_3_DW 768   // 0x0300 
#define DIA_H_PLUS_1_3_DH 12288 // 0x3000 

#define DIA_X_PLUS_2_3_DW 4     // 0x0004 
#define DIA_Y_PLUS_2_3_DH 64    // 0x0040 
#define DIA_W_PLUS_2_3_DW 1024  // 0x0400 
#define DIA_H_PLUS_2_3_DH 16384 // 0x4000 

#define DIA_X_PLUS_1_4_DW 5     // 0x0005 
#define DIA_Y_PLUS_1_4_DH 80    // 0x0050 
#define DIA_W_PLUS_1_4_DW 1280  // 0x0500 
#define DIA_H_PLUS_1_4_DH 20480 // 0x5000 

#define DIA_X_PLUS_3_4_DW 6     // 0x0006 
#define DIA_Y_PLUS_3_4_DH 96    // 0x0060 
#define DIA_W_PLUS_3_4_DW 1536  // 0x0600 
#define DIA_H_PLUS_3_4_DH 24576 // 0x6000 

#define DIA_H_GRANULAR  32768   // 0x8000 

// Form attribute flags 

// Try to keep the same DIA state as in previous form 
#define DIA_FORM_KEEP_LAST         1
// Keep the same DIA state as previous form using the same bin 
// If DIA_FORM_KEEP_LAST and DIA_FORM_USE_BIN are both set, then
//   we keep last DIA state to get the initial DIA state for the bin
//   the first time a form from that bin is used.
#define DIA_FORM_USE_BIN           2
// Disable DIA state change
#define DIA_FORM_NO_RESIZE         4
// Handera-specific form
#define DIA_FORM_HANDERA           8


#define STD_EXTENT_X        160
#define STD_EXTENT_Y        160

#define MAX_EXTENT_X        4096
#define MAX_EXTENT_Y        4096

#define DIA_STATE_MAX        0
#define DIA_STATE_MIN        1
#define DIA_STATE_NO_STATUS_BAR  2
#define DIA_STATE_UNDEFINED   255

// No actual object is allowed to have this--this is used within resize config info
#define GSI_OBJECT_ID   65280   // 0xFF00
