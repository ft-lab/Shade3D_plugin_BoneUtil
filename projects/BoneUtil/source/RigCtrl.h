/**
 * @brief  人体リグ用.
 * @date   2014.08.05 - 2014.08.05.
 */

#ifndef _RIGCTRL_H
#define _RIGCTRL_H

#include "GlobalHeader.h"



class RIG_BONE_INFO {
public:
	int bone_index;					// インデックス.
	int parent_bone_index;			// 親のインデックス.
	int tail_bone_index;			// 子のインデックス (参照のみで実際のボーン構成に影響することはない).
	std::string name_jp;			// 日本語名.
	std::string name_en;			// 英語名.
	std::string name_default;		// Shade 3D向けデフォルト名.
	sxsdk::vec3 pos;				// 身長1700 mm 時の位置.

	sxsdk::shape_class* pShape;

	RIG_BONE_INFO(const int _bone_index, const int _parent_bone_index, const int _tail_bone_index, const std::string _name_jp, const std::string _name_en, const std::string _name_default, const sxsdk::vec3 _pos = sxsdk::vec3(0, 0, 0)) {
		bone_index        = _bone_index;
		parent_bone_index = _parent_bone_index;
		tail_bone_index   = _tail_bone_index;
		name_jp           = _name_jp;
		name_en           = _name_en;
		name_default      = _name_default;
		pos               = _pos;
		pShape            = NULL;
	}
	RIG_BONE_INFO() {
		bone_index = -1;
		pShape     = NULL;
	}
};

/**
 * 人体リグを構成する重要なポイント.
 * これを移動させることで身長や手の長さなどに柔軟に対応させる.
 */
class CRigPoint {
public:
	enum {
		root = 0,			// ルート (Hips).
		upper_body,			// 上半身.
		neck,				// 首.
		head_tail,			// 頭の先.
		shoulder_left,		// 左肩.
		elbow_left,			// 左ひじ.
		wrist_left,			// 左手首.
		finger_tail_left,	// 左手指先.
		shoulder_right,		// 右肩.
		elbow_right,		// 右ひじ.
		wrist_right,		// 右手首.
		finger_tail_right,	// 右手指先.
		leg_left,			// 左足.
		knee_left,			// 左ひざ.
		ankle_left,			// 左足首.
		leg_right,			// 右足.
		knee_right,			// 右ひざ.
		ankle_right,		// 右足首.
		last
	};

	class RIG_POINT_INFO {
	public:
		std::string name;		// 名前.
		sxsdk::vec3 pos;		// 位置.

		RIG_POINT_INFO(const std::string& _name, const sxsdk::vec3& _pos) {
			name = _name;
			pos  = _pos;
		}
	};
	std::vector<RIG_POINT_INFO> rigPoint;		// 基準となる制御点の位置.
	float defaultHeight;						// 元の身長の高さ.

	std::vector<int> rigPointLine;				// 各制御点の接続情報 (上2バイト - 下2バイト、で2つのインデックス).

	std::vector<int> rigMirrorPoint;			// ミラーリング情報.

