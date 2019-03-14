/**
 * @brief  人体リグ用.
 * @date   2014.08.05 - 2014.08.05.
 */

#include "RigCtrl.h"
#include "BoneUtil.h"

/**
 * HumanIK
 */
// http://area.autodesk.jp/column/trend_tech/nextmedia_m_and_e/simple_human_ik/
// http://download.autodesk.com/global/docs/motionbuilder2013/ja_jp/index.html?url=files/GUID-A900A0BA-A1A6-4707-A6AE-26FE8F49BEC6.htm,topicNumber=d30e110514
// http://knowledge.autodesk.com/support/maya/getting-started/caas/mne-help/us/maya/Maya_2014_GettingStarted_jpn/files/GUID-5CA619C2-2F7F-4A39-A452-FC3427608C53-htm.html
// http://www.slideshare.net/nyaakobayashi/bd-maya-ltcharaanime131128
// Hips から開始される.


namespace {

// ----------------------------------------------------.
// 人体リグのボーン情報（Maya LT 2015/HumanIK）.
// 1700mmの身長を基準.
// 「xxxx_Hips」というのがボーンノード名。xxxxには任意の形状名が入る.
// ----------------------------------------------------.
RIG_BONE_INFO rigHumanIKBoneInfo [] = {
	RIG_BONE_INFO(  0,   -1,   -1, "Hips"             , "" , "" , sxsdk::vec3(      0,   928.0f,       0)),

	RIG_BONE_INFO(  1,    0,   -1, "LeftUpReg"        , "" , "" , sxsdk::vec3(  80.0f,   874.0f,       0)),
	RIG_BONE_INFO(  2,    1,   -1, "LeftLeg"          , "" , "" , sxsdk::vec3(  82.0f,   457.0f,    8.0f)),
	RIG_BONE_INFO(  3,    2,   -1, "LeftFoot"         , "" , "" , sxsdk::vec3(  81.0f,    78.0f,       0)),
	RIG_BONE_INFO(  4,    3,   -1, "LeftToeBase"      , "" , "" , sxsdk::vec3(  81.0f,    24.0f,  125.0f)),

	RIG_BONE_INFO(  5,    0,   -1, "RightUpReg"       , "" , "" , sxsdk::vec3( -80.0f,   874.0f,       0)),
	RIG_BONE_INFO(  6,    5,   -1, "RightLeg"         , "" , "" , sxsdk::vec3( -82.0f,   457.0f,    8.0f)),
	RIG_BONE_INFO(  7,    6,   -1, "RightFoot"        , "" , "" , sxsdk::vec3( -81.0f,    78.0f,       0)),
	RIG_BONE_INFO(  8,    7,   -1, "RightToeBase"     , "" , "" , sxsdk::vec3( -81.0f,    24.0f,  125.0f)),

	RIG_BONE_INFO(  9,    0,   -1, "Spine"            , "" , "" , sxsdk::vec3(   0.0f,  1000.0f,    0.0f)),
	RIG_BONE_INFO( 10,    9,   -1, "Spine1"           , "" , "" , sxsdk::vec3(   0.0f,  1109.0f,    0.0f)),
	RIG_BONE_INFO( 11,   10,   -1, "Spine2"           , "" , "" , sxsdk::vec3(   0.0f,  1239.0f,    0.0f)),
	RIG_BONE_INFO( 12,   11,   -1, "Neck"             , "" , "" , sxsdk::vec3(   0.0f,  1349.0f,    0.0f)),
	RIG_BONE_INFO( 13,   12,   -1, "Head"             , "" , "" , sxsdk::vec3(   0.0f,  1529.0f,   47.0f)),

	RIG_BONE_INFO( 14,   11,   -1, "LeftShoulder"     , "" , "" , sxsdk::vec3(  60.0f,  1361.0f,    0.0f)),
	RIG_BONE_INFO( 15,   14,   -1, "LeftArm"          , "" , "" , sxsdk::vec3( 161.0f,  1361.0f,    0.0f)),
	RIG_BONE_INFO( 16,   15,   -1, "LeftForeArm"      , "" , "" , sxsdk::vec3( 413.0f,  1361.0f,    0.0f)),
	RIG_BONE_INFO( 17,   16,   -1, "LeftHand"         , "" , "" , sxsdk::vec3( 657.0f,  1361.0f,    0.0f)),

	RIG_BONE_INFO( 18,   17,   -1, "LeftHandThumb1"   , "" , "" , sxsdk::vec3( 702.0f,  1361.0f,   50.0f)),
	RIG_BONE_INFO( 19,   18,   -1, "LeftHandThumb2"   , "" , "" , sxsdk::vec3( 727.0f,  1354.0f,   56.0f)),
	RIG_BONE_INFO( 20,   19,   -1, "LeftHandThumb3"   , "" , "" , sxsdk::vec3( 751.0f,  1354.0f,   57.0f)),
	RIG_BONE_INFO( 21,   20,   -1, "LeftHandThumb4"   , "" , "" , sxsdk::vec3( 777.0f,  1354.0f,   57.0f)),

	RIG_BONE_INFO( 22,   17,   -1, "LeftHandIndex1"   , "" , "" , sxsdk::vec3( 747.0f,  1361.0f,   33.0f)),
	RIG_BONE_INFO( 23,   22,   -1, "LeftHandIndex2"   , "" , "" , sxsdk::vec3( 790.0f,  1361.0f,   33.0f)),
	RIG_BONE_INFO( 24,   23,   -1, "LeftHandIndex3"   , "" , "" , sxsdk::vec3( 816.0f,  1361.0f,   35.0f)),
	RIG_BONE_INFO( 25,   24,   -1, "LeftHandIndex4"   , "" , "" , sxsdk::vec3( 838.0f,  1361.0f,   34.0f)),

	RIG_BONE_INFO( 26,   17,   -1, "LeftHandMiddle1"  , "" , "" , sxsdk::vec3( 746.0f,  1361.0f,   11.0f)),
	RIG_BONE_INFO( 27,   26,   -1, "LeftHandMiddle2"  , "" , "" , sxsdk::vec3( 798.0f,  1361.0f,   11.0f)),
	RIG_BONE_INFO( 28,   27,   -1, "LeftHandMiddle3"  , "" , "" , sxsdk::vec3( 826.0f,  1361.0f,   11.0f)),
	RIG_BONE_INFO( 29,   28,   -1, "LeftHandMiddle4"  , "" , "" , sxsdk::vec3( 847.0f,  1361.0f,   10.0f)),

	RIG_BONE_INFO( 30,   17,   -1, "LeftHandRing1"    , "" , "" , sxsdk::vec3( 747.0f,  1361.0f,  -11.0f)),
	RIG_BONE_INFO( 31,   30,   -1, "LeftHandRing2"    , "" , "" , sxsdk::vec3( 793.0f,  1361.0f,  -11.0f)),
	RIG_BONE_INFO( 32,   31,   -1, "LeftHandRing3"    , "" , "" , sxsdk::vec3( 817.0f,  1361.0f,  -11.0f)),
	RIG_BONE_INFO( 33,   32,   -1, "LeftHandRing4"    , "" , "" , sxsdk::vec3( 835.0f,  1361.0f,  -11.0f)),

	RIG_BONE_INFO( 34,   17,   -1, "LeftHandPinky1"   , "" , "" , sxsdk::vec3( 746.0f,  1361.0f,  -26.0f)),
	RIG_BONE_INFO( 35,   34,   -1, "LeftHandPinky2"   , "" , "" , sxsdk::vec3( 776.0f,  1361.0f,  -26.0f)),
	RIG_BONE_INFO( 36,   35,   -1, "LeftHandPinky3"   , "" , "" , sxsdk::vec3( 795.0f,  1361.0f,  -26.0f)),
	RIG_BONE_INFO( 37,   36,   -1, "LeftHandPinky4"   , "" , "" , sxsdk::vec3( 812.0f,  1361.0f,  -26.0f)),

	RIG_BONE_INFO( 38,   11,   -1, "RightShoulder"    , "" , "" , sxsdk::vec3( -60.0f,  1361.0f,    0.0f)),
	RIG_BONE_INFO( 39,   38,   -1, "RightArm"         , "" , "" , sxsdk::vec3(-161.0f,  1361.0f,    0.0f)),
	RIG_BONE_INFO( 40,   39,   -1, "RightForeArm"     , "" , "" , sxsdk::vec3(-413.0f,  1361.0f,    0.0f)),
	RIG_BONE_INFO( 41,   40,   -1, "RightHand"        , "" , "" , sxsdk::vec3(-657.0f,  1361.0f,    0.0f)),

	RIG_BONE_INFO( 42,   41,   -1, "RightHandThumb1"  , "" , "" , sxsdk::vec3(-702.0f,  1361.0f,   50.0f)),
	RIG_BONE_INFO( 43,   42,   -1, "RightHandThumb2"  , "" , "" , sxsdk::vec3(-727.0f,  1354.0f,   56.0f)),
	RIG_BONE_INFO( 44,   43,   -1, "RightHandThumb3"  , "" , "" , sxsdk::vec3(-751.0f,  1354.0f,   57.0f)),
	RIG_BONE_INFO( 45,   44,   -1, "RightHandThumb4"  , "" , "" , sxsdk::vec3(-777.0f,  1354.0f,   57.0f)),

	RIG_BONE_INFO( 46,   41,   -1, "RightHandIndex1"  , "" , "" , sxsdk::vec3(-747.0f,  1361.0f,   33.0f)),
	RIG_BONE_INFO( 47,   46,   -1, "RightHandIndex2"  , "" , "" , sxsdk::vec3(-790.0f,  1361.0f,   33.0f)),
	RIG_BONE_INFO( 48,   47,   -1, "RightHandIndex3"  , "" , "" , sxsdk::vec3(-816.0f,  1361.0f,   35.0f)),
	RIG_BONE_INFO( 49,   48,   -1, "RightHandIndex4"  , "" , "" , sxsdk::vec3(-838.0f,  1361.0f,   34.0f)),

	RIG_BONE_INFO( 50,   41,   -1, "RightHandMiddle1" , "" , "" , sxsdk::vec3(-746.0f,  1361.0f,   11.0f)),
	RIG_BONE_INFO( 51,   50,   -1, "RightHandMiddle2" , "" , "" , sxsdk::vec3(-798.0f,  1361.0f,   11.0f)),
	RIG_BONE_INFO( 52,   51,   -1, "RightHandMiddle3" , "" , "" , sxsdk::vec3(-826.0f,  1361.0f,   11.0f)),
	RIG_BONE_INFO( 53,   52,   -1, "RightHandMiddle4" , "" , "" , sxsdk::vec3(-847.0f,  1361.0f,   10.0f)),

	RIG_BONE_INFO( 54,   41,   -1, "RightHandRing1"   , "" , "" , sxsdk::vec3(-747.0f,  1361.0f,  -11.0f)),
	RIG_BONE_INFO( 55,   54,   -1, "RightHandRing2"   , "" , "" , sxsdk::vec3(-793.0f,  1361.0f,  -11.0f)),
	RIG_BONE_INFO( 56,   55,   -1, "RightHandRing3"   , "" , "" , sxsdk::vec3(-817.0f,  1361.0f,  -11.0f)),
	RIG_BONE_INFO( 57,   56,   -1, "RightHandRing4"   , "" , "" , sxsdk::vec3(-835.0f,  1361.0f,  -11.0f)),

	RIG_BONE_INFO( 58,   41,   -1, "RightHandPinky1"  , "" , "" , sxsdk::vec3(-746.0f,  1361.0f,  -26.0f)),
	RIG_BONE_INFO( 59,   58,   -1, "RightHandPinky2"  , "" , "" , sxsdk::vec3(-776.0f,  1361.0f,  -26.0f)),
	RIG_BONE_INFO( 60,   59,   -1, "RightHandPinky3"  , "" , "" , sxsdk::vec3(-795.0f,  1361.0f,  -26.0f)),
	RIG_BONE_INFO( 61,   60,   -1, "RightHandPinky4"  , "" , "" , sxsdk::vec3(-812.0f,  1361.0f,  -26.0f)),

	RIG_BONE_INFO( -1,   -1,   -1, ""                 , "" , "")
};

// ----------------------------------------------------.
// 人体リグのボーン情報（MMDの初音ミクモデル、A-Pose）.
// ----------------------------------------------------.
RIG_BONE_INFO rigBoneInfo [] = {
	RIG_BONE_INFO(  0,   -1,    1, "センター"      , "center"          , "center"               , sxsdk::vec3(      0,   725.0f,       0)),
	RIG_BONE_INFO(  1,    0,   -1, "センター先"    , "center2"         , "center2"              , sxsdk::vec3(      0,   325.0f,       0)),

	RIG_BONE_INFO(  2,    0,    3, "上半身"        , "upper body"      , "upper_body"           , sxsdk::vec3(      0,  1175.0f,       0)),

	RIG_BONE_INFO(  3,    2,    4, "首"            , "neck"            , "neck"                 , sxsdk::vec3(      0,  1383.0f,       0)),
	RIG_BONE_INFO(  4,    3,    5, "頭"            , "head"            , "head"                 , sxsdk::vec3(      0,  1443.0f,       0)),
	RIG_BONE_INFO(  5,    4,   -1, "頭先"          , "head2"           , "head2"                , sxsdk::vec3(      0,  1564.0f,   20.0f)),

	RIG_BONE_INFO(  6,    4,    7, "左目"          , "eye_L"           , "eye_left"             , sxsdk::vec3(  51.0f,  1529.0f,   65.0f)),
	RIG_BONE_INFO(  7,    6,   -1, "左目先"        , "eye_L2"          , "eye_left2"            , sxsdk::vec3(  51.0f,  1529.0f,   92.0f)),

	RIG_BONE_INFO(  8,    4,    9, "右目"          , "eye_R"           , "eye_right"            , sxsdk::vec3( -51.0f,  1529.0f,   65.0f)),
	RIG_BONE_INFO(  9,    8,   -1, "右目先"        , "eye_R2"          , "eye_right2"           , sxsdk::vec3( -51.0f,  1529.0f,   92.0f)),

	RIG_BONE_INFO( 10,    0,   11, "下半身"        , "lower body"      , "lower_body"           , sxsdk::vec3(      0,  1175.0f,       0)),
	RIG_BONE_INFO( 11,   10,   -1, "下半身先"      , "lower body2"     , "lower_body2"          , sxsdk::vec3(      0,   960.0f,       0)),

	RIG_BONE_INFO( 12,    2,   13, "左肩"          , "shoulder_L"      , "shoulder_left"        , sxsdk::vec3(  33.0f,  1370.0f,       0)),
	RIG_BONE_INFO( 13,   12,   14, "左腕"          , "arm_L"           , "arm_left"             , sxsdk::vec3( 102.0f,  1340.0f,       0)),
	RIG_BONE_INFO( 14,   13,   15, "左ひじ"        , "elbow_L"         , "elbow_left"           , sxsdk::vec3( 284.0f,  1216.0f,  -19.0f)),
	RIG_BONE_INFO( 15,   14,   16, "左手首"        , "wrist_L"         , "wrist_left"           , sxsdk::vec3( 432.0f,  1094.0f,  -19.0f)),
	RIG_BONE_INFO( 16,   15,   -1, "左手先"        , "wrist_L2"        , "wrist_left2"          , sxsdk::vec3( 489.0f,  1054.0f,  -19.0f)),
	RIG_BONE_INFO( 17,   15,   18, "左親指１"      , "thumb1_L"        , "thumb1_left"          , sxsdk::vec3( 480.0f,  1086.0f,   43.0f)),
	RIG_BONE_INFO( 18,   17,   19, "左親指２"      , "thumb2_L"        , "thumb2_left"          , sxsdk::vec3( 514.0f,  1063.0f,   74.0f)),
	RIG_BONE_INFO( 19,   18,   -1, "左親指先"      , "thumb3_L"        , "thumb3_left"          , sxsdk::vec3( 534.0f,  1043.0f,   93.0f)),
	RIG_BONE_INFO( 20,   15,   21, "左人指１"      , "fore1_L"         , "fore1_left"           , sxsdk::vec3( 507.0f,  1075.0f,    8.0f)),
	RIG_BONE_INFO( 21,   20,   22, "左人指２"      , "fore2_L"         , "fore2_left"           , sxsdk::vec3( 540.0f,  1053.0f,   21.0f)),
	RIG_BONE_INFO( 22,   21,   23, "左人指３"      , "fore3_L"         , "fore3_left"           , sxsdk::vec3( 588.0f,  1026.0f,   35.0f)),
	RIG_BONE_INFO( 23,   22,   -1, "左人指先"      , "fore4_L"         , "fore4_left"           , sxsdk::vec3( 615.0f,  1002.0f,   41.0f)),
	RIG_BONE_INFO( 24,   15,   25, "左中指１"      , "middle1_L"       , "middle1_left"         , sxsdk::vec3( 510.0f,  1076.0f,  -33.0f)),
	RIG_BONE_INFO( 25,   24,   26, "左中指２"      , "middle2_L"       , "middle2_left"         , sxsdk::vec3( 544.0f,  1052.0f,  -34.0f)),
	RIG_BONE_INFO( 26,   25,   27, "左中指３"      , "middle3_L"       , "middle3_left"         , sxsdk::vec3( 602.0f,  1020.0f,  -33.0f)),
	RIG_BONE_INFO( 27,   26,   -1, "左中指先"      , "middle4_L"       , "middle4_left"         , sxsdk::vec3( 646.0f,  1008.0f,  -33.0f)),
	RIG_BONE_INFO( 28,   15,   29, "左薬指１"      , "third1_L"        , "third1_left"          , sxsdk::vec3( 512.0f,  1078.0f,  -69.0f)),
	RIG_BONE_INFO( 29,   28,   30, "左薬指２"      , "third2_L"        , "third2_left"          , sxsdk::vec3( 546.0f,  1052.0f,  -73.0f)),
	RIG_BONE_INFO( 30,   29,   31, "左薬指３"      , "third3_L"        , "third3_left"          , sxsdk::vec3( 595.0f,  1022.0f,  -74.0f)),
	RIG_BONE_INFO( 31,   30,   -1, "左薬指先"      , "third4_L"        , "third4_left"          , sxsdk::vec3( 628.0f,   997.0f,  -75.0f)),
	RIG_BONE_INFO( 32,   15,   33, "左小指１"      , "little1_L"       , "little1_left"         , sxsdk::vec3( 497.0f,  1082.0f, -108.0f)),
	RIG_BONE_INFO( 33,   32,   34, "左小指２"      , "little2_L"       , "little2_left"         , sxsdk::vec3( 529.0f,  1058.0f, -115.0f)),
	RIG_BONE_INFO( 34,   33,   35, "左小指３"      , "little3_L"       , "little3_left"         , sxsdk::vec3( 572.0f,  1032.0f, -117.0f)),
	RIG_BONE_INFO( 35,   34,   -1, "左小指先"      , "little4_L"       , "little4_left"         , sxsdk::vec3( 607.0f,  1010.0f, -117.0f)),

	RIG_BONE_INFO( 36,   10,   37, "左足"          , "leg_L"           , "leg_left"             , sxsdk::vec3(  70.0f,   943.0f,       0)),
	RIG_BONE_INFO( 37,   36,   38, "左ひざ"        , "knee_L"          , "knee_left"            , sxsdk::vec3(  70.0f,   547.0f,   27.0f)),
	RIG_BONE_INFO( 38,   37,   39, "左足首"        , "ankle_L"         , "ankle_left"           , sxsdk::vec3(  70.0f,    91.0f,    9.0f)),
	RIG_BONE_INFO( 39,   38,   -1, "左つま先"      , "ankle_L2"        , "ankle_left2"          , sxsdk::vec3(  70.0f,    30.0f,  200.0f)),

	RIG_BONE_INFO( 40,    2,   41, "右肩"          , "shoulder_R"      , "shoulder_right"       , sxsdk::vec3( -33.0f,  1370.0f,       0)),
	RIG_BONE_INFO( 41,   40,   42, "右腕"          , "arm_R"           , "arm_right"            , sxsdk::vec3(-102.0f,  1340.0f,       0)),
	RIG_BONE_INFO( 42,   41,   43, "右ひじ"        , "elbow_R"         , "elbow_right"          , sxsdk::vec3(-284.0f,  1216.0f,  -19.0f)),       
	RIG_BONE_INFO( 43,   42,   44, "右手首"        , "wrist_R"         , "wrist_right"          , sxsdk::vec3(-432.0f,  1094.0f,  -19.0f)),
	RIG_BONE_INFO( 44,   43,   -1, "右手先"        , "wrist_R2"        , "wrist_right2"         , sxsdk::vec3(-489.0f,  1054.0f,  -19.0f)),
	RIG_BONE_INFO( 45,   43,   46, "右親指１"      , "thumb1_R"        , "thumb1_right"         , sxsdk::vec3(-480.0f,  1086.0f,   43.0f)),
	RIG_BONE_INFO( 46,   45,   47, "右親指２"      , "thumb2_R"        , "thumb2_right"         , sxsdk::vec3(-514.0f,  1063.0f,   74.0f)),
	RIG_BONE_INFO( 47,   46,   -1, "右親指先"      , "thumb3_R"        , "thumb3_right"         , sxsdk::vec3(-534.0f,  1043.0f,   93.0f)),
	RIG_BONE_INFO( 48,   43,   49, "右人指１"      , "fore1_R"         , "fore1_right"          , sxsdk::vec3(-507.0f,  1075.0f,    8.0f)),
	RIG_BONE_INFO( 49,   48,   50, "右人指２"      , "fore2_R"         , "fore2_right"          , sxsdk::vec3(-540.0f,  1053.0f,   21.0f)),
	RIG_BONE_INFO( 50,   49,   51, "右人指３"      , "fore3_R"         , "fore3_right"          , sxsdk::vec3(-588.0f,  1026.0f,   35.0f)),
	RIG_BONE_INFO( 51,   50,   -1, "右人指先"      , "fore4_R"         , "fore4_right"          , sxsdk::vec3(-615.0f,  1002.0f,   41.0f)),
	RIG_BONE_INFO( 52,   43,   53, "右中指１"      , "middle1_R"       , "middle1_right"        , sxsdk::vec3(-510.0f,  1076.0f,  -33.0f)),
	RIG_BONE_INFO( 53,   52,   54, "右中指２"      , "middle2_R"       , "middle2_right"        , sxsdk::vec3(-544.0f,  1052.0f,  -34.0f)),
	RIG_BONE_INFO( 54,   53,   55, "右中指３"      , "middle3_R"       , "middle3_right"        , sxsdk::vec3(-602.0f,  1020.0f,  -33.0f)),
	RIG_BONE_INFO( 55,   54,   -1, "右中指先"      , "middle4_R"       , "middle4_right"        , sxsdk::vec3(-646.0f,  1008.0f,  -33.0f)),
	RIG_BONE_INFO( 56,   43,   57, "右薬指１"      , "third1_R"        , "third1_right"         , sxsdk::vec3(-512.0f,  1078.0f,  -69.0f)),
	RIG_BONE_INFO( 57,   56,   58, "右薬指２"      , "third2_R"        , "third2_right"         , sxsdk::vec3(-546.0f,  1052.0f,  -73.0f)),
	RIG_BONE_INFO( 58,   57,   59, "右薬指３"      , "third3_R"        , "third3_right"         , sxsdk::vec3(-595.0f,  1022.0f,  -74.0f)),
	RIG_BONE_INFO( 59,   58,   -1, "右薬指先"      , "third4_R"        , "third4_right"         , sxsdk::vec3(-628.0f,   997.0f,  -75.0f)),
	RIG_BONE_INFO( 60,   43,   61, "右小指１"      , "little1_R"       , "little1_right"        , sxsdk::vec3(-497.0f,  1082.0f, -108.0f)),
	RIG_BONE_INFO( 61,   60,   62, "右小指２"      , "little2_R"       , "little2_right"        , sxsdk::vec3(-529.0f,  1058.0f, -115.0f)),
	RIG_BONE_INFO( 62,   61,   63, "右小指３"      , "little3_R"       , "little3_right"        , sxsdk::vec3(-572.0f,  1032.0f, -117.0f)),
	RIG_BONE_INFO( 63,   62,   -1, "右小指先"      , "little4_R"       , "little4_right"        , sxsdk::vec3(-607.0f,  1010.0f, -117.0f)),

	RIG_BONE_INFO( 64,   10,   65, "右足"          , "leg_R"           , "leg_right"            , sxsdk::vec3( -70.0f,   943.0f,       0)),
	RIG_BONE_INFO( 65,   64,   66, "右ひざ"        , "knee_R"          , "knee_right"           , sxsdk::vec3( -70.0f,   547.0f,   27.0f)),
	RIG_BONE_INFO( 66,   65,   67, "右足首"        , "ankle_R"         , "ankle_right"          , sxsdk::vec3( -70.0f,    91.0f,    9.0f)),
	RIG_BONE_INFO( 67,   66,   -1, "右つま先"      , "ankle_R2"        , "ankle_right2"         , sxsdk::vec3( -70.0f,    30.0f,  200.0f)),

	RIG_BONE_INFO( -1,   -1,   -1, ""              , ""                , "")
};

}

