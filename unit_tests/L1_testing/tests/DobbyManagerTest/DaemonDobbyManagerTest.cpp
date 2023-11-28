/*
* If not stated otherwise in this file or this component's LICENSE file the
* following copyright and licenses apply:
*
* Copyright 2023 Synamedia
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
* http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*/

#include <gtest/gtest.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/prctl.h>
#include <sys/resource.h>
#include <fstream>
#include <unordered_map>
#include <chrono>
#include <thread>

#include "DobbyRdkPluginUtilsMock.h"
#include "DobbyLoggerMock.h"

#include "DobbyRunCMock.h"
#include "IDobbyUtilsMock.h"

#include "DobbyManager.h"
#include "DobbyContainerMock.h"
#include "DobbyRdkPluginManagerMock.h"
#include "DobbyStartStateMock.h"
#include "DobbyRootfsMock.h"

#if defined(LEGACY_COMPONENTS)
#include "DobbySpecConfigMock.h"
#endif //defined(LEGACY_COMPONENTS)

#include "DobbyBundleMock.h"
#include "DobbyConfigMock.h"
#include "DobbyBundleConfigMock.h"
#include "IDobbySettingsMock.h"
#include "IDobbyIPCUtilsMock.h"
#include "IDobbyEnvMock.h"
#include "IAsyncReplySenderMock.h"
#include "DobbyStatsMock.h"
#include "DobbyFileAccessFixerMock.h"
#include "DobbyRunCMock.h"
#include "DobbyLegacyPluginManagerMock.h"
#include "DobbyStreamMock.h"
#include "ContainerIdMock.h"
#include "IDobbyRdkLoggingPluginMock.h"
#include "IpcFileDescriptorMock.h"

#include <unistd.h>
#define TEST_LOG(x, ...) fprintf(stderr, "\033[1;32m[%s:%d](%s)<PID:%d><TID:%d>" x "\n\033[0m", __FILE__, __LINE__, __FUNCTION__, getpid(), gettid(), ##__VA_ARGS__); fflush(stderr); 

DobbyContainerImpl* DobbyContainer::impl = nullptr;
DobbyRdkPluginManagerImpl* DobbyRdkPluginManager::impl = nullptr;
DobbyRootfsImpl* DobbyRootfs::impl = nullptr;

#if defined(LEGACY_COMPONENTS)
DobbySpecConfigImpl* DobbySpecConfig::impl = nullptr;
#endif //defined(LEGACY_COMPONENTS)

DobbyBundleImpl* DobbyBundle::impl = nullptr;
DobbyConfigImpl* DobbyConfig::impl = nullptr;
DobbyBundleConfigImpl* DobbyBundleConfig::impl = nullptr;
DobbyRdkPluginUtilsImpl* DobbyRdkPluginUtils::impl = nullptr;
DobbyStartStateImpl* DobbyStartState::impl = nullptr;
AI_IPC::IAsyncReplySenderApiImpl* AI_IPC::IAsyncReplySender::impl = nullptr;
ContainerIdImpl* ContainerId::impl = nullptr;
DobbyFileAccessFixerImpl* DobbyFileAccessFixer::impl = nullptr;
DobbyLoggerImpl* DobbyLogger::impl = nullptr;
DobbyRunCImpl* DobbyRunC::impl = nullptr;
DobbyBufferStreamImpl* DobbyBufferStream::impl = nullptr;
DobbyLegacyPluginManagerImpl* DobbyLegacyPluginManager::impl = nullptr;
DobbyStatsImpl* DobbyStats::impl = nullptr;
AI_IPC::IpcFileDescriptorApiImpl* AI_IPC::IpcFileDescriptor::impl = nullptr;

using ::testing::NiceMock;

class DaemonDobbyManagerTest : public ::testing::Test {

protected:

void onContainerStarted(int32_t cd, const ContainerId& id)
{
}

void onContainerStopped(int32_t cd, const ContainerId& id, int status)
{
}

    typedef std::function<void(int32_t cd, const ContainerId& id)> ContainerStartedFunc;
    typedef std::function<void(int32_t cd, const ContainerId& id, int32_t status)> ContainerStoppedFunc;

    ContainerStartedFunc startcb =
        std::bind(&DaemonDobbyManagerTest::onContainerStarted, this,
                  std::placeholders::_1, std::placeholders::_2);

    ContainerStoppedFunc stopcb =
        std::bind(&DaemonDobbyManagerTest::onContainerStopped, this,
                  std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);

    DobbyContainerMock*  p_containerMock = nullptr;
    DobbyRdkPluginManagerMock*  p_rdkPluginManagerMock = nullptr;
    DobbyStartStateMock*  p_startStateMock = nullptr;
    DobbyRootfsMock*  p_rootfsMock = nullptr;

#if defined(LEGACY_COMPONENTS)
    DobbySpecConfigMock*  p_specConfigMock = nullptr;
#endif //defined(LEGACY_COMPONENTS)

    DobbyBundleMock*  p_bundleMock = nullptr;
    DobbyConfigMock*  p_configMock = nullptr;
    DobbyBundleConfigMock*  p_bundleConfigMock = nullptr;
    DobbyRdkPluginUtilsMock*  p_rdkPluginUtilsMock = nullptr;
    AI_IPC::IAsyncReplySenderMock*  p_asyncReplySenderMock = nullptr;
    ContainerIdMock*  p_containerIdMock = nullptr;
    DobbyFileAccessFixerMock*  p_fileAccessFixerMock = nullptr;
    DobbyRunCMock*  p_runcMock = nullptr;
    DobbyStreamMock*  p_streamMock = nullptr;
    DobbyLegacyPluginManagerMock *  p_legacyPluginManagerMock = nullptr;
    DobbyStatsMock*  p_statsMock = nullptr;
    DobbyLoggerMock*  p_loggerMock = nullptr;

    DobbyContainer *p_dobbyContainer = nullptr;
    DobbyRdkPluginManager *p_rdkPluginManager = nullptr;
    DobbyRootfs *p_rootfs = nullptr;

#if defined(LEGACY_COMPONENTS)
    DobbySpecConfig *p_specConfig = nullptr;
#endif //defined(LEGACY_COMPONENTS)

    DobbyStartState *p_startState = nullptr;
    DobbyBundle *p_bundle = nullptr;
    DobbyConfig *p_config = nullptr;
    DobbyBundleConfig *p_bundleConfig = nullptr;
    DobbyRdkPluginUtils *p_rdkPluginUtils = nullptr;
    AI_IPC::IAsyncReplySender *p_iasyncReplySender = nullptr;
    ContainerId *p_containerId = nullptr;
    DobbyFileAccessFixer *p_fileAccessFixer = nullptr;
    DobbyRunC *p_runc = nullptr;
    DobbyBufferStream *p_stream = nullptr;
    DobbyLegacyPluginManager *p_legacyPluginManager = nullptr;
    DobbyStats *p_stats = nullptr;
    DobbyLogger *p_logger = nullptr;
    IDobbyUtils *p_idobbyUtils = nullptr;
    std::shared_ptr<IDobbyEnv>  p_idobbyenvMock =  nullptr;
    std::shared_ptr<IDobbyUtils>  p_idobbyutilsMock =  nullptr;
    std::shared_ptr<IDobbyIPCUtils>  p_idobbyipcutilsMock =  nullptr;
    std::shared_ptr<IDobbySettings>  p_idobbysettingsMock =  nullptr;

    std::shared_ptr<DobbyManager> dobbyManager_test;

