/********************************************************************************************************
 * @file    zrc_profileCommands.h
 *
 * @brief   This is the header file for zrc_profileCommands.h
 *
 * @author	Zigbee GROUP
 * @date    2021
 *
 * @par     Copyright (c) 2021, Telink Semiconductor (Shanghai) Co., Ltd. ("TELINK")
 *
 *          Licensed under the Apache License, Version 2.0 (the "License");
 *          you may not use this file except in compliance with the License.
 *          You may obtain a copy of the License at
 *
 *              http://www.apache.org/licenses/LICENSE-2.0
 *
 *          Unless required by applicable law or agreed to in writing, software
 *          distributed under the License is distributed on an "AS IS" BASIS,
 *          WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *          See the License for the specific language governing permissions and
 *          limitations under the License.
 *******************************************************************************************************/

#ifndef _ZRCOMMANDS_H_
#define _ZRCOMMANDS_H_


/******************************************************************************
*******************************************************************************
* Public Macros
*******************************************************************************
******************************************************************************/
/* List of ZRC command codes */


/* List of user control pressed RC command codes */
#define TRG_key01                        0x07

#define ZRCmdRC_Select                   0x00
#define ZRCmdRC_Up                       0x01
#define ZRCmdRC_Down                     0x02
#define ZRCmdRC_Left                     0x03
#define ZRCmdRC_Right                    0x04
#define ZRCmdRC_RightUp                  0x05
#define ZRCmdRC_RightDown                0x06
#define ZRCmdRC_LeftUp                   0x07
#define ZRCmdRC_LeftDown                 0x08
#define ZRCmdRC_RootMenu                 0x09
#define ZRCmdRC_SetupMenu                0x0A
#define ZRCmdRContentsMenu               0x0B
#define ZRCmdRC_FavoriteMenu             0x0C
#define ZRCmdRC_Exit                     0x0D

#define ZRCmdRC_MediaTopMenu             0x10
#define ZRCmdRC_MediaContextSensitive    0x11

#define ZRCmdRC_NumpadEntryMode          0x1D
#define ZRCmdRC_Numpad11                 0x1E
#define ZRCmdRC_Numpad12                 0x1F             
#define ZRCmdRC_Numpad0_or_10            0x20
#define ZRCmdRC_Numpad1                  0x21
#define ZRCmdRC_Numpad2                  0x22
#define ZRCmdRC_Numpad3                  0x23
#define ZRCmdRC_Numpad4                  0x24
#define ZRCmdRC_Numpad5                  0x25
#define ZRCmdRC_Numpad6                  0x26
#define ZRCmdRC_Numpad7                  0x27
#define ZRCmdRC_Numpad8                  0x28
#define ZRCmdRC_Numpad9                  0x29
#define ZRCmdRC_Dot                      0x2A
#define ZRCmdRC_Enter                    0x2B
#define ZRCmdRClear                      0x2C

#define ZRCmdRC_NextFavorite             0x2F
#define ZRCmdRChannelUp                  0x30
#define ZRCmdRChannelDown                0x31
#define ZRCmdRC_PreviousChannel          0x32
#define ZRCmdRC_SoundSelect              0x33
#define ZRCmdRC_InputSelect              0x34
#define ZRCmdRC_DisplayInfo              0x35
#define ZRCmdRC_Help                     0x36
#define ZRCmdRC_PageUp                   0x37
#define ZRCmdRC_PageDown                 0x38

#define ZRCmdRC_Power                    0x40
#define ZRCmdRC_VolumeUp                 0x41
#define ZRCmdRC_VolumeDown               0x42
#define ZRCmdRC_Mute                     0x43
#define ZRCmdRC_Play                     0x44
#define ZRCmdRC_Stop                     0x45
#define ZRCmdRC_Pause                    0x46
#define ZRCmdRC_Record                   0x47
#define ZRCmdRC_Rewind                   0x48
#define ZRCmdRC_FastForward              0x49
#define ZRCmdRC_Eject                    0x4A
#define ZRCmdRC_Forward                  0x4B
#define ZRCmdRC_Backward                 0x4C
#define ZRCmdRC_StopRecord               0x4D
#define ZRCmdRC_PauseRecord              0x4E

