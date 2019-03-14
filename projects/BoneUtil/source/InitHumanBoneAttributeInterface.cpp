/**
 *   @brief  ボーンの制御点用パートを生成.
 */

#include "InitHumanBoneAttributeInterface.h"
#include "RigCtrl.h"
#include "StreamCtrl.h"

enum {
	dlg_height_id = 101,		// 身長.
	dlg_pose_id   = 102,		// T-Pose/A-Pose (30 degrees)/A-Pose (45 degrees).
	dlg_mirror_id = 103,		// Mirror.
};

CInitHumanBoneAttributeInterface::CInitHumanBoneAttributeInterface(sxsdk::shade_interface &shade) : shade(shade)
{
	m_pRigCtrl = new CRigCtrl(&shade);
}

CInitHumanBoneAttributeInterface::~CInitHumanBoneAttributeInterface()
{
	if (m_pRigCtrl) delete m_pRigCtrl;
}

// カスタム情報が選択された.
bool CInitHumanBoneAttributeInterface::ask_shape (sxsdk::shape_class &shape, void *)
{
	compointer<sxsdk::scene_interface> scene(shape.get_scene_interface());
	if (scene == NULL) return false;

	m_rigPointsData = StreamCtrl::LoadHumanBoneRigInfo(scene);

	compointer<sxsdk::dialog_interface> dlg(shade.create_dialog_interface_with_uuid(INIT_HUMANBONE_ATTRIBUTE_INTERFACE_ID));
	dlg->set_resource_name("create_rig_points_dlg");
	dlg->set_responder(this);
	this->AddRef();			// set_responder()に合わせて、参照カウンタを増やす。 .

	// ダイアログの表示.
	if(dlg->ask()) {
		CRigCtrl rigCtrl(&shade);
		rigCtrl.InitializeRigPonts(scene, m_rigPointsData.pose_type, m_rigPointsData.height, m_rigPointsData.mirror);
		StreamCtrl::SaveHumanBoneRigInfo(scene, m_rigPointsData);
	}

	return true;
}

/**
 * ワイヤーフレームの描画処理.
 */
void CInitHumanBoneAttributeInterface::make_wireframe (sxsdk::shape_class &shape, const sxsdk::mat4 &mat, int view, int flags, void *aux)
{
	if (!shape.has_son()) return;

	const sxsdk::mat4 trans = sxsdk::mat4::scale(sxsdk::vec3(50.0f, 50.0f, 50.0f));

	// 個々の制御点位置を取得.
	const sxsdk::mat4 lwMat = shape.get_transformation() * mat;

	sxsdk::shape_class* pShape = shape.get_son();
	const int rigPointCou = m_pRigCtrl->GetRigPointCount();
	std::vector<sxsdk::vec3> rigPointPos;
	rigPointPos.resize(rigPointCou);
	int cou = 0;
	while (pShape->has_bro()) {
		pShape = pShape->get_bro();
		const int rIndex = m_pRigCtrl->GetRigPointIndex(*pShape);
		if (rIndex >= 0) {
			if (rIndex < rigPointCou) {
				rigPointPos[rIndex] = pShape->get_sphere().get_center();
			}
			cou++;
		}
	}
	if (cou != rigPointCou) return;

	// 制御点同士を線で結ぶ.
	std::vector<sxsdk::vec3> posA;
	posA.resize(2);
	const int linkCou = m_pRigCtrl->GetRigPointLinkCount();
	int p1, p2;
	for (int i = 0; i < linkCou; i++) {
		m_pRigCtrl->GetRigPointLink(i, &p1, &p2);
		posA[0] = rigPointPos[p1];
		posA[1] = rigPointPos[p2];
		shape.make_line_wireframe(lwMat, 2, &(posA[0]));
	}
}

/**
 * 形状が移動した.
 */