CRigCtrl::CRigCtrl(sxsdk::shade_interface *shade) {
	m_shade = shade;
}

CRigCtrl::~CRigCtrl() {
}

/**
 * リグ構造のボーン群を作成.
 * 現在のリグ構成より、ボーンの位置も調整して配置する.
 */
void CRigCtrl::CreateMMDHumanRig(sxsdk::scene_interface* scene, const int type, const bool create_legs_ik)
{
	// 現在のリグ（球で表現される）の制御点位置を取得.
	CRigPoint rigPoint(human_pose_T);
	UpdateRigPoints(scene, rigPoint);

	// 再配置したボーン位置を返す.
	std::vector<RIG_BONE_INFO> bonesInfo = m_CalcBonesPos(rigPoint, false);

	try {
		sxsdk::mat4 lwMat = scene->active_shape().get_local_to_world_matrix();

		scene->begin_creating();
		m_CreateMMDHumanRigLoop(0, 0, scene, type, &lwMat, bonesInfo);
		scene->end_creating();
	} catch (...) { }

	// ボーン位置より、IKを割り当て.
	if (create_legs_ik) {
		sxsdk::vec3 AnkleLPos     = bonesInfo[38].pos;		// 「左足首」のワールド位置.
		sxsdk::vec3 AnkleLTailPos = bonesInfo[39].pos;		// 「左つま先」のワールド位置.
		sxsdk::vec3 AnkleRPos     = bonesInfo[66].pos;		// 「右足首」のワールド位置.
		sxsdk::vec3 AnkleRTailPos = bonesInfo[67].pos;		// 「右つま先」のワールド位置.

		// 接地で乱れないように少しずらす（MMDのリグ構成をまねた）.
		const float zureDist = sxsdk::absolute(AnkleLTailPos - AnkleLPos) * 0.6f;
		AnkleLPos.z     -= zureDist;
		AnkleLTailPos.y -= zureDist;
		AnkleRPos.z     -= zureDist;
		AnkleRTailPos.y -= zureDist;

		// ゴールに相当するボールジョイントを生成.
		scene->begin_creating();
		std::vector<sxsdk::shape_class *> LegL_Goal = m_CreateIKGoalBallJoints(scene, AnkleLPos, "legL_goal", AnkleLTailPos, "legL2_goal");
		std::vector<sxsdk::shape_class *> LegR_Goal = m_CreateIKGoalBallJoints(scene, AnkleRPos, "legR_goal", AnkleRTailPos, "legR2_goal");
		scene->end_creating();

		// IKの割り当て.
		sxsdk::ik_class& ik = scene->get_ik();
		ik.set_ik(bonesInfo[36].pShape, bonesInfo[38].pShape, LegL_Goal[0]);
		ik.set_ik(bonesInfo[38].pShape, bonesInfo[39].pShape, LegL_Goal[1]);
		ik.set_ik(bonesInfo[64].pShape, bonesInfo[66].pShape, LegR_Goal[0]);
		ik.set_ik(bonesInfo[66].pShape, bonesInfo[67].pShape, LegR_Goal[1]);
	}

	// ボーンのルート形状を選択.
	if (bonesInfo.size() > 0) scene->select(bonesInfo[0].pShape);
}

