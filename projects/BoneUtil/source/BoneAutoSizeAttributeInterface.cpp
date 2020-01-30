/**
 *  @brief  ボーンのサイズを長さから自動調整する機能.
 *  @date   2014.08.07 - 2014.08.07
 */

#include "BoneAutoSizeAttributeInterface.h"
#include "BoneUtil.h"

enum {
	dlg_change_by_dist_id = 101,
	dlg_bone_size_id = 102,
	dlg_scale_id = 103,
};

CBoneAutoSizeAttributeInterface::CBoneAutoSizeAttributeInterface(sxsdk::shade_interface &shade) : shade(shade)
{
}

CBoneAutoSizeAttributeInterface::~CBoneAutoSizeAttributeInterface()
{
}

// カスタム情報が選択された.
bool CBoneAutoSizeAttributeInterface::ask_shape (sxsdk::shape_class &shape, void *)
{
	compointer<sxsdk::scene_interface> scene(shape.get_scene_interface());
	if (scene == NULL) return false;

	if (!BoneUtil::IsBone(scene->active_shape())) {
		scene->show_message_box(scene->gettext("msg_not_bone"), false);
		return false;
	}

	// ダイアログデータの読み込み.
	m_data = m_LoadData(scene);

	compointer<sxsdk::dialog_interface> dlg(shade.create_dialog_interface_with_uuid(BONEAUTOSIZE_ATTRIBUTE_INTERFACE_ID));
	dlg->set_resource_name("bone_autosize_attrib_dlg");
	dlg->set_responder(this);
	this->AddRef();			// set_responder()に合わせて、参照カウンタを増やす。 .

	// ダイアログの表示.
	if(dlg->ask()) {
		m_SaveData(scene, m_data);
		m_DoBoneAutoSize(scene);
	}

	return true;
}

/**
 * ボーンのサイズ変更処理実行.
 */
void CBoneAutoSizeAttributeInterface::m_DoBoneAutoSize(sxsdk::scene_interface *scene)
{
	sxsdk::shape_class& active_shape = scene->active_shape();
	if (!BoneUtil::IsBone(active_shape)) {
		scene->show_message_box(scene->gettext("msg_not_bone"), false);
		return;
	}

	// ボーン構造を複製しつつ、ボーンのサイズ変更処理を実行.
	m_DoBoneAutoSizeLoop(scene, active_shape, NULL);
}

void CBoneAutoSizeAttributeInterface::m_DoBoneAutoSizeLoop(sxsdk::scene_interface *scene, sxsdk::shape_class& shape, sxsdk::shape_class* prevShape)
{
	if (!BoneUtil::IsBone(shape)) return;

	float size = 0.0f;

	sxsdk::vec3 center = BoneUtil::GetBoneCenter(shape, &size);

	if (!m_data.changeByDistance) {
		// 普通にボーンサイズを指定のものに変更.
		try {
			compointer<sxsdk::bone_joint_interface> bone(shape.get_bone_joint_interface());
			bone->set_size(m_data.boneSize);
		} catch (...) { }
	} else {
		float dist = 0.0f;
		float sizePrev = size;
		sxsdk::vec3 centerPrev = center;
		if (prevShape) {
			centerPrev = BoneUtil::GetBoneCenter(*prevShape, &sizePrev);
			dist = sxsdk::distance3(centerPrev, center);
		}
		if (dist > 0.0f) {
			size = (dist / 8.0f) * m_data.scale;
			try {
				compointer<sxsdk::bone_joint_interface> bone(shape.get_bone_joint_interface());
				bone->set_size(size);
			} catch (...) { }
			if (prevShape) {
				try {
					compointer<sxsdk::bone_joint_interface> bone(prevShape->get_bone_joint_interface());
					bone->set_size(size);
				} catch (...) { }
			}
		}
	}

	if (shape.has_son()) {
		sxsdk::shape_class* pShape = shape.get_son();
		while (pShape->has_bro()) {
			pShape = pShape->get_bro();
			m_DoBoneAutoSizeLoop(scene, *pShape, &shape);
		}
	}
}

