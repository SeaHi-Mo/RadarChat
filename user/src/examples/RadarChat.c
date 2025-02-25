/**
 * @file RadarChat.c
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2025-02-19
 *
 * @copyright Copyright (c) 2025
 *
*/
#include "user_config.h"
#include "user_event.h"
#include "user_gpio.h"
#include "user_player.h"
#include "uni_iot.h"
#include "uni_log.h"
#include "user_timer.h"
#include "uni_hal_timer.h"
#include "RadarChat.h"
#include "uni_types.h"
#include "user_uart.h"
#include "user_asr.h"

#define LOG_BUFFER_SIZE 512
char log_buffer[LOG_BUFFER_SIZE];
uint16_t log_wr_index = 0;


// 用于表示雷达任务的线程
static uni_pthread_t radar_pthread;
// 用于存储雷达任务的参数
static thread_param radarTaskParam;
// 用于处理雷达任务的消息
MessageHandle radarTaskMsg;
// 用于存储雷达任务的上行通知值
static MessageContext ulNotificationValue;
// 用于计数雷达聊天无人状态下的计时器计数
static uint32_t radarChat_no_man_timer_cnt = 0;
// 用于存储雷达聊天超时时间
static uint32_t radarChat_Timer_out = 0;
// 用于表示雷达聊天检查功能是否启用
static bool RadarChat_check_en = false;
// 用于表示定时器2是否启用
static bool eTIMER2_en = false;
// 用于表示当前状态
static bool current_state = false;
// 用于存储雷达系统的状态
static SystemState radar_sys;

// 用于存储当前的存在状态（例如是否有物体存在）
static PresenceStatus p_status = { 0 };

// 用于存储剩余的命令信息
static remainder_cmd_t remainder_cmd = { 0 };

// 该函数用于获取当前系统时间，单位为毫秒
static uint32_t get_current_time_ms(void); // 获取系统时间（毫秒）

