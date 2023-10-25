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

#ifndef AI_IPC_IIPCSERVICE_H
#define AI_IPC_IIPCSERVICE_H

#include "IpcCommon.h"

#include <memory>
#include <string>
#include <set>

namespace AI_IPC
{
class IIpcServiceImpl {

public:
    virtual ~IIpcServiceImpl() = default;
    virtual bool isValid() const = 0;
    virtual std::shared_ptr<IAsyncReplyGetter> invokeMethod(const Method& method, const VariantList& args, int timeoutMs = -1) = 0;
    virtual bool invokeMethod(const Method& method, const VariantList& args, VariantList& replyArgs, int timeoutMs = -1) = 0;
    virtual bool emitSignal(const Signal& signal, const VariantList& args) = 0;
    virtual std::string registerMethodHandler(const Method& method, const MethodHandler& handler) = 0;
    virtual std::string registerSignalHandler(const Signal& signal, const SignalHandler& handler) = 0;
    virtual bool unregisterHandler(const std::string& regId) = 0;
    virtual bool enableMonitor(const std::set<std::string>& matchRules, const MonitorHandler& handler) = 0;
    virtual void flush() = 0;
};
class IIpcService {

protected:
static IIpcServiceImpl* impl;

public:

        static void setImpl(IIpcServiceImpl* newImpl)
        {
            impl = newImpl;
        }

        static IIpcService* getInstance()
        {
            static IIpcService* instance =  nullptr;
            if(nullptr != instance)
            {
               new IIpcService();
            }
            return instance;
        }

        static bool isValid() {
           return impl->isValid();
        }
        static std::shared_ptr<IAsyncReplyGetter> invokeMethod(const Method& method, const VariantList& args, int timeoutMs = -1) {
            return impl->invokeMethod(method, args, timeoutMs);
        }

        static bool invokeMethod(const Method& method, const VariantList& args, VariantList& replyArgs, int timeoutMs = -1) {
            return impl->invokeMethod(method, args, replyArgs, timeoutMs);
        }

        static bool emitSignal(const Signal& signal, const VariantList& args) {
            return impl->emitSignal(signal, args);
        }

        static std::string registerMethodHandler(const Method& method, const MethodHandler& handler) {
            return impl->registerMethodHandler(method, handler);
        }

        static std::string registerSignalHandler(const Signal& signal, const SignalHandler& handler) {
            return impl->registerSignalHandler(signal, handler);
        }

        static bool unregisterHandler(const std::string& regId) {
            return impl->unregisterHandler(regId);
        }

        static bool enableMonitor(const std::set<std::string>& matchRules, const MonitorHandler& handler) {
            return impl->enableMonitor(matchRules, handler);
        }

        static void flush() {
            impl->flush();
        }
};

}
#endif