void CInitHumanBoneAttributeInterface::shapes_moved (bool &b, sxsdk::scene_interface *scene, int n, sxsdk::shape_class *const *shapes, const sxsdk::mat4 &t, void *aux)
{
	bool loadStreamF = false;
	HumanBoneRigPointsData humanRigsInfo;
	sxsdk::vec3 rootWPos;
	sxsdk::shape_class* pRigPart = NULL;

	for (int i = 0; i < n; i++) {
		if (shapes[i]->get_type() != sxsdk::enums::sphere) continue;

		// リグの制御点かどうか.
		const int rIndex = m_pRigCtrl->GetRigPointIndex(*shapes[i]);
		if (rIndex < 0) continue;

		// 設定の読み込み.
		if (!loadStreamF) {
			loadStreamF   = true;
			humanRigsInfo = StreamCtrl::LoadHumanBoneRigInfo(scene);
			if (!humanRigsInfo.mirror) break;
			pRigPart      = scene->get_shape_by_name(RIGPOINT_PART_NAME);
			if (!pRigPart) break;

			// 制御点のルートの球を取得.
			sxsdk::shape_class* pRootShape = m_pRigCtrl->GetRigPointShape(scene, 0, pRigPart);
			if (!pRootShape) break;

			// ルート形状のワールド位置.
			rootWPos = (pRootShape->get_sphere().get_center()) * (pRootShape->get_local_to_world_matrix());
		}

		// ミラーの対称の制御点番号を取得.
		const int mirrorIndex = m_pRigCtrl->GetRigMirrorPoint(rIndex);
		if (mirrorIndex < 0) continue;
		sxsdk::shape_class* mirrorShape = m_pRigCtrl->GetRigPointShape(scene, mirrorIndex, pRigPart);
		if (!mirrorShape) continue;

		const sxsdk::vec3 wPos = (shapes[i]->get_sphere().get_center()) * (shapes[i]->get_local_to_world_matrix());
		sxsdk::vec3 wMirrorPos(-(wPos.x - rootWPos.x) + rootWPos.x, wPos.y, wPos.z);

		const sxsdk::mat4 mirrorLWMat = mirrorShape->get_local_to_world_matrix();
		mirrorShape->get_sphere().set_center(wMirrorPos * inv(mirrorLWMat));
	}
}

//--------------------------------------------------//
//	ダイアログのイベント処理用						//
//--------------------------------------------------//
// ダイアログの初期化.
void CInitHumanBoneAttributeInterface::initialize_dialog (sxsdk::dialog_interface &d, void *)
{
}

// ダイアログのイベントを受け取る.
bool CInitHumanBoneAttributeInterface::respond(sxsdk::dialog_interface &d, sxsdk::dialog_item_class &item, int action, void *)
{
	const int id = item.get_id();		// アクションがあったダイアログアイテムのID.

	if (id == dlg_height_id) {
		m_rigPointsData.height = item.get_float();
		return true;
	}
	if (id == dlg_pose_id) {
		m_rigPointsData.pose_type = item.get_selection();
		return true;
	}
	if (id == dlg_mirror_id) {
		m_rigPointsData.mirror = item.get_bool();
		return true;
	}

	return false;
}

// ダイアログのデータを設定する.
void CInitHumanBoneAttributeInterface::load_dialog_data (sxsdk::dialog_interface &d, void *)
{
	{
		sxsdk::dialog_item_class* item;
		item = &(d.get_dialog_item(dlg_height_id));
		item->set_float(m_rigPointsData.height);
	}
	{
		sxsdk::dialog_item_class* item;
		item = &(d.get_dialog_item(dlg_pose_id));
		item->set_selection(m_rigPointsData.pose_type);
	}
	{
		sxsdk::dialog_item_class* item;
		item = &(d.get_dialog_item(dlg_mirror_id));
		item->set_bool(m_rigPointsData.mirror);
	}
}

// 値の変更を保存するときに呼ばれる.
void CInitHumanBoneAttributeInterface::save_dialog_data (sxsdk::dialog_interface &d, void *)
{
}

// ダイアログが閉じられるときに呼ばれる.
int CInitHumanBoneAttributeInterface::terminate (sxsdk::dialog_interface &d, int result, void *)
{
	return result;
}
