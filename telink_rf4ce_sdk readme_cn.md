# telink_rf4ce_sdk README

## SDK 介绍

telink_rf4ce_sdk 是一款基于泰凌微电子 TLSR825x、TLSR827x、TLSR952x、TL321x 等系列 SoC 的 RF4CE 软件开发平台，支持 ZRC2、MSO 等 RF4CE 应用层协议标准，支持红外发射、红外学习、音频以及 OTA 功能，帮助您高效构建射频遥控器产品。

RF4CE 是专为消费电子产品设计的射频遥控协议标准，具有非视距遥控、双向通讯、超低功耗、跨品牌互操作性等特点。

本 SDK 提供了完备的软件体系，包括芯片底层驱动、适配层驱动、RF4CE 协议栈以及丰富的示例工程（包含遥控器端和接收端），全面支持从产品原型设计到量产部署的整个研发周期。

![SDK architecture](Figures/SDK_architecture.png)

**核心能力**

| 类别 | 能力 |
| --- | --- |
| 无线连接 | 支持 RF4CE 协议栈，底层基于 IEEE 802.15.4 协议，支持 ZRC2、MSO 等 RF4CE 应用层协议标准 |
| 软件框架 | 提供标准化外设驱动、安全组件、常见红外码发送示例；采用模块化分层设计，便于功能扩展与维护 |
| 系统服务 | 集成时钟管理、事件调度、功耗管理、非易失性存储（NV）管理以及 OTA 固件升级管理，保障系统高效稳定运行 |
| 示例工程 | 遥控器端（RC）与接收端（Dongle）等典型参考工程 |

**典型应用**

| 无线技术 | 应用领域 | 典型产品 |
| --- | --- | --- |
| RF4CE（基于 IEEE 802.15.4） | 消费电子 | 电视机、机顶盒射频遥控器，学习型遥控器，红外 + 射频遥控器等 |
|  | 智能家居 | 智能家居中控遥控、多合一遥控器等 |

**支持信息**

关于完整、准确的芯片型号、对应的开发板、开发平台、工具链以及 SDK 版本的详细信息，请参考 [Release Notes](./doc/telink_rf4ce_sdk_Release_Note.md)。

## 文档与资源

**文档导航**

| 文档 | 说明 |
| --- | --- |
| [快速入门指南](https://doc.telink-semi.cn/doc/zh/software/res/sdk/rf4ce/get_started/telink_rf4ce_sdk_get_started_cn/) | 开发环境配置、SDK 获取及快速上手方法 |
| [Release Notes](./doc/telink_rf4ce_sdk_Release_Note.md) | 支持平台、版本说明与详细变化 |

**社区与资源**

| 资源 | 说明 |
| --- | --- |
| [Telink 官方论坛](https://forum.telink-semi.cn/) | 技术支持与讨论 |
| [Telink 官方网站](https://www.telink-semi.com/) | 产品与文档中心 |
| [GitHub](https://github.com/telink-semi/telink_rf4ce_sdk) / [Gitee](https://gitee.com/telink-semi/telink_rf4ce_sdk) | SDK 源码仓库 |

## 许可证

本项目采用以下许可证：

**Apache License, Version 2.0**

Licensed under the Apache License, Version 2.0 (the "License"); You may not use this file except in compliance with the License.

You may obtain a copy of the License at: [http://www.apache.org/licenses/LICENSE-2.0](http://www.apache.org/licenses/LICENSE-2.0)

Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.

See the License for the specific language governing permissions and limitations under the License.