#ifndef console_util_h__
#define console_util_h__

/*
*	console_util.hpp 
*
*   v0.1 2018-11 By GuoJH
*/

#include <string>
#include <platform/platform_cfg.hpp>

namespace util {
namespace win  {

// ָ����
static const uint32_t STD_INPUT       = ((uint32_t)-10);       // STD_INPUT_HANDLE
static const uint32_t STD_OUTPUT      = ((uint32_t)-11);       // STD_OUTPUT_HANDLE
static const uint32_t STD_ERROR       = ((uint32_t)-12);       // STD_ERROR_HANDLE

// ��ɫ��ӡ
static const uint16_t COLOR_DEF       = 0;
static const uint16_t COLOR_RED       = 0x0004; // FOREGROUND_RED       text color contains red.
static const uint16_t COLOR_GREEN     = 0x0002; // FOREGROUND_GREEN     text color contains green.
static const uint16_t COLOR_BLUE      = 0x0001; // FOREGROUND_BLUE      text color contains blue.
static const uint16_t COLOR_INTENSITY = 0x0008; // FOREGROUND_INTENSITY text color is intensified.

// ����
struct coord
{
    short x;
    short y;
};

/*
*	ָ����ɫ, ָ����, ���
*/
UTILITY_FUNCT_DECL void output(
    const std::string& text, 
    const uint32_t handle = STD_OUTPUT,
    const uint16_t color  = COLOR_DEF);

/*
*	�����Ļ
*/
UTILITY_FUNCT_DECL void clear();

/*
*	�ƶ���ӡ��굽ָ��λ��
*/
UTILITY_FUNCT_DECL void cursor_goto(const coord& pos);
UTILITY_FUNCT_DECL void cursor_goto(short x, short y);

/*
*	���ع��λ��
*/
UTILITY_FUNCT_DECL coord cursor_pos();

/*
*	��ӡ������
*   ����:
*   >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>=================== 20%
*/
UTILITY_FUNCT_DECL void output_progress(
    int progress, const char prefix = '>', const char suffix = '=');

} // console
} // util

#ifndef UTILITY_DISABLE_HEADONLY
#   include "impl/console_util.ipp"
#endif

#endif // console_util_h__
