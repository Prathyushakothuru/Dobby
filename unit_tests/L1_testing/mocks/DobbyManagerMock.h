/* If not stated otherwise in this file or this component's LICENSE file the
# following copyright and licenses apply:
#
# Copyright 2023 RDK Management
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
# http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
*/

#pragma once

#include "DobbyManager.h"
#include <gmock/gmock.h>

class  DobbyManagerMock : public DobbyManagerImpl {

public:

    MOCK_METHOD(int32_t, startContainerFromSpec, (const ContainerId& id,
                                              const std::string& jsonSpec,
                                              const std::list<int>& files,
                                              const std::string& command,
                                              const std::string& displaySocket,
                                              const std::vector<std::string>& envVars),(const));

    MOCK_METHOD(int32_t, startContainerFromBundle, (const ContainerId& id,
                                                const std::string& bundlePath,
                                                const std::list<int>& files,
                                                const std::string& command,
                                                const std::string& displaySocket,
                                                const std::vector<std::string>& envVars), (const));

    MOCK_METHOD(bool, stopContainer, (int32_t cd, bool withPrejudice), ());

    MOCK_METHOD(bool, pauseContainer, (int32_t cd), ());

    MOCK_METHOD(bool, resumeContainer, (int32_t cd), ());

    MOCK_METHOD(bool, execInContainer, (int32_t cd,
                                   const std::string& options,
                                   const std::string& command), ());

    MOCK_METHOD((std::list<std::pair<int32_t, ContainerId>>), listContainers, (), (const));

    MOCK_METHOD(int32_t, stateOfContainer, (int32_t cd), (const));

    MOCK_METHOD(std::string, statsOfContainer, (int32_t cd), (const));

    MOCK_METHOD(std::string, ociConfigOfContainer, (int32_t cd), (const));

    MOCK_METHOD(std::string, specOfContainer, (int32_t cd), (const));

    MOCK_METHOD(bool, createBundle, (const ContainerId& id, const std::string& jsonSpec), ());

};