    virtual void SetUp()
    {
        p_containerMock = new NiceMock <DobbyContainerMock>;
        p_rdkPluginManagerMock = new NiceMock <DobbyRdkPluginManagerMock>;
        p_startStateMock = new NiceMock <DobbyStartStateMock>;
        p_rootfsMock = new NiceMock <DobbyRootfsMock>;

#if defined(LEGACY_COMPONENTS)
        p_specConfigMock = new NiceMock <DobbySpecConfigMock>;
#endif //defined(LEGACY_COMPONENTS)

        p_bundleMock = new NiceMock <DobbyBundleMock>;
        p_configMock = new NiceMock <DobbyConfigMock>;
        p_bundleConfigMock = new NiceMock <DobbyBundleConfigMock>;
        p_rdkPluginUtilsMock = new NiceMock <DobbyRdkPluginUtilsMock>;
        p_asyncReplySenderMock = new NiceMock <AI_IPC::IAsyncReplySenderMock>;
        p_containerIdMock= new NiceMock <ContainerIdMock>;
        p_fileAccessFixerMock = new NiceMock <DobbyFileAccessFixerMock>;
        p_runcMock = new NiceMock <DobbyRunCMock>;
        p_streamMock = new NiceMock <DobbyStreamMock>;
        p_legacyPluginManagerMock = new NiceMock <DobbyLegacyPluginManagerMock>;
        p_statsMock = new NiceMock <DobbyStatsMock>;
        p_loggerMock = new NiceMock <DobbyLoggerMock>;

        p_dobbyContainer =  DobbyContainer::getInstance();
        EXPECT_NE(p_dobbyContainer, nullptr);
        p_rdkPluginManager =  DobbyRdkPluginManager::getInstance();
        EXPECT_NE(p_rdkPluginManager, nullptr);
        p_rootfs =  DobbyRootfs::getInstance();
        EXPECT_NE(p_rootfs, nullptr);
#if defined(LEGACY_COMPONENTS)
        p_specConfig =  DobbySpecConfig::getInstance();
        EXPECT_NE(p_specConfig, nullptr);
#endif //defined(LEGACY_COMPONENTS)

        p_startState =  DobbyStartState::getInstance();
        EXPECT_NE(p_startState, nullptr);
        p_bundle =  DobbyBundle::getInstance();
        EXPECT_NE(p_bundle, nullptr);
        p_config =  DobbyConfig::getInstance();
        EXPECT_NE(p_config, nullptr);
        p_bundleConfig =  DobbyBundleConfig::getInstance();
        EXPECT_NE(p_bundleConfig, nullptr);
        p_rdkPluginUtils =  DobbyRdkPluginUtils::getInstance();
        EXPECT_NE(p_rdkPluginUtils, nullptr);
        p_containerId =  ContainerId::getInstance();
        EXPECT_NE(p_containerId, nullptr);
        p_fileAccessFixer =  DobbyFileAccessFixer::getInstance();
        EXPECT_NE(p_fileAccessFixer, nullptr);
        p_logger =  DobbyLogger::getInstance();
        EXPECT_NE(p_logger, nullptr);
        p_runc =  DobbyRunC::getInstance();
        EXPECT_NE(p_runc, nullptr);
        p_stream =  DobbyBufferStream::getInstance();
        EXPECT_NE(p_stream, nullptr);
        p_legacyPluginManager =  DobbyLegacyPluginManager::getInstance();
        EXPECT_NE(p_legacyPluginManager, nullptr);
        p_stats =  DobbyStats::getInstance();
        EXPECT_NE(p_stats, nullptr);

        p_dobbyContainer->setImpl(p_containerMock);
        p_rdkPluginManager->setImpl(p_rdkPluginManagerMock);
        p_rootfs->setImpl(p_rootfsMock);
        p_startState->setImpl(p_startStateMock);
#if defined(LEGACY_COMPONENTS)
        p_specConfig->setImpl(p_specConfigMock);
#endif //defined(LEGACY_COMPONENTS)

        p_bundle->setImpl(p_bundleMock);
        p_config->setImpl(p_configMock);
        p_bundleConfig->setImpl(p_bundleConfigMock);
        p_rdkPluginUtils->setImpl(p_rdkPluginUtilsMock);
        p_iasyncReplySender->setImpl(p_asyncReplySenderMock);
        p_containerId->setImpl(p_containerIdMock);
        p_fileAccessFixer->setImpl(p_fileAccessFixerMock);
        p_logger->setImpl(p_loggerMock);
        p_runc->setImpl(p_runcMock);
        p_stream->setImpl(p_streamMock);
        p_legacyPluginManager->setImpl(p_legacyPluginManagerMock);
        p_stats->setImpl(p_statsMock);

        p_idobbyenvMock =  std::make_shared<NiceMock<IDobbyEnvMock>>();
        p_idobbyutilsMock =  std::make_shared<NiceMock<IDobbyUtilsMock>>();
        p_idobbyipcutilsMock =  std::make_shared<NiceMock<IDobbyIPCUtilsMock>>();
        p_idobbysettingsMock = std::make_shared<NiceMock<IDobbySettingsMock>>();

        EXPECT_CALL(dynamic_cast<IDobbyUtilsMock&>(*p_idobbyutilsMock),writeTextFile(::testing::_, ::testing::_, ::testing::_, ::testing::_))
            .Times(1)
                .WillOnce(::testing::Invoke(
                [](const std::string& path,const std::string& str,int flags, mode_t mode){
                    return true;
        }));

        const std::string expectedWorkDir = "unit_tests/L1_testing/tests";
        EXPECT_CALL(*p_runcMock, getWorkingDir())
            .Times(1)
                .WillOnce(::testing::Return(expectedWorkDir));

            dobbyManager_test = std::make_shared<NiceMock<DobbyManager>>(p_idobbyenvMock,p_idobbyutilsMock,p_idobbyipcutilsMock,p_idobbysettingsMock,startcb,stopcb);

    }

    virtual void TearDown()
    {
        dobbyManager_test.reset();
                
        p_idobbyenvMock.reset();
        p_idobbyutilsMock.reset();
        p_idobbyipcutilsMock.reset();
        p_idobbysettingsMock.reset();

        p_dobbyContainer->setImpl(nullptr);
        p_rdkPluginManager->setImpl(nullptr);
        p_rootfs->setImpl(nullptr);
        p_startState->setImpl(nullptr);
#if defined(LEGACY_COMPONENTS)
        p_specConfig->setImpl(nullptr);
#endif //defined(LEGACY_COMPONENTS)

        p_bundle->setImpl(nullptr);
        p_config->setImpl(nullptr);
        p_bundleConfig->setImpl(nullptr);
        p_rdkPluginUtils->setImpl(nullptr);
        p_iasyncReplySender->setImpl(nullptr);
        p_containerId->setImpl(nullptr);
        p_fileAccessFixer->setImpl(nullptr);
        p_logger->setImpl(nullptr);
        p_runc->setImpl(nullptr);
        p_stream->setImpl(nullptr);
        p_legacyPluginManager->setImpl(nullptr);
        p_stats->setImpl(nullptr);

        if( p_rdkPluginManagerMock != nullptr)
        {
            delete  p_rdkPluginManagerMock;
            p_rdkPluginManagerMock = nullptr;
        }

        if( p_startStateMock != nullptr)
        {
            delete  p_startStateMock;
            p_startStateMock = nullptr;
        }

        if( p_rootfsMock != nullptr)
        {
            delete  p_rootfsMock;
            p_rootfsMock = nullptr;
        }

#if defined(LEGACY_COMPONENTS)
        if( p_specConfigMock != nullptr)
        {
            delete  p_specConfigMock;
            p_specConfigMock = nullptr;
        }
#endif //defined(LEGACY_COMPONENTS)

        if( p_bundleMock != nullptr)
        {
            delete  p_bundleMock;
            p_bundleMock = nullptr;
        }

        if( p_configMock != nullptr)
        {
            delete  p_configMock;
            p_configMock = nullptr;
        }

        if( p_bundleConfigMock != nullptr)
        {
            delete  p_bundleConfigMock;
            p_bundleConfigMock = nullptr;
        }

        if( p_rdkPluginUtilsMock != nullptr)
        {
            delete  p_rdkPluginUtilsMock;
            p_rdkPluginUtilsMock = nullptr;
        }

        if( p_asyncReplySenderMock != nullptr)
        {
            delete  p_asyncReplySenderMock;
            p_asyncReplySenderMock = nullptr;
        }

        if( p_containerIdMock != nullptr)
        {
            delete  p_containerIdMock;
            p_containerIdMock = nullptr;
        }

        if( p_fileAccessFixerMock != nullptr)
        {
            delete  p_fileAccessFixerMock;
            p_fileAccessFixerMock = nullptr;
        }

        if( p_loggerMock != nullptr)
        {
            delete  p_loggerMock;
            p_loggerMock = nullptr;
        }

        if( p_runcMock != nullptr)
        {
            delete  p_runcMock;
            p_runcMock = nullptr;
        }

        if( p_streamMock != nullptr)
        {
            delete  p_streamMock;
            p_streamMock = nullptr;
        }

        if( p_legacyPluginManagerMock != nullptr)
        {
            delete  p_legacyPluginManagerMock;
            p_legacyPluginManagerMock = nullptr;
        }

        if( p_statsMock != nullptr)
        {
            delete  p_statsMock;
            p_statsMock = nullptr;
        }

        if(p_containerMock != nullptr)
        {
            delete  p_containerMock;
            p_containerMock = nullptr;
        }

    }

