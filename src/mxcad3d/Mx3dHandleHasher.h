/******************************************************************************************
Copyright (c) 2025 Chengdu Dreamkaide Technology Co., Ltd. <https://www.webcadsdk.com/>
All rights reserved.
Applications incorporating this software must include the following copyright notice.
This application shall enter into an agreement with Chengdu Dreamkaide Technology Co., Ltd.
for the use of this software, its documentation or related materials.
*******************************************************************************************/

#pragma once

#include <Standard_Handle.hxx>

namespace std {

	template<typename T> struct hash<opencascade::handle<T>> {
		inline std::size_t operator()(const opencascade::handle<T>& hnd) const {
			return hash<T*>{}(hnd.get());
		}
	};
}