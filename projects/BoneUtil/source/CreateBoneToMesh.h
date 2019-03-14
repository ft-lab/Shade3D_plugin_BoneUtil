/**
 *  @brief  ボーンよりメッシュを生成.
 *  @date   2014.08.10 - 2014.08.10.
 */

#ifndef _CREATEBONETOMESH_H
#define _CREATEBONETOMESH_H

#include "GlobalHeader.h"

class CCreateBoneToMesh {
private:
	sxsdk::shade_interface* m_shade;

	class CBoneInfo {
	public:
		sxsdk::shape_class* pShape;		// 形状のポインタ.
		sxsdk::vec3 wPos;				// ワールド座標位置.
		int parentBoneIndex;			// 親ボーン.
		float boneSize;					// ボーンの大きさ.

		CBoneInfo() {
			pShape = NULL;
			wPos   = sxsdk::vec3(0, 0, 0);
			parentBoneIndex = -1;
			boneSize = 1.0f;
		}
	};

	std::vector<CBoneInfo> m_bonesInfo;		// 一時的に蓄えるボーン情報.

	/**
	 * ボーンリストを取得.
	 */
	void m_GetBonesList(sxsdk::shape_class& boneRootShape);
	void m_GetBonesListLoop(int depth, sxsdk::shape_class& shape, const int parentBoneIndex);

	/**
	 * 指定の始点と終点を結ぶボックスのメッシュ頂点を取得.
	 */
	std::vector<sxsdk::vec3> m_CalcBox(const sxsdk::vec3 stWPos, const sxsdk::vec3 endWPos, const float size);

public:
	CCreateBoneToMesh(sxsdk::shade_interface* shade);
	~CCreateBoneToMesh();

	/**
	 * ボーンからそれを囲むメッシュを作成.
	 */
	sxsdk::shape_class* CreateMesh(sxsdk::scene_interface* scene, sxsdk::shape_class& boneRootShape, const float fatSize = 1.0f);
};

#endif