#define ZRCmdRC_Angle                    0x50
#define ZRCmdRC_SubPicture               0x51
#define ZRCmdRC_VideoOnDemand            0x52
#define ZRCmdRC_EPc                      0x53
#define ZRCmdRC_TimerProgramminc         0x54
#define ZRCmdRC_InitialConfic            0x55
#define ZRCmdRC_SelectBroadcastType      0x56
#define ZRCmdRC_SelectSoundPresentation  0x57

#define ZRCmdRC_PlayFunction             0x60
#define ZRCmdRC_PausePlayFunction        0x61
#define ZRCmdRC_RecordFunction           0x62
#define ZRCmdRC_PauseRecordFunction      0x63
#define ZRCmdRC_StopFunction             0x64
#define ZRCmdRC_MuteFunction             0x65
#define ZRCmdRC_RestoreVolumeFunction    0x66
#define ZRCmdRC_TuneFunction             0x67
#define ZRCmdRC_SelectMediaFunction      0x68
#define ZRCmdRC_SelectAVInputFunction    0x69
#define ZRCmdRC_SelectAudioInputFunction 0x6A
#define ZRCmdRC_PowerToggleFunction      0x6B
#define ZRCmdRC_PowerOffFunction         0x6C
#define ZRCmdRC_PowerOnFunction          0x6D

#define ZRCmdRC_F1                       0x71
#define ZRCmdRC_F2                       0x72
#define ZRCmdRC_F3                       0x73
#define ZRCmdRC_F4                       0x74
#define ZRCmdRC_F5                       0x75
#define ZRCmdRC_Data                     0x76

/******************************************************************************/
/* List of supported commands */
#if (RF4CE_DEV_TYPE == (1<<6) )

#define ZRCmdRC_Select_Supported                   TRUE
#define ZRCmdRC_Up_Supported                       TRUE
#define ZRCmdRC_Down_Supported                     TRUE
#define ZRCmdRC_Left_Supported                     TRUE
#define ZRCmdRC_Right_Supported                    TRUE
#define ZRCmdRC_RightUp_Supported                  FALSE
#define ZRCmdRC_RightDown_Supported                FALSE
#define ZRCmdRC_LeftUp_Supported                   FALSE
#define ZRCmdRC_LeftDown_Supported                 FALSE
#define ZRCmdRC_RootMenu_Supported                 TRUE
#define ZRCmdRC_SetupMenu_Supported                FALSE
#define ZRCmdRContentsMenu_Supported               FALSE
#define ZRCmdRC_FavoriteMenu_Supported             FALSE
#define ZRCmdRC_Exit_Supported                     TRUE

#define ZRCmdRC_MediaTopMenu_Supported             FALSE
#define ZRCmdRC_MediaContextSensitive_Supported    FALSE

#define ZRCmdRC_NumpadEntryMode_Supported          FALSE
#define ZRCmdRC_Numpad11_Supported                 FALSE
#define ZRCmdRC_Numpad12_Supported                 FALSE          
#define ZRCmdRC_Numpad0_or_10_Supported            FALSE
#define ZRCmdRC_Numpad1_Supported                  FALSE
#define ZRCmdRC_Numpad2_Supported                  FALSE
#define ZRCmdRC_Numpad3_Supported                  FALSE
#define ZRCmdRC_Numpad4_Supported                  FALSE
#define ZRCmdRC_Numpad5_Supported                  FALSE
#define ZRCmdRC_Numpad6_Supported                  FALSE
#define ZRCmdRC_Numpad7_Supported                  FALSE
#define ZRCmdRC_Numpad8_Supported                  FALSE
#define ZRCmdRC_Numpad9_Supported                  FALSE
#define ZRCmdRC_Dot_Supported                      FALSE
#define ZRCmdRC_Enter_Supported                    FALSE
#define ZRCmdRClear_Supported                      FALSE

