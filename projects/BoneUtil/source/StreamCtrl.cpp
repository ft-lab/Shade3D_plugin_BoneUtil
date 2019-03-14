/**
 *  @brief   stream保存、読み込み用.
 */

#include "StreamCtrl.h"

/**
 * リグ構成の制御点を生成する際の情報を取得.
 */
HumanBoneRigPointsData StreamCtrl::LoadHumanBoneRigInfo(sxsdk::scene_interface* scene)
{
	HumanBoneRigPointsData data;
	try {
		compointer<sxsdk::stream_interface> stream(scene->get_attribute_stream_interface_with_uuid(INIT_HUMANBONE_ATTRIBUTE_INTERFACE_ID));
		if (stream == NULL) return data;

		stream->set_pointer(0);

		int iDat = 0;
		stream->read_int(iDat);
		if (iDat != INIT_HUMAN_BONE_VERSION) return data;

		stream->read_float(data.height);
		stream->read_int(data.pose_type);

		stream->read_int(iDat);
		data.mirror = iDat ? true : false;

	} catch (...) { }

	return data;
}

/**
 * リグ構成の制御点を生成する際の情報を保持.
 */
void StreamCtrl::SaveHumanBoneRigInfo(sxsdk::scene_interface *scene, const HumanBoneRigPointsData& data)
{
	// シーンに保存.
	try {
		compointer<sxsdk::stream_interface> stream(scene->create_attribute_stream_interface_with_uuid(INIT_HUMANBONE_ATTRIBUTE_INTERFACE_ID));
		if (stream == NULL) return;

		stream->set_pointer(0);
		stream->set_size(0);

		int iDat = INIT_HUMAN_BONE_VERSION;
		stream->write_int(iDat);

		stream->write_float(data.height);
		stream->write_int(data.pose_type);

		iDat = data.mirror ? 1 : 0;
		stream->write_int(iDat);

	} catch (...) { }

	// 形状に保存。make_wireframe用.
	try {
		sxsdk::shape_class* pTargetPart = scene->get_shape_by_name(RIGPOINT_PART_NAME);
		if (pTargetPart == NULL) return;

		compointer<sxsdk::stream_interface> stream(pTargetPart->create_attribute_stream_interface_with_uuid(INIT_HUMANBONE_ATTRIBUTE_INTERFACE_ID,
																											INIT_HUMANBONE_ATTRIBUTE_INTERFACE_ID));	// ワイヤーフレームの接続線を描画.
		if (stream == NULL) return;

		stream->set_pointer(0);
		stream->set_size(0);

		int iDat = INIT_HUMAN_BONE_VERSION;
		stream->write_int(iDat);
	} catch (...) { }
}

/**
 * リグ構成のからボーンを生成する際の情報を取得.
 */
HumanBoneCreateData StreamCtrl::LoadHumanBoneCreateInfo(sxsdk::scene_interface* scene)
{
	HumanBoneCreateData data;
	try {
		compointer<sxsdk::stream_interface> stream(scene->get_attribute_stream_interface_with_uuid(CREATE_HUMANBONE_ATTRIBUTE_INTERFACE_ID));
		if (stream == NULL) return data;

		stream->set_pointer(0);

		int iDat = 0;
		stream->read_int(iDat);
		if (iDat != CREATE_HUMAN_BONE_VERSION) return data;

		stream->read_int(data.bones_type);

		stream->read_int(iDat);
		data.create_legs_IK = iDat ? true : false;

		char szStr[260];
		memset(szStr, 0, 256);
		stream->read(255, szStr);
		data.name = szStr;

		stream->read_int(iDat);
		data.option_eyes = iDat ? true : false;

		stream->read_int(iDat);
		data.option_jaw = iDat ? true : false;

	} catch (...) { }

	return data;
}

/**
 * リグ構成のからボーンを生成する際の情報を保持.
 */
void StreamCtrl::SaveHumanBoneCreateInfo(sxsdk::scene_interface *scene, const HumanBoneCreateData& data)
{
	try {
		compointer<sxsdk::stream_interface> stream(scene->create_attribute_stream_interface_with_uuid(CREATE_HUMANBONE_ATTRIBUTE_INTERFACE_ID));
		if (stream == NULL) return;

		stream->set_pointer(0);
		stream->set_size(0);

		int iDat = CREATE_HUMAN_BONE_VERSION;
		stream->write_int(iDat);

		stream->write_int(data.bones_type);

		iDat = data.create_legs_IK ? 1 : 0;
		stream->write_int(iDat);

		char szStr[260];
		memset(szStr, 0, 256);
		if (data.name.length() < 255) strcpy(szStr, data.name.c_str());
		else strncpy(szStr, data.name.c_str(), 255);
		stream->write(255, szStr);

		iDat = data.option_eyes ? 1 : 0;
		stream->write_int(iDat);

		iDat = data.option_jaw ? 1 : 0;
		stream->write_int(iDat);

	} catch (...) { }
}

/**
 * ボーンからメッシュを生成する際の情報を取得.
 */