	CRigPoint(const int pose = human_pose_T) {
		// 1700 mmの身長の場合のポイント位置を格納している.
		rigPoint.push_back(RIG_POINT_INFO("hips"               , sxsdk::vec3(      0,   960.0f,       0)));
		rigPoint.push_back(RIG_POINT_INFO("upper_body"         , sxsdk::vec3(      0,  1175.0f,       0)));
		rigPoint.push_back(RIG_POINT_INFO("neck_center"	       , sxsdk::vec3(      0,  1440.0f,       0)));
		rigPoint.push_back(RIG_POINT_INFO("head_tail"          , sxsdk::vec3(      0,  1682.0f,   28.0f)));
		rigPoint.push_back(RIG_POINT_INFO("shoulder_left"      , sxsdk::vec3(  85.0f,  1370.0f,       0)));
		rigPoint.push_back(RIG_POINT_INFO("elbow_left"         , sxsdk::vec3( 283.0f,  1216.0f,       0)));
		rigPoint.push_back(RIG_POINT_INFO("wrist_left"         , sxsdk::vec3( 432.0f,  1094.0f,       0)));
		rigPoint.push_back(RIG_POINT_INFO("finger_tail_left"   , sxsdk::vec3( 580.0f,  1094.0f,       0)));
		rigPoint.push_back(RIG_POINT_INFO("shoulder_right"     , sxsdk::vec3( -85.0f,  1370.0f,       0)));
		rigPoint.push_back(RIG_POINT_INFO("elbow_right"        , sxsdk::vec3(-283.0f,  1216.0f,       0)));
		rigPoint.push_back(RIG_POINT_INFO("wrist_right"        , sxsdk::vec3(-432.0f,  1094.0f,       0)));
		rigPoint.push_back(RIG_POINT_INFO("finger_tail_right"  , sxsdk::vec3(-580.0f,  1094.0f,       0)));
		rigPoint.push_back(RIG_POINT_INFO("leg_left"           , sxsdk::vec3(  70.0f,   943.0f,       0)));
		rigPoint.push_back(RIG_POINT_INFO("knee_left"          , sxsdk::vec3(  70.0f,   547.0f,   15.0f)));
		rigPoint.push_back(RIG_POINT_INFO("ankle_left"         , sxsdk::vec3(  70.0f,    60.0f,   10.0f)));
		rigPoint.push_back(RIG_POINT_INFO("leg_right"          , sxsdk::vec3( -70.0f,   943.0f,       0)));
		rigPoint.push_back(RIG_POINT_INFO("knee_right"         , sxsdk::vec3( -70.0f,   547.0f,   15.0f)));
		rigPoint.push_back(RIG_POINT_INFO("ankle_right"        , sxsdk::vec3( -70.0f,    60.0f,   10.0f)));

		// T-pose時の位置.
		{
			rigPoint[elbow_left].pos         = sxsdk::vec3(324.0f, 1371.0f, 0.0f);
			rigPoint[wrist_left].pos         = sxsdk::vec3(516.0f, 1373.0f, 0.0f);
			rigPoint[finger_tail_left].pos   = sxsdk::vec3(714.0f, 1373.0f, 0.0f);

			rigPoint[elbow_right].pos        = sxsdk::vec3(-324.0f, 1371.0f, 0.0f);
			rigPoint[wrist_right].pos        = sxsdk::vec3(-516.0f, 1373.0f, 0.0f);
			rigPoint[finger_tail_right].pos  = sxsdk::vec3(-714.0f, 1373.0f, 0.0f);
		}

		// A-pose時の位置.
		if (pose == human_pose_A_30 || pose == human_pose_A_45) {
			const float angleV = ((pose == human_pose_A_30) ? 30.0f : 45.0f) * sx::pi / 180.0f;
			const sxsdk::mat4 rotM = sxsdk::mat4::rotate(sxsdk::vec3(1, 0, 0), sxsdk::vec3((float)std::cos(angleV), -(float)std::sin(angleV), 0));

			{
				const sxsdk::vec3 centerV      = rigPoint[shoulder_left].pos;
				rigPoint[elbow_left].pos       = ((rigPoint[elbow_left].pos - centerV) * rotM) + centerV;
				rigPoint[wrist_left].pos       = ((rigPoint[wrist_left].pos - centerV) * rotM) + centerV;
				rigPoint[finger_tail_left].pos = ((rigPoint[finger_tail_left].pos - centerV) * rotM) + centerV;
			}
			{
				const sxsdk::mat4 invRotM = inv(rotM);
				const sxsdk::vec3 centerV       = rigPoint[shoulder_right].pos;
				rigPoint[elbow_right].pos       = ((rigPoint[elbow_right].pos - centerV) * invRotM) + centerV;
				rigPoint[wrist_right].pos       = ((rigPoint[wrist_right].pos - centerV) * invRotM) + centerV;
				rigPoint[finger_tail_right].pos = ((rigPoint[finger_tail_right].pos - centerV) * invRotM) + centerV;
			}
		}

		defaultHeight = 1700.0f;

		// 制御点同士の接続情報.
		rigPointLine.push_back((upper_body << 16) | (neck));
		rigPointLine.push_back((neck << 16) | (head_tail));
		rigPointLine.push_back((upper_body << 16) | (root));
		rigPointLine.push_back((upper_body << 16) | (shoulder_left));
		rigPointLine.push_back((shoulder_left << 16) | (elbow_left));
		rigPointLine.push_back((elbow_left << 16) | (wrist_left));
		rigPointLine.push_back((wrist_left << 16) | (finger_tail_left));
		rigPointLine.push_back((upper_body << 16) | (shoulder_right));
		rigPointLine.push_back((shoulder_right << 16) | (elbow_right));
		rigPointLine.push_back((elbow_right << 16) | (wrist_right));
		rigPointLine.push_back((wrist_right << 16) | (finger_tail_right));
		rigPointLine.push_back((root << 16) | (leg_left));
		rigPointLine.push_back((leg_left << 16) | (knee_left));
		rigPointLine.push_back((knee_left << 16) | (ankle_left));
		rigPointLine.push_back((root << 16) | (leg_right));
		rigPointLine.push_back((leg_right << 16) | (knee_right));
		rigPointLine.push_back((knee_right << 16) | (ankle_right));

		// ミラーリング情報.
		rigMirrorPoint.push_back(-1);
		rigMirrorPoint.push_back(-1);
		rigMirrorPoint.push_back(-1);
		rigMirrorPoint.push_back(-1);
		rigMirrorPoint.push_back(shoulder_right);
		rigMirrorPoint.push_back(elbow_right);
		rigMirrorPoint.push_back(wrist_right);
		rigMirrorPoint.push_back(finger_tail_right);
		rigMirrorPoint.push_back(shoulder_left);
		rigMirrorPoint.push_back(elbow_left);
		rigMirrorPoint.push_back(wrist_left);
		rigMirrorPoint.push_back(finger_tail_left);
		rigMirrorPoint.push_back(leg_right);
		rigMirrorPoint.push_back(knee_right);
		rigMirrorPoint.push_back(ankle_right);
		rigMirrorPoint.push_back(leg_left);
		rigMirrorPoint.push_back(knee_left);
		rigMirrorPoint.push_back(ankle_left);
	}