#define ZRCmdRC_NextFavorite_Supported             FALSE
#define ZRCmdRChannelUp_Supported                  FALSE
#define ZRCmdRChannelDown_Supported                FALSE
#define ZRCmdRC_PreviousChannel_Supported          FALSE
#define ZRCmdRC_SoundSelect_Supported              FALSE
#define ZRCmdRC_InputSelect_Supported              FALSE
#define ZRCmdRC_DisplayInfo_Supported              FALSE
#define ZRCmdRC_Help_Supported                     FALSE
#define ZRCmdRC_PageUp_Supported                   FALSE
#define ZRCmdRC_PageDown_Supported                 FALSE

#define ZRCmdRC_Power_Supported                    FALSE
#define ZRCmdRC_VolumeUp_Supported                 FALSE
#define ZRCmdRC_VolumeDown_Supported               FALSE
#define ZRCmdRC_Mute_Supported                     FALSE
#define ZRCmdRC_Play_Supported                     TRUE
#define ZRCmdRC_Stop_Supported                     TRUE
#define ZRCmdRC_Pause_Supported                    TRUE
#define ZRCmdRC_Record_Supported                   TRUE
#define ZRCmdRC_Rewind_Supported                   TRUE
#define ZRCmdRC_FastForward_Supported              TRUE
#define ZRCmdRC_Eject_Supported                    FALSE
#define ZRCmdRC_Forward_Supported                  FALSE
#define ZRCmdRC_Backward_Supported                 FALSE
#define ZRCmdRC_StopRecord_Supported               FALSE
#define ZRCmdRC_PauseRecord_Supported              FALSE

#define ZRCmdRC_Angle_Supported                    FALSE
#define ZRCmdRC_SubPicture_Supported               FALSE
#define ZRCmdRC_VideoOnDemand_Supported            FALSE
#define ZRCmdRC_EPSupported                        FALSE
#define ZRCmdRC_TimerProgramminSupported           FALSE
#define ZRCmdRC_InitialConfiSupported              FALSE
#define ZRCmdRC_SelectBroadcastType_Supported      FALSE
#define ZRCmdRC_SelectSoundPresentation_Supported  FALSE

#define ZRCmdRC_PlayFunction_Supported             FALSE
#define ZRCmdRC_PausePlayFunction_Supported        FALSE
#define ZRCmdRC_RecordFunction_Supported           FALSE
#define ZRCmdRC_PauseRecordFunction_Supported      FALSE
#define ZRCmdRC_StopFunction_Supported             FALSE
#define ZRCmdRC_MuteFunction_Supported             FALSE
#define ZRCmdRC_RestoreVolumeFunction_Supported    FALSE
#define ZRCmdRC_TuneFunction_Supported             FALSE
#define ZRCmdRC_SelectMediaFunction_Supported      FALSE
#define ZRCmdRC_SelectAVInputFunction_Supported    FALSE
#define ZRCmdRC_SelectAudioInputFunction_Supported FALSE
#define ZRCmdRC_PowerToggleFunction_Supported      TRUE
#define ZRCmdRC_PowerOffFunction_Supported         TRUE
#define ZRCmdRC_PowerOnFunction_Supported          TRUE

#define ZRCmdRC_F1_Supported                       FALSE
#define ZRCmdRC_F2_Supported                       FALSE
#define ZRCmdRC_F3_Supported                       FALSE
#define ZRCmdRC_F4_Supported                       FALSE
#define ZRCmdRC_F5_Supported                       FALSE
#define ZRCmdRC_Data_Supported                     FALSE

#elif (RF4CE_DEV_TYPE == (1<<9))

