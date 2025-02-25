#ifndef INC_UNI_CMD_CODE_H_
#define INC_UNI_CMD_CODE_H_

typedef struct {
  uni_u8      cmd_code; /* cmd code fro send base on SUCP */
  const char  *cmd_str; /* action string on UDP */;
} cmd_code_map_t;

const cmd_code_map_t g_cmd_code_arry[] = {
  {0x0, "wakeup_uni"},
  {0x1, "exitUni"},
  {0x2, "L1"},
  {0x3, "L2"},
  {0x4, "L3"},
  {0x5, "L4"},
  {0x6, "L5"},
  {0x7, "L6"},
  {0x8, "L7"},
  {0x9, "L8"},
  {0xa, "L9"},
  {0xb, "close1"},
  {0xc, "close2"},
  {0xd, "L10"},
  {0xe, "L20"},
  {0xf, "L30"},
  {0x10, "L40"},
  {0x11, "L50"},
};

#endif