void CRigCtrl::m_CreateMMDHumanRigLoop(const int depth, const int index, sxsdk::scene_interface* scene, const int type, sxsdk::mat4* pParentLWMat, std::vector<RIG_BONE_INFO>& bonesInfo)
{
	RIG_BONE_INFO& curInfo = bonesInfo[index];

	sxsdk::vec3 pos(0, 0, 0);
	sxsdk::vec3 dir(1, 0, 0);
	float size = 15.0f;
	std::string name = curInfo.name_default;
	if (type == human_rig_type_mmd_jp) name = BoneUtil::GetUTF8Text(*m_shade, curInfo.name_jp);
	else if (type == human_rig_type_mmd_en) name = curInfo.name_en;

	sxsdk::mat4 lwMat = sxsdk::mat4::identity;
	if (pParentLWMat) lwMat = *pParentLWMat;
	pos = curInfo.pos * inv(lwMat);

	curInfo.pShape = &(scene->begin_bone_joint(pos, size, false, dir, name.c_str()));

	sxsdk::mat4 lwMat2 = sxsdk::mat4::translate(pos) * lwMat;

	// tailボーンを生成.
	if (curInfo.tail_bone_index >= 0) {
		m_CreateMMDHumanRigLoop(depth + 1, curInfo.tail_bone_index, scene, type, &lwMat2, bonesInfo);
	}

	// 親ボーンがindexのものを生成.
	for (int i = 0; i < bonesInfo.size(); i++) {
		RIG_BONE_INFO& info = bonesInfo[i];
		if (info.bone_index < 0) break;
		if (info.bone_index == index || info.bone_index == curInfo.tail_bone_index) continue;
		if (info.parent_bone_index == index) {
			m_CreateMMDHumanRigLoop(depth + 1, info.bone_index, scene, type, &lwMat2, bonesInfo);
		}
	}

	scene->end_bone_joint();
}

