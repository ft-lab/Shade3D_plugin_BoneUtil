/**
 *   @brief  ボーンの制御点用パートを生成.
 */

#ifndef _INITHUMANBONEATTRIBUTEINTERFACE_H
#define _INITHUMANBONEATTRIBUTEINTERFACE_H

#include "GlobalHeader.h"
#include "RigCtrl.h"


class CInitHumanBoneAttributeInterface : public sxsdk::attribute_interface
{
private:
	sxsdk::shade_interface& shade;
	HumanBoneRigPointsData m_rigPointsData;
	CRigCtrl* m_pRigCtrl;

	/**
	 * ワイヤーフレームの描画処理.
	 */
	virtual void make_wireframe (sxsdk::shape_class &shape, const sxsdk::mat4 &mat, int view, int flags, void *aux=0);

	/**
	 * 形状が移動した.
	 */
	virtual void shapes_moved (bool &b, sxsdk::scene_interface *scene, int n, sxsdk::shape_class *const *shapes, const sxsdk::mat4 &t, void *aux=0);

public:
	CInitHumanBoneAttributeInterface(sxsdk::shade_interface &shade);
	virtual ~CInitHumanBoneAttributeInterface();

	virtual int get_shade_version () const { return SHADE_BUILD_NUMBER; }
	virtual sx::uuid_class get_uuid (void * = 0) { return INIT_HUMANBONE_ATTRIBUTE_INTERFACE_ID; }

	// プラグイン名をSXUL(text.sxul)より取得.
	static const char *name(sxsdk::shade_interface *shade) { return shade->gettext("init_human_bone_title"); }

	// 形状のカスタム情報を有効化.
	virtual void accepts_shape (bool &accept, void *) { accept = true; }

	// カスタム情報が選択された.
	virtual bool ask_shape (sxsdk::shape_class &shape, void * = 0);


	//--------------------------------------------------//
	//	ダイアログのイベント処理用						//
	//--------------------------------------------------//
	// ダイアログの初期化.
	virtual void initialize_dialog (sxsdk::dialog_interface &d, void * = 0);

	// ダイアログのイベントを受け取る.
	virtual bool respond(sxsdk::dialog_interface &d, sxsdk::dialog_item_class &item, int action, void * = 0);

	// ダイアログのデータを設定する.
	virtual void load_dialog_data (sxsdk::dialog_interface &d, void * = 0);

	// 値の変更を保存するときに呼ばれる.
	virtual void save_dialog_data (sxsdk::dialog_interface &d, void * = 0);

	// ダイアログが閉じられるときに呼ばれる.
	virtual int terminate (sxsdk::dialog_interface &d, int result, void * = 0);
};

#endif
