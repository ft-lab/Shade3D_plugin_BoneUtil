/**
 *  @file   GlobalHeader.h
 *  @brief  共通して使用する変数など.
 *  @author Yutaka Yoshisaka
 *  @date   2014.08.08 - 2014.08.15
 */

#ifndef _GLOBALHEADER_H
#define _GLOBALHEADER_H

#include "sxsdk.cxx"

/**
 * ボーンの変換行列の回転とスケールをクリアするattribute ID.
 */
#define BONECLEAR_ATTRIBUTE_INTERFACE_ID sx::uuid_class("BA463A06-9C3C-469F-BFCD-300CCCD3313B")

/**
 * ボーンをミラーリングするattribute ID.
 */
#define BONEMIRROR_ATTRIBUTE_INTERFACE_ID sx::uuid_class("AA4B6C67-7111-4A21-BDEF-6FD037582FFA")

/**
 * ボーンのサイズを自動調整するattribute ID.
 */
#define BONEAUTOSIZE_ATTRIBUTE_INTERFACE_ID sx::uuid_class("266612C7-98B6-49DA-BAE6-161BEDCA0511")

/**
 * 人体ボーンの制御点の初期化.
 */
#define INIT_HUMANBONE_ATTRIBUTE_INTERFACE_ID sx::uuid_class("3BC1BF8E-D39F-44E0-B29B-6850EDFA09A8")

/**
 * 人体ボーンの制御点より、ボーン生成.
 */
#define CREATE_HUMANBONE_ATTRIBUTE_INTERFACE_ID sx::uuid_class("FFB57CA5-59E8-4FD9-9025-D772DF13DBBC")

/**
 * 人体ボーンの名前変更.
 */
#define RENAME_HUMANBONE_ATTRIBUTE_INTERFACE_ID sx::uuid_class("9A39D67E-8736-4307-8BDF-879553BD7CB7")

/**
 * ボーンを膨らませたMeshを割り当てる機能.
 */
#define CREATE_BONES_MESH_ATTRIBUTE_INTERFACE_ID sx::uuid_class("776C153F-DEDC-425C-B06D-E66B7755FFA9")

/**
 * ボーンの軸方向を統一するattribute ID.
 */
#define BONE_DIRECTION_ATTRIBUTE_INTERFACE_ID sx::uuid_class("12D958A8-6BA3-4E32-9A9F-10A911AF4E0B")

/**
 * リグ構成のための制御点を結ぶstream ID.
 */
#define RIG_POINT_STERAM_ID sx::uuid_class("607E8D7C-9A4D-4425-92B8-44F588C7D379")

/**
 * stream保存時のバージョン.
 */
#define BONEMIRROR_DATA_VERSION			0x100
#define BONEAUTOSIZE_DATA_VERSION		0x100
#define BONECLEAR_DATA_VERSION			0x100
#define INIT_HUMAN_BONE_VERSION			0x100			// Human Boneのためのリグ構成の制御点生成時.
#define CREATE_HUMAN_BONE_VERSION		0x100			// Human Boneの生成時.
#define CREATE_BONES_MESH_VERSION		0x100			// ボーンからメッシュ生成時.
#define RENAME_HUMAN_BONE_VERSION		0x100			// Human Boneのボーン名変更時.
#define BONEDIRECTION_DATA_VERSION		0x100			// ボーンの軸方向変更用.

/**
 * 人体のポーズ.
 */
enum {
	human_pose_T = 0,				// Tポーズ.
	human_pose_A_30,				// Aポーズ（30度手を下ろしたもの）.
	human_pose_A_45,				// Aポーズ（45度手を下ろしたもの）.
};

/**
 * 人体リグの種類.
 */
enum {
	human_type_HumanIK = 0,			// HumanIK.
	human_type_MMD     = 1,			// MMD.
};

/**
 * 人体リグのボーン名タイプ.
 */
enum {
	human_rig_type_none    = -1,
	human_rig_type_humanIK = 0,		// humanIKのボーン名/ボーン構造.
	human_rig_type_mmd_jp  = 1,		// MMDの日本語ボーン名.
	human_rig_type_mmd_en  = 2,		// MMDの英語ボーン名.
};

#define RIGPOINT_PART_NAME		"RigPoints"			// 制御用のRigの球を入れるパート名.

/**
 * 人体リグの制御点を生成する際のダイアログ情報.
 */
class HumanBoneRigPointsData {
public:
	float height;			// 身長.
	int pose_type;			// ポーズの種類（human_pose_T / human_pose_A_30 / human_pose_A_45 ）.
	bool mirror;			// ミラーリング操作を行うか.

	HumanBoneRigPointsData() {
		height    = 1700.0f;
		pose_type = human_pose_T;
		mirror    = true;
	}
};

/**
 * 人体ボーン生成のダイアログ情報.
 */
class HumanBoneCreateData {
public:
	int bones_type;				// human_rig_type_humanIK / human_rig_type_mmd_jp / human_rig_type_mmd_en.
	bool create_legs_IK;		// 足のIKを自動生成.
	std::string name;			// humanIK時の名称 (名称_Hips などがボーン名になる).

	bool option_eyes;			// HumanIKの場合の目のボーン生成.
	bool option_jaw;			// HumanIKの場合のあごのボーン生成.

	HumanBoneCreateData() {
		bones_type     = human_rig_type_humanIK;
		create_legs_IK = false;
		name           = "bone";

		option_eyes    = false;
		option_jaw     = false;
	}
};

/**
 * ボーンを膨らませたMeshを与える機能でのダイアログ情報.
 */
class BonesMeshCreateData {
public:
	float fat_size;				// 膨らませる大きさ.

	BonesMeshCreateData() {
		fat_size = 1.0f;
	}
};

/**
 * 人体ボーンの名前変更情報.
 */
class HumanBoneRenameData {
public:
	int conv_bones_type;			// 変換後のボーン名（human_rig_type_default / human_rig_type_mmd_jp / human_rig_type_mmd_en）.

	HumanBoneRenameData() {
		conv_bones_type     = human_rig_type_humanIK;
	}
};

/**
 * ボーンクリア時の情報.
 */
class BoneClearData {
public:
	int replaceAndUpdateSkin;			// ボーンを置き換えて、対応ポリゴンメッシュのスキンも更新.

	BoneClearData() {
		replaceAndUpdateSkin = false;
	}
};

/**
 * ボーンの軸方向の情報.
 */
class BoneDirectionData {
public:
	bool fitDirection;		// 軸方向を進行方向に一致させる.

	BoneDirectionData() {
		fitDirection = true;
	}
};

#endif