/**
 * HumanIK向けのリグ構造のボーン群を作成.
 */
void CRigCtrl::CreateHumanIKRig(sxsdk::scene_interface* scene, const std::string name, const bool createEyes, const bool createJaw, const bool create_legs_ik)
{
	// 現在のリグ（球で表現される）の制御点位置を取得.
	CRigPoint rigPoint(human_pose_T);
	UpdateRigPoints(scene, rigPoint);

	// 再配置したボーン位置を返す.
	std::vector<RIG_BONE_INFO> bonesInfo = m_CalcBonesPos(rigPoint, true, createEyes, createJaw);

	try {
		sxsdk::mat4 lwMat = scene->active_shape().get_local_to_world_matrix();

		scene->begin_creating();
		m_CreateHumanIKRigLoop(0, 0, scene, name, &lwMat, bonesInfo);
		scene->end_creating();
	} catch (...) { }

	// ボーンのルート形状を選択.
	if (bonesInfo.size() > 0) scene->select(bonesInfo[0].pShape);
}

void CRigCtrl::m_CreateHumanIKRigLoop(const int depth, const int index, sxsdk::scene_interface* scene, const std::string name, sxsdk::mat4* pParentLWMat, std::vector<RIG_BONE_INFO>& bonesInfo)
{
	RIG_BONE_INFO& curInfo = bonesInfo[index];

	sxsdk::vec3 pos(0, 0, 0);
	sxsdk::vec3 dir(1, 0, 0);
	float size = 15.0f;
	std::string name2 = (name.length() > 0) ? (name + std::string("_") + curInfo.name_jp) : curInfo.name_jp;

	sxsdk::mat4 lwMat = sxsdk::mat4::identity;
	if (pParentLWMat) lwMat = *pParentLWMat;
	pos = curInfo.pos * inv(lwMat);

	curInfo.pShape = &(scene->begin_bone_joint(pos, size, false, dir, name2.c_str()));

	sxsdk::mat4 lwMat2 = sxsdk::mat4::translate(pos) * lwMat;

	// 親ボーンがindexのものを生成.
	for (int i = 0; i < bonesInfo.size(); i++) {
		RIG_BONE_INFO& info = bonesInfo[i];
		if (info.bone_index < 0) break;
		if (info.bone_index == index || info.bone_index == curInfo.tail_bone_index) continue;
		if (info.parent_bone_index == index) {
			m_CreateHumanIKRigLoop(depth + 1, info.bone_index, scene, name, &lwMat2, bonesInfo);
		}
	}

	scene->end_bone_joint();
}