// 定义一个静态字符指针数组，用于存储不同的提醒命令字符串
static char* remainder_cmd_str[] = {
    "CMD_REMIND_OFF",               // 关闭提醒
    "CMD_REMIND_1_MINUTE",          // 1分钟后提醒
    "CMD_REMIND_10_MINUTES",        // 10分钟后提醒
    "CMD_REMIND_30_MINUTES",        // 30分钟后提醒
    "CMD_REMIND_1_HOUR",            // 1小时后提醒
    "CMD_WATER_REMIND_2_HOURS",     // 2小时后提醒喝水
    "CMD_WATER_REMIND_10_MINUTES",  // 10分钟后提醒喝水
    "CMD_WATER_REMIND_20_MINUTES",  // 20分钟后提醒喝水
    "CMD_WATER_REMIND_30_MINUTES",  // 30分钟后提醒喝水
    "CMD_WATER_REMIND_1_HOUR",      // 1小时后提醒喝水
    "CMD_WATER_REMIND_10_SECONDS",  // 10秒后提醒喝水
    "CMD_WATER_REMIND_20_SECONDS",  // 20秒后提醒喝水
    "CMD_WATER_REMIND_30_SECONDS",  // 30秒后提醒喝水
    "CMD_WATER_REMIND_40_SECONDS",  // 40秒后提醒喝水
    "CMD_WATER_REMIND_50_SECONDS",  // 50秒后提醒喝水
};
/**
 * @brief 用户log 函数
 *
 * @param fmt
 * @param ...
*/
void user_log(const char* fmt, ...)
{
    // 计算剩余的缓冲区空间
    int remain = LOG_BUFFER_SIZE - log_wr_index;
    if (remain < 128) return; // 防止缓冲区溢出
    // 初始化可变参数列表
    va_list args;
    va_start(args, fmt);
    // 将格式化的字符串写入日志缓冲区
    int len = vsnprintf(&log_buffer[log_wr_index], remain, fmt, args);
    // 更新写指针位置
    log_wr_index += len;
    // 结束可变参数列表
    va_end(args);

    // 发送日志缓冲区内容
    user_uart_send(log_buffer, log_wr_index);
    // 重置写指针位置
    log_wr_index = 0;
}
/**
 * @brief 用于根据输入的定时器索引枚举值获取对应的定时器索引
 *
 * @param idx
 * @return TIMER_INDEX
*/
static TIMER_INDEX _get_timer_index(eTIMER_IDX idx) {
    // 初始化定时器索引变量 TimerIdx，默认值为 0xFF（表示无效索引）
    TIMER_INDEX TimerIdx = 0xFF;
    // 使用 switch 语句根据输入的定时器索引枚举值 idx 进行匹配
    switch (idx) {
        // 如果 idx 等于 eTIMER2，将 TimerIdx 设置为 TIMER2 的值
        case eTIMER2:
            TimerIdx = TIMER2;
            break;
            // 如果 idx 等于 eTIMER5，将 TimerIdx 设置为 TIMER5 的值
        case eTIMER5:
            TimerIdx = TIMER5;
            break;
            // 如果 idx 等于 eTIMER6，将 TimerIdx 设置为 TIMER6 的值
        case eTIMER6:
            TimerIdx = TIMER6;
            break;
            // 如果 idx 不匹配任何已知值，执行默认操作（此处为空，不做任何处理）
        default:
            break;
    }
    // 返回最终确定的定时器索引值 TimerIdx
    return TimerIdx;
}
/**
 * @brief 用于处理定时器回调
 *
 * @param TimerIdx
*/
static void vTimerCallback_no_man(TIMER_INDEX TimerIdx)
{
    // 增加全局变量 radarChat_no_man_timer_cnt 的值，用于记录定时器回调的次数
    radarChat_no_man_timer_cnt++;
}
/**
 * @brief
 *
 * @param arg
*/
static void radarChat_task(void* arg)
{
    for (;;) {
        /* 阻塞等待队列消息 */
        if (MessageRecv(radarTaskMsg, &ulNotificationValue, 100) == TRUE) {
            // /* 成功接收消息后创建定时器 */
            // 打印日志信息，显示当前处理的命令字符串
            user_log("RadarChat_task: %s\r\n", remainder_cmd_str[ulNotificationValue.msgId]);
            // 检查当前消息ID是否为关闭提醒命令
            if (ulNotificationValue.msgId == CMD_REMIND_OFF) {
                //关闭提醒功能
                RadarChat_check_en = false;
                // 继续下一次循环，跳过后续代码
                continue;;
            }
            // 启用提醒功能
            RadarChat_check_en = true;
            // 检查定时器是否未启动
            if (!eTIMER2_en) {
                // 启动定时器，并获取启动结果
                TIMER_ERROR_CODE  error = uni_hal_timer_start(_get_timer_index(eTIMER2));
                // 检查定时器启动是否成功
                if (error != TIMER_ERROR_OK) {
                    // 如果启动失败，打印错误日志
                    user_log("Timer start failed\r\n");
                }
                else {
                    // 如果启动成功，打印成功日志
                    user_log("Timer start OK\r\n");
                }
                // 设置定时器启用标志
                eTIMER2_en = true;
            }
            // 获取通知消息的ID，并将其赋值给remainder_cmd变量
            remainder_cmd = ulNotificationValue.msgId;
            // 使用switch语句根据remainder_cmd的值设置不同的定时器超时时间
            switch (remainder_cmd)
            {
                // 设置定时器超时时间为1分钟（60秒）
                case CMD_REMIND_1_MINUTE:
                    radarChat_Timer_out = 60*1000;
                    break;
                    // 设置定时器超时时间为10分钟
                case CMD_WATER_REMIND_10_MINUTES:
                case CMD_REMIND_10_MINUTES:
                    radarChat_Timer_out = 10*60*1000;
                    break;
                    // 设置定时器超时时间为30分钟
                case CMD_WATER_REMIND_30_MINUTES:
                case CMD_REMIND_30_MINUTES:
                    radarChat_Timer_out = 30*60*1000;
                    break;
                    // 设置定时器超时时间为1小时
                case CMD_WATER_REMIND_1_HOUR:
                case CMD_REMIND_1_HOUR:
                    radarChat_Timer_out = 60*60*1000;
                    break;
                    // 设置定时器超时时间为20分钟
                case CMD_WATER_REMIND_20_MINUTES:
                    radarChat_Timer_out = 20*60*1000;
                    break;
                    // 设置定时器超时时间为2小时
                case CMD_WATER_REMIND_2_HOURS:
                    radarChat_Timer_out = 60*60*1000*2;
                    break;
                    // 设置定时器超时时间为10秒
                case CMD_WATER_REMIND_10_SECONDS:
                    radarChat_Timer_out = 10*1000;
                    break;
                    // 设置定时器超时时间为20秒
                case CMD_WATER_REMIND_20_SECONDS:
                    radarChat_Timer_out = 20*1000;
                    break;
                    // 设置定时器超时时间为30秒
                case CMD_WATER_REMIND_30_SECONDS:
                    radarChat_Timer_out = 30*1000;
                    break;
                    // 设置定时器超时时间为40秒
                case CMD_WATER_REMIND_40_SECONDS:
                    radarChat_Timer_out = 40*1000;
                    break;
                    // 设置定时器超时时间为50秒
                case CMD_WATER_REMIND_50_SECONDS:
                    radarChat_Timer_out = 50*1000;
                    break;
                    // 不执行任何操作
                default:
                    break;
            }

            user_log("Recv message: %d seconds\r\n", radarChat_Timer_out/1000);
            // 调用system_reset函数，重置radar_sys系统
            system_reset(&radar_sys);
        }
        //如果开启了提醒

        // 检查雷达聊天功能是否启用
        if (RadarChat_check_en) {
            // 如果启用，则调用sedentary_reminder函数进行久坐提醒
            sedentary_reminder(&radar_sys);
        }
        // 更新用户的存在状态
        update_presence_status();

        // 检查是否应该播放提醒
        if (should_play_reminder()) {
            // 如果需要播放提醒，则调用user_player_play函数播放音频回复
            user_player_play(AUDIO_PLAY_REPLY, "[107]");
            // 调用user_asr_goto_awakend函数使设备进入唤醒状态
            user_asr_goto_awakend();
        }
    }
}
/**
 * @brief
 *
*/
// 初始化雷达聊天任务
void radarChat_task_init(void)
{
    // 设置任务名称为 "radarChatTask"
    sprintf(radarTaskParam.task_name, "radarChatTask");
    // 设置任务堆栈大小为 512 字节
    radarTaskParam.stack_size = 256*2;
    // 设置任务优先级为普通优先级
    radarTaskParam.priority = OS_PRIORITY_NORMAL;
    // 注册一个消息队列，队列大小为 5
    radarTaskMsg = MessageRegister(5);

    /**
     * @brief 创建一个线程，用于执行雷达任务
     * @param radar_pthread 线程句柄，用于标识线程
     * @param radarTaskParam 线程参数，包含线程的属性和优先级等信息
     * @param radarChat_task 线程入口函数，即线程要执行的任务函数
     * @param NULL 传递给线程入口函数的参数，这里为NULL表示不需要传递参数
    */

    uni_pthread_create(&radar_pthread, &radarTaskParam, radarChat_task, NULL);
    // 初始化定时器2，设置周期为1000微秒（1毫秒），初始计数值为0，回调函数为vTimerCallback_no_man
    TIMER_ERROR_CODE error = uni_hal_timer_init(_get_timer_index(eTIMER2), 1000, 0, vTimerCallback_no_man);//1ms 定时
    // 检查定时器初始化是否成功
    if (error != TIMER_ERROR_OK) {
        // 如果初始化失败，打印错误日志
        user_log("Timer2 init failed\r\n");
    }
    // 检查定时器2是否已经启用
    if (!eTIMER2_en) {
        // 启动定时器2
        error = uni_hal_timer_start(_get_timer_index(eTIMER2));
        // 检查定时器启动是否成功
        if (error != TIMER_ERROR_OK) {
            // 如果启动失败，打印错误日志
            user_log("Timer start failed\r\n");
        }
        // 如果启动成功，打印成功日志
        user_log("Timer start OK\r\n");
        // 设置定时器2启用标志为true
        eTIMER2_en = true;
    }
    // 将雷达系统的当前状态设置为监控状态
    radar_sys.current_state = MONITORING;
    // 初始化雷达系统的监控开始时间为0
    radar_sys.sit_start_time = 0;
    // 初始化雷达系统的离开开始时间为0
    radar_sys.away_start_time = 0;
    // 初始化雷达系统的警报计数为0
    radar_sys.alert_count = 0;
}
/**
 * @brief 雷达检测函数
 *
 * @return true 有人
 * @return false 无人
*/