	/**
	 * 指定のリグポイントの、指定の身長での位置を取得.
	 */
	sxsdk::vec3 GetRigPoint(int index, const float height) {
		const float scale = height / defaultHeight;
		return rigPoint[index].pos * scale;
	}

	/**
	 * 指定の名称でのインデックスを取得.
	 */
	int GetIndex(const std::string name) {
		int index = -1;
		for (int i = 0; i < rigPoint.size(); i++) {
			if (rigPoint[i].name.compare(name) == 0) {
				index = i;
				break;
			}
		}
		return index;
	}
};

class CRigCtrl
{
private:
	sxsdk::shade_interface* m_shade;
	CRigPoint m_rigPoint;

	void m_CreateMMDHumanRigLoop(const int depth, const int index, sxsdk::scene_interface* scene, const int type, sxsdk::mat4* pParentLWMat, std::vector<RIG_BONE_INFO>& bonesInfo);

	void m_CreateHumanIKRigLoop(const int depth, const int index, sxsdk::scene_interface* scene, const std::string name, sxsdk::mat4* pParentLWMat, std::vector<RIG_BONE_INFO>& bonesInfo);

	/**
	 * 制御点位置より、個々のボーンの位置を再配置.
	 */
	std::vector<RIG_BONE_INFO> m_CalcBonesPos(CRigPoint& rigPoint, const bool humanIK, const bool createEyes = false, const bool createJaw = false);

