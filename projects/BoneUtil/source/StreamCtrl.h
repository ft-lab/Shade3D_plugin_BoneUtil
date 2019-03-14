/**
 *  @brief   stream保存、読み込み用.
 */

#ifndef _STREAMCTRL_H
#define _STREAMCTRL_H

#include "GlobalHeader.h"

namespace StreamCtrl {
	/**
	 * リグ構成の制御点を生成する際の情報を取得.
	 */
	HumanBoneRigPointsData LoadHumanBoneRigInfo(sxsdk::scene_interface* scene);

	/**
	 * リグ構成の制御点を生成する際の情報を保持.
	 */
	void SaveHumanBoneRigInfo(sxsdk::scene_interface *scene, const HumanBoneRigPointsData& data);

	/**
	 * リグ構成のからボーンを生成する際の情報を取得.
	 */
	HumanBoneCreateData LoadHumanBoneCreateInfo(sxsdk::scene_interface* scene);

	/**
	 * リグ構成のからボーンを生成する際の情報を保持.
	 */
	void SaveHumanBoneCreateInfo(sxsdk::scene_interface *scene, const HumanBoneCreateData& data);

	/**
	 * ボーンからメッシュを生成する際の情報を取得.
	 */
	BonesMeshCreateData LoadBonesMeshCreateInfo(sxsdk::scene_interface* scene);

	/**
	 * ボーンからメッシュを生成する際の情報を保持.
	 */
	void SaveBonesMeshCreateInfo(sxsdk::scene_interface* scene, const BonesMeshCreateData& data);

	/**
	 * ボーン名を変換する際の情報を取得.
	 */
	HumanBoneRenameData LoadHumanBoneRenameInfo(sxsdk::scene_interface* scene);

	/**
	 * ボーン名を変換する際の情報を保持.
	 */
	void SaveHumanBoneRenameInfo(sxsdk::scene_interface *scene, const HumanBoneRenameData& data);

	/**
	 * ボーンクリア時の情報を保持.
	 */
	void SaveBonesClearInfo(sxsdk::scene_interface *scene, const BoneClearData& data);

	/**
	 * ボーンクリア時の情報を取得.
	 */
	BoneClearData LoadBonesClearInfo(sxsdk::scene_interface* scene);

	/**
	 * ボーンの軸方向の情報を保持.
	 */
	void SaveBonesDirectionInfo(sxsdk::scene_interface *scene, const BoneDirectionData& data);

	/**
	 * ボーンの軸方向の情報を取得.
	 */
	BoneDirectionData LoadBonesDirectionInfo(sxsdk::scene_interface* scene);

}

#endif