#define ZRCmdRC_Select_Supported                   TRUE
#define ZRCmdRC_Up_Supported                       TRUE
#define ZRCmdRC_Down_Supported                     TRUE
#define ZRCmdRC_Left_Supported                     TRUE
#define ZRCmdRC_Right_Supported                    TRUE
#define ZRCmdRC_RightUp_Supported                  FALSE
#define ZRCmdRC_RightDown_Supported                FALSE
#define ZRCmdRC_LeftUp_Supported                   FALSE
#define ZRCmdRC_LeftDown_Supported                 FALSE
#define ZRCmdRC_RootMenu_Supported                 TRUE 
#define ZRCmdRC_SetupMenu_Supported                FALSE
#define ZRCmdRContentsMenu_Supported               FALSE
#define ZRCmdRC_FavoriteMenu_Supported             FALSE
#define ZRCmdRC_Exit_Supported                     TRUE

#define ZRCmdRC_MediaTopMenu_Supported             FALSE
#define ZRCmdRC_MediaContextSensitive_Supported    FALSE

#define ZRCmdRC_NumpadEntryMode_Supported          FALSE
#define ZRCmdRC_Numpad11_Supported                 FALSE
#define ZRCmdRC_Numpad12_Supported                 FALSE          
#define ZRCmdRC_Numpad0_or_10_Supported            FALSE
#define ZRCmdRC_Numpad1_Supported                  FALSE
#define ZRCmdRC_Numpad2_Supported                  FALSE
#define ZRCmdRC_Numpad3_Supported                  FALSE
#define ZRCmdRC_Numpad4_Supported                  FALSE
#define ZRCmdRC_Numpad5_Supported                  FALSE
#define ZRCmdRC_Numpad6_Supported                  FALSE
#define ZRCmdRC_Numpad7_Supported                  FALSE
#define ZRCmdRC_Numpad8_Supported                  FALSE
#define ZRCmdRC_Numpad9_Supported                  FALSE
#define ZRCmdRC_Dot_Supported                      FALSE
#define ZRCmdRC_Enter_Supported                    FALSE
#define ZRCmdRClear_Supported                      FALSE

#define ZRCmdRC_NextFavorite_Supported             FALSE
#define ZRCmdRChannelUp_Supported                  TRUE
#define ZRCmdRChannelDown_Supported                TRUE
#define ZRCmdRC_PreviousChannel_Supported          FALSE
#define ZRCmdRC_SoundSelect_Supported              FALSE
#define ZRCmdRC_InputSelect_Supported              FALSE
#define ZRCmdRC_DisplayInfo_Supported              FALSE
#define ZRCmdRC_Help_Supported                     FALSE
#define ZRCmdRC_PageUp_Supported                   FALSE
#define ZRCmdRC_PageDown_Supported                 FALSE

#define ZRCmdRC_Power_Supported                    FALSE
#define ZRCmdRC_VolumeUp_Supported                 FALSE
#define ZRCmdRC_VolumeDown_Supported               FALSE
#define ZRCmdRC_Mute_Supported                     FALSE
#define ZRCmdRC_Play_Supported                     FALSE
#define ZRCmdRC_Stop_Supported                     FALSE
#define ZRCmdRC_Pause_Supported                    FALSE
#define ZRCmdRC_Record_Supported                   FALSE
#define ZRCmdRC_Rewind_Supported                   FALSE
#define ZRCmdRC_FastForward_Supported              FALSE
#define ZRCmdRC_Eject_Supported                    FALSE
#define ZRCmdRC_Forward_Supported                  FALSE
#define ZRCmdRC_Backward_Supported                 FALSE
#define ZRCmdRC_StopRecord_Supported               FALSE
#define ZRCmdRC_PauseRecord_Supported              FALSE

#define ZRCmdRC_Angle_Supported                    FALSE
#define ZRCmdRC_SubPicture_Supported               FALSE
#define ZRCmdRC_VideoOnDemand_Supported            FALSE
#define ZRCmdRC_EPSupported                        FALSE
#define ZRCmdRC_TimerProgramminSupported           FALSE
#define ZRCmdRC_InitialConfiSupported              FALSE
#define ZRCmdRC_SelectBroadcastType_Supported      FALSE
#define ZRCmdRC_SelectSoundPresentation_Supported  FALSE