    void expect_startContainerFromBundle(int32_t cd)
    {
    EXPECT_CALL(*p_bundleConfigMock, isValid())
        .Times(1)
            .WillOnce(::testing::Return(true));

    EXPECT_CALL(*p_bundleMock, isValid())
        .Times(1)
            .WillOnce(::testing::Return(true));

    EXPECT_CALL(*p_rootfsMock, isValid())
        .Times(1)
           .WillOnce(::testing::Return(true));

    EXPECT_CALL(*p_startStateMock, isValid())
        .Times(1)
            .WillOnce(::testing::Return(true));

    std::map<std::string, Json::Value> sampleData;
    sampleData["plugin1"] = Json::Value("value1");
    sampleData["plugin2"] = Json::Value("value2");

// Set the expectation to return the sample data
    EXPECT_CALL(*p_bundleConfigMock, rdkPlugins())
        .Times(1)
            .WillOnce(::testing::ReturnRef(sampleData));

    const std::string validPath = "/unit_tests/L1_testing/tests/";

// Set the expectation to return the valid path
    EXPECT_CALL(*p_rootfsMock, path())
        .Times(4)
            .WillOnce(::testing::ReturnRef(validPath))
            .WillOnce(::testing::ReturnRef(validPath))
            .WillOnce(::testing::ReturnRef(validPath))
            .WillOnce(::testing::ReturnRef(validPath));

    std::string valid_path = "/unit_tests/L1_testing/tests/DobbyManagerTest/DaemonDobbyManagerTest.cpp";

    EXPECT_CALL(*p_bundleMock, path())
        .Times(4)
            .WillOnce(::testing::ReturnRef(valid_path))
            .WillOnce(::testing::ReturnRef(valid_path))

            .WillOnce(::testing::ReturnRef(valid_path))
            .WillOnce(::testing::ReturnRef(valid_path));

    EXPECT_CALL(*p_bundleConfigMock, config())
        .Times(2)
            .WillOnce(::testing::Return(std::make_shared<rt_dobby_schema>()))
            .WillOnce(::testing::Return(std::make_shared<rt_dobby_schema>()));

    const std::vector<std::string> expectedStrings = {"string1", "string2", "string3"};

    EXPECT_CALL(*p_rdkPluginManagerMock, listLoadedPlugins())
        .Times(1)
            .WillOnce(::testing::Return(expectedStrings));

    std::map<std::string, Json::Value> data;
    data["key1"] = Json::Value("value1");
    data["key2"] = Json::Value("value2");

    EXPECT_CALL(*p_configMock, legacyPlugins())
        .Times(3)
            .WillOnce(testing::ReturnRef(data))
            .WillOnce(testing::ReturnRef(data))
            .WillOnce(testing::ReturnRef(data));

    EXPECT_CALL(*p_legacyPluginManagerMock, executePostConstructionHooks(::testing::_, ::testing::_, ::testing::_, ::testing::_))
        .Times(1)
            .WillOnce(::testing::Invoke(
            [](const std::map<std::string, Json::Value>& plugins,const ContainerId& id,const std::shared_ptr<IDobbyStartState>& startupState,const std::string& rootfsPath) {
                return true;
            }));

    EXPECT_CALL(*p_rdkPluginManagerMock, runPlugins(::testing::_))
        .Times(2)
            .WillOnce(::testing::Invoke(
            [](const IDobbyRdkPlugin::HintFlags &hookPoint) {
                return true;
    }))
            .WillOnce(::testing::Invoke(
            [](const IDobbyRdkPlugin::HintFlags &hookPoint) {
                return true;
    }));

    EXPECT_CALL(*p_bundleConfigMock, writeConfigJson(::testing::_))
        .Times(2)
            .WillOnce(::testing::Invoke(
            [](const std::string& filePath) {
                return true;
            }))

            .WillOnce(::testing::Invoke(
            [](const std::string& filePath) {
                return true;
            }));

    EXPECT_CALL(*p_startStateMock, files())
        .Times(1)
            .WillOnce(::testing::Return(std::list<int>{1, 2, 3}));

    EXPECT_CALL(*p_rdkPluginManagerMock, getContainerLogger())
        .Times(1)
            .WillOnce(::testing::Return(std::make_shared<IDobbyRdkLoggingPluginMock>()));

    pid_t pid1 = 1234;
    pid_t pid2 = 5678;

    EXPECT_CALL(*p_legacyPluginManagerMock, executePreStartHooks(::testing::_, ::testing::_, ::testing::_, ::testing::_))
        .Times(1)
            .WillOnce(::testing::Invoke(
            [](const std::map<std::string, Json::Value>& plugins,const ContainerId& id,pid_t pid,const std::string& rootfsPath) {
                return true;
            }));

    EXPECT_CALL(*p_legacyPluginManagerMock, executePostStartHooks(::testing::_, ::testing::_,::testing::_,::testing::_))
        .Times(1)
            .WillOnce(::testing::Invoke(
            [](const std::map<std::string, Json::Value>& plugins,const ContainerId& id,pid_t pid,const std::string& rootfsPath) {
                return true;
            }));

    EXPECT_CALL(*p_runcMock, create(::testing::_,::testing::_,::testing::_,::testing::_,::testing::_))
        .Times(1)
            .WillOnce(::testing::Invoke(
            [pid1, pid2](const ContainerId &id,const std::shared_ptr<const DobbyBundle> &bundle,const std::shared_ptr<const IDobbyStream> &console,const std::list<int> &files = std::list<int>(),const std::string& customConfigPath) {
                return std::make_pair(pid1, pid2);
            }));

    EXPECT_CALL(*p_loggerMock, DumpBuffer(::testing::_,::testing::_,::testing::_))
        .Times(2)
            .WillOnce(::testing::Invoke(
            [](int bufferMemFd,pid_t containerPid,std::shared_ptr<IDobbyRdkLoggingPlugin> loggingPlugin){
                return true;
    }))
            .WillOnce(::testing::Invoke(
            [](int bufferMemFd,pid_t containerPid,std::shared_ptr<IDobbyRdkLoggingPlugin> loggingPlugin){
                return true;
    }));

    EXPECT_CALL(*p_runcMock, start(::testing::_,::testing::_))
        .Times(1)
            .WillOnce(::testing::Invoke(
            [](const ContainerId &id, const std::shared_ptr<const IDobbyStream> &console) {
                return true;
            }));

    EXPECT_CALL(*p_loggerMock, StartContainerLogging(::testing::_,::testing::_,::testing::_,::testing::_))
        .Times(1)
            .WillOnce(::testing::Invoke(
            [](std::string containerId,pid_t runtimePid,pid_t containerPid,std::shared_ptr<IDobbyRdkLoggingPlugin> loggingPlugin){
                return true;
    }));

    const ContainerId id = ContainerId::create("container_123");
    const std::string bundlePath = "/path/to/bundle";
    const std::list<int> files = {1, 2, 3};
    const std::string command = "ls -l";
    const std::string displaySocket = "/tmp/display";
    std::vector<std::string> envVars;
    envVars = {"PATH=/usr/bin", "HOME=/home/user"};

    int result = dobbyManager_test->startContainerFromBundle(id,bundlePath,files,command,displaySocket,envVars);

    EXPECT_NE(result, -1);

}

};

/****************************************************************************************************
 * Test functions for :startContainerFromSpec
 *  @brief Where the magic begins .... attempts to create a container
 *  from a Dobby spec file.
 *
 *  @param[in]  id          The id string for the container
 *  @param[in]  jsonSpec    The sky json spec with the container details
 *  @param[in]  files       A list of file descriptors to pass into the
 *                          container, can be empty.
 *  @param[in]  command     The custom command to run instead of the args in the
 *                          config file (optional)
 *
 *  @return a container descriptor, which is just a unique number that
 *  identifies the container.

  * Use case coverage:
 *                @Success :2
 *                @Failure :7

 ***************************************************************************************************/

/**
 * @brief Test startContainerFromSpec with valid inputs and with Rdk plugins
 * returns a container descriptor, which is just a unique number that
 *  identifies the container.
 */

#if defined(LEGACY_COMPONENTS)