/**
 * 指定の位置に、IK goal用のボールジョイントを生成.
 */
std::vector<sxsdk::shape_class *> CRigCtrl::m_CreateIKGoalBallJoints(sxsdk::scene_interface* scene, const sxsdk::vec3 wPos0, const std::string name0, const sxsdk::vec3 wPos1, const std::string name1)
{
	const sxsdk::mat4 lwMat = scene->active_shape().get_local_to_world_matrix();
	const float size = 15.0f;

	sxsdk::vec3 pos0 = wPos0 * inv(lwMat);
	sxsdk::vec3 pos1 = wPos1 * inv(lwMat);

	sxsdk::shape_class* pShape0 = &(scene->begin_ball_joint(pos0, size, name0.c_str()));
	sxsdk::shape_class* pShape1 = &(scene->begin_ball_joint(pos1, size, name1.c_str()));
	scene->end_bone_joint();
	scene->end_bone_joint();

	std::vector<sxsdk::shape_class *> retS;
	retS.push_back(pShape0);
	retS.push_back(pShape1);

	return retS;
}

/**
 * 基準となる制御点の位置を指定する形状を作成.
 * ここで生成した構成間をワイヤーフレームで結んで、人体っぽくみせては。.
 * @param[in]  scene   scene_interfaceのポインタ.
 * @param[in]  pose    TポーズまたはAポーズ (human_pose_T/human_pose_A_30/human_pose_A_45).
 * @param[in]  height  身長.
 * @param[in]  mirror  ミラーリング操作に対応するか.
 */
void CRigCtrl::InitializeRigPonts(sxsdk::scene_interface* scene, const int pose, const float height, const bool mirror)
{
	CRigPoint rigPoint(pose);
	const float r = 20.0f;

	scene->begin_creating();
	scene->begin_part(RIGPOINT_PART_NAME);

	const sxsdk::mat4 lwMat  = scene->active_shape().get_local_to_world_matrix();

	for (int i = 0; i < rigPoint.rigPoint.size(); i++) {
		const sxsdk::vec3 pos = rigPoint.GetRigPoint(i, height) * inv(lwMat);
		const std::string name = BoneUtil::GetUTF8Text(*m_shade, rigPoint.rigPoint[i].name);
		sxsdk::sphere_class& sphere = scene->create_sphere(name.c_str(), pos, r);

		// リグの制御点番号をstreamに保持.
		try {
			compointer<sxsdk::stream_interface> stream(sphere.create_attribute_stream_interface_with_uuid(RIG_POINT_STERAM_ID));
			stream->set_pointer(0);
			stream->set_size(0);
			stream->write_int(i);

			int iDat = mirror ? 1 : 0;
			stream->write_int(iDat);
		} catch (...) { }
	}

	scene->end_part();
	scene->end_creating();
}

/**
 * シーンより、RigPointsパート内の球の位置で再取得.
 */
bool CRigCtrl::UpdateRigPoints(sxsdk::scene_interface* scene, CRigPoint& rigPoint)
{
	sxsdk::shape_class* pTargetPart = scene->get_shape_by_name(RIGPOINT_PART_NAME);
	if (pTargetPart == NULL) return false;

	return UpdateRigPointsInShape(pTargetPart, rigPoint);
}

bool CRigCtrl::UpdateRigPointsInShape(sxsdk::shape_class* pRigPointsPart, CRigPoint& rigPoint)
{
	if (pRigPointsPart->has_son()) {
		sxsdk::shape_class* pShape = pRigPointsPart->get_son();
		while (pShape->has_bro()) {
			pShape = pShape->get_bro();
			const int index = rigPoint.GetIndex(pShape->get_name());
			if (index >= 0 && pShape->get_type() == sxsdk::enums::sphere) {
				const sxsdk::mat4 lwMat  = pShape->get_local_to_world_matrix();
				const sxsdk::vec3 center = pShape->get_sphere().get_center();
				rigPoint.rigPoint[index].pos = center * lwMat;
			}
		}
	}

	return true;
}

/**
 * 制御点位置より、個々のボーンの位置を再配置.
 */
