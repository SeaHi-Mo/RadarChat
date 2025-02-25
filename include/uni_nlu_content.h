#ifndef INC_UNI_NLU_CONTENT_H_
#define INC_UNI_NLU_CONTENT_H_

typedef struct {
  uni_u32 key_word_hash_code; /* 存放识别词汇对应的hashcode */
  uni_u8  nlu_content_str_index; /* 存放nlu映射表中的索引，实现多个识别词汇可对应同一个nlu，暂支持256条，如果不够换u16 */
  char    *hash_collision_orginal_str /* 类似Java String equal，当hash发生碰撞时，赋值为识别词汇，否则设置为NULL */;
} uni_nlu_content_mapping_t;

enum {
  eCMD_wakeup_uni,
  eCMD_exitUni,
  eCMD_L1,
  eCMD_L2,
  eCMD_L3,
  eCMD_L4,
  eCMD_L5,
  eCMD_L6,
  eCMD_L7,
  eCMD_L8,
  eCMD_L9,
  eCMD_close1,
  eCMD_close2,
  eCMD_L10,
  eCMD_L20,
  eCMD_L30,
  eCMD_L40,
  eCMD_L50,
};

const char* const g_nlu_content_str[] = {
[eCMD_wakeup_uni] = "{\"asr\":\"hello radar chat\",\"cmd\":\"wakeup_uni\",\"pcm\":\"[103, 104, 105, 106]\"}",
[eCMD_exitUni] = "{\"asr\":\"see you\",\"cmd\":\"exitUni\",\"pcm\":\"[107]\"}",
[eCMD_L1] = "{\"asr\":\"remind me to get up in a minute\",\"cmd\":\"L1\",\"pcm\":\"[109, 110]\"}",
[eCMD_L2] = "{\"asr\":\"remind me in ten minutes\",\"cmd\":\"L2\",\"pcm\":\"[111]\"}",
[eCMD_L3] = "{\"asr\":\"remind me in thirty minutes\",\"cmd\":\"L3\",\"pcm\":\"[112]\"}",
[eCMD_L4] = "{\"asr\":\"remind me in an hour\",\"cmd\":\"L4\",\"pcm\":\"[113]\"}",
[eCMD_L5] = "{\"asr\":\"remind me to drink water after two hours\",\"cmd\":\"L5\",\"pcm\":\"[114, 115]\"}",
[eCMD_L6] = "{\"asr\":\"remind me to drink water after ten minutes\",\"cmd\":\"L6\",\"pcm\":\"[116, 117]\"}",
[eCMD_L7] = "{\"asr\":\"remind me to drink water after twenty minutes\",\"cmd\":\"L7\",\"pcm\":\"[118, 119, 120]\"}",
[eCMD_L8] = "{\"asr\":\"remind me to drink water after thirteen minutes\",\"cmd\":\"L8\",\"pcm\":\"[121, 119, 117]\"}",
[eCMD_L9] = "{\"asr\":\"remind me to drink water after an hour\",\"cmd\":\"L9\",\"pcm\":\"[122, 119, 117]\"}",
[eCMD_close1] = "{\"asr\":\"turn off sedentary reminders\",\"cmd\":\"close1\",\"pcm\":\"[123]\"}",
[eCMD_close2] = "{\"asr\":\"turn off water reminders\",\"cmd\":\"close2\",\"pcm\":\"[124]\"}",
[eCMD_L10] = "{\"asr\":\"reminder to drink water after ten seconds\",\"cmd\":\"L10\",\"pcm\":\"[125]\"}",
[eCMD_L20] = "{\"asr\":\"reminder to drink water after twenty seconds\",\"cmd\":\"L20\",\"pcm\":\"[126]\"}",
[eCMD_L30] = "{\"asr\":\"reminder to drink water after thirty seconds\",\"cmd\":\"L30\",\"pcm\":\"[127]\"}",
[eCMD_L40] = "{\"asr\":\"reminder to drink water after forty seconds\",\"cmd\":\"L40\",\"pcm\":\"[128]\"}",
[eCMD_L50] = "{\"asr\":\"reminder to drink water after fifty seconds\",\"cmd\":\"L50\",\"pcm\":\"[129]\"}",
};

/*TODO perf sort by hashcode O(logN), now version O(N)*/
const uni_nlu_content_mapping_t g_nlu_content_mapping[] = {
  {1284918336U/*hello radar chat*/, eCMD_wakeup_uni, NULL},
  {1973903286U/*hey radar chat*/, eCMD_wakeup_uni, NULL},
  {66860209U/*hi radar chat*/, eCMD_wakeup_uni, NULL},
  {1969601234U/*see you*/, eCMD_exitUni, NULL},
  {207022353U/*goodbye*/, eCMD_exitUni, NULL},
  {1025929035U/*remind me to get up in a minute*/, eCMD_L1, NULL},
  {3382770631U/*set a one minute reminder*/, eCMD_L1, NULL},
  {3534510734U/*remind me in ten minutes*/, eCMD_L2, NULL},
  {2533322234U/*wake me up in ten minutes*/, eCMD_L2, NULL},
  {797424815U/*remind me in thirty minutes*/, eCMD_L3, NULL},
  {1274496065U/*cue me in thirty minutes*/, eCMD_L3, NULL},
  {876078505U/*remind me in an hour*/, eCMD_L4, NULL},
  {4249540684U/*remind me to exercise in an hour*/, eCMD_L4, NULL},
  {1157400430U/*remind me to drink water after two hours*/, eCMD_L5, NULL},
  {879418959U/*remind me to drink water after ten minutes*/, eCMD_L6, NULL},
  {1346508381U/*remind me to drink water after twenty minutes*/, eCMD_L7, NULL},
  {655133219U/*remind me to drink water after thirteen minutes*/, eCMD_L8, NULL},
  {92828138U/*remind me to drink water after an hour*/, eCMD_L9, NULL},
  {2365837564U/*turn off sedentary reminders*/, eCMD_close1, NULL},
  {3272596900U/*turn off water reminders*/, eCMD_close2, NULL},
  {1360465392U/*reminder to drink water after ten seconds*/, eCMD_L10, NULL},
  {2238150236U/*reminder to drink water after twenty seconds*/, eCMD_L20, NULL},
  {4262688077U/*reminder to drink water after thirty seconds*/, eCMD_L30, NULL},
  {3684961505U/*reminder to drink water after forty seconds*/, eCMD_L40, NULL},
  {2999045019U/*reminder to drink water after fifty seconds*/, eCMD_L50, NULL},
};

#endif