TEST_F(DaemonDobbyManagerTest, startContainerFromSpec_ValidInputs)
{
//    expect_startContainerFromSpec(123);

        EXPECT_CALL(*p_bundleMock, isValid())
            .Times(1)
                .WillOnce(::testing::Return(true));

        EXPECT_CALL(*p_specConfigMock, isValid())
            .Times(1)
                .WillOnce(::testing::Return(true));

        EXPECT_CALL(*p_rootfsMock, isValid())
            .Times(1)
               .WillOnce(::testing::Return(true));

        EXPECT_CALL(*p_startStateMock, isValid())
            .Times(1)
                .WillOnce(::testing::Return(true));

        std::map<std::string, Json::Value> sampleData;
        sampleData["plugin1"] = Json::Value("value1");
        sampleData["plugin2"] = Json::Value("value2");

// Set the expectation to return the sample data
        EXPECT_CALL(*p_specConfigMock, rdkPlugins())
            .Times(1)
                .WillOnce(::testing::ReturnRef(sampleData));

        const std::string validPath = "/unit_tests/L1_testing/tests/";

// Set the expectation to return the valid path
        EXPECT_CALL(*p_rootfsMock, path())
            .Times(4)
                .WillOnce(::testing::ReturnRef(validPath))
                 .WillOnce(::testing::ReturnRef(validPath))
                .WillOnce(::testing::ReturnRef(validPath))
                .WillOnce(::testing::ReturnRef(validPath));

        std::string valid_path = "/unit_tests/L1_testing/tests/DobbyManagerTest/DaemonDobbyManagerTest.cpp";
        EXPECT_CALL(*p_bundleMock, path())
            .Times(2)
                .WillOnce(::testing::ReturnRef(valid_path))
                .WillOnce(::testing::ReturnRef(valid_path));

        EXPECT_CALL(*p_rdkPluginManagerMock, runPlugins(::testing::_))
            .Times(2)
                .WillOnce(::testing::Invoke(
                [](const IDobbyRdkPlugin::HintFlags &hookPoint) {
                    return true;
        }))
                 .WillOnce(::testing::Invoke(
                 [](const IDobbyRdkPlugin::HintFlags &hookPoint) {
                     return true;
        }));

        EXPECT_CALL(*p_specConfigMock, config())
            .Times(2)
                .WillOnce(::testing::Return(std::make_shared<rt_dobby_schema>()))
                .WillOnce(::testing::Return(std::make_shared<rt_dobby_schema>()));

        const std::vector<std::string> expectedStrings = {"string1", "string2", "string3"};

        EXPECT_CALL(*p_rdkPluginManagerMock, listLoadedPlugins())
            .Times(1)
                .WillOnce(::testing::Return(expectedStrings));

        EXPECT_CALL(*p_specConfigMock, writeConfigJson(::testing::_))
            .Times(1)
                .WillOnce(::testing::Invoke(
                [](const std::string& filePath) {
                    return true;
                }));

        EXPECT_CALL(*p_startStateMock, files())
            .Times(1)
                .WillOnce(::testing::Return(std::list<int>{1, 2, 3}));

        std::map<std::string, Json::Value> data;
        data["key1"] = Json::Value("value1");
        data["key2"] = Json::Value("value2");
        EXPECT_CALL(*p_configMock, legacyPlugins())
            .Times(3)
                .WillOnce(testing::ReturnRef(data))
                .WillOnce(testing::ReturnRef(data))
                .WillOnce(testing::ReturnRef(data));

        EXPECT_CALL(*p_legacyPluginManagerMock, executePostConstructionHooks(::testing::_, ::testing::_, ::testing::_, ::testing::_))
            .Times(1)
                .WillOnce(::testing::Invoke(
                [](const std::map<std::string, Json::Value>& plugins,const ContainerId& id,const std::shared_ptr<IDobbyStartState>& startupState,const std::string& rootfsPath) {
                    return true;
            }));

        EXPECT_CALL(*p_legacyPluginManagerMock, executePreStartHooks(::testing::_, ::testing::_, ::testing::_, ::testing::_))
            .Times(1)
                .WillOnce(::testing::Invoke(
                [](const std::map<std::string, Json::Value>& plugins,const ContainerId& id,pid_t pid,const std::string& rootfsPath) {
                    return true;
                }));

        EXPECT_CALL(*p_legacyPluginManagerMock, executePostStartHooks(::testing::_, ::testing::_,::testing::_,::testing::_))
            .Times(1)
                .WillOnce(::testing::Invoke(
                [](const std::map<std::string, Json::Value>& plugins,const ContainerId& id,pid_t pid,const std::string& rootfsPath) {
                    return true;
            }));

        EXPECT_CALL(*p_rdkPluginManagerMock, getContainerLogger())
            .Times(1)
                .WillOnce(::testing::Return(std::make_shared<IDobbyRdkLoggingPluginMock>()));

        pid_t pid1 = 1234;
        pid_t pid2 = 5678;
        EXPECT_CALL(*p_runcMock, create(::testing::_,::testing::_,::testing::_,::testing::_,::testing::_))
            .Times(1)
                .WillOnce(::testing::Invoke(
                [pid1, pid2](const ContainerId &id,const std::shared_ptr<const DobbyBundle> &bundle,const std::shared_ptr<const IDobbyStream> &console,const std::list<int> &files = std::list<int>(),const std::string& customConfigPath) {
                    return std::make_pair(pid1, pid2);
            }));

        EXPECT_CALL(*p_runcMock, start(::testing::_,::testing::_))
            .Times(1)
                .WillOnce(::testing::Invoke(
                [](const ContainerId &id, const std::shared_ptr<const IDobbyStream> &console) {
                    return true;
            }));

        EXPECT_CALL(*p_streamMock, getMemFd())
            .Times(2)
                .WillOnce(::testing::Return(123))
                .WillOnce(::testing::Return(123));

        EXPECT_CALL(*p_loggerMock, DumpBuffer(::testing::_,::testing::_,::testing::_))
            .Times(2)
                .WillOnce(::testing::Invoke(
                [](int bufferMemFd,pid_t containerPid,std::shared_ptr<IDobbyRdkLoggingPlugin> loggingPlugin){
                    return true;
        }))
                .WillOnce(::testing::Invoke(
                [](int bufferMemFd,pid_t containerPid,std::shared_ptr<IDobbyRdkLoggingPlugin> loggingPlugin){
                    return true;
        }));

        EXPECT_CALL(*p_loggerMock, StartContainerLogging(::testing::_,::testing::_,::testing::_,::testing::_))
            .Times(1)
                .WillOnce(::testing::Invoke(
                [](std::string containerId,pid_t runtimePid,pid_t containerPid,std::shared_ptr<IDobbyRdkLoggingPlugin> loggingPlugin){
                    return true;
        }));

    ContainerId id = ContainerId::create("container_123");
    std::string jsonSpec = "{\"key\": \"value\", \"number\": 42}";
    std::list<int> files = {1, 2, 3};
    std::string command ="ls -l";
    std::string displaySocket = "/tmp/display";
    std::vector<std::string> envVars;
    envVars = {"PATH=/usr/bin", "HOME=/home/user"};

    int result = dobbyManager_test->startContainerFromSpec(id,jsonSpec,files,command,displaySocket,envVars);

    EXPECT_NE(result, -1);

    }


/**
 * @brief Test startContainerFromSpec with valid inputs and without Rdk plugins
 * returns a container descriptor, which is just a unique number that
 *  identifies the container.
 */

TEST_F(DaemonDobbyManagerTest, startContainerFromSpec_SuccessWithoutRdkPlugins)
{
    EXPECT_CALL(*p_bundleMock, isValid())
        .Times(1)
            .WillOnce(::testing::Return(true));

    EXPECT_CALL(*p_specConfigMock, isValid())
        .Times(1)
            .WillOnce(::testing::Return(true));

    EXPECT_CALL(*p_rootfsMock, isValid())
        .Times(1)
           .WillOnce(::testing::Return(true));

    EXPECT_CALL(*p_startStateMock, isValid())
        .Times(1)
            .WillOnce(::testing::Return(true));

    std::map<std::string, Json::Value> emptyMap;

// Set up the mock behavior for rdkPlugins() to return the empty map
    EXPECT_CALL(*p_specConfigMock, rdkPlugins())
        .Times(1)
            .WillOnce(::testing::ReturnRef(emptyMap));

    const std::string validPath = "/unit_tests/L1_testing/tests/";

// Set the expectation to return the valid path
    EXPECT_CALL(*p_rootfsMock, path())
        .Times(3)
            .WillOnce(::testing::ReturnRef(validPath))
            .WillOnce(::testing::ReturnRef(validPath))
            .WillOnce(::testing::ReturnRef(validPath));

    std::string valid_path = "/unit_tests/L1_testing/tests/DobbyManagerTest/DaemonDobbyManagerTest.cpp";
    EXPECT_CALL(*p_bundleMock, path())
        .Times(1)
            .WillOnce(::testing::ReturnRef(valid_path));

    EXPECT_CALL(*p_specConfigMock, writeConfigJson(::testing::_))
        .Times(1)
            .WillOnce(::testing::Invoke(
            [](const std::string& filePath) {
                return true;
            }));

    EXPECT_CALL(*p_startStateMock, files())
        .Times(1)
            .WillOnce(::testing::Return(std::list<int>{1, 2, 3}));

    std::map<std::string, Json::Value> data;
    data["key1"] = Json::Value("value1");
    data["key2"] = Json::Value("value2");
    EXPECT_CALL(*p_configMock, legacyPlugins())
        .Times(3)
            .WillOnce(testing::ReturnRef(data))
            .WillOnce(testing::ReturnRef(data))
            .WillOnce(testing::ReturnRef(data));

    EXPECT_CALL(*p_legacyPluginManagerMock, executePostConstructionHooks(::testing::_, ::testing::_, ::testing::_, ::testing::_))
        .Times(1)
            .WillOnce(::testing::Invoke(
            [](const std::map<std::string, Json::Value>& plugins,const ContainerId& id,const std::shared_ptr<IDobbyStartState>& startupState,const std::string& rootfsPath) {
                return true;
            }));

    EXPECT_CALL(*p_legacyPluginManagerMock, executePreStartHooks(::testing::_, ::testing::_, ::testing::_, ::testing::_))
        .Times(1)
            .WillOnce(::testing::Invoke(
            [](const std::map<std::string, Json::Value>& plugins,const ContainerId& id,pid_t pid,const std::string& rootfsPath) {
                return true;
            }));

    EXPECT_CALL(*p_legacyPluginManagerMock, executePostStartHooks(::testing::_, ::testing::_,::testing::_,::testing::_))
        .Times(1)
            .WillOnce(::testing::Invoke(
            [](const std::map<std::string, Json::Value>& plugins,const ContainerId& id,pid_t pid,const std::string& rootfsPath) {
                return true;
            }));

    pid_t pid1 = 1234;
    pid_t pid2 = 5678;
    EXPECT_CALL(*p_runcMock, create(::testing::_,::testing::_,::testing::_,::testing::_,::testing::_))
        .Times(1)
            .WillOnce(::testing::Invoke(
            [pid1, pid2](const ContainerId &id,const std::shared_ptr<const DobbyBundle> &bundle,const std::shared_ptr<const IDobbyStream> &console,const std::list<int> &files = std::list<int>(),const std::string& customConfigPath) {
                return std::make_pair(pid1, pid2);
            }));

    EXPECT_CALL(*p_runcMock, start(::testing::_,::testing::_))
        .Times(1)
            .WillOnce(::testing::Invoke(
            [](const ContainerId &id, const std::shared_ptr<const IDobbyStream> &console) {
                return true;
            }));

    ContainerId id = ContainerId::create("container_123");
    std::string jsonSpec = "{\"key\": \"value\", \"number\": 42}";
    std::list<int> files = {1, 2, 3};
    std::string command ="ls -l";
    std::string displaySocket = "/tmp/display";
    std::vector<std::string> envVars;
    envVars = {"PATH=/usr/bin", "HOME=/home/user"};

    int result = dobbyManager_test->startContainerFromSpec(id,jsonSpec,files,command,displaySocket,envVars);

    EXPECT_NE(result, -1);

}