BonesMeshCreateData StreamCtrl::LoadBonesMeshCreateInfo(sxsdk::scene_interface* scene)
{
	BonesMeshCreateData data;
	try {
		compointer<sxsdk::stream_interface> stream(scene->get_attribute_stream_interface_with_uuid(CREATE_BONES_MESH_ATTRIBUTE_INTERFACE_ID));
		if (stream == NULL) return data;

		stream->set_pointer(0);

		int iDat = 0;
		stream->read_int(iDat);
		if (iDat != CREATE_BONES_MESH_VERSION) return data;

		stream->read_float(data.fat_size);

	} catch (...) { }

	return data;
}

/**
 * ボーンからメッシュを生成する際の情報を保持.
 */
void StreamCtrl::SaveBonesMeshCreateInfo(sxsdk::scene_interface* scene, const BonesMeshCreateData& data)
{
	try {
		compointer<sxsdk::stream_interface> stream(scene->create_attribute_stream_interface_with_uuid(CREATE_BONES_MESH_ATTRIBUTE_INTERFACE_ID));
		if (stream == NULL) return;

		stream->set_pointer(0);
		stream->set_size(0);

		int iDat = CREATE_BONES_MESH_VERSION;
		stream->write_int(iDat);

		stream->write_float(data.fat_size);

	} catch (...) { }
}

/**
 * ボーン名を変換する際の情報を取得.
 */
HumanBoneRenameData StreamCtrl::LoadHumanBoneRenameInfo(sxsdk::scene_interface* scene)
{
	HumanBoneRenameData data;
	try {
		compointer<sxsdk::stream_interface> stream(scene->get_attribute_stream_interface_with_uuid(RENAME_HUMANBONE_ATTRIBUTE_INTERFACE_ID));
		if (stream == NULL) return data;

		stream->set_pointer(0);

		int iDat = 0;
		stream->read_int(iDat);
		if (iDat != RENAME_HUMAN_BONE_VERSION) return data;

		stream->read_int(data.conv_bones_type);

	} catch (...) { }

	return data;
}

/**
 * ボーンクリア時の情報を保持.
 */
void StreamCtrl::SaveBonesClearInfo(sxsdk::scene_interface *scene, const BoneClearData& data)
{
	try {
		compointer<sxsdk::stream_interface> stream(scene->create_attribute_stream_interface_with_uuid(BONECLEAR_ATTRIBUTE_INTERFACE_ID));
		if (stream == NULL) return;

		stream->set_pointer(0);
		stream->set_size(0);

		int iDat = BONECLEAR_DATA_VERSION;
		stream->write_int(iDat);

		iDat = data.replaceAndUpdateSkin ? 1 : 0;
		stream->write_int(iDat);

	} catch (...) { }
}

/**
 * ボーンクリア時の情報を取得.
 */
BoneClearData StreamCtrl::LoadBonesClearInfo(sxsdk::scene_interface* scene)
{
	BoneClearData data;
	try {
		compointer<sxsdk::stream_interface> stream(scene->get_attribute_stream_interface_with_uuid(BONECLEAR_ATTRIBUTE_INTERFACE_ID));
		if (stream == NULL) return data;

		stream->set_pointer(0);

		int iDat = 0;
		stream->read_int(iDat);
		if (iDat != BONECLEAR_DATA_VERSION) return data;

		stream->read_int(iDat);
		data.replaceAndUpdateSkin = iDat ? true : false;

	} catch (...) { }

	return data;
}

/**
 * ボーンの軸方向の情報を保持.
 */
void StreamCtrl::SaveBonesDirectionInfo(sxsdk::scene_interface *scene, const BoneDirectionData& data)
{
	try {
		compointer<sxsdk::stream_interface> stream(scene->create_attribute_stream_interface_with_uuid(BONE_DIRECTION_ATTRIBUTE_INTERFACE_ID));
		if (stream == NULL) return;

		stream->set_pointer(0);
		stream->set_size(0);

		int iDat = BONEDIRECTION_DATA_VERSION;
		stream->write_int(iDat);

		iDat = data.fitDirection ? 1 : 0;
		stream->write_int(iDat);

	} catch (...) { }
}

/**
 * ボーンの軸方向の情報を取得.
 */
BoneDirectionData StreamCtrl::LoadBonesDirectionInfo(sxsdk::scene_interface* scene)
{
	BoneDirectionData data;
	try {
		compointer<sxsdk::stream_interface> stream(scene->get_attribute_stream_interface_with_uuid(BONE_DIRECTION_ATTRIBUTE_INTERFACE_ID));
		if (stream == NULL) return data;

		stream->set_pointer(0);

		int iDat = 0;
		stream->read_int(iDat);
		if (iDat != BONEDIRECTION_DATA_VERSION) return data;

		stream->read_int(iDat);
		data.fitDirection = iDat ? true : false;

	} catch (...) { }

	return data;
}

/**
 * ボーン名を変換する際の情報を保持.
 */
void StreamCtrl::SaveHumanBoneRenameInfo(sxsdk::scene_interface *scene, const HumanBoneRenameData& data)
{
	try {
		compointer<sxsdk::stream_interface> stream(scene->create_attribute_stream_interface_with_uuid(RENAME_HUMANBONE_ATTRIBUTE_INTERFACE_ID));
		if (stream == NULL) return;

		stream->set_pointer(0);
		stream->set_size(0);

		int iDat = RENAME_HUMAN_BONE_VERSION;
		stream->write_int(iDat);

		stream->write_int(data.conv_bones_type);

	} catch (...) { }
}