	/**
	 * 指定の位置に、IK goal用のボールジョイントを生成.
	 */
	std::vector<sxsdk::shape_class *> m_CreateIKGoalBallJoints(sxsdk::scene_interface* scene, const sxsdk::vec3 wPos0, const std::string name0, const sxsdk::vec3 wPos1, const std::string name1);

	/**
	 * 指定のボーンから再帰でたどり、形状名を取得.
	 */
	void m_GetBonesName(const int depth, sxsdk::shape_class& shape, std::vector<std::string>& bonesName);

public:
	CRigCtrl(sxsdk::shade_interface *shade);
	virtual ~CRigCtrl();

	/**
	 * MMD向けのリグ構造のボーン群を作成.
	 */
	void CreateMMDHumanRig(sxsdk::scene_interface* scene, const int type = human_rig_type_mmd_jp, const bool create_legs_ik = true);

	/**
	 * HumanIK向けのリグ構造のボーン群を作成.
	 */
	void CreateHumanIKRig(sxsdk::scene_interface* scene, const std::string name, const bool createEyes, const bool createJaw, const bool create_legs_ik = true);

	/**
	 * 基準となる制御点の位置を指定する形状を作成.
	 * @param[in]  scene   scene_interfaceのポインタ.
	 * @param[in]  pose    TポーズまたはAポーズ (human_pose_T/human_pose_A_30/human_pose_A_45).
	 * @param[in]  height  身長.
	 * @param[in]  mirror  ミラーリング操作に対応するか.
	 */
	void InitializeRigPonts(sxsdk::scene_interface* scene, const int pose = human_pose_T, const float height = 1700.0f, const bool mirror = true);

	/**
	 * シーンより、RigPointsパート内の球の位置で再取得.
	 */
	bool UpdateRigPoints(sxsdk::scene_interface* scene, CRigPoint& rigPoint);
	bool UpdateRigPointsInShape(sxsdk::shape_class* pRigPointsPart, CRigPoint& rigPoint);

	/**
	 * リグの制御点の数を取得.
	 */
	int GetRigPointCount();

	/**
	 * 指定の形状でのリグ(CRigPointで与えたもの)の制御点番号を取得.
	 */
	int GetRigPointIndex(sxsdk::shape_class& shape);

	/**
	 * リグの制御点での接続数を取得.
	 */
	int GetRigPointLinkCount();

	/**
	 * リグの制御点での接続情報を取得.
	 */
	bool GetRigPointLink(const int index, int *pRetP1, int *pRetP2);

	/**
	 * ミラーリングでのリグの対称となる制御点を取得.
	 */
	int GetRigMirrorPoint(const int index);

	/**
	 * 指定の番号の形状を取得.
	 */
	sxsdk::shape_class* GetRigPointShape(sxsdk::scene_interface* scene, const int index, sxsdk::shape_class* pRigTargetPart = NULL);

	/**
	 * 指定のボーン構造がMMDで使用するものとほぼ一致するか調べる.
	 * @param[in]  bone_root          ルートのボーン.
	 * @param[out] pRetHumanRigType   リグの種類（human_rig_type_default/human_rig_type_mmd_jp/human_rig_type_mmd_en）.
	 * @return  完全一致の場合は1.0を返す。1.0に近づくほどMMDのボーンの可能性が高い.
	 */
	float CheckMMDBones(sxsdk::shape_class& bone_root, int* pRetHumanRigType);

	/**
	 * 人体リグの、指定のボーン名に対応するインデックスを取得.
	 */
	static int GetHumanBoneIndex(sxsdk::shade_interface *shade, std::string boneName, const int rigType = human_rig_type_mmd_jp);

	/**
	 * 人体リグの、指定のボーンインデックスに対応する名前を取得.
	 */
	static std::string GetHumanBoneName(sxsdk::shade_interface *shade, const int index, const int rigType = human_rig_type_mmd_jp);

	/**
	 * ガイド用の球を捜し、これらがHumanIK用かMMD用か判定.
	 */
	static bool IsHumanIKGuide(sxsdk::scene_interface* scene);

};

#endif
