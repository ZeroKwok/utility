// This file is part of the utility project.
// Copyright (c) 2020-2024 zero <zero.kwok@foxmail.com>
//
// For the full copyright and license information, please view the LICENSE
// file that was distributed with this source code.

#ifndef filesystem_file_guard_h__
#define filesystem_file_guard_h__

#include "filesystem.h"

namespace UTILITY_NAMESPACE {
namespace fs {

/*!
 *  \brief 文件资源管理类
 */
class UTILITY_API file_guard
{
    file _fd = nullptr;
public:
    file_guard();
    ~file_guard();

    /*!
     *  \brief 使用文件描述符构造 file_guard
     *  \param f 文件对象, 由 open() 打开文件后返回
     *  \note 构造后, f 的所有权将移交 file_guard 管理, 外部不应该再调用 close() 方法
     */
    file_guard(const file& f); 

    /*!
     *  \brief 拷贝构造函数，从另一个实例构造，right 将失去文件的所有权
     */
    file_guard(file_guard& right);

#if __HAS_CPP11
    file_guard(file_guard&& right);
    file_guard& operator=(file_guard&& right);
#endif

    /*!
     *  \brief 赋值后 right 将失去文件的所有权
     */
    file_guard& operator=(file_guard& right);

    /*!
     *  \brief 如果文件有效, 则关闭文件
     */
    void close();

    /*!
     *  \brief 检查文件是否有效
     * 
     *  \return 如果文件有效，返回 true，否则返回 false
     */
    bool valid() const;

    /*!
     *  \brief 检查文件是否有效（与 valid() 相同）
     */
    operator bool() const;

    /*!
     *  \brief 返回内部持有的文件对象
     */
    operator file() const;
};

} // fs
} // UTILITY_NAMESPACE

#endif // filesystem_file_guard_h__