/**
 * @brief Test startContainerFromSpec where bundle is not created
 * returns  (-1)
 */

TEST_F(DaemonDobbyManagerTest, startContainerFromSpec_InvalidBundleCreation)
{
    EXPECT_CALL(*p_bundleMock, isValid())
        .Times(1)
            .WillOnce(::testing::Return(false));

    ContainerId id = ContainerId::create("container_123");
    std::string jsonSpec = "{\"key\": \"value\", \"number\": 42}";
    std::list<int> files = {1, 2, 3};
    std::string command ="ls -l";
    std::string displaySocket = "/tmp/display";
    std::vector<std::string> envVars;
    envVars = {"PATH=/usr/bin", "HOME=/home/user"};

    int result = dobbyManager_test->startContainerFromSpec(id,jsonSpec,files,command,displaySocket,envVars);

    EXPECT_EQ(result, -1);

}

/**
 * @brief Test startContainerFromSpec config object is not created from OCI bundle config
 * returns  (-1)
 */

TEST_F(DaemonDobbyManagerTest, startContainerFromSpec_InvalidConfigObject)
{

    EXPECT_CALL(*p_bundleMock, isValid())
        .Times(1)
            .WillOnce(::testing::Return(true));

    EXPECT_CALL(*p_specConfigMock, isValid())
        .Times(1)
            .WillOnce(::testing::Return(false));

    ContainerId id = ContainerId::create("container_123");
    std::string jsonSpec = "{\"key\": \"value\", \"number\": 42}";
    std::list<int> files = {1, 2, 3};
    std::string command ="ls -l";
    std::string displaySocket = "/tmp/display";
    std::vector<std::string> envVars;
    envVars = {"PATH=/usr/bin", "HOME=/home/user"};

    int result = dobbyManager_test->startContainerFromSpec(id,jsonSpec,files,command,displaySocket,envVars);

    EXPECT_EQ(result, -1);

}

/**
 * @brief Test startContainerFromSpec where rootfs is not created
 * returns  (-1)
 */

TEST_F(DaemonDobbyManagerTest, startContainerFromSpec_InvalidRootfsCreation)
{

    EXPECT_CALL(*p_bundleMock, isValid())
        .Times(1)
            .WillOnce(::testing::Return(true));

    EXPECT_CALL(*p_specConfigMock, isValid())
        .Times(1)
            .WillOnce(::testing::Return(true));

    EXPECT_CALL(*p_rootfsMock, isValid())
        .Times(1)
           .WillOnce(::testing::Return(false));

    ContainerId id = ContainerId::create("container_123");
    std::string jsonSpec = "{\"key\": \"value\", \"number\": 42}";
    std::list<int> files = {1, 2, 3};
    std::string command ="ls -l";
    std::string displaySocket = "/tmp/display";
    std::vector<std::string> envVars;
    envVars = {"PATH=/usr/bin", "HOME=/home/user"};

    int result = dobbyManager_test->startContainerFromSpec(id,jsonSpec,files,command,displaySocket,envVars);

    EXPECT_EQ(result, -1);

}

/**
 * @brief Test startContainerFromSpec where startstate object is not created
 * returns  (-1)
 */

TEST_F(DaemonDobbyManagerTest, startContainerFromSpec_InvalidStartStateObject)
{

    EXPECT_CALL(*p_bundleMock, isValid())
        .Times(1)
            .WillOnce(::testing::Return(true));

    EXPECT_CALL(*p_specConfigMock, isValid())
        .Times(1)
            .WillOnce(::testing::Return(true));

    EXPECT_CALL(*p_rootfsMock, isValid())
        .Times(1)
           .WillOnce(::testing::Return(true));

    EXPECT_CALL(*p_startStateMock, isValid())
        .Times(1)
            .WillOnce(::testing::Return(false));

    ContainerId id = ContainerId::create("container_123");
    std::string jsonSpec = "{\"key\": \"value\", \"number\": 42}";
    std::list<int> files = {1, 2, 3};
    std::string command ="ls -l";
    std::string displaySocket = "/tmp/display";
    std::vector<std::string> envVars;
    envVars = {"PATH=/usr/bin", "HOME=/home/user"};

    int result = dobbyManager_test->startContainerFromSpec(id,jsonSpec,files,command,displaySocket,envVars);

    EXPECT_EQ(result, -1);

}

/**
 * @brief Test startContainerFromSpec with onPostConstructionHookFailure
 * returns  (-1)
 */

TEST_F(DaemonDobbyManagerTest, startContainerFromSpec_onPostConstructionHookFailure)
{

    EXPECT_CALL(*p_bundleMock, isValid())
        .Times(1)
            .WillOnce(::testing::Return(true));

    EXPECT_CALL(*p_specConfigMock, isValid())
        .Times(1)
            .WillOnce(::testing::Return(true));

    EXPECT_CALL(*p_rootfsMock, isValid())
        .Times(1)
           .WillOnce(::testing::Return(true));

    EXPECT_CALL(*p_startStateMock, isValid())
        .Times(1)
            .WillOnce(::testing::Return(true));

    std::map<std::string, Json::Value> sampleData;
    sampleData["plugin1"] = Json::Value("value1");
    sampleData["plugin2"] = Json::Value("value2");

// Set the expectation to return the sample data
    EXPECT_CALL(*p_specConfigMock, rdkPlugins())
        .Times(1)
            .WillOnce(::testing::ReturnRef(sampleData));

    const std::string validPath = "/unit_tests/L1_testing/tests/";

// Set the expectation to return the valid path
    EXPECT_CALL(*p_rootfsMock, path())
        .Times(3)
            .WillOnce(::testing::ReturnRef(validPath))
            .WillOnce(::testing::ReturnRef(validPath))
            .WillOnce(::testing::ReturnRef(validPath));

    EXPECT_CALL(*p_specConfigMock, config())
        .Times(2)
            .WillOnce(::testing::Return(std::make_shared<rt_dobby_schema>()))
            .WillOnce(::testing::Return(std::make_shared<rt_dobby_schema>()));

    const std::vector<std::string> expectedStrings = {"plugin", "plugin", "plugin"};

    EXPECT_CALL(*p_rdkPluginManagerMock, listLoadedPlugins())
        .Times(1)
            .WillOnce(::testing::Return(expectedStrings));

    std::map<std::string, Json::Value> data;
    data["key1"] = Json::Value("value1");
    data["key2"] = Json::Value("value2");

    EXPECT_CALL(*p_configMock, legacyPlugins())
        .Times(2)
            .WillOnce(testing::ReturnRef(data))
            .WillOnce(testing::ReturnRef(data));

    EXPECT_CALL(*p_legacyPluginManagerMock, executePostConstructionHooks(::testing::_, ::testing::_, ::testing::_, ::testing::_))
        .Times(1)
            .WillOnce(::testing::Invoke(
            [](const std::map<std::string, Json::Value>& plugins,const ContainerId& id,const std::shared_ptr<IDobbyStartState>& startupState,const std::string& rootfsPath) {
                return false;
            }));

    EXPECT_CALL(*p_legacyPluginManagerMock, executePreDestructionHooks(::testing::_, ::testing::_, ::testing::_))
        .Times(1)
            .WillOnce(::testing::Invoke(
            [](const std::map<std::string, Json::Value>& plugins,const ContainerId& id,const std::string& rootfsPath) {
                return true;
            }));

    ContainerId id = ContainerId::create("container_123");
    std::string jsonSpec = "{\"key\": \"value\", \"number\": 42}";
    std::list<int> files = {1, 2, 3};
    std::string command ="ls -l";
    std::string displaySocket = "/tmp/display";
    std::vector<std::string> envVars;
    envVars = {"PATH=/usr/bin", "HOME=/home/user"};

    int result = dobbyManager_test->startContainerFromSpec(id,jsonSpec,files,command,displaySocket,envVars);

    EXPECT_EQ(result, -1);

}

/**
 * @brief Test startContainerFromSpec with config json failure
 * returns  (-1)
 */

