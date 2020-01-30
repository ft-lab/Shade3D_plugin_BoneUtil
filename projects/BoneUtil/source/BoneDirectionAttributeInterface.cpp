/**
 *  @brief  ボーンの「軸方向」をまとめて変更する機能.
 *  @date   2014.09.13 - 2014.09.13.  
 */

#include "BoneDirectionAttributeInterface.h"
#include "BoneUtil.h"
#include "StreamCtrl.h"

enum {
	dlg_fit_direction_id = 101,			// ボーンの進行方向にあわせる.
};

CBoneDirectionAttributeInterface::CBoneDirectionAttributeInterface(sxsdk::shade_interface &shade) : shade(shade)
{
}

CBoneDirectionAttributeInterface::~CBoneDirectionAttributeInterface()
{
}

// カスタム情報が選択された.
bool CBoneDirectionAttributeInterface::ask_shape (sxsdk::shape_class &shape, void *)
{
	compointer<sxsdk::scene_interface> scene(shape.get_scene_interface());
	if (scene == NULL) return false;

	if (!BoneUtil::IsBoneBallJoint(shape)) {
		scene->show_message_box(scene->gettext("msg_not_bone_balljoint"), false);
		return false;
	}

	compointer<sxsdk::dialog_interface> dlg(shade.create_dialog_interface_with_uuid(BONE_DIRECTION_ATTRIBUTE_INTERFACE_ID));
	dlg->set_resource_name("bone_direction_attrib_dlg");
	dlg->set_responder(this);
	this->AddRef();			// set_responder()に合わせて、参照カウンタを増やす。 .

	m_data = StreamCtrl::LoadBonesDirectionInfo(scene);

	// ダイアログの表示.
	if(dlg->ask()) {
		StreamCtrl::SaveBonesDirectionInfo(scene, m_data);
		m_DoBoneDirection(shape);		// ボーンの軸方向の変更.
	}

	return true;
}

// 軸方向の変更処理.
void CBoneDirectionAttributeInterface::m_DoBoneDirection(sxsdk::shape_class &shape)
{
	m_DoBoneDirectionLoop(shape, NULL);
}

void CBoneDirectionAttributeInterface::m_DoBoneDirectionLoop(sxsdk::shape_class& shape, sxsdk::part_class* parentPart)
{
	if (!BoneUtil::IsBoneBallJoint(shape)) return;

	const sxsdk::mat4 lwMat = shape.get_transformation() * shape.get_local_to_world_matrix();		//(parentPart) ? ((parentPart->get_transformation()) * (parentPart->get_local_to_world_matrix())) : shape.get_local_to_world_matrix();
	const sxsdk::mat4 wlMat = inv(lwMat);

	bool firstF = true;
	if (shape.has_son()) {
		sxsdk::part_class* pPart   = &shape.get_part();
		sxsdk::shape_class* pShape = shape.get_son();
		while (pShape->has_bro()) {
			pShape = pShape->get_bro();

			if (firstF) {
				firstF = false;
				if (BoneUtil::IsBone(*pShape)) {
					sxsdk::vec3 v0, v1;

					v0 = BoneUtil::GetBoneCenter(shape, NULL) * wlMat;
					v1 = BoneUtil::GetBoneCenter(*pShape, NULL) * wlMat;

					const sxsdk::vec3 vDir = normalize(v1 - v0);
					try {
						compointer<sxsdk::bone_joint_interface> bone(shape.get_bone_joint_interface());
						if (!(bone->get_auto_direction())) {
							if (m_data.fitDirection) {
								bone->set_axis_dir(vDir);
							} else {
								bone->set_axis_dir(sxsdk::vec3(1, 0, 0));
							}
						}
					} catch (...) { }
				}
			}

			m_DoBoneDirectionLoop(*pShape, pPart);
		}
	}
}

//--------------------------------------------------//
//	ダイアログのイベント処理用						//
//--------------------------------------------------//
// ダイアログの初期化.
void CBoneDirectionAttributeInterface::initialize_dialog (sxsdk::dialog_interface &d, void *)
{
}

// ダイアログのイベントを受け取る.
bool CBoneDirectionAttributeInterface::respond(sxsdk::dialog_interface &d, sxsdk::dialog_item_class &item, int action, void *)
{
	const int id = item.get_id();		// アクションがあったダイアログアイテムのID.

	if (id == dlg_fit_direction_id) {
		m_data.fitDirection = item.get_bool();
		return true;
	}

	return false;
}

// ダイアログのデータを設定する.
void CBoneDirectionAttributeInterface::load_dialog_data (sxsdk::dialog_interface &d, void *)
{
	{
		sxsdk::dialog_item_class* item;
		item = &(d.get_dialog_item(dlg_fit_direction_id));
		item->set_bool(m_data.fitDirection);
	}
}

// 値の変更を保存するときに呼ばれる.
void CBoneDirectionAttributeInterface::save_dialog_data (sxsdk::dialog_interface &d, void *)
{
}

// ダイアログが閉じられるときに呼ばれる.
int CBoneDirectionAttributeInterface::terminate (sxsdk::dialog_interface &d, int result, void *)
{
	return result;
}
