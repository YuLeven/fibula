#pragma once

#include "CoreMinimal.h"

namespace ServerConfig
{
    static FString GetGameServerAddress()
    {
        FString ServerAddress = FPlatformMisc::GetEnvironmentVariable(TEXT("GAME_SERVER_ADDRESS"));
        if (ServerAddress.IsEmpty())
        {
            return TEXT("127.0.0.1");
        }
        return ServerAddress;
    }

    static FString GetServerUsername()
    {
        FString BackendUsername = FPlatformMisc::GetEnvironmentVariable(TEXT("BACKEND_USERNAME"));
        if (BackendUsername.IsEmpty())
        {
            return TEXT("server");
        }
        return BackendUsername;
    }

    static FString GetBackendServer()
    {
        FString BackendServer = FPlatformMisc::GetEnvironmentVariable(TEXT("BACKEND_SERVER"));
        if (BackendServer.IsEmpty())
        {
            return TEXT("127.0.0.1:4001");
        }
        return BackendServer;
    }

    static FString GetServerPassword()
    {
        FString BackendPassword = FPlatformMisc::GetEnvironmentVariable(TEXT("BACKEND_PASSWORD"));
        if (BackendPassword.IsEmpty())
        {
            return TEXT("secret");
        }
        return BackendPassword;
    }

    static FString GetLoginServer()
    {
        FString LoginServer = FPlatformMisc::GetEnvironmentVariable(TEXT("LOGIN_SERVER"));
        if (LoginServer.IsEmpty())
        {
#if UE_BUILD_SHIPPING
            return TEXT("https://loginserver.fibulapvp.com");
#else
            return TEXT("http://127.0.0.1:4000");
#endif
        }
        return LoginServer;
    }

    static FString GAME_VERSION = TEXT("1.6");
}