std::vector<RIG_BONE_INFO> CRigCtrl::m_CalcBonesPos(CRigPoint& rigPoint, const bool humanIK, const bool createEyes, const bool createJaw)
{
	std::vector<RIG_BONE_INFO> retBonesInfo;

	//--------------------------------------------.
	// MMDのボーン構造の場合.
	//--------------------------------------------.
	if (!humanIK) {
		// ルート位置の計算と移動。これはhipsと地面を1/3したときの距離.
		{
			const sxsdk::vec3 hipPos   = rigPoint.rigPoint[rigPoint.root].pos;
			const sxsdk::vec3 underPos = (rigPoint.rigPoint[rigPoint.ankle_left].pos + rigPoint.rigPoint[rigPoint.ankle_right].pos) * 0.5f;
			sxsdk::vec3 rootPos  = (hipPos - sxsdk::vec3(hipPos.x, underPos.y, hipPos.z)) * 0.6666f;
			sxsdk::vec3 rootPos2 = (hipPos - sxsdk::vec3(hipPos.x, underPos.y, hipPos.z)) * 0.3333f;
			rootPos.y  += underPos.y;
			rootPos2.y += underPos.y;

			sxsdk::vec3 dV = rootPos - rigBoneInfo[0].pos;
			int iPos = 0;
			while (true) {
				RIG_BONE_INFO dstInfo;
				const RIG_BONE_INFO& bInfo = rigBoneInfo[iPos];
				if (bInfo.bone_index < 0) break;
				dstInfo = bInfo;
				dstInfo.pos = bInfo.pos + dV;
				retBonesInfo.push_back(dstInfo);
				iPos++;
			}
			retBonesInfo[1].pos = rootPos2;			// センター先.
		}

		// upper_body.
		{
			const sxsdk::vec3 upperBodyPos = rigPoint.rigPoint[rigPoint.upper_body].pos;
			retBonesInfo[2].pos = upperBodyPos;											// 上半身.
		}

		// neckより上の再配置.
		{
			const sxsdk::vec3 neckCenter    = rigPoint.rigPoint[rigPoint.neck].pos;
			const sxsdk::vec3 headTail      = rigPoint.rigPoint[rigPoint.head_tail].pos;
			const sxsdk::vec3 shoulderLeft  = rigPoint.rigPoint[rigPoint.shoulder_left].pos;
			const sxsdk::vec3 shoulderRight = rigPoint.rigPoint[rigPoint.shoulder_right].pos;
			const sxsdk::vec3 shoulderC     = (shoulderLeft + shoulderRight) * 0.5f;

			const float dHeight = headTail.y - shoulderC.y;
			const float sHeight = rigBoneInfo[5].pos.y - rigBoneInfo[3].pos.y;		// 頭先 - 首の長さ.
			const float scale   = dHeight / sHeight;
			sxsdk::vec3 dV      = -rigBoneInfo[3].pos;

			for (int i = 5; i <= 9; i++) {											// 首 - 右目先までの移動.
				retBonesInfo[i].pos = shoulderC + (rigBoneInfo[i].pos + dV) * scale;
			}
			retBonesInfo[3].pos = (neckCenter + shoulderC) * 0.5f;					// 首.
			retBonesInfo[4].pos = (neckCenter * 0.8f) + (headTail * 0.2f);			// 頭.
		}

		// lower_body.
		{
			const sxsdk::vec3 hipsPos      = rigPoint.rigPoint[rigPoint.root].pos;
			const sxsdk::vec3 upperBodyPos = rigPoint.rigPoint[rigPoint.upper_body].pos;
			retBonesInfo[10].pos = upperBodyPos;		// 下半身.
			retBonesInfo[11].pos = hipsPos;				// 下半身先.
		}

		// shoulder_leftより先.
		{
			const sxsdk::vec3 shoulderLeft = rigPoint.rigPoint[rigPoint.shoulder_left].pos;
			const sxsdk::vec3 elbowLeft    = rigPoint.rigPoint[rigPoint.elbow_left].pos;
			const sxsdk::vec3 wristLeft    = rigPoint.rigPoint[rigPoint.wrist_left].pos;
			const sxsdk::vec3 armLeft      = (shoulderLeft * 0.75f) + (elbowLeft * 0.25f);
		
			sxsdk::vec3 dFingerV           = rigPoint.rigPoint[rigPoint.finger_tail_left].pos - wristLeft;		// 手の向き.
			const float dFingerLen         = sxsdk::absolute(dFingerV);

			sxsdk::vec3 sFingerV           = rigBoneInfo[27].pos - rigBoneInfo[15].pos;		// 左中指先 - 左手首.
			const float sFingerLen         = sxsdk::absolute(sFingerV);
			const float fingerScale        = dFingerLen / sFingerLen;

			dFingerV = normalize(dFingerV);
			sFingerV = normalize(sFingerV);

			retBonesInfo[12].pos = shoulderLeft;				// 左肩.
			retBonesInfo[13].pos = armLeft;						// 左腕.
			retBonesInfo[14].pos = elbowLeft;					// 左ひじ.
			retBonesInfo[15].pos = wristLeft;					// 左手首.

			// 指は、手首を中心にsFingerVの向きをdFingerVに向ける.
			const sxsdk::mat4 rotM = sxsdk::mat4::rotate(sFingerV, dFingerV);
			for (int i = 16; i <= 35; i++) {
				sxsdk::vec3 pos = (rigBoneInfo[i].pos - rigBoneInfo[15].pos) * fingerScale;
				retBonesInfo[i].pos = (pos * rotM) + wristLeft;
			}
		}

		// shoulder_rightより先.
		{
			const sxsdk::vec3 shoulderRight = rigPoint.rigPoint[rigPoint.shoulder_right].pos;
			const sxsdk::vec3 elbowRight    = rigPoint.rigPoint[rigPoint.elbow_right].pos;
			const sxsdk::vec3 wristRight    = rigPoint.rigPoint[rigPoint.wrist_right].pos;
			const sxsdk::vec3 armRight      = (shoulderRight * 0.75f) + (elbowRight * 0.25f);
		
			sxsdk::vec3 dFingerV           = rigPoint.rigPoint[rigPoint.finger_tail_right].pos - wristRight;		// 手の向き.
			const float dFingerLen         = sxsdk::absolute(dFingerV);

			sxsdk::vec3 sFingerV           = rigBoneInfo[55].pos - rigBoneInfo[43].pos;		// 右中指先 - 右手首.
			const float sFingerLen         = sxsdk::absolute(sFingerV);
			const float fingerScale        = dFingerLen / sFingerLen;

			dFingerV = normalize(dFingerV);
			sFingerV = normalize(sFingerV);

			retBonesInfo[40].pos = shoulderRight;				// 右肩.
			retBonesInfo[41].pos = armRight;					// 右腕.
			retBonesInfo[42].pos = elbowRight;					// 右ひじ.
			retBonesInfo[43].pos = wristRight;					// 右手首.

			// 指は、手首を中心にsFingerVの向きをdFingerVに向ける.
			const sxsdk::mat4 rotM = sxsdk::mat4::rotate(sFingerV, dFingerV);
			for (int i = 44; i <= 63; i++) {
				sxsdk::vec3 pos = (rigBoneInfo[i].pos - rigBoneInfo[43].pos) * fingerScale;
				retBonesInfo[i].pos = (pos * rotM) + wristRight;
			}
		}

		// leg_leftより下.
		{
			const sxsdk::vec3 legLeft   = rigPoint.rigPoint[rigPoint.leg_left].pos;
			const sxsdk::vec3 kneeLeft  = rigPoint.rigPoint[rigPoint.knee_left].pos;
			const sxsdk::vec3 ankleLeft = rigPoint.rigPoint[rigPoint.ankle_left].pos;
			const float scale = sxsdk::absolute(kneeLeft - ankleLeft) / sxsdk::absolute(rigBoneInfo[38].pos - rigBoneInfo[37].pos) * 0.7f;

			retBonesInfo[36].pos = legLeft;					// 左足.
			retBonesInfo[37].pos = kneeLeft;				// 左ひざ.
			retBonesInfo[38].pos = ankleLeft;				// 左足首.
			retBonesInfo[39].pos = (rigBoneInfo[39].pos - rigBoneInfo[38].pos) * scale + ankleLeft;			// 左足首.
		}

		// leg_rightより下.
		{
			const sxsdk::vec3 legRight   = rigPoint.rigPoint[rigPoint.leg_right].pos;
			const sxsdk::vec3 kneeRight  = rigPoint.rigPoint[rigPoint.knee_right].pos;
			const sxsdk::vec3 ankleRight = rigPoint.rigPoint[rigPoint.ankle_right].pos;
			const float scale = sxsdk::absolute(kneeRight - ankleRight) / sxsdk::absolute(rigBoneInfo[66].pos - rigBoneInfo[65].pos) * 0.7f;

			retBonesInfo[64].pos = legRight;				// 右足.
			retBonesInfo[65].pos = kneeRight;				// 右ひざ.
			retBonesInfo[66].pos = ankleRight;				// 右足首.
			retBonesInfo[67].pos = (rigBoneInfo[67].pos - rigBoneInfo[66].pos) * scale + ankleRight;			// 右足首.
		}
	} else {
		//--------------------------------------------.
		// HumanIKのボーン構造の場合.
		//--------------------------------------------.

		// ルート位置(hips)の移動.
		{
			const sxsdk::vec3 rootPos = rigPoint.rigPoint[rigPoint.root].pos;

			sxsdk::vec3 dV = rootPos - rigHumanIKBoneInfo[0].pos;
			int iPos = 0;
			while (true) {
				RIG_BONE_INFO dstInfo;
				const RIG_BONE_INFO& bInfo = rigHumanIKBoneInfo[iPos];
				if (bInfo.bone_index < 0) break;
				dstInfo = bInfo;
				dstInfo.pos = bInfo.pos + dV;
				retBonesInfo.push_back(dstInfo);
				iPos++;
			}
		}

		// 上半身の再配置.
		{
			const sxsdk::vec3 hipsPos      = rigPoint.rigPoint[rigPoint.root].pos;
			const sxsdk::vec3 neckPos      = rigPoint.rigPoint[rigPoint.neck].pos;
			const sxsdk::vec3 upperBodyPos = rigPoint.rigPoint[rigPoint.upper_body].pos;

			const sxsdk::vec3 vD = (upperBodyPos - hipsPos) / 3.0f;
			retBonesInfo[9].pos  = (vD * 0.9f) + hipsPos;								// Spine.
			retBonesInfo[10].pos = (vD * 2.0f) + hipsPos;								// Spine1.
			retBonesInfo[11].pos = upperBodyPos;										// Spine2.
			retBonesInfo[12].pos = (neckPos - upperBodyPos) * 0.7f + upperBodyPos;		// Neck.

			const sxsdk::vec3 headTail = rigPoint.rigPoint[rigPoint.head_tail].pos;

			retBonesInfo[13].pos = (headTail - neckPos) * 0.6f + neckPos;		// Head.
		}

		// 腕の配置.
		{
			const sxsdk::vec3 shoulderPos = rigPoint.rigPoint[rigPoint.shoulder_left].pos;
			const sxsdk::vec3 elbowPos    = rigPoint.rigPoint[rigPoint.elbow_left].pos;
			const sxsdk::vec3 wristPos    = rigPoint.rigPoint[rigPoint.wrist_left].pos;
			const sxsdk::vec3 fingerPos   = rigPoint.rigPoint[rigPoint.finger_tail_left].pos;

			sxsdk::vec3 dFingerV     = fingerPos - wristPos;		// 手の向き.
			const float dFingerLen   = sxsdk::absolute(dFingerV);

			sxsdk::vec3 sFingerV     = rigHumanIKBoneInfo[29].pos - rigHumanIKBoneInfo[17].pos;		// HandMiddle4 - Hand.
			const float sFingerLen   = sxsdk::absolute(sFingerV);
			const float fingerScale  = dFingerLen / sFingerLen;

			dFingerV = normalize(dFingerV);
			sFingerV = normalize(sFingerV);

			retBonesInfo[14].pos = shoulderPos;											// Shoulder.
			retBonesInfo[15].pos = (elbowPos - shoulderPos) * 0.3f + shoulderPos;		// Arm.
			retBonesInfo[16].pos = elbowPos;											// ForeArm.
			retBonesInfo[17].pos = wristPos;											// Hand.

			// 指は、手首を中心にsFingerVの向きをdFingerVに向ける.
			const sxsdk::mat4 rotM = sxsdk::mat4::rotate(sFingerV, dFingerV);
			for (int i = 18; i <= 37; i++) {
				sxsdk::vec3 pos = (rigHumanIKBoneInfo[i].pos - rigHumanIKBoneInfo[17].pos) * fingerScale;
				retBonesInfo[i].pos = (pos * rotM) + wristPos;
			}
		}
		{
			const sxsdk::vec3 shoulderPos = rigPoint.rigPoint[rigPoint.shoulder_right].pos;
			const sxsdk::vec3 elbowPos    = rigPoint.rigPoint[rigPoint.elbow_right].pos;
			const sxsdk::vec3 wristPos    = rigPoint.rigPoint[rigPoint.wrist_right].pos;
			const sxsdk::vec3 fingerPos   = rigPoint.rigPoint[rigPoint.finger_tail_right].pos;

			sxsdk::vec3 dFingerV     = fingerPos - wristPos;		// 手の向き.
			const float dFingerLen   = sxsdk::absolute(dFingerV);

			sxsdk::vec3 sFingerV     = rigHumanIKBoneInfo[53].pos - rigHumanIKBoneInfo[41].pos;		// HandMiddle4 - Hand.
			const float sFingerLen   = sxsdk::absolute(sFingerV);
			const float fingerScale  = dFingerLen / sFingerLen;

			dFingerV = normalize(dFingerV);
			sFingerV = normalize(sFingerV);

			retBonesInfo[38].pos = shoulderPos;											// Shoulder.
			retBonesInfo[39].pos = (elbowPos - shoulderPos) * 0.3f + shoulderPos;		// Arm.
			retBonesInfo[40].pos = elbowPos;											// ForeArm.
			retBonesInfo[41].pos = wristPos;											// Hand.

			// 指は、手首を中心にsFingerVの向きをdFingerVに向ける.
			const sxsdk::mat4 rotM = sxsdk::mat4::rotate(sFingerV, dFingerV);
			for (int i = 42; i <= 61; i++) {
				sxsdk::vec3 pos = (rigHumanIKBoneInfo[i].pos - rigHumanIKBoneInfo[41].pos) * fingerScale;
				retBonesInfo[i].pos = (pos * rotM) + wristPos;
			}
		}

		// leg_leftより下.
		{
			const sxsdk::vec3 hipsPos   = rigPoint.rigPoint[rigPoint.root].pos;
			const sxsdk::vec3 legLeft   = rigPoint.rigPoint[rigPoint.leg_left].pos;
			const sxsdk::vec3 kneeLeft  = rigPoint.rigPoint[rigPoint.knee_left].pos;
			const sxsdk::vec3 ankleLeft = rigPoint.rigPoint[rigPoint.ankle_left].pos;
			const float scale = sxsdk::absolute(kneeLeft - ankleLeft) / sxsdk::absolute(rigHumanIKBoneInfo[3].pos - rigHumanIKBoneInfo[2].pos) * 0.7f;		// 左足首 - 左ひざ.

			retBonesInfo[1].pos = legLeft;					// UpLeg.
			retBonesInfo[2].pos = kneeLeft;					// Leg.
			retBonesInfo[3].pos = ankleLeft;				// Foot.
			retBonesInfo[4].pos = (rigHumanIKBoneInfo[4].pos - rigHumanIKBoneInfo[3].pos) * scale + ankleLeft;			// ToeBase.
		}

		// leg_rightより下.
		{
			const sxsdk::vec3 legRight   = rigPoint.rigPoint[rigPoint.leg_right].pos;
			const sxsdk::vec3 kneeRight  = rigPoint.rigPoint[rigPoint.knee_right].pos;
			const sxsdk::vec3 ankleRight = rigPoint.rigPoint[rigPoint.ankle_right].pos;
			const float scale = sxsdk::absolute(kneeRight - ankleRight) / sxsdk::absolute(rigHumanIKBoneInfo[7].pos - rigHumanIKBoneInfo[6].pos) * 0.7f;		// 左足首 - 左ひざ.

			retBonesInfo[5].pos = legRight;					// UpLeg.
			retBonesInfo[6].pos = kneeRight;				// Leg.
			retBonesInfo[7].pos = ankleRight;				// Foot.
			retBonesInfo[8].pos = (rigHumanIKBoneInfo[8].pos - rigHumanIKBoneInfo[7].pos) * scale + ankleRight;			// ToeBase.
		}

		// 目もしくはあごを生成する場合、Head位置を少し下げてHead2を設ける.
		if (createEyes || createJaw) {
			const sxsdk::vec3 neckPos  = rigPoint.rigPoint[rigPoint.neck].pos;
			const sxsdk::vec3 headTail = rigPoint.rigPoint[rigPoint.head_tail].pos;

			retBonesInfo[13].pos = (headTail - neckPos) * 0.15f + neckPos;		// Head.
			{
				const int index = retBonesInfo.size();
				RIG_BONE_INFO bInfo(index, 13, -1, "Head2", "", "", headTail);
				retBonesInfo.push_back(bInfo);
			}
		}

		// 目の生成.
		if (createEyes) {
			const sxsdk::vec3 neckPos  = rigPoint.rigPoint[rigPoint.neck].pos;
			const sxsdk::vec3 headTail = rigPoint.rigPoint[rigPoint.head_tail].pos;

			sxsdk::vec3 eyeCenterPos = (neckPos + headTail) * 0.5f;
			float len = sxsdk::absolute(headTail - neckPos);
			eyeCenterPos.z += len * 0.3f;

			const float eyeD  = len * 0.2f;
			const float eyeD2 = len * 0.3f;

			sxsdk::vec3 leftEyePos = eyeCenterPos;
			leftEyePos.x += eyeD;		// 2015/02/02 修正.
			sxsdk::vec3 leftEyePos2 = leftEyePos;
			leftEyePos2.z += eyeD2;

			sxsdk::vec3 rightEyePos = eyeCenterPos;
			rightEyePos.x -= eyeD;		// 2015/02/02 修正.
			sxsdk::vec3 rightEyePos2 = rightEyePos;
			rightEyePos2.z += eyeD2;

			{
				const int index = retBonesInfo.size();
				RIG_BONE_INFO bInfo(index, 13, -1, "LeftEye", "", "", leftEyePos);
				retBonesInfo.push_back(bInfo);

				const int index2 = index + 1;
				RIG_BONE_INFO bInfo2(index2, index, -1, "LeftEye2", "", "", leftEyePos2);
				retBonesInfo.push_back(bInfo2);
			}

			{
				const int index = retBonesInfo.size();
				RIG_BONE_INFO bInfo(index, 13, -1, "RightEye", "", "", rightEyePos);
				retBonesInfo.push_back(bInfo);

				const int index2 = index + 1;
				RIG_BONE_INFO bInfo2(index2, index, -1, "RightEye2", "", "", rightEyePos2);
				retBonesInfo.push_back(bInfo2);
			}
		}

		// あごの生成.
		if (createJaw) {
			const sxsdk::vec3 neckPos  = rigPoint.rigPoint[rigPoint.neck].pos;
			const sxsdk::vec3 headTail = rigPoint.rigPoint[rigPoint.head_tail].pos;

			const sxsdk::vec3 jawPos = (headTail - neckPos) * 0.25f + neckPos;
			float len = sxsdk::absolute(headTail - neckPos);
			sxsdk::vec3 jawPos2 = jawPos;
			jawPos2.z += len * 0.5f;
			jawPos2.y -= len * 0.2f;

			{
				const int index = retBonesInfo.size();
				RIG_BONE_INFO bInfo(index, 13, -1, "Jaw", "", "", jawPos);
				retBonesInfo.push_back(bInfo);

				const int index2 = index + 1;
				RIG_BONE_INFO bInfo2(index2, index, -1, "Jaw2", "", "", jawPos2);
				retBonesInfo.push_back(bInfo2);
			}
		}
	}

	return retBonesInfo;
}