#define ZRCmdRC_PlayFunction_Supported             FALSE
#define ZRCmdRC_PausePlayFunction_Supported        FALSE
#define ZRCmdRC_RecordFunction_Supported           FALSE
#define ZRCmdRC_PauseRecordFunction_Supported      FALSE
#define ZRCmdRC_StopFunction_Supported             FALSE
#define ZRCmdRC_MuteFunction_Supported             FALSE
#define ZRCmdRC_RestoreVolumeFunction_Supported    FALSE
#define ZRCmdRC_TuneFunction_Supported             FALSE
#define ZRCmdRC_SelectMediaFunction_Supported      FALSE
#define ZRCmdRC_SelectAVInputFunction_Supported    FALSE
#define ZRCmdRC_SelectAudioInputFunction_Supported FALSE
#define ZRCmdRC_PowerToggleFunction_Supported      TRUE
#define ZRCmdRC_PowerOffFunction_Supported         TRUE
#define ZRCmdRC_PowerOnFunction_Supported          TRUE

#define ZRCmdRC_F1_Supported                       FALSE
#define ZRCmdRC_F2_Supported                       FALSE
#define ZRCmdRC_F3_Supported                       FALSE
#define ZRCmdRC_F4_Supported                       FALSE
#define ZRCmdRC_F5_Supported                       FALSE
#define ZRCmdRC_Data_Supported                     FALSE

#else  //default is TV
#define ZRCmdRC_Select_Supported                   TRUE
#define ZRCmdRC_Up_Supported                       TRUE
#define ZRCmdRC_Down_Supported                     TRUE
#define ZRCmdRC_Left_Supported                     TRUE
#define ZRCmdRC_Right_Supported                    TRUE
#define ZRCmdRC_RightUp_Supported                  FALSE
#define ZRCmdRC_RightDown_Supported                FALSE
#define ZRCmdRC_LeftUp_Supported                   FALSE
#define ZRCmdRC_LeftDown_Supported                 FALSE
#define ZRCmdRC_RootMenu_Supported                 TRUE
#define ZRCmdRC_SetupMenu_Supported                TRUE
#define ZRCmdRContentsMenu_Supported               FALSE
#define ZRCmdRC_FavoriteMenu_Supported             FALSE
#define ZRCmdRC_Exit_Supported                     TRUE

#define ZRCmdRC_MediaTopMenu_Supported             FALSE
#define ZRCmdRC_MediaContextSensitive_Supported    FALSE

#define ZRCmdRC_NumpadEntryMode_Supported          FALSE
#define ZRCmdRC_Numpad11_Supported                 FALSE
#define ZRCmdRC_Numpad12_Supported                 FALSE          
#define ZRCmdRC_Numpad0_or_10_Supported            FALSE
#define ZRCmdRC_Numpad1_Supported                  FALSE
#define ZRCmdRC_Numpad2_Supported                  FALSE
#define ZRCmdRC_Numpad3_Supported                  FALSE
#define ZRCmdRC_Numpad4_Supported                  FALSE
#define ZRCmdRC_Numpad5_Supported                  FALSE
#define ZRCmdRC_Numpad6_Supported                  FALSE
#define ZRCmdRC_Numpad7_Supported                  FALSE
#define ZRCmdRC_Numpad8_Supported                  FALSE
#define ZRCmdRC_Numpad9_Supported                  FALSE
#define ZRCmdRC_Dot_Supported                      FALSE
#define ZRCmdRC_Enter_Supported                    FALSE
#define ZRCmdRClear_Supported                      FALSE

#define ZRCmdRC_NextFavorite_Supported             FALSE
#define ZRCmdRChannelUp_Supported                  TRUE
#define ZRCmdRChannelDown_Supported                TRUE
#define ZRCmdRC_PreviousChannel_Supported          FALSE
#define ZRCmdRC_SoundSelect_Supported              FALSE
#define ZRCmdRC_InputSelect_Supported              TRUE
#define ZRCmdRC_DisplayInfo_Supported              FALSE
#define ZRCmdRC_Help_Supported                     FALSE
#define ZRCmdRC_PageUp_Supported                   FALSE
#define ZRCmdRC_PageDown_Supported                 FALSE

