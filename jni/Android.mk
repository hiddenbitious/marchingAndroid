# Copyright (C) 2009 The Android Open Source Project
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)

APP_OPTIM		:= release
LOCAL_MODULE    := marchingcubes
LOCAL_CFLAGS    := -Werror -O3
LOCAL_SRC_FILES := 	bbox.cpp vectors.cpp main.cpp camera.cpp frustum.cpp mmath.cpp plane.cpp\
					quaternion.cpp glsl/glsl.cpp metaballs/cubeGrid.cpp timer.cpp\
					debug.cpp
LOCAL_LDLIBS    := -landroid -lEGL -lGLESv2
TARGET_ARCH		:= arm
TARGET_ARCH_ABI	:= armeabi-v7a
LOCAL_STATIC_LIBRARIES := android_native_app_glue

include $(BUILD_SHARED_LIBRARY)

$(call import-module,android/native_app_glue)
