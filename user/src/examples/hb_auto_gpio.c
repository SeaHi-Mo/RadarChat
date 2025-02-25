#include "user_config.h"
#include "user_event.h"
#include "user_gpio.h"
#include "user_player.h"
#include "user_pwm.h"
#include "user_timer.h"
#include "user_uart.h"
#include "RadarChat.h"

#define TAG "auto_gpio"

#define UART_SEND_MAX      16

typedef struct {
  char  data[UART_SEND_MAX];
  int   len;
}uart_data_t;

const uart_data_t g_uart_buf[] = {

};

static MessageContext radarMsg;
/**
 * @brief
 *
 * @param event
 * @param context
*/
static void _custom_setting_cb(USER_EVENT_TYPE event, user_event_context_t* context)
{
  event_custom_setting_t* setting = NULL;
  if (context) {
    setting = &context->custom_setting;
    user_log("user command: %s\r\n", setting->cmd);
    //设置一分钟提醒
    if (strncmp(setting->cmd, "L1", 2)==0)
    {
      user_log("Audio cmd: %s\r\n", setting->word_str);
      //启动读取雷达检测，并启动定时器
      radarMsg.msgId = CMD_REMIND_1_MINUTE;//一分钟
      user_player_play(AUDIO_PLAY_REPLY, "[109]");
    }
    //设置十分钟久坐提醒
    if (strncmp(setting->cmd, "L2", 2)==0)
    {
      user_log("Audio cmd: %s\r\n", setting->word_str);
      //启动读取雷达检测，并启动定时器
      radarMsg.msgId = CMD_REMIND_10_MINUTES;//十分钟
      user_player_play(AUDIO_PLAY_REPLY, "[111]");
    }
    //设置三十分钟久坐提醒
    if (strncmp(setting->cmd, "L3", 2)==0)
    {
      user_log("Audio cmd: %s\r\n", setting->word_str);
      //启动读取雷达检测，并启动定时器
      radarMsg.msgId = CMD_REMIND_30_MINUTES;//三十分钟

      user_player_play(AUDIO_PLAY_REPLY, "[112]");
    }
    //设置一小时久坐提醒
    if (strncmp(setting->cmd, "L4", 2)==0)
    {
      user_log("Audio cmd: %s\r\n", setting->word_str);
      //启动读取雷达检测，并启动定时器
      radarMsg.msgId = CMD_REMIND_1_HOUR;//一小时

      user_player_play(AUDIO_PLAY_REPLY, "[113]");
    }
    //设置2小时喝水
    if (strncmp(setting->cmd, "L5", 2)==0)
    {
      user_log("Audio cmd: %s\r\n", setting->word_str);
      //启动读取雷达检测，并启动定时器
      radarMsg.msgId = CMD_WATER_REMIND_2_HOURS;//两小时
      user_player_play(AUDIO_PLAY_REPLY, "[114]");
    }
    //设置10分钟喝水
    if (strncmp(setting->cmd, "L6", 2)==0)
    {
      user_log("Audio cmd: %s\r\n", setting->word_str);
      //启动读取雷达检测，并启动定时器
      radarMsg.msgId = CMD_WATER_REMIND_10_MINUTES;//十分钟
      user_player_play(AUDIO_PLAY_REPLY, "[116]");
    }
    //设置20分钟喝水
    if (strncmp(setting->cmd, "L7", 2)==0)
    {
      user_log("Audio cmd: %s\r\n", setting->word_str);
      //启动读取雷达检测，并启动定时器
      radarMsg.msgId = CMD_WATER_REMIND_20_MINUTES;//一小时
      user_player_play(AUDIO_PLAY_REPLY, "[118]");
    }
    //设置30分钟喝水
    if (strncmp(setting->cmd, "L8", 2)==0)
    {
      user_log("Audio cmd: %s\r\n", setting->word_str);
      //启动读取雷达检测，并启动定时器
      radarMsg.msgId = CMD_WATER_REMIND_30_MINUTES;//一小时
      user_player_play(AUDIO_PLAY_REPLY, "[121]");
    }
    //设置1小时喝水
    if (strncmp(setting->cmd, "L9", 2)==0)
    {
      user_log("Audio cmd: %s\r\n", setting->word_str);
      //启动读取雷达检测，并启动定时器
      radarMsg.msgId = CMD_WATER_REMIND_1_HOUR;//一小时
      user_player_play(AUDIO_PLAY_REPLY, "[122]");
    }
    //设置10秒喝水
    if (strncmp(setting->cmd, "L10", 3)==0)
    {
      user_log("Audio cmd: %s\r\n", setting->word_str);
      //启动读取雷达检测，并启动定时器
      radarMsg.msgId = CMD_WATER_REMIND_10_SECONDS;//10秒
      user_player_play(AUDIO_PLAY_REPLY, "[125]");
    }
    //设置20秒喝水
    if (strncmp(setting->cmd, "L20", 3)==0)
    {
      user_log("Audio cmd: %s\r\n", setting->word_str);
      //启动读取雷达检测，并启动定时器
      radarMsg.msgId = CMD_WATER_REMIND_20_SECONDS;//20秒
      user_player_play(AUDIO_PLAY_REPLY, "[126]");
    }
    //设置30秒喝水
    if (strncmp(setting->cmd, "L30", 3)==0)
    {
      user_log("Audio cmd: %s\r\n", setting->word_str);
      //启动读取雷达检测，并启动定时器
      radarMsg.msgId = CMD_WATER_REMIND_30_SECONDS;//30秒
      user_player_play(AUDIO_PLAY_REPLY, "[127]");
    }
    //设置40秒喝水
    if (strncmp(setting->cmd, "L40", 3)==0)
    {
      user_log("Audio cmd: %s\r\n", setting->word_str);
      //启动读取雷达检测，并启动定时器
      radarMsg.msgId = CMD_WATER_REMIND_40_SECONDS;//40秒
      user_player_play(AUDIO_PLAY_REPLY, "[128]");
    }
    //设置50秒喝水
    if (strncmp(setting->cmd, "L50", 3)==0)
    {
      user_log("Audio cmd: %s\r\n", setting->word_str);
      //启动读取雷达检测，并启动定时器
      radarMsg.msgId = CMD_WATER_REMIND_50_SECONDS;//50秒
      user_player_play(AUDIO_PLAY_REPLY, "[129]");
    }
    //关闭提醒
    if (strncmp(setting->cmd, "close1", 6)==0)
    {
      /* code */
      user_log("Audio cmd: %s\r\n", setting->word_str);
      radarMsg.msgId = 0;
      user_player_play(AUDIO_PLAY_REPLY, "[123]");
    }
    //关闭喝水提醒
    if (strncmp(setting->cmd, "close2", 6)==0)
    {
      /* code */
      user_log("Audio cmd: %s\r\n", setting->word_str);
      radarMsg.msgId = 0;
      user_player_play(AUDIO_PLAY_REPLY, "[124]");
    }

    MessageSend(radarTaskMsg, &radarMsg);
  }
}


static void _register_event_callback(void)
{
  user_event_subscribe_event(USER_CUSTOM_SETTING, _custom_setting_cb);
}

int hb_auto_gpio(void) {
  user_gpio_init();
  user_uart_init(NULL);
  user_gpio_set_mode(GPIO_NUM_A25, GPIO_MODE_IN);
  user_gpio_set_value(GPIO_NUM_A25, 0);
  user_gpio_set_mode(GPIO_NUM_A26, GPIO_MODE_OUT);
  user_gpio_set_value(GPIO_NUM_A26, 0);
  user_gpio_set_mode(GPIO_NUM_A27, GPIO_MODE_OUT);
  user_gpio_set_value(GPIO_NUM_A27, 0);
  user_gpio_set_mode(GPIO_NUM_A28, GPIO_MODE_OUT);
  user_gpio_set_value(GPIO_NUM_A28, 0);
  user_gpio_set_mode(GPIO_NUM_B2, GPIO_MODE_OUT);
  user_gpio_set_value(GPIO_NUM_B2, 0);
  user_gpio_set_mode(GPIO_NUM_B3, GPIO_MODE_OUT);
  user_gpio_set_value(GPIO_NUM_B3, 0);
  radarChat_task_init();
  _register_event_callback();
  return 0;
}