/**
 * リグの制御点の数を取得.
 */
int CRigCtrl::GetRigPointCount()
{
	return m_rigPoint.rigPoint.size();
}

/**
 * 指定の形状でのリグ(CRigPointで与えたもの)の制御点番号を取得.
 */
int CRigCtrl::GetRigPointIndex(sxsdk::shape_class& shape)
{
	if (shape.get_type() != sxsdk::enums::sphere) return -1;

	int index = -1;
	try {
		compointer<sxsdk::stream_interface> stream(shape.get_attribute_stream_interface_with_uuid(RIG_POINT_STERAM_ID));
		if (stream == NULL) return -1;
		stream->set_pointer(0);
		stream->read_int(index);
	} catch (...) { }

	return index;
}

/**
 * リグの制御点での接続数を取得.
 */
int CRigCtrl::GetRigPointLinkCount()
{
	return m_rigPoint.rigPointLine.size();
}

/**
 * リグの制御点での接続情報を取得.
 */
bool CRigCtrl::GetRigPointLink(const int index, int *pRetP1, int *pRetP2)
{
	const int iVal = m_rigPoint.rigPointLine[index];
	*pRetP1 = (iVal >> 16);
	*pRetP2 = iVal & 0x00ff;

	return true;
}

/**
 * ミラーリングでのリグの対称となる制御点を取得.
 */