/**
 * streamから設定を呼び出し.
 */
CBoneAutoSizeData CBoneAutoSizeAttributeInterface::m_LoadData(sxsdk::scene_interface *scene)
{
	CBoneAutoSizeData data;

	try {
		compointer<sxsdk::stream_interface> stream(scene->get_attribute_stream_interface_with_uuid(BONEAUTOSIZE_ATTRIBUTE_INTERFACE_ID));
		if (!stream) return data;

		stream->set_pointer(0);

		int iDat;
		stream->read_int(iDat);
		if (iDat != BONEAUTOSIZE_DATA_VERSION) return data;

		stream->read_int(iDat);
		data.changeByDistance = iDat ? true : false;

		stream->read_float(data.boneSize);
		stream->read_float(data.scale);
	} catch (...) { }

	return data;
}

/**
 * 設定を保存.
 */
void CBoneAutoSizeAttributeInterface::m_SaveData(sxsdk::scene_interface *scene, const CBoneAutoSizeData data)
{
	try {
		compointer<sxsdk::stream_interface> stream(scene->create_attribute_stream_interface_with_uuid(BONEAUTOSIZE_ATTRIBUTE_INTERFACE_ID));
		if (!stream) return;

		stream->set_pointer(0);
		stream->set_size(0);

		int iDat = BONEAUTOSIZE_DATA_VERSION;
		stream->write_int(iDat);

		iDat = data.changeByDistance ? 1 : 0;
		stream->write_int(iDat);

		stream->write_float(data.boneSize);
		stream->write_float(data.scale);
	} catch (...) { }
}

//--------------------------------------------------//
//	ダイアログのイベント処理用						//
//--------------------------------------------------//
// ダイアログの初期化.
void CBoneAutoSizeAttributeInterface::initialize_dialog (sxsdk::dialog_interface &d, void *)
{
}

// ダイアログのイベントを受け取る.
bool CBoneAutoSizeAttributeInterface::respond(sxsdk::dialog_interface &d, sxsdk::dialog_item_class &item, int action, void *)
{
	const int id = item.get_id();		// アクションがあったダイアログアイテムのID.

	if (id == dlg_change_by_dist_id) {
		m_data.changeByDistance = item.get_bool();
		{
			sxsdk::dialog_item_class* item2;
			item2 = &(d.get_dialog_item(dlg_bone_size_id));
			item2->set_enabled(!m_data.changeByDistance);

			item2 = &(d.get_dialog_item(dlg_scale_id));
			item2->set_enabled(m_data.changeByDistance);
		}
		return true;
	}
	if (id == dlg_bone_size_id) {
		m_data.boneSize = item.get_float();
		return true;
	}
	if (id == dlg_scale_id) {
		m_data.scale = item.get_float();
		return true;
	}

	return false;
}

// ダイアログのデータを設定する.
void CBoneAutoSizeAttributeInterface::load_dialog_data (sxsdk::dialog_interface &d, void *)
{
	{
		sxsdk::dialog_item_class* item;
		item = &(d.get_dialog_item(dlg_change_by_dist_id));
		item->set_bool(m_data.changeByDistance);
	}
	{
		sxsdk::dialog_item_class* item;
		item = &(d.get_dialog_item(dlg_bone_size_id));
		item->set_float(m_data.boneSize);
		item->set_enabled(!m_data.changeByDistance);
	}
	{
		sxsdk::dialog_item_class* item;
		item = &(d.get_dialog_item(dlg_scale_id));
		item->set_float(m_data.scale);
		item->set_enabled(m_data.changeByDistance);
	}
}

// 値の変更を保存するときに呼ばれる.
void CBoneAutoSizeAttributeInterface::save_dialog_data (sxsdk::dialog_interface &d, void *)
{
}

// ダイアログが閉じられるときに呼ばれる.
int CBoneAutoSizeAttributeInterface::terminate (sxsdk::dialog_interface &d, int result, void *)
{
	return result;
}

