/**
 *  @brief  ボーンのミラーリング複製機能.
 *  @date   2014.08.05 - 2014.08.07.  
 */

#include "BoneMirrorAttributeInterface.h"
#include "BoneUtil.h"

enum {
	dlg_center_axis_id = 101,
	dlg_center_position_id = 102,
	dlg_replace_name_id = 103,
	dlg_replace_src_id = 104,
	dlg_replace_dst_id = 105,
};

CBoneMirrorAttributeInterface::CBoneMirrorAttributeInterface(sxsdk::shade_interface &shade) : shade(shade) {
}

CBoneMirrorAttributeInterface::~CBoneMirrorAttributeInterface() {
}

// カスタム情報が選択された.
bool CBoneMirrorAttributeInterface::ask_shape(sxsdk::shape_class &shape, void *) {

	compointer<sxsdk::scene_interface> scene(shape.get_scene_interface());
	if (scene == NULL) return false;

	if (!BoneUtil::IsBone(scene->active_shape())) {
		scene->show_message_box(scene->gettext("msg_not_bone"), false);
		return false;
	}

	// ダイアログデータの読み込み.
	m_boneMirrorData = m_LoadData(scene);

	compointer<sxsdk::dialog_interface> dlg(shade.create_dialog_interface_with_uuid(BONEMIRROR_ATTRIBUTE_INTERFACE_ID));
	dlg->set_resource_name("bone_mirror_attrib_dlg");
	dlg->set_responder(this);
	this->AddRef();			// set_responder()に合わせて、参照カウンタを増やす。 .

	// ダイアログの表示.
	if(dlg->ask()) {
		m_SaveData(scene, m_boneMirrorData);

		// ミラーリング実行.
		m_DoBoneMirror(scene, m_boneMirrorData);
	}

	return true;
}

/**
 * streamから設定を呼び出し.
 */
CBoneMirrorData CBoneMirrorAttributeInterface::m_LoadData(sxsdk::scene_interface *scene) {
	CBoneMirrorData data;

	try {
		compointer<sxsdk::stream_interface> stream(scene->get_attribute_stream_interface_with_uuid(BONEMIRROR_ATTRIBUTE_INTERFACE_ID));
		if (!stream) return data;

		stream->set_pointer(0);

		int iDat;
		stream->read_int(iDat);
		if (iDat != BONEMIRROR_DATA_VERSION) return data;

		stream->read_int(data.axis);
		stream->read_float(data.position);
		stream->read_int(iDat);
		data.replaceName = (iDat) ? true : false;

		char szStr[128];
		stream->read(128, szStr);
		data.replaceSrcText = szStr;

		stream->read(128, szStr);
		data.replaceDstText = szStr;
	} catch (...) { }

	return data;
}

/**
 * 設定を保存.
 */
void CBoneMirrorAttributeInterface::m_SaveData(sxsdk::scene_interface *scene, const CBoneMirrorData data) {
	try {
		compointer<sxsdk::stream_interface> stream(scene->create_attribute_stream_interface_with_uuid(BONEMIRROR_ATTRIBUTE_INTERFACE_ID));
		if (!stream) return;

		stream->set_pointer(0);
		stream->set_size(0);

		int iDat = BONEMIRROR_DATA_VERSION;
		stream->write_int(iDat);

		stream->write_int(data.axis);
		stream->write_float(data.position);
		iDat = data.replaceName ? 1 : 0;
		stream->write_int(iDat);

		char szStr[128];
		memset(szStr, 0, 127);
		if (data.replaceSrcText.size() < 127) {
			strcpy(szStr, data.replaceSrcText.c_str());
		}
		stream->write(128, szStr);

		memset(szStr, 0, 127);
		if (data.replaceDstText.size() < 127) {
			strcpy(szStr, data.replaceDstText.c_str());
		}
		stream->write(128, szStr);
	} catch (...) { }
}

/**
 * ミラーリング実行.
 */
void CBoneMirrorAttributeInterface::m_DoBoneMirror(sxsdk::scene_interface *scene, const CBoneMirrorData& data)
{
	sxsdk::shape_class& active_shape = scene->active_shape();
	if (!BoneUtil::IsBone(active_shape)) {
		scene->show_message_box(scene->gettext("msg_not_bone"), false);
		return;
	}

	// ボーン構造を複製.
	std::vector<sxsdk::shape_class *> cShapeList;
	scene->begin_creating_at(&active_shape);
	m_DoBoneMirrorLoop(scene, active_shape, NULL, data, cShapeList);
	scene->end_creating();

	if (cShapeList.size() > 0) cShapeList[0]->select();
}

