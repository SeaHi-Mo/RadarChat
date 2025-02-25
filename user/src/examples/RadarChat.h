/**
 * @file RadarChat.h
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2025-02-19
 *
 * @copyright Copyright (c) 2025
 *
*/
#ifndef RADARCHAT_H
#define RADARCHAT_H
#include "rtos_api.h"
#include <stdio.h>
#include <stdarg.h>

#define PRESENCE_TIMER_OUT 10000 // 离开时间阈值（毫秒）
// 定义一个宏，用于表示久坐提醒的音频文件标识符
#define SEDENTARY_REMINDER_AUDIO_FILE "[110]"
// 定义一个宏，用于表示喝水提醒的音频文件标识符
#define DRINK_WATER_REMINDER_AUDIO_FILE "[117]"
// 指令ID
// 定义一个枚举类型remainder_cmd_t，用于表示不同的提醒命令
typedef enum {
    CMD_REMIND_OFF = 0, // 关闭提醒，值为0
    CMD_REMIND_1_MINUTE, // 1分钟提醒，值为1
    CMD_REMIND_10_MINUTES, // 10分钟提醒，值为2
    CMD_REMIND_30_MINUTES, // 30分钟提醒，值为3
    CMD_REMIND_1_HOUR, // 1小时提醒，值为4
    CMD_WATER_REMIND_2_HOURS, // 每2小时提醒一次，值为5
    CMD_WATER_REMIND_10_MINUTES, // 每10分钟提醒一次，值为6
    CMD_WATER_REMIND_20_MINUTES, // 每20分钟提醒一次，值为7
    CMD_WATER_REMIND_30_MINUTES, // 每30分钟提醒一次，值为8
    CMD_WATER_REMIND_1_HOUR, // 每小时提醒一次，值为9
    CMD_WATER_REMIND_10_SECONDS, // 每10秒提醒一次，值为10
    CMD_WATER_REMIND_20_SECONDS, // 每20秒提醒一次，值为11
    CMD_WATER_REMIND_30_SECONDS, // 每30秒提醒一次，值为12
    CMD_WATER_REMIND_40_SECONDS, // 每40秒提醒一次，值为13
    CMD_WATER_REMIND_50_SECONDS, // 每50秒提醒一次，值为14
}remainder_cmd_t; // 枚举类型的名称为remainder_cmd_t
// 状态机定义
typedef enum {
    MONITORING,     // 监测中
    SITTING,        // 持续监测
    AWAY_CHECK,     // 离开检测
    ALERTING        // 提醒处理
} State;
// 系统状态结构体
typedef struct {
    // 当前系统状态，使用 State 枚举类型表示
    State current_state;
    // 坐姿开始时间，以秒为单位的时间戳
    uint32_t sit_start_time;
    // 离开开始时间，以秒为单位的时间戳
    uint32_t away_start_time;
    // 警报计数，记录发生的警报次数
    uint8_t alert_count;
} SystemState;
// 状态跟踪结构体
typedef struct {
    uint32_t absence_start;  // 离座计时起点
    bool is_absent;          // 离座状态标志
    bool need_remind;        // 待提醒标志
} PresenceStatus;

// 声明一个外部变量 radarTaskMsg，类型为 MessageHandle
// 该变量可能在其他文件中定义，用于处理雷达任务的消息
extern MessageHandle radarTaskMsg;
// 声明一个外部变量 RadarChat_check，类型为 int
// 该变量可能在其他文件中定义，用于检查雷达聊天功能的状态
extern int RadarChat_check;
// 声明一个函数 radarChat_task_init，无返回值，无参数
// 该函数可能在其他文件中定义，用于初始化雷达聊天任务
void radarChat_task_init(void);
// 声明一个函数 user_log，无返回值，参数为 const char* fmt 和可变参数 ...
// 该函数可能在其他文件中定义，用于记录日志信息
// fmt 参数是一个格式化字符串，类似于 printf 的格式化字符串
// ... 表示该函数可以接受多个参数，具体参数根据 fmt 字符串中的格式说明符来确定
void user_log(const char* fmt, ...); // for logging

// 声明一个函数，用于执行久坐提醒功能
void sedentary_reminder(SystemState* sys);
// 声明一个函数，用于重置系统状态
void system_reset(SystemState* sys);
// 声明一个函数，用于更新人的存在状态
void update_presence_status(void);
// 声明一个函数，用于判断是否应该播放提醒
bool should_play_reminder(void);
// 声明一个函数，用于雷达检测是否有人
bool radar_detect_human(void);      // 雷达检测函数
// 声明一个函数，用于触发提醒
void trigger_alert(void);           // 触发提醒
// 声明一个函数，用于检查提醒是否已被确认
bool check_alert_confirmed(void);   // 检查提醒确认
#endif