TEST_F(DaemonDobbyManagerTest, startContainerFromSpec_WriteConfigJsonFailure)
{

    EXPECT_CALL(*p_bundleMock, isValid())
        .Times(1)
            .WillOnce(::testing::Return(true));

    EXPECT_CALL(*p_specConfigMock, isValid())
        .Times(1)
            .WillOnce(::testing::Return(true));

    EXPECT_CALL(*p_rootfsMock, isValid())
        .Times(1)
           .WillOnce(::testing::Return(true));

    EXPECT_CALL(*p_startStateMock, isValid())
        .Times(1)
            .WillOnce(::testing::Return(true));

    std::map<std::string, Json::Value> sampleData;
    sampleData["plugin1"] = Json::Value("value1");
    sampleData["plugin2"] = Json::Value("value2");

// Set the expectation to return the sample data
    EXPECT_CALL(*p_specConfigMock, rdkPlugins())
        .Times(1)
            .WillOnce(::testing::ReturnRef(sampleData));

    const std::string validPath = "/unit_tests/L1_testing/tests/";

// Set the expectation to return the valid path
    EXPECT_CALL(*p_rootfsMock, path())
        .Times(3)
            .WillOnce(::testing::ReturnRef(validPath))
            .WillOnce(::testing::ReturnRef(validPath))
            .WillOnce(::testing::ReturnRef(validPath));

    std::string valid_path = "/unit_tests/L1_testing/tests/DobbyManagerTest/DaemonDobbyManagerTest.cpp";

    EXPECT_CALL(*p_bundleMock, path())
        .Times(2)
            .WillOnce(::testing::ReturnRef(valid_path))
            .WillOnce(::testing::ReturnRef(valid_path));


    EXPECT_CALL(*p_specConfigMock, config())
        .Times(2)
            .WillOnce(::testing::Return(std::make_shared<rt_dobby_schema>()))
            .WillOnce(::testing::Return(std::make_shared<rt_dobby_schema>()));

    const std::vector<std::string> expectedStrings = {"string1", "string2", "string3"};

    EXPECT_CALL(*p_rdkPluginManagerMock, listLoadedPlugins())
        .Times(1)
            .WillOnce(::testing::Return(expectedStrings));

    std::map<std::string, Json::Value> data;
    data["key1"] = Json::Value("value1");
    data["key2"] = Json::Value("value2");

    EXPECT_CALL(*p_configMock, legacyPlugins())
        .Times(2)
            .WillOnce(testing::ReturnRef(data))
            .WillOnce(testing::ReturnRef(data));

    EXPECT_CALL(*p_legacyPluginManagerMock, executePostConstructionHooks(::testing::_, ::testing::_, ::testing::_, ::testing::_))
        .Times(1)
            .WillOnce(::testing::Invoke(
            [](const std::map<std::string, Json::Value>& plugins,const ContainerId& id,const std::shared_ptr<IDobbyStartState>& startupState,const std::string& rootfsPath) {
                return true;
            }));

    EXPECT_CALL(*p_rdkPluginManagerMock, runPlugins(::testing::_))
        .Times(2)
            .WillOnce(::testing::Invoke(
            [](const IDobbyRdkPlugin::HintFlags &hookPoint) {
                return true;
    }))
            .WillOnce(::testing::Invoke(
            [](const IDobbyRdkPlugin::HintFlags &hookPoint) {
                return true;
    }));

    EXPECT_CALL(*p_specConfigMock, writeConfigJson(::testing::_))
        .Times(1)
            .WillOnce(::testing::Invoke(
            [](const std::string& filePath) {
                return false;
            }));

    EXPECT_CALL(*p_legacyPluginManagerMock, executePreDestructionHooks(::testing::_, ::testing::_, ::testing::_))
        .Times(1)
            .WillOnce(::testing::Invoke(
            [](const std::map<std::string, Json::Value>& plugins,const ContainerId& id,const std::string& rootfsPath) {
                return true;
            }));

    ContainerId id = ContainerId::create("container_123");
    std::string jsonSpec = "{\"key\": \"value\", \"number\": 42}";
    std::list<int> files = {1, 2, 3};
    std::string command ="ls -l";
    std::string displaySocket = "/tmp/display";
    std::vector<std::string> envVars;
    envVars = {"PATH=/usr/bin", "HOME=/home/user"};

    int result = dobbyManager_test->startContainerFromSpec(id,jsonSpec,files,command,displaySocket,envVars);

    EXPECT_EQ(result, -1);

}

/**
 * @brief Test startContainerFromSpec with container that is already running.
 * returns  (-1)
 */

TEST_F(DaemonDobbyManagerTest, startContainerFromSpec_FailedAsContainerAlreadyRunning)
{

    expect_startContainerFromBundle(123);

    ContainerId id = ContainerId::create("container_123");
    std::string jsonSpec = "{\"key\": \"value\", \"number\": 42}";
    std::list<int> files = {1, 2, 3};
    std::string command ="ls -l";
    std::string displaySocket = "/tmp/display";
    std::vector<std::string> envVars;
    envVars = {"PATH=/usr/bin", "HOME=/home/user"};

    int result = dobbyManager_test->startContainerFromSpec(id,jsonSpec,files,command,displaySocket,envVars);

    EXPECT_EQ(result, -1);

}
/*Test cases for startContainerFromSpec ends here*/

/****************************************************************************************************
 * Test functions for :createBundle
 *  @brief Debugging method to allow you to create a bundle with rootfs and
 *  config.json without actually running runc on it.
 *
 *
 *  @param[in]  id          The id of the new bundle to create.
 *  @param[in]  jsonSpec    The spec file to use to generate the rootfs and
 *                          config.json within the bundle.
 *
 *  @return true on success, false on failure.

  * Use case coverage:
 *                @Success :1
 *                @Failure :3

 ***************************************************************************************************/

/**
 * @brief Test createBundle with valid inputs
 * returns true if the bundle is create is created successfully
 * otherwise returns false.
 */

TEST_F(DaemonDobbyManagerTest, createBundle_Success)
{

    EXPECT_CALL(*p_bundleMock, isValid())
        .Times(1)
            .WillOnce(::testing::Return(true));

    EXPECT_CALL(*p_specConfigMock, isValid())
        .Times(1)
            .WillOnce(::testing::Return(true));

    EXPECT_CALL(*p_rootfsMock, isValid())
        .Times(1)
           .WillOnce(::testing::Return(true));

    std::string valid_path = "/unit_tests/L1_testing/tests/DobbyManagerTest/DaemonDobbyManagerTest.cpp";
    EXPECT_CALL(*p_bundleMock, path())
        .Times(1)
            .WillOnce(::testing::ReturnRef(valid_path));

    EXPECT_CALL(*p_specConfigMock, writeConfigJson(::testing::_))
        .Times(1)
            .WillOnce(::testing::Invoke(
            [](const std::string& filePath) {
                return true;
            }));

    EXPECT_CALL(*p_rootfsMock, setPersistence(::testing::_))
        .Times(1)
            .WillOnce(::testing::Invoke(
            [](bool persist) {
            }));

    EXPECT_CALL(*p_bundleMock, setPersistence(::testing::_))
        .Times(1)
            .WillOnce(::testing::Invoke(
            [](bool persist) {
            }));

    ContainerId id = ContainerId::create("container_111");
    std::string jsonSpec = "{\"key\": \"value\", \"number\": 44}";

    bool result = dobbyManager_test->createBundle(id,jsonSpec);

    EXPECT_EQ(result, true);

}

/**
 * @brief Test createBundle with failure in bundle
 * returns false
 */

TEST_F(DaemonDobbyManagerTest, createBundle_BundleFailure)
{
    EXPECT_CALL(*p_bundleMock, isValid())
        .Times(1)
            .WillOnce(::testing::Return(false));

    ContainerId id = ContainerId::create("container_111");
    std::string jsonSpec = "{\"key\": \"value\", \"number\": 44}";

    bool result = dobbyManager_test->createBundle(id,jsonSpec);

    EXPECT_EQ(result, false);

}

/**
 * @brief Test createBundle with failure in config object creation.
 * returns false
 */

TEST_F(DaemonDobbyManagerTest, createBundle_CreateConfigObjectFailure)
{

    EXPECT_CALL(*p_bundleMock, isValid())
        .Times(1)
            .WillOnce(::testing::Return(true));

    EXPECT_CALL(*p_specConfigMock, isValid())
        .Times(1)
            .WillOnce(::testing::Return(false));

    ContainerId id = ContainerId::create("container_111");
    std::string jsonSpec = "{\"key\": \"value\", \"number\": 44}";

    bool result = dobbyManager_test->createBundle(id,jsonSpec);

    EXPECT_EQ(result, false);

}

/**
 * @brief Test createBundle with failure in rootfs creation
 * returns false
 */

TEST_F(DaemonDobbyManagerTest, createBundle_RootfsCreationFailure)
{

    EXPECT_CALL(*p_bundleMock, isValid())
        .Times(1)
            .WillOnce(::testing::Return(true));

    EXPECT_CALL(*p_specConfigMock, isValid())
        .Times(1)
            .WillOnce(::testing::Return(true));

    EXPECT_CALL(*p_rootfsMock, isValid())
        .Times(1)
           .WillOnce(::testing::Return(false));

    ContainerId id = ContainerId::create("container_111");
    std::string jsonSpec = "{\"key\": \"value\", \"number\": 44}";

    bool result = dobbyManager_test->createBundle(id,jsonSpec);

    EXPECT_EQ(result, false);

}

#endif //defined(LEGACY_COMPONENTS)

/*Test cases for createBundle ends here*/

/****************************************************************************************************
 * Test functions for :startContainerFromBundle
 *  @brief Where the magic begins ... attempts to create a container from
 *         an OCI bundle*
 *
 *
 *  @param[in]  id          The id string for the container
 *  @param[in]  bundlePath  The absolute path to the OCI bundle*
 *  @param[in]  files       A list of file descriptors to pass into the
 *                          container, can be empty.
 * @param[in]   command     The custom command to run instead of the args in the
 *                          config file (optional)
 *
 *  @return a container descriptor, which is just a unique number that
 *  identifies the container.

  * Use case coverage:
 *                @Success :2
 *                @Failure :8

 ***************************************************************************************************/

