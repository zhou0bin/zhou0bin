// /**
//  * @file common.h
//  * @brief 公用类型
//  * @author 马少民 (ma-shm@reachauto.com)
//  * @version 2.0
//  * @date 2022-08-05
//  *
//  * @par 日志:
//  * <table>
//  * <tr><th>Date       <th>Version <th>Author  <th>Description
//  * <tr><td>2022-08-05 <td>2.0     <td>马少民     <td>新建
//  * </table>
//  */
// #ifndef VCPROXYSERVICEWAPPER_HTTPCOMMON_H
// #define VCPROXYSERVICEWAPPER_HTTPCOMMON_H

// #include <iostream>

// namespace middleware
// {
//     namespace vcproxyservicewapper
//     {
//         /**
//          * @brief 枚举类型
//          */
//         enum class Type
//         {
//             GET,
//             POST,
//             PUT,
//             DELETE,
//             DOWNLOAD,
//             UPLOADBYPOST,
//             UPLOADBYPUT,
//             UNKNOWN
//         };

//         /**
//          * @brief 认证枚举类型 ONE_WAY：单向认证，TOW_WAY：双向认证，NO_WAY：无认证
//          */
//         enum class AuthsModeTypeEN : uint8_t
//         {
//             ONE_WAY = 0x1,
//             TOW_WAY = 0x2,
//             NO_WAY = 0x3
//         };
//     }
// }

// #endif // VCPROXYSERVICEWAPPER_HTTPCOMMON_H