bool radar_detect_human(void)
{
    // 调用user_gpio_get_value函数获取GPIO_NUM_A25引脚的值
    // GPIO_NUM_A25是一个宏定义，代表具体的GPIO引脚编号
    // user_gpio_get_value函数用于读取指定GPIO引脚的电平状态
    // 返回值：0表示没人，1表示有人
    return user_gpio_get_value(GPIO_NUM_A25);
}
/**
 * @brief 发出提醒
 *
*/
// 定义一个函数，用于触发警报
void trigger_alert(void)
{
    // 调用user_log函数，记录日志信息，显示当前剩余命令
    // remainder_cmd_str是一个字符串数组，remainder_cmd是当前命令的索引
    user_log("remainder command:%s\r\n", remainder_cmd_str[remainder_cmd]);
    // 判断当前命令是否小于CMD_WATER_REMIND_2_HOURS
    if (remainder_cmd< CMD_WATER_REMIND_2_HOURS) {
        // 如果小于，则播放音频"[113]"
        user_player_play(AUDIO_PLAY_REPLY, SEDENTARY_REMINDER_AUDIO_FILE);
    }
    else {
        // 如果大于等于，则播放音频"[119]"
        user_player_play(AUDIO_PLAY_REPLY, DRINK_WATER_REMINDER_AUDIO_FILE);
    }

}
/**
 * @brief 检查提醒确认
 *
 * @return true
 * @return false
*/
// 函数声明：check_alert_confirmed
// 功能：检查警报是否已确认
// 返回值：布尔类型，如果雷达检测到人类则返回false，否则返回true
bool check_alert_confirmed(void)
{
    // 调用radar_detect_human函数，判断雷达是否检测到人类
    // radar_detect_human()返回true表示检测到人类，返回false表示未检测到人类
    return radar_detect_human()?false:true;
    // 如果radar_detect_human()返回true（检测到人类），则整个函数返回false
    // 如果radar_detect_human()返回false（未检测到人类），则整个函数返回true
}
/**
 * @brief Get the current time ms object
 *
 * @return uint32_t
*/
// 定义一个静态函数，用于获取当前系统时间，单位为毫秒
static uint32_t get_current_time_ms(void) // 获取系统时间（毫秒）
{
    // 返回一个全局变量 radarChat_no_man_timer_cnt 的值，该变量应记录了系统时间的毫秒数
    return radarChat_no_man_timer_cnt;
}
/**
 * @brief
 *
 * @param sys
*/
// 函数：sedentary_reminder
// 功能：久坐提醒功能，根据系统状态进行不同的处理
// 参数：sys - 指向系统状态结构的指针
void sedentary_reminder(SystemState* sys) {
    // 获取当前时间（毫秒）
    uint32_t current_time = get_current_time_ms();

    // 根据当前系统状态进行不同的处理
    switch (sys->current_state) {
        case MONITORING: // 监控状态
            user_log("Monitoring\r\n"); // 打印日志
            if (radar_detect_human()) { // 如果雷达检测到人
                sys->sit_start_time = current_time; // 记录坐下开始时间
                sys->current_state = SITTING; // 切换到坐下状态
            }
            break;

        case SITTING: { // 坐下状态
            user_log("Sitting\r\n"); // 打印日志
            if (!radar_detect_human()) { // 如果雷达未检测到人
                sys->away_start_time = current_time; // 记录离开开始时间
                sys->current_state = AWAY_CHECK; // 切换到离开检查状态
                break;
            }

            // 计算坐下持续时间
            uint32_t duration = current_time - sys->sit_start_time;
            if (duration >=  radarChat_Timer_out) { // 如果持续时间超过设定阈值

                trigger_alert(); // 触发提醒
                sys->current_state = ALERTING; // 切换到提醒状态
                sys->alert_count++;
            }
            break;
        }

        case AWAY_CHECK: { // 离开检查状态
            user_log("Away Check\r\n"); // 打印日志，记录当前状态为“离开检查”
            if (radar_detect_human()) { // 调用函数检测雷达是否检测到人类
                sys->current_state = SITTING; // 保持原有计时
                break;
            }

            // 计算系统处于离开状态的时间，单位为毫秒
            uint32_t away_duration = current_time - sys->away_start_time;
            // 检查离开时间是否已经达到或超过20秒
            if (away_duration >= 20 * 1000) { // 20秒
                // 重置坐下开始时间为0，表示没有坐下
                sys->sit_start_time = 0;
                // 将系统当前状态设置为监控状态
                sys->current_state = MONITORING;

            }
            // 结束当前分支
            break;
        }

        case ALERTING: // 当前状态为ALERTING（警报中）
            user_log("Alerting\r\n"); // 记录日志，表示当前处于警报状态
            if (check_alert_confirmed()) { // 检查警报是否已确认
                if (radar_detect_human()) { // 使用雷达检测是否有人员存在
                    sys->sit_start_time = current_time; // 记录当前时间为坐下开始时间
                    sys->current_state = SITTING; // 将系统状态设置为SITTING（坐下）
                }
                else { // 如果雷达未检测到人员
                    user_player_play(AUDIO_PLAY_REPLY, "[107]"); // 播放音频回复，内容为"[107]"
                    user_asr_goto_awakend(); // 语音唤醒功能
                    sys->current_state = MONITORING; // 将系统状态设置为MONITORING（监控中）
                }
                sys->alert_count = 0; // 重置警报计数器
            }
            // 超时处理（示例：10秒无响应自动重置）
            else if ((current_time - sys->sit_start_time) > 10 * 1000) { // 如果当前时间与坐下开始时间的差值超过10秒
                sys->current_state = MONITORING; // 将系统状态设置为MONITORING（监控中）
            }

            // 最大提醒次数限制
            if (sys->alert_count >= 3) { // 如果警报计数器达到或超过3次
                sys->current_state = MONITORING; // 将系统状态设置为MONITORING（监控中）
                sys->alert_count = 0; // 重置警报计数器
            }
            break; // 结束当前case分支
    }
}

