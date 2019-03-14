/**
 *  @file   main.cpp
 *  @brief  ボーンのミラーリング複製機能.
 *  @author Yutaka Yoshisaka
 *  @date   2014.08.05 - 2014.09.13.
 */

#include "sxsdk.cxx"
#include "BoneMirrorAttributeInterface.h"
#include "BoneClearAttributeInterface.h"
#include "BoneAutoSizeAttributeInterface.h"
#include "CreateHumanBoneAttributeInterface.h"
#include "CreateBonesMeshAttributeInterface.h"
#include "InitHumanBoneAttributeInterface.h"
#include "RenameHumanBoneAttributeInterface.h"
#include "BoneDirectionAttributeInterface.h"

//**************************************************//
//	グローバル関数									//
//**************************************************//
/**
 * プラグインインターフェースの生成.
 */
extern "C" void STDCALL create_interface (const IID &iid, int i, void **p, sxsdk::shade_interface *shade, void *) {
	unknown_interface *u = NULL;
	
	if(iid == attribute_iid) {
		if(i == 0) {
			u = new CBoneClearAttributeInterface(*shade);
		}
		if(i == 1) {
			u = new CBoneDirectionAttributeInterface(*shade);
		}
		if(i == 2) {
			u = new CBoneAutoSizeAttributeInterface(*shade);
		}
		if(i == 3) {
			u = new CBoneMirrorAttributeInterface(*shade);
		}
		if(i == 4) {
			u = new CInitHumanBoneAttributeInterface(*shade);
		}
		if(i == 5) {
			u = new CCreateHumanBoneAttributeInterface(*shade);
		}
		if(i == 6) {
			u = new CCreateBonesMeshAttributeInterface(*shade);
		}
	}

	if(u) {
		u->AddRef();
		*p = (void *)u;
	}
}

/**
 * インターフェースの数を返す.
 */
extern "C" int STDCALL has_interface (const IID &iid, sxsdk::shade_interface *shade) {
	if (iid == attribute_iid) return 7;
	return 0;
}

/**
 * インターフェース名を返す.
 */
extern "C" const char * STDCALL get_name (const IID &iid, int i, sxsdk::shade_interface *shade, void *) {
	// SXULより、プラグイン名を取得して渡す.
	if (iid == attribute_iid) {
		if (i == 0) {
			return CBoneClearAttributeInterface::name(shade);
		}
		if (i == 1) {
			return CBoneDirectionAttributeInterface::name(shade);
		}
		if (i == 2) {
			return CBoneAutoSizeAttributeInterface::name(shade);
		}
		if (i == 3) {
			return CBoneMirrorAttributeInterface::name(shade);
		}
		if (i == 4) {
			return CInitHumanBoneAttributeInterface::name(shade);
		}
		if (i == 5) {
			return CCreateHumanBoneAttributeInterface::name(shade);
		}
		if (i == 6) {
			return CCreateBonesMeshAttributeInterface::name(shade);
		}
	}
	return 0;
}

/**
 * プラグインのUUIDを返す.
 */
extern "C" sx::uuid_class STDCALL get_uuid (const IID &iid, int i, void *) {
	if (iid == attribute_iid) {
		if (i == 0) {
			return BONECLEAR_ATTRIBUTE_INTERFACE_ID;
		}
		if (i == 1) {
			return BONE_DIRECTION_ATTRIBUTE_INTERFACE_ID;
		}
		if (i == 2) {
			return BONEAUTOSIZE_ATTRIBUTE_INTERFACE_ID;
		}
		if (i == 3) {
			return BONEMIRROR_ATTRIBUTE_INTERFACE_ID;
		}
		if (i == 4) {
			return INIT_HUMANBONE_ATTRIBUTE_INTERFACE_ID;
		}
		if (i == 5) {
			return CREATE_HUMANBONE_ATTRIBUTE_INTERFACE_ID;
		}
		if (i == 6) {
			return CREATE_BONES_MESH_ATTRIBUTE_INTERFACE_ID;
		}
	}

	return sx::uuid_class(0, 0, 0, 0);
}

/**
 * メニューのグループ名.
 */
//extern "C" const char * get_group_name (const IID &iid, sxsdk::shade_interface *shade, void *) {
//	return shade->gettext("boneUtil");
//}

/**
 * バージョン情報.
 */
extern "C" void STDCALL get_info (sxsdk::shade_plugin_info &info, sxsdk::shade_interface *shade, void *) {
	info.sdk_version = SHADE_BUILD_NUMBER;
	info.recommended_shade_version = 410000;
	info.major_version = 1;
	info.minor_version = 1;
	info.micro_version = 0;
	info.build_number =  0;
}

/**
 * 常駐プラグイン.
 */
extern "C" bool STDCALL is_resident (const IID &iid, int i, void *) {
	return true;
}