/**
 * @brief Test startContainerFromBundle with failure in config object creation
 * returns (-1)
 */

TEST_F(DaemonDobbyManagerTest, startContainerFromBundle_CreateConfigObjectFailure)
{

    EXPECT_CALL(*p_bundleConfigMock, isValid())
        .Times(1)
            .WillOnce(::testing::Return(false));

    const ContainerId id = ContainerId::create("test_container_123");
    const std::string bundlePath = "/path/to/bundle";
    const std::list<int> files = {1, 2, 3};
    const std::string command = "ls -l";
    const std::string displaySocket = "/tmp/display";
    std::vector<std::string> envVars;
    envVars = {"PATH=/usr/bin", "HOME=/home/user"};

    int result = dobbyManager_test->startContainerFromBundle(id,bundlePath,files,command,displaySocket,envVars);

    EXPECT_EQ(result, -1);

}

/**
 * @brief Test startContainerFromBundle with failure in bundle creation
 * returns (-1)
 */

TEST_F(DaemonDobbyManagerTest, startContainerFromBundle_DobbyBundleFailure)
{

    EXPECT_CALL(*p_bundleConfigMock, isValid())
        .Times(1)
            .WillOnce(::testing::Return(true));

    EXPECT_CALL(*p_bundleMock, isValid())
        .Times(1)
            .WillOnce(::testing::Return(false));

    const ContainerId id = ContainerId::create("test_container_123");
    const std::string bundlePath = "/path/to/bundle";
    const std::list<int> files = {1, 2, 3};
    const std::string command = "ls -l";
    const std::string displaySocket = "/tmp/display";
    std::vector<std::string> envVars;
    envVars = {"PATH=/usr/bin", "HOME=/home/user"};

    int result = dobbyManager_test->startContainerFromBundle(id,bundlePath,files,command,displaySocket,envVars);

    EXPECT_EQ(result, -1);

}

/**
 * @brief Test startContainerFromBundle with failure in rootfs creation
 * returns (-1)
 */

TEST_F(DaemonDobbyManagerTest, startContainerFromBundle_RootfsCreationFailure)
{
    EXPECT_CALL(*p_bundleConfigMock, isValid())
        .Times(1)
            .WillOnce(::testing::Return(true));

    EXPECT_CALL(*p_bundleMock, isValid())
        .Times(1)
            .WillOnce(::testing::Return(true));

    EXPECT_CALL(*p_rootfsMock, isValid())
        .Times(1)
           .WillOnce(::testing::Return(false));

    const ContainerId id = ContainerId::create("test_container_123");
    const std::string bundlePath = "/path/to/bundle";
    const std::list<int> files = {1, 2, 3};
    const std::string command = "ls -l";
    const std::string displaySocket = "/tmp/display";
    std::vector<std::string> envVars;
    envVars = {"PATH=/usr/bin", "HOME=/home/user"};

    int result = dobbyManager_test->startContainerFromBundle(id,bundlePath,files,command,displaySocket,envVars);

    EXPECT_EQ(result, -1);

}

/**
 * @brief Test startContainerFromBundle with failure in startstate object creation
 * returns (-1)
 */

TEST_F(DaemonDobbyManagerTest, startContainerFromBundle_StartStateObjectFailure)
{

    EXPECT_CALL(*p_bundleConfigMock, isValid())
        .Times(1)
            .WillOnce(::testing::Return(true));

    EXPECT_CALL(*p_bundleMock, isValid())
        .Times(1)
            .WillOnce(::testing::Return(true));

    EXPECT_CALL(*p_rootfsMock, isValid())
        .Times(1)
           .WillOnce(::testing::Return(true));

    EXPECT_CALL(*p_startStateMock, isValid())
        .Times(1)
            .WillOnce(::testing::Return(false));

    const ContainerId id = ContainerId::create("test_container_123");
    const std::string bundlePath = "/path/to/bundle";
    const std::list<int> files = {1, 2, 3};
    const std::string command = "ls -l";
    const std::string displaySocket = "/tmp/display";
    std::vector<std::string> envVars;
    envVars = {"PATH=/usr/bin", "HOME=/home/user"};

    int result = dobbyManager_test->startContainerFromBundle(id,bundlePath,files,command,displaySocket,envVars);

    EXPECT_EQ(result, -1);

}

/**
 * @brief Test startContainerFromBundle with failure in postConstructionHook
 * returns (-1)
 */

TEST_F(DaemonDobbyManagerTest, startContainerFromBundle_onPostConstructionHookFailure)
{

    EXPECT_CALL(*p_bundleConfigMock, isValid())
        .Times(1)
            .WillOnce(::testing::Return(true));

    EXPECT_CALL(*p_bundleMock, isValid())
        .Times(1)
            .WillOnce(::testing::Return(true));

    EXPECT_CALL(*p_rootfsMock, isValid())
        .Times(1)
           .WillOnce(::testing::Return(true));

    EXPECT_CALL(*p_startStateMock, isValid())
        .Times(1)
            .WillOnce(::testing::Return(true));

    std::map<std::string, Json::Value> sampleData;
    sampleData["plugin1"] = Json::Value("value1");
    sampleData["plugin2"] = Json::Value("value2");

// Set the expectation to return the sample data
    EXPECT_CALL(*p_bundleConfigMock, rdkPlugins())
        .Times(1)
            .WillOnce(::testing::ReturnRef(sampleData));

    const std::string validPath = "/unit_tests/L1_testing/tests/";

// Set the expectation to return the valid path
    EXPECT_CALL(*p_rootfsMock, path())
        .Times(3)
            .WillOnce(::testing::ReturnRef(validPath))
            .WillOnce(::testing::ReturnRef(validPath))
            .WillOnce(::testing::ReturnRef(validPath));

    EXPECT_CALL(*p_bundleConfigMock, config())
        .Times(2)
            .WillOnce(::testing::Return(std::make_shared<rt_dobby_schema>()))
            .WillOnce(::testing::Return(std::make_shared<rt_dobby_schema>()));

    const std::vector<std::string> expectedStrings = {"string1", "string2", "string3"};

    EXPECT_CALL(*p_rdkPluginManagerMock, listLoadedPlugins())
        .Times(1)
            .WillOnce(::testing::Return(expectedStrings));

    std::map<std::string, Json::Value> data;
    data["key1"] = Json::Value("value1");
    data["key2"] = Json::Value("value2");

    EXPECT_CALL(*p_configMock, legacyPlugins())
        .Times(2)
            .WillOnce(testing::ReturnRef(data))
            .WillOnce(testing::ReturnRef(data));

    EXPECT_CALL(*p_legacyPluginManagerMock, executePostConstructionHooks(::testing::_, ::testing::_, ::testing::_, ::testing::_))
        .Times(1)
            .WillOnce(::testing::Invoke(
            [](const std::map<std::string, Json::Value>& plugins,const ContainerId& id,const std::shared_ptr<IDobbyStartState>& startupState,const std::string& rootfsPath) {
                return false;
            }));

    EXPECT_CALL(*p_legacyPluginManagerMock, executePreDestructionHooks(::testing::_, ::testing::_, ::testing::_))
        .Times(1)
            .WillOnce(::testing::Invoke(
            [](const std::map<std::string, Json::Value>& plugins,const ContainerId& id,const std::string& rootfsPath) {
                return true;
            }));

    const ContainerId id = ContainerId::create("test_container_123");
    const std::string bundlePath = "/path/to/bundle";
    const std::list<int> files = {1, 2, 3};
    const std::string command = "ls -l";
    const std::string displaySocket = "/tmp/display";
    std::vector<std::string> envVars;
    envVars = {"PATH=/usr/bin", "HOME=/home/user"};

    int result = dobbyManager_test->startContainerFromBundle(id,bundlePath,files,command,displaySocket,envVars);

    EXPECT_EQ(result, -1);

}

/**
 * @brief Test startContainerFromBundle with failure in configJsonFile creation
 * returns (-1)
 */