// 手动重置函数（供外部调用）
// 定义一个函数，用于重置系统状态
void system_reset(SystemState* sys) {
    // 将系统的当前状态设置为MONITORING（监控状态）
    sys->current_state = MONITORING;
    // 将系统的sit_start_time（坐下开始时间）重置为0
    sys->sit_start_time = 0;
    // 将系统的away_start_time（离开开始时间）重置为0
    sys->away_start_time = 0;
    // 将系统的alert_count（警报计数）重置为0
    sys->alert_count = 0;
}

// 获取状态描述（调试用）
// 定义一个函数，用于获取状态名称
// 参数：s - 状态枚举值
// 返回值：指向状态名称字符串的指针
const char* get_state_name(State s) {
    // 定义一个静态常量字符指针数组，用于存储状态名称
    // 静态变量在函数调用之间保持其值，因此只需初始化一次
    static const char* names[] = {
        "MONITORING", "SITTING",
        "AWAY_CHECK", "ALERTING"
    };
    return names[s];
}

// 离座状态检测（需在主循环定期调用）
void update_presence_status(void) {
    // 调用radar_detect_human函数检测是否有人存在
    bool present = radar_detect_human();

    // 如果检测到离座且之前状态不是离座
    if (!present && !p_status.is_absent) {
        // 首次检测到离座
        p_status.absence_start = get_current_time_ms(); // 记录离座开始时间
        p_status.is_absent = true; // 设置状态为离座
        user_log("human os_absent \r\n"); // 记录日志
    }
    // 如果检测到有人存在且之前状态是离座
    else if (present && p_status.is_absent) {
        // 离座期间重新检测到人
        if (get_current_time_ms() - p_status.absence_start > PRESENCE_TIMER_OUT) {

            // 如果离座时间超过预设的超时时间
            p_status.need_remind = true; // 满足触发条件，需要提醒
            user_log("human need_remind \r\n"); // 记录日志
        }
        p_status.is_absent = false; // 设置状态为在座
        user_log("Less than %d seconds. \r\n", PRESENCE_TIMER_OUT/ 1000); // 记录日志，显示未超时
    }
}
// 提醒触发检测
// 函数声明：should_play_reminder
// 功能：检查是否需要播放提醒，并清除提醒标志
// 返回值：如果需要播放提醒，返回true；否则返回false
bool should_play_reminder(void) {
    // 检查全局变量p_status中的need_remind标志是否为true
    if (p_status.need_remind) {
        // 如果需要提醒，则将need_remind标志设置为false，表示已处理
        p_status.need_remind = false; // 清除标志
        // 返回true，表示需要播放提醒
        return true;
    }
    // 如果不需要提醒，返回false
    return false;
}