void CBoneMirrorAttributeInterface::m_DoBoneMirrorLoop(sxsdk::scene_interface *scene, sxsdk::shape_class& shape, sxsdk::part_class* parentPart, const CBoneMirrorData& data, std::vector<sxsdk::shape_class *>& cShapeList)
{
	if (!BoneUtil::IsBone(shape)) return;

	float size = 0.0f;
	const sxsdk::mat4 lwMat = (parentPart) ? ((parentPart->get_transformation()) * (parentPart->get_local_to_world_matrix())) : shape.get_local_to_world_matrix();

	sxsdk::vec3 center = BoneUtil::GetBoneCenter(shape, &size);
	switch (data.axis) {
	case 0:
		center.x = -(center.x - data.position) + data.position;
		break;
	case 1:
		center.y = -(center.y - data.position) + data.position;
		break;
	case 2:
		center.z = -(center.z - data.position) + data.position;
		break;
	}

	center = center * inv(lwMat);
	sxsdk::vec3 axis_dir(1, 0, 0);

	sxsdk::part_class& part = scene->begin_bone_joint(center, size, false, axis_dir);
	cShapeList.push_back(&part);

	// 名前の変更.
	std::string name = shape.get_name();
	if (data.replaceName) {
		int pos = name.find(data.replaceSrcText);
		if (pos != std::string::npos) {
			name.replace(pos, data.replaceSrcText.length(), data.replaceDstText);
		}
	}
	part.set_name(name.c_str());

	if (shape.has_son()) {
		sxsdk::shape_class* pShape = shape.get_son();
		while (pShape->has_bro()) {
			pShape = pShape->get_bro();
			m_DoBoneMirrorLoop(scene, *pShape, &part, data, cShapeList);
		}
	}

	scene->end_bone_joint();
}

//--------------------------------------------------//
//	ダイアログのイベント処理用						//
//--------------------------------------------------//
// ダイアログの初期化.
void CBoneMirrorAttributeInterface::initialize_dialog (sxsdk::dialog_interface &d, void *) {
}

// ダイアログのイベントを受け取る.
bool CBoneMirrorAttributeInterface::respond(sxsdk::dialog_interface &d, sxsdk::dialog_item_class &item, int action, void *) {
	const int id = item.get_id();		// アクションがあったダイアログアイテムのID.

	if (id == dlg_center_axis_id) {
		m_boneMirrorData.axis = (int)item.get_selection();
		return true;
	}
	if (id == dlg_center_position_id) {
		m_boneMirrorData.position = item.get_float();
		return true;
	}
	if (id == dlg_replace_name_id) {
		m_boneMirrorData.replaceName = item.get_bool();
		{
			sxsdk::dialog_item_class* item2;
			item2 = &(d.get_dialog_item(dlg_replace_src_id));
			item2->set_enabled(m_boneMirrorData.replaceName);
		}
		{
			sxsdk::dialog_item_class* item2;
			item2 = &(d.get_dialog_item(dlg_replace_dst_id));
			item2->set_enabled(m_boneMirrorData.replaceName);
		}

		return true;
	}
	if (id == dlg_replace_src_id) {
		m_boneMirrorData.replaceSrcText = item.get_string();
		return true;
	}
	if (id == dlg_replace_dst_id) {
		m_boneMirrorData.replaceDstText = item.get_string();
		return true;
	}

	return false;
}

// ダイアログのデータを設定する.
void CBoneMirrorAttributeInterface::load_dialog_data (sxsdk::dialog_interface &d, void *) {
	{
		sxsdk::dialog_item_class* item;
		item = &(d.get_dialog_item(dlg_center_axis_id));
		item->set_selection(m_boneMirrorData.axis);
	}
	{
		sxsdk::dialog_item_class* item;
		item = &(d.get_dialog_item(dlg_center_position_id));
		item->set_float(m_boneMirrorData.position);
	}
	{
		sxsdk::dialog_item_class* item;
		item = &(d.get_dialog_item(dlg_replace_name_id));
		item->set_bool(m_boneMirrorData.replaceName);
	}
	{
		sxsdk::dialog_item_class* item;
		item = &(d.get_dialog_item(dlg_replace_src_id));
		item->set_string(m_boneMirrorData.replaceSrcText.c_str());
		item->set_enabled(m_boneMirrorData.replaceName);
	}
	{
		sxsdk::dialog_item_class* item;
		item = &(d.get_dialog_item(dlg_replace_dst_id));
		item->set_string(m_boneMirrorData.replaceDstText.c_str());
		item->set_enabled(m_boneMirrorData.replaceName);
	}
}

// 値の変更を保存するときに呼ばれる.
void CBoneMirrorAttributeInterface::save_dialog_data (sxsdk::dialog_interface &d, void *) {
}

// ダイアログが閉じられるときに呼ばれる.
int CBoneMirrorAttributeInterface::terminate (sxsdk::dialog_interface &d, int result, void *) {
	return result;
}
