/**
 *  @brief   ボーン制御用のユーティリティー.
 *  @date    2014.08.07 - 2014.09.12.
 */

#ifndef _BONEUTIL_H
#define _BONEUTIL_H

#include "GlobalHeader.h"

namespace BoneUtil
{
	/**
	 * テキストをSJISに変換.
	 */
	std::string ConvUTF8ToSJIS(sxsdk::shade_interface& shade, const std::string str);

	/**
	 * テキストをSJISからUTF-8に変換.
	 */
	std::string ConvSJISToUTF8(sxsdk::shade_interface& shade, const std::string str);

	/**
	 * C/C++上に埋め込まれたテキストをUTF-8として取得.
	 * Win環境の場合は、C/C++のテキストはSJISになっている.
	 * MacはUTF-8かも.
	 */
	std::string GetUTF8Text(sxsdk::shade_interface& shade, const std::string str);

	/**
	 * 指定の形状がボーンかどうか.
	 */
	bool IsBone (sxsdk::shape_class& shape);

	/**
	 * 指定の形状がボールジョイントかどうか.
	 */
	bool IsBallJoint (sxsdk::shape_class& shape);

	/**
	 * 指定の形状がボーンまたはボールジョイントかどうか.
	 */
	bool IsBoneBallJoint (sxsdk::shape_class& shape);

	/**
	 * 指定のボーンの親をたどっていくと、ルートボーンにたどりつくか.
	 */
	bool ChkRootBone(sxsdk::shape_class& shape, sxsdk::shape_class& rootBone);

	/**
	 * ボーンのワールド座標での中心位置とボーンサイズを取得.
	 */
	sxsdk::vec3 GetBoneCenter(sxsdk::shape_class& shape, float *size);

	/**
	 * ボールジョイントのワールド座標での中心位置とボールジョイントサイズを取得.
	 */
	sxsdk::vec3 GetBallJointCenter(sxsdk::shape_class& shape, float *size);

	/**
	 * ボーンまたはボールジョイントのワールド座標での中心位置とサイズを取得.
	 */
	sxsdk::vec3 GetBoneBallJointCenter(sxsdk::shape_class& shape, float *size);

	/**
	 * 指定のポリゴンメッシュに割り当てられているボーン/ボールジョイントのルートを取得.
	 */
	sxsdk::shape_class* GetBoneRoot(sxsdk::shape_class& shapePolygonMesh);

	/**
	 * シーン内に存在するボーン/ボールジョイントとメッシュの組み合わせを取得.
	 */
	int GetBoneMeshsList(sxsdk::scene_interface* scene, std::vector<sxsdk::shape_class *>& retMeshList, std::vector<sxsdk::shape_class *>& retBoneRootList);

}

#endif
