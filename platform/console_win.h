#ifndef console_win_h__
#define console_win_h__

/*
*   console_win.h  
*
*   v0.1 2018-11 By GuoJH
*/

#include <utility.hpp>

#ifndef OS_WIN
#   error console_win.h only supports Windows.
#endif

#include <string>
#include <platform/platform_cfg.h>

namespace util {
namespace win  {

//! 指定流
static const uint32_t STD_INPUT       = ((uint32_t)-10);       //!< STD_INPUT_HANDLE
static const uint32_t STD_OUTPUT      = ((uint32_t)-11);       //!< STD_OUTPUT_HANDLE
static const uint32_t STD_ERROR       = ((uint32_t)-12);       //!< STD_ERROR_HANDLE

//! 颜色打印
static const uint16_t COLOR_DEF       = 0;
static const uint16_t COLOR_RED       = 0x0004; //!< FOREGROUND_RED       text color contains red.
static const uint16_t COLOR_GREEN     = 0x0002; //!< FOREGROUND_GREEN     text color contains green.
static const uint16_t COLOR_BLUE      = 0x0001; //!< FOREGROUND_BLUE      text color contains blue.
static const uint16_t COLOR_INTENSITY = 0x0008; //!< FOREGROUND_INTENSITY text color is intensified.

//! 坐标
struct coord
{
    short x;
    short y;
};

/*!
 *  指定颜色, 指定流, 输出
 */
UTILITY_FUNCT_DECL void output(
    const std::string& text, 
    const uint32_t handle = STD_OUTPUT,
    const uint16_t color  = COLOR_DEF);

/*!
 *  清除屏幕
 */
UTILITY_FUNCT_DECL void clear();

/*!
 *  判断指定流是否重定向
 */
UTILITY_FUNCT_DECL bool is_redirect(const uint32_t handle = STD_OUTPUT);

/*!
 *  移动打印光标到指定位置
 */
UTILITY_FUNCT_DECL void cursor_goto(const coord& pos);
UTILITY_FUNCT_DECL void cursor_goto(short x, short y);

/*!
 *  返回光标位置
 */
UTILITY_FUNCT_DECL coord cursor_pos();

/*!
 *  打印进度条
 *  例子:
 *  >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>=================== 20%
 */
UTILITY_FUNCT_DECL void output_progress(
    int progress, const char prefix = '>', const char suffix = '=');

} // console
} // util

#ifndef UTILITY_DISABLE_HEADONLY
#   include "impl/console_win.ipp"
#endif

#endif // console_win_h__