#define ZRCmdRC_Power_Supported                    FALSE
#define ZRCmdRC_VolumeUp_Supported                 TRUE
#define ZRCmdRC_VolumeDown_Supported               TRUE
#define ZRCmdRC_Mute_Supported                     TRUE
#define ZRCmdRC_Play_Supported                     TRUE
#define ZRCmdRC_Stop_Supported                     FALSE
#define ZRCmdRC_Pause_Supported                    FALSE
#define ZRCmdRC_Record_Supported                   FALSE
#define ZRCmdRC_Rewind_Supported                   FALSE
#define ZRCmdRC_FastForward_Supported              FALSE
#define ZRCmdRC_Eject_Supported                    FALSE
#define ZRCmdRC_Forward_Supported                  FALSE
#define ZRCmdRC_Backward_Supported                 FALSE
#define ZRCmdRC_StopRecord_Supported               FALSE
#define ZRCmdRC_PauseRecord_Supported              FALSE

#define ZRCmdRC_Angle_Supported                    FALSE
#define ZRCmdRC_SubPicture_Supported               FALSE
#define ZRCmdRC_VideoOnDemand_Supported            FALSE
#define ZRCmdRC_EPSupported                        FALSE
#define ZRCmdRC_TimerProgramminSupported           FALSE
#define ZRCmdRC_InitialConfiSupported              FALSE
#define ZRCmdRC_SelectBroadcastType_Supported      FALSE
#define ZRCmdRC_SelectSoundPresentation_Supported  FALSE

#define ZRCmdRC_PlayFunction_Supported             FALSE
#define ZRCmdRC_PausePlayFunction_Supported        FALSE
#define ZRCmdRC_RecordFunction_Supported           FALSE
#define ZRCmdRC_PauseRecordFunction_Supported      FALSE
#define ZRCmdRC_StopFunction_Supported             FALSE
#define ZRCmdRC_MuteFunction_Supported             FALSE
#define ZRCmdRC_RestoreVolumeFunction_Supported    FALSE
#define ZRCmdRC_TuneFunction_Supported             FALSE
#define ZRCmdRC_SelectMediaFunction_Supported      FALSE
#define ZRCmdRC_SelectAVInputFunction_Supported    FALSE
#define ZRCmdRC_SelectAudioInputFunction_Supported FALSE
#define ZRCmdRC_PowerToggleFunction_Supported      TRUE
#define ZRCmdRC_PowerOffFunction_Supported         TRUE
#define ZRCmdRC_PowerOnFunction_Supported          TRUE

#define ZRCmdRC_F1_Supported                       FALSE
#define ZRCmdRC_F2_Supported                       FALSE
#define ZRCmdRC_F3_Supported                       FALSE
#define ZRCmdRC_F4_Supported                       FALSE
#define ZRCmdRC_F5_Supported                       FALSE
#define ZRCmdRC_Data_Supported                     FALSE

#endif
/******************************************************************************/
/* List of supported commands */
#define ZRCCmdSupportedBitMap  { \
/* Byte 0 */ \
((ZRCmdRC_Select_Supported     * (0x01 << 0x00)) | \
 (ZRCmdRC_Up_Supported         * (0x01 << 0x01)) | \
 (ZRCmdRC_Down_Supported       * (0x01 << 0x02)) | \
 (ZRCmdRC_Left_Supported       * (0x01 << 0x03)) | \
 (ZRCmdRC_Right_Supported      * (0x01 << 0x04)) | \
 (ZRCmdRC_RightUp_Supported    * (0x01 << 0x05)) | \
 (ZRCmdRC_RightDown_Supported  * (0x01 << 0x06)) | \
 (ZRCmdRC_LeftUp_Supported     * (0x01 << 0x07))), \
