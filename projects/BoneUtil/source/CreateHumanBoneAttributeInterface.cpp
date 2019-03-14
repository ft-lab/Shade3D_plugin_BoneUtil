/**
 *   @brief  ボーンの制御点情報をアタリにしてボーンを生成.
 */

#include "CreateHumanBoneAttributeInterface.h"
#include "RigCtrl.h"
#include "StreamCtrl.h"

enum {
	dlg_bones_type_id = 101,		// 出力するボーン名の種類.
	dlg_humanIK_name_id = 102,		// HumanIKのボーン名.
	//dlg_create_legs_IK_id = 103,	// 足のIKを自動生成.
	dlg_option_eyes_id = 111,		// HumanIK時に目のボーン生成.
	dlg_option_jaw_id  = 112,		// HumanIK時にあごのボーン生成.
};

CCreateHumanBoneAttributeInterface::CCreateHumanBoneAttributeInterface(sxsdk::shade_interface &shade) : shade(shade)
{
}

CCreateHumanBoneAttributeInterface::~CCreateHumanBoneAttributeInterface()
{
}

// カスタム情報が選択された.
bool CCreateHumanBoneAttributeInterface::ask_shape (sxsdk::shape_class &shape, void *)
{
	compointer<sxsdk::scene_interface> scene(shape.get_scene_interface());
	if (scene == NULL) return false;

	m_createData = StreamCtrl::LoadHumanBoneCreateInfo(scene);

	compointer<sxsdk::dialog_interface> dlg(shade.create_dialog_interface_with_uuid(CREATE_HUMANBONE_ATTRIBUTE_INTERFACE_ID));
	dlg->set_resource_name("create_rig_to_bones_dlg");
	dlg->set_responder(this);
	this->AddRef();			// set_responder()に合わせて、参照カウンタを増やす。 .

	if(dlg->ask()) {
		CRigCtrl rigCtrl(&shade);
		if (m_createData.bones_type == human_rig_type_humanIK) {
			rigCtrl.CreateHumanIKRig(scene, m_createData.name, m_createData.option_eyes, m_createData.option_jaw, m_createData.create_legs_IK);		// HumanIK用リグの生成.

		} else {
			// MMD式のボーン構造を生成.
			rigCtrl.CreateMMDHumanRig(scene, m_createData.bones_type, m_createData.create_legs_IK);
		}

		StreamCtrl::SaveHumanBoneCreateInfo(scene, m_createData);
	}

	return true;
}

//--------------------------------------------------//
//	ダイアログのイベント処理用						//
//--------------------------------------------------//
// ダイアログの初期化.
void CCreateHumanBoneAttributeInterface::initialize_dialog (sxsdk::dialog_interface &d, void *)
{
}

// ダイアログのイベントを受け取る.
bool CCreateHumanBoneAttributeInterface::respond(sxsdk::dialog_interface &d, sxsdk::dialog_item_class &item, int action, void *)
{
	const int id = item.get_id();		// アクションがあったダイアログアイテムのID.

	if (id == dlg_bones_type_id) {
		m_createData.bones_type = item.get_selection();

		{
		//	sxsdk::dialog_item_class* item2;
		//	item2 = &(d.get_dialog_item(dlg_create_legs_IK_id));
		//	item2->set_enabled(m_createData.bones_type != human_rig_type_humanIK);
		}
		{
			sxsdk::dialog_item_class* item2;
			item2 = &(d.get_dialog_item(dlg_humanIK_name_id));
			item2->set_enabled(m_createData.bones_type == human_rig_type_humanIK);
		}
		{
			sxsdk::dialog_item_class* item2;
			item2 = &(d.get_dialog_item(dlg_option_eyes_id));
			item2->set_enabled(m_createData.bones_type == human_rig_type_humanIK);
		}
		{
			sxsdk::dialog_item_class* item2;
			item2 = &(d.get_dialog_item(dlg_option_jaw_id));
			item2->set_enabled(m_createData.bones_type == human_rig_type_humanIK);
		}

		return true;
	}
	//if (id == dlg_create_legs_IK_id) {
	//	m_createData.create_legs_IK = item.get_bool();
	//	return true;
	//}
	if (id == dlg_humanIK_name_id) {
		m_createData.name = item.get_string();
		return true;
	}
	if (id == dlg_option_eyes_id) {
		m_createData.option_eyes = item.get_bool();
		return true;
	}
	if (id == dlg_option_jaw_id) {
		m_createData.option_jaw = item.get_bool();
		return true;
	}

	return false;
}

// ダイアログのデータを設定する.
void CCreateHumanBoneAttributeInterface::load_dialog_data (sxsdk::dialog_interface &d, void *)
{
	{
		sxsdk::dialog_item_class* item;
		item = &(d.get_dialog_item(dlg_bones_type_id));
		item->set_selection(m_createData.bones_type);
	}
	{
	//	sxsdk::dialog_item_class* item;
	//	item = &(d.get_dialog_item(dlg_create_legs_IK_id));
	//	item->set_bool(m_createData.create_legs_IK);
	//	item->set_enabled(m_createData.bones_type != human_rig_type_humanIK);
	}
	{
		sxsdk::dialog_item_class* item;
		item = &(d.get_dialog_item(dlg_humanIK_name_id));
		item->set_string(m_createData.name.c_str());
		item->set_enabled(m_createData.bones_type == human_rig_type_humanIK);
	}
	{
		sxsdk::dialog_item_class* item;
		item = &(d.get_dialog_item(dlg_option_eyes_id));
		item->set_bool(m_createData.option_eyes);
		item->set_enabled(m_createData.bones_type == human_rig_type_humanIK);
	}
	{
		sxsdk::dialog_item_class* item;
		item = &(d.get_dialog_item(dlg_option_jaw_id));
		item->set_bool(m_createData.option_jaw);
		item->set_enabled(m_createData.bones_type == human_rig_type_humanIK);
	}
}

// 値の変更を保存するときに呼ばれる.
void CCreateHumanBoneAttributeInterface::save_dialog_data (sxsdk::dialog_interface &d, void *)
{
}

// ダイアログが閉じられるときに呼ばれる.
int CCreateHumanBoneAttributeInterface::terminate (sxsdk::dialog_interface &d, int result, void *)
{
	return result;
}