int CRigCtrl::GetRigMirrorPoint(const int index)
{
	return m_rigPoint.rigMirrorPoint[index];
}

/**
 * 指定の番号の形状を取得.
 */
sxsdk::shape_class* CRigCtrl::GetRigPointShape(sxsdk::scene_interface* scene, const int index, sxsdk::shape_class* pRigTargetPart)
{
	sxsdk::shape_class* pTargetPart = (pRigTargetPart) ? pRigTargetPart : (scene->get_shape_by_name(RIGPOINT_PART_NAME));
	if (pTargetPart == NULL) return NULL;

	sxsdk::shape_class* pRetShape = NULL;
	if (pTargetPart->has_son()) {
		sxsdk::shape_class* pShape = pTargetPart->get_son();
		while (pShape->has_bro()) {
			pShape = pShape->get_bro();
			if (pShape->get_type() == sxsdk::enums::sphere) {
				const int index2 = GetRigPointIndex(*pShape);
				if (index2 == index) {
					pRetShape = pShape;
					break;
				}
			}
		}
	}

	return pRetShape;
}

/**
 * 人体リグの、指定のボーン名に対応するインデックスを取得.
 */
int CRigCtrl::GetHumanBoneIndex(sxsdk::shade_interface *shade, std::string boneName, const int rigType)
{
	int index = -1;

	int iPos = 0;
	while (true) {
		RIG_BONE_INFO& rInfo = rigBoneInfo[iPos];
		if (rInfo.bone_index < 0) break;
		if (rigType == human_rig_type_mmd_jp) {
			const std::string nameUTF8 = BoneUtil::GetUTF8Text(*shade, rInfo.name_jp);
			if (nameUTF8.compare(boneName) == 0) {
				index = iPos;
				break;
			}
		} else if (rigType == human_rig_type_mmd_en) {
			if (rInfo.name_en.compare(boneName) == 0) {
				index = iPos;
				break;
			}
		}
		iPos++;
	}

	return index;
}

/**
 * 人体リグの、指定のボーンインデックスに対応する名前を取得.
 */
std::string CRigCtrl::GetHumanBoneName(sxsdk::shade_interface *shade, const int index, const int rigType)
{
	if (index < 0) return "";

	if (rigType == human_rig_type_mmd_jp) {
		const std::string nameUTF8 = BoneUtil::GetUTF8Text(*shade, rigBoneInfo[index].name_jp);
		return nameUTF8;
	}
	if (rigType == human_rig_type_mmd_en) {
		return rigBoneInfo[index].name_en;
	}

	return "";
}


/**
 * 指定のボーン構造がMMDで使用するものとほぼ一致するか調べる.
 * @param[in]  bone_root          ルートのボーン.
 * @param[out] pRetHumanRigType   リグの種類（human_rig_type_default/human_rig_type_mmd_jp/human_rig_type_mmd_en）.
 * @return  完全一致の場合は1.0を返す。1.0に近づくほどMMDのボーンの可能性が高い.
 */
float CRigCtrl::CheckMMDBones(sxsdk::shape_class& bone_root, int* pRetHumanRigType)
{
	// ボーン名を取得.
	std::vector<std::string> bonesName;
	m_GetBonesName(0, bone_root, bonesName);
	if (bonesName.size() == 0) return 0.0f;

	// 比較する最低限のボーンの数.
	int templateBonesCou = 0;
	while (true) {
		RIG_BONE_INFO& bInfo = rigBoneInfo[templateBonesCou];
		if (bInfo.bone_index < 0) break;
		templateBonesCou++;
	}
	if (templateBonesCou == 0) return 0.0f;

	int bCouJP      = 0;
	int bCouEN      = 0;
	int bCouDefault = 0;
	{
		// MMDの日本語ボーン名と完全一致するか.
		for (int i = 0; i < templateBonesCou; i++) {
			RIG_BONE_INFO& bInfo = rigBoneInfo[i];
			const std::string name = BoneUtil::GetUTF8Text(*m_shade, bInfo.name_jp);			// C/C++のソースから取得したテキストはSJISであるため、UTF-8に変換.
			for (int j = 0; j < bonesName.size(); j++) {
				if (name.compare(bonesName[j]) == 0) {
					bCouJP++;
					break;
				}
			}
		}
	}
	{
		// MMDの英語ボーン名と完全一致するか.
		for (int i = 0; i < templateBonesCou; i++) {
			RIG_BONE_INFO& bInfo = rigBoneInfo[i];
			const std::string name = bInfo.name_en;
			for (int j = 0; j < bonesName.size(); j++) {
				if (name.compare(bonesName[j]) == 0) {
					bCouEN++;
					break;
				}
			}
		}
	}
	{
		// デフォルトボーン名と完全一致するか.
		for (int i = 0; i < templateBonesCou; i++) {
			RIG_BONE_INFO& bInfo = rigBoneInfo[i];
			const std::string name = bInfo.name_default;
			for (int j = 0; j < bonesName.size(); j++) {
				if (name.compare(bonesName[j]) == 0) {
					bCouDefault++;
					break;
				}
			}
		}
	}

	int bCou    = bCouJP;
	int rigType = human_rig_type_mmd_jp;
	if (bCou < bCouEN) {
		bCou    = bCouEN;
		rigType = human_rig_type_mmd_en;
	}
	if (bCou < bCouDefault) {
		bCou    = bCouDefault;
		rigType = human_rig_type_none;
	}
	if (pRetHumanRigType) *pRetHumanRigType = rigType;

	return (float)bCou / (float)templateBonesCou;
}

/**
 * 指定のボーンから再帰でたどり、形状名を取得.
 */
void CRigCtrl::m_GetBonesName(const int depth, sxsdk::shape_class& shape, std::vector<std::string>& bonesName)
{
	if (!BoneUtil::IsBone(shape)) return;

	bonesName.push_back(shape.get_name());

	if (shape.has_son()) {
		sxsdk::shape_class *pShape = shape.get_son();
		while (pShape->has_bro()) {
			pShape = pShape->get_bro();
			m_GetBonesName(depth + 1, *pShape, bonesName);
		}
	}
}

/**
 * ガイド用の球を捜し、これらがHumanIK用かMMD用か判定.
 */
bool CRigCtrl::IsHumanIKGuide(sxsdk::scene_interface* scene)
{
	// ガイドが存在しない場合は、HumanIKのボーンを構成するとする.
	sxsdk::shape_class* pTargetPart = scene->get_shape_by_name(RIGPOINT_PART_NAME);
	if (pTargetPart == NULL) return true;


	return false;
}