/* Byte 1 */  \
((ZRCmdRC_LeftDown_Supported       * (0x01 << 0x00)) | \
 (ZRCmdRC_RootMenu_Supported       * (0x01 << 0x01)) | \
 (ZRCmdRC_SetupMenu_Supported      * (0x01 << 0x02)) | \
 (ZRCmdRContentsMenu_Supported   * (0x01 << 0x03)) | \
 (ZRCmdRC_FavoriteMenu_Supported   * (0x01 << 0x04)) | \
 (ZRCmdRC_Exit_Supported           * (0x01 << 0x05))), \
/* Byte 2 */  \
((ZRCmdRC_MediaTopMenu_Supported            * (0x01 << 0x00)) | \
 (ZRCmdRC_MediaContextSensitive_Supported   * (0x01 << 0x01))), \
/* Byte 3 */  \
 ((ZRCmdRC_NumpadEntryMode_Supported   * (0x01 << 0x05)) | \
  (ZRCmdRC_Numpad11_Supported          * (0x01 << 0x06)) | \
  (ZRCmdRC_Numpad12_Supported          * (0x01 << 0x07))), \
/* Byte 4 */  \
((ZRCmdRC_Numpad0_or_10_Supported   * (0x01 << 0x00)) | \
 (ZRCmdRC_Numpad1_Supported         * (0x01 << 0x01)) | \
 (ZRCmdRC_Numpad2_Supported         * (0x01 << 0x02)) | \
 (ZRCmdRC_Numpad3_Supported         * (0x01 << 0x03)) | \
 (ZRCmdRC_Numpad4_Supported         * (0x01 << 0x04)) | \
 (ZRCmdRC_Numpad5_Supported         * (0x01 << 0x05)) | \
 (ZRCmdRC_Numpad6_Supported         * (0x01 << 0x06)) | \
 (ZRCmdRC_Numpad7_Supported         * (0x01 << 0x07))), \
/* Byte 5 */  \
((ZRCmdRC_Numpad8_Supported      * (0x01 << 0x00)) | \
 (ZRCmdRC_Numpad9_Supported      * (0x01 << 0x01)) | \
 (ZRCmdRC_Dot_Supported          * (0x01 << 0x02)) | \
 (ZRCmdRC_Enter_Supported        * (0x01 << 0x03)) | \
 (ZRCmdRClear_Supported        * (0x01 << 0x04)) | \
 (ZRCmdRC_NextFavorite_Supported * (0x01 << 0x07))), \
/* Byte 6 */  \
((ZRCmdRChannelUp_Supported         * (0x01 << 0x00)) | \
 (ZRCmdRChannelDown_Supported       * (0x01 << 0x01)) | \
 (ZRCmdRC_PreviousChannel_Supported   * (0x01 << 0x02)) | \
 (ZRCmdRC_SoundSelect_Supported       * (0x01 << 0x03)) | \
 (ZRCmdRC_InputSelect_Supported       * (0x01 << 0x04)) | \
 (ZRCmdRC_DisplayInfo_Supported       * (0x01 << 0x05)) | \
 (ZRCmdRC_Help_Supported              * (0x01 << 0x06)) | \
 (ZRCmdRC_PageUp_Supported            * (0x01 << 0x07))), \
/* Byte 7 */   \
((ZRCmdRC_PageDown_Supported     * (0x01 << 0x00)) ), \
/* Byte 8 */  \
((ZRCmdRC_Power_Supported         * (0x01 << 0x00)) | \
 (ZRCmdRC_VolumeUp_Supported      * (0x01 << 0x01)) | \
 (ZRCmdRC_VolumeDown_Supported    * (0x01 << 0x02)) | \
 (ZRCmdRC_Mute_Supported          * (0x01 << 0x03)) | \
 (ZRCmdRC_Play_Supported          * (0x01 << 0x04)) | \
 (ZRCmdRC_Stop_Supported          * (0x01 << 0x05)) | \
 (ZRCmdRC_Pause_Supported         * (0x01 << 0x06)) | \
 (ZRCmdRC_Record_Supported        * (0x01 << 0x07))), \
