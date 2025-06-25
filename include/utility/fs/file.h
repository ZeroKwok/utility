// This file is part of the utility project.
// Copyright (c) 2018-2025 zero <zero.kwok@foxmail.com>
//
// For the full copyright and license information, please view the LICENSE
// file that was distributed with this source code.

#ifndef filesystem_file_h__
#define filesystem_file_h__

#include "utility/config.h"

namespace UTILITY_NAMESPACE {
namespace fs {

//! \brief 文件的不透明对象
typedef struct _file* fptr;

/*!
 * \brief 文件资源RAII包装器，自动管理文件生命周期
 */
class UTILITY_API file
{
    fptr _fd = nullptr;

    // 禁止拷贝构造和拷贝赋值
    file(const file&) = delete;
    file& operator=(const file&) = delete;
public:
    file() noexcept;
    ~file() noexcept;

    /*!
     * \brief 接管已有文件描述符
     * \param f 要接管的文件指针
     * \note 构造后原指针所有权转移
     */
    file(const fptr& f) noexcept; 

    /*!
     * \brief 移动构造
     * \param right 源文件对象
     * \post 源对象变为无效状态
     * \return 当前对象引用
     */
    file(file&& right) noexcept;
    file& operator=(file&& right) noexcept;

    /*!
     * \brief 显式关闭文件
     * \note 若文件无效则无操作
     */
    void close();

    //! \brief 检查文件是否有效
    bool valid() const;

    //! \brief 检查文件是否有效（与 valid() 相同）
    operator bool() const;
    operator fptr() const;
};

} // fs
} // UTILITY_NAMESPACE

#endif // filesystem_file_h__