TEST_F(DaemonDobbyManagerTest, startContainerFromBundle_ConfigJsonFileCreationFailure)
{

    EXPECT_CALL(*p_bundleConfigMock, isValid())
        .Times(1)
            .WillOnce(::testing::Return(true));

    EXPECT_CALL(*p_bundleMock, isValid())
        .Times(1)
            .WillOnce(::testing::Return(true));

    EXPECT_CALL(*p_rootfsMock, isValid())
        .Times(1)
           .WillOnce(::testing::Return(true));

    EXPECT_CALL(*p_startStateMock, isValid())
        .Times(1)
            .WillOnce(::testing::Return(true));

    std::map<std::string, Json::Value> sampleData;
    sampleData["plugin1"] = Json::Value("value1");
    sampleData["plugin2"] = Json::Value("value2");

// Set the expectation to return the sample data
    EXPECT_CALL(*p_bundleConfigMock, rdkPlugins())
        .Times(1)
            .WillOnce(::testing::ReturnRef(sampleData));

    const std::string validPath = "/unit_tests/L1_testing/tests/";

// Set the expectation to return the valid path
    EXPECT_CALL(*p_rootfsMock, path())
        .Times(3)
            .WillOnce(::testing::ReturnRef(validPath))
            .WillOnce(::testing::ReturnRef(validPath))
            .WillOnce(::testing::ReturnRef(validPath));

    std::string valid_path = "/unit_tests/L1_testing/tests/DobbyManagerTest/DaemonDobbyManagerTest.cpp";

    EXPECT_CALL(*p_bundleMock, path())
        .Times(2)
            .WillOnce(::testing::ReturnRef(valid_path))
            .WillOnce(::testing::ReturnRef(valid_path));

    EXPECT_CALL(*p_bundleConfigMock, config())
        .Times(2)
            .WillOnce(::testing::Return(std::make_shared<rt_dobby_schema>()))
            .WillOnce(::testing::Return(std::make_shared<rt_dobby_schema>()));

    const std::vector<std::string> expectedStrings = {"string1", "string2", "string3"};

    EXPECT_CALL(*p_rdkPluginManagerMock, listLoadedPlugins())
        .Times(1)
            .WillOnce(::testing::Return(expectedStrings));

    std::map<std::string, Json::Value> data;
    data["key1"] = Json::Value("value1");
    data["key2"] = Json::Value("value2");

    EXPECT_CALL(*p_configMock, legacyPlugins())
        .Times(2)
            .WillOnce(testing::ReturnRef(data))
            .WillOnce(testing::ReturnRef(data));

    EXPECT_CALL(*p_legacyPluginManagerMock, executePostConstructionHooks(::testing::_, ::testing::_, ::testing::_, ::testing::_))
        .Times(1)
            .WillOnce(::testing::Invoke(
            [](const std::map<std::string, Json::Value>& plugins,const ContainerId& id,const std::shared_ptr<IDobbyStartState>& startupState,const std::string& rootfsPath) {
                return true;
            }));

    EXPECT_CALL(*p_rdkPluginManagerMock, runPlugins(::testing::_))
        .Times(2)
            .WillOnce(::testing::Invoke(
            [](const IDobbyRdkPlugin::HintFlags &hookPoint) {
                return true;
    }))
            .WillOnce(::testing::Invoke(
            [](const IDobbyRdkPlugin::HintFlags &hookPoint) {
                return true;
    }));

    EXPECT_CALL(*p_bundleConfigMock, writeConfigJson(::testing::_))
        .Times(1)
            .WillOnce(::testing::Invoke(
            [](const std::string& filePath) {
                return false;
            }));

    EXPECT_CALL(*p_legacyPluginManagerMock, executePreDestructionHooks(::testing::_, ::testing::_, ::testing::_))
        .Times(1)
            .WillOnce(::testing::Invoke(
            [](const std::map<std::string, Json::Value>& plugins,const ContainerId& id,const std::string& rootfsPath) {
                return true;
            }));

    const ContainerId id = ContainerId::create("test_container_123");
    const std::string bundlePath = "/path/to/bundle";
    const std::list<int> files = {1, 2, 3};
    const std::string command = "ls -l";
    const std::string displaySocket = "/tmp/display";
    std::vector<std::string> envVars;
    envVars = {"PATH=/usr/bin", "HOME=/home/user"};

    int result = dobbyManager_test->startContainerFromBundle(id,bundlePath,files,command,displaySocket,envVars);

    EXPECT_EQ(result, -1);

}

/**
 * @brief Test startContainerFromBundle with valid inputs and Rdk plugins
 * returns descriptor(cd) which is an integer.
 */

TEST_F(DaemonDobbyManagerTest, startContainerFromBundle_ValidInputs)
{

    expect_startContainerFromBundle(123);
}

/**
 * @brief Test startContainerFromBundle using containerId that is already running
 * returns (-1)
 */


TEST_F(DaemonDobbyManagerTest, startContainerFromBundle_FailedAsContainerAlreadyRunning)
{

    expect_startContainerFromBundle(123);

    const ContainerId id = ContainerId::create("container_123");
    const std::string bundlePath = "/path/to/bundle";
    const std::list<int> files = {1, 2, 3};
    const std::string command = "ls -l";
    const std::string displaySocket = "/tmp/display";
    std::vector<std::string> envVars;
    envVars = {"PATH=/usr/bin", "HOME=/home/user"};


    int result = dobbyManager_test->startContainerFromBundle(id,bundlePath,files,command,displaySocket,envVars);

    EXPECT_EQ(result, -1);

}

/**
 * @brief Test startContainerFromBundle of valid inputs and without Rdk plugins
 * returns decsriptor (cd) an integer value
 */

TEST_F(DaemonDobbyManagerTest, startContainerFromBundle_SuccessWithoutRdkPlugins)
{
    EXPECT_CALL(*p_bundleConfigMock, isValid())
        .Times(1)
            .WillOnce(::testing::Return(true));

    EXPECT_CALL(*p_bundleMock, isValid())
        .Times(1)
            .WillOnce(::testing::Return(true));

    EXPECT_CALL(*p_rootfsMock, isValid())
        .Times(1)
           .WillOnce(::testing::Return(true));

    EXPECT_CALL(*p_startStateMock, isValid())
        .Times(1)
            .WillOnce(::testing::Return(true));

    std::map<std::string, Json::Value> emptyMap;

// Set up the mock behavior for rdkPlugins() to return the empty map
    EXPECT_CALL(*p_bundleConfigMock, rdkPlugins())
        .Times(1)
            .WillOnce(::testing::ReturnRef(emptyMap));

    const std::string validPath = "/unit_tests/L1_testing/tests/";

// Set the expectation to return the valid path
    EXPECT_CALL(*p_rootfsMock, path())
        .Times(3)
            .WillOnce(::testing::ReturnRef(validPath))
            .WillOnce(::testing::ReturnRef(validPath))
            .WillOnce(::testing::ReturnRef(validPath));

    std::string valid_path = "/unit_tests/L1_testing/tests/DobbyManagerTest/DaemonDobbyManagerTest.cpp";
    EXPECT_CALL(*p_bundleMock, path())
        .Times(2)
            .WillOnce(::testing::ReturnRef(valid_path))
            .WillOnce(::testing::ReturnRef(valid_path));

    EXPECT_CALL(*p_bundleConfigMock, writeConfigJson(::testing::_))
        .Times(2)
            .WillOnce(::testing::Invoke(
            [](const std::string& filePath) {
                return true;
            }))

            .WillOnce(::testing::Invoke(
            [](const std::string& filePath) {
                return true;
            }));

    std::map<std::string, Json::Value> data;
    data["key1"] = Json::Value("value1");
    data["key2"] = Json::Value("value2");
    EXPECT_CALL(*p_configMock, legacyPlugins())
        .Times(3)
            .WillOnce(testing::ReturnRef(data))
            .WillOnce(testing::ReturnRef(data))

            .WillOnce(testing::ReturnRef(data));

    EXPECT_CALL(*p_legacyPluginManagerMock, executePostConstructionHooks(::testing::_, ::testing::_, ::testing::_, ::testing::_))
        .Times(1)
            .WillOnce(::testing::Invoke(
            [](const std::map<std::string, Json::Value>& plugins,const ContainerId& id,const std::shared_ptr<IDobbyStartState>& startupState,const std::string& rootfsPath) {
                return true;
            }));

    EXPECT_CALL(*p_legacyPluginManagerMock, executePreStartHooks(::testing::_, ::testing::_, ::testing::_, ::testing::_))
        .Times(1)
            .WillOnce(::testing::Invoke(
            [](const std::map<std::string, Json::Value>& plugins,const ContainerId& id,pid_t pid,const std::string& rootfsPath) {
                return true;
            }));

    EXPECT_CALL(*p_legacyPluginManagerMock, executePostStartHooks(::testing::_, ::testing::_,::testing::_,::testing::_))
        .Times(1)
            .WillOnce(::testing::Invoke(
            [](const std::map<std::string, Json::Value>& plugins,const ContainerId& id,pid_t pid,const std::string& rootfsPath) {
                return true;
            }));

    pid_t pid1 = 1234;
    pid_t pid2 = 5678;
    EXPECT_CALL(*p_runcMock, create(::testing::_,::testing::_,::testing::_,::testing::_,::testing::_))
        .Times(1)
            .WillOnce(::testing::Invoke(
            [pid1, pid2](const ContainerId &id,const std::shared_ptr<const DobbyBundle> &bundle,const std::shared_ptr<const IDobbyStream> &console,const std::list<int> &files = std::list<int>(),const std::string& customConfigPath) {
                return std::make_pair(pid1, pid2);
            }));

    EXPECT_CALL(*p_runcMock, start(::testing::_,::testing::_))
        .Times(1)
            .WillOnce(::testing::Invoke(
            [](const ContainerId &id, const std::shared_ptr<const IDobbyStream> &console) {
                return true;
            }));

    const ContainerId id = ContainerId::create("container_123");
    const std::string bundlePath = "/path/to/bundle";
    const std::list<int> files = {1, 2, 3};
    const std::string command = "ls -l";
    const std::string displaySocket = "/tmp/display";
    std::vector<std::string> envVars;
    envVars = {"PATH=/usr/bin", "HOME=/home/user"};

    int result = dobbyManager_test->startContainerFromBundle(id,bundlePath,files,command,displaySocket,envVars);
    std::cout << "Result: " << result << std::endl; // Add this print statement

    EXPECT_NE(result, -1);
    std::cout << "Result check passed" << std::endl; // Add this print statement

}