/* Byte 9 */  \
((ZRCmdRC_Rewind_Supported        * (0x01 << 0x00)) | \
 (ZRCmdRC_FastForward_Supported   * (0x01 << 0x01)) | \
 (ZRCmdRC_Eject_Supported         * (0x01 << 0x02)) | \
 (ZRCmdRC_Forward_Supported       * (0x01 << 0x03)) | \
 (ZRCmdRC_Backward_Supported      * (0x01 << 0x04)) | \
 (ZRCmdRC_StopRecord_Supported    * (0x01 << 0x05)) | \
 (ZRCmdRC_PauseRecord_Supported   * (0x01 << 0x06))), \
 /* Byte 10 */  \
((ZRCmdRC_Angle_Supported                   * (0x01 << 0x00)) | \
 (ZRCmdRC_SubPicture_Supported              * (0x01 << 0x01)) | \
 (ZRCmdRC_VideoOnDemand_Supported           * (0x01 << 0x02)) | \
 (ZRCmdRC_EPSupported                     * (0x01 << 0x03)) | \
 (ZRCmdRC_TimerProgramminSupported        * (0x01 << 0x04)) | \
 (ZRCmdRC_InitialConfiSupported           * (0x01 << 0x05)) | \
 (ZRCmdRC_SelectBroadcastType_Supported     * (0x01 << 0x06)) | \
 (ZRCmdRC_SelectSoundPresentation_Supported * (0x01 << 0x07))), \
 /* Byte 11 */  \
(0x00), \
/* Byte 12 */   \
((ZRCmdRC_PlayFunction_Supported           * (0x01 << 0x00)) | \
 (ZRCmdRC_PausePlayFunction_Supported      * (0x01 << 0x01)) | \
 (ZRCmdRC_RecordFunction_Supported         * (0x01 << 0x02)) | \
 (ZRCmdRC_PauseRecordFunction_Supported    * (0x01 << 0x03)) | \
 (ZRCmdRC_StopFunction_Supported           * (0x01 << 0x04)) | \
 (ZRCmdRC_MuteFunction_Supported           * (0x01 << 0x05)) | \
 (ZRCmdRC_RestoreVolumeFunction_Supported  * (0x01 << 0x06)) | \
 (ZRCmdRC_TuneFunction_Supported           * (0x01 << 0x07))), \
 /* Byte 13 */  \
((ZRCmdRC_SelectMediaFunction_Supported       * (0x01 << 0x00)) | \
 (ZRCmdRC_SelectAVInputFunction_Supported     * (0x01 << 0x01)) | \
 (ZRCmdRC_SelectAudioInputFunction_Supported  * (0x01 << 0x02)) | \
 (ZRCmdRC_PowerToggleFunction_Supported       * (0x01 << 0x03)) | \
 (ZRCmdRC_PowerOffFunction_Supported          * (0x01 << 0x04)) | \
 (ZRCmdRC_PowerOnFunction_Supported           * (0x01 << 0x05))), \
 /* Byte 14 */  \
((ZRCmdRC_F1_Supported         * (0x01 << 0x01)) | \
 (ZRCmdRC_F2_Supported         * (0x01 << 0x02)) | \
 (ZRCmdRC_F3_Supported         * (0x01 << 0x03)) | \
 (ZRCmdRC_F4_Supported         * (0x01 << 0x04)) | \
 (ZRCmdRC_F5_Supported         * (0x01 << 0x05)) | \
 (ZRCmdRC_Data_Supported       * (0x01 << 0x06))), \
 /* Rest of bytes */ \
 0x00, 0x00, 0x00, 0x00, 0x00,0x00, 0x00, 0x00,\
 0x00, 0x00, 0x00,0x00, 0x00, 0x00, 0x00, 0x00, 0x00 \
 }                                                      

#endif /* _ZRCOMMANDS_H_ */
