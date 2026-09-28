[CmdletBinding()]
param(
    [string]$EngineRoot = $env:FIBULA_UE_ROOT,
    [ValidateSet("TeamBattle", "FFA")]
    [string]$Mode = "TeamBattle",
    [ValidateRange(45, 300)]
    [int]$TimeoutSeconds = 120
)

$ErrorActionPreference = "Stop"
$repositoryRoot = (Resolve-Path (Join-Path $PSScriptRoot "..")).Path
$projectFile = Join-Path $repositoryRoot "Fibula.uproject"

if (-not $EngineRoot) {
    $candidates = @(
        "D:\LauncherEngine\UE_5.5",
        "C:\Program Files\Epic Games\UE_5.5"
    )
    $EngineRoot = $candidates | Where-Object {
        Test-Path (Join-Path $_ "Engine\Build\BatchFiles\Build.bat")
    } | Select-Object -First 1
}

$buildScript = if ($EngineRoot) {
    Join-Path $EngineRoot "Engine\Build\BatchFiles\Build.bat"
}
if (-not $buildScript -or -not (Test-Path $buildScript)) {
    throw "Set FIBULA_UE_ROOT or pass -EngineRoot with a UE 5.5 source build. Engine\Build\BatchFiles\Build.bat was not found."
}

$editorExe = Join-Path $EngineRoot "Engine\Binaries\Win64\UnrealEditor.exe"
if (-not (Test-Path $editorExe)) {
    throw "UnrealEditor.exe was not found under EngineRoot: $EngineRoot"
}

$timestamp = Get-Date -Format "yyyyMMdd_HHmmss"
$logRoot = Join-Path $env:TEMP "FibulaBotTest_$timestamp"
New-Item -ItemType Directory -Path $logRoot -Force | Out-Null
$serverLog = Join-Path $logRoot "server.log"
$driverLog = Join-Path $logRoot "driver-client.log"
$secondClientLog = Join-Path $logRoot "second-client.log"
$processes = [System.Collections.Generic.List[System.Diagnostics.Process]]::new()

try {
    foreach ($target in @("FibulaEditor", "FibulaClient", "FibulaServer")) {
        Write-Host "Building $target (Win64 Development)..."
        $ubtLog = Join-Path $logRoot "ubt-$target.log"
        & $buildScript $target Win64 Development "-Project=$projectFile" "-Log=$ubtLog" -WaitMutex
        if ($LASTEXITCODE -ne 0) {
            throw "Build failed for $target with exit code $LASTEXITCODE. Build log: $logRoot"
        }
    }

    $serverArguments = @(
        $projectFile,
        "/Game/Maps/Ankrahmun?game=$Mode",
        "-server",
        "-game",
        "-log",
        "-nullrhi",
        "-unattended",
        "-port=7777",
        "-abslog=$serverLog",
        "-FibulaBotTest",
        "-FibulaBotSeed=271828"
    )
    $server = Start-Process -FilePath $editorExe -ArgumentList $serverArguments -WorkingDirectory $repositoryRoot -WindowStyle Hidden -PassThru
    $processes.Add($server)

    $listenDeadline = [DateTime]::UtcNow.AddSeconds(30)
    $serverReady = $false
    while ([DateTime]::UtcNow -lt $listenDeadline) {
        if ($server.HasExited) {
            throw "Dedicated test server exited before listening. Log: $serverLog"
        }
        if ((Test-Path $serverLog) -and (Select-String -Path $serverLog -Pattern "listening on port 7777" -Quiet)) {
            $serverReady = $true
            break
        }
        Start-Sleep -Milliseconds 500
    }
    if (-not $serverReady) {
        throw "Dedicated test server did not listen on UDP port 7777. Log: $serverLog"
    }

    $driverArguments = @(
        $projectFile,
        "127.0.0.1:7777",
        "-game",
        "-log",
        "-unattended",
        "-abslog=$driverLog",
        "-FibulaBotTestDriver"
    )
    $driver = Start-Process -FilePath $editorExe -ArgumentList $driverArguments -WorkingDirectory $repositoryRoot -WindowStyle Hidden -PassThru
    $processes.Add($driver)

    Start-Sleep -Seconds 2
    $secondClientArguments = @(
        $projectFile,
        "127.0.0.1:7777",
        "-game",
        "-log",
        "-unattended",
        "-abslog=$secondClientLog",
        "-FibulaBotTestDriver"
    )
    $secondClient = Start-Process -FilePath $editorExe -ArgumentList $secondClientArguments -WorkingDirectory $repositoryRoot -WindowStyle Hidden -PassThru
    $processes.Add($secondClient)

    $testDeadline = [DateTime]::UtcNow.AddSeconds($TimeoutSeconds)
    $serverPassed = $false
    $driverPassed = $false
    $secondClientPassed = $false
    $clientFailed = $false
    while ([DateTime]::UtcNow -lt $testDeadline) {
        $serverText = if (Test-Path $serverLog) { Get-Content $serverLog -Raw } else { '' }
        if ($serverText -match "FIBULA_BOT_TEST_RESULT FAIL") {
            throw "Server bot assertions failed. Logs: $logRoot"
        }
        $serverPassed = $serverText -match "FIBULA_BOT_TEST_RESULT PASS"

        $driverText = if (Test-Path $driverLog) { Get-Content $driverLog -Raw } else { '' }
        $secondClientText = if (Test-Path $secondClientLog) { Get-Content $secondClientLog -Raw } else { '' }
        $driverPassed = $driverText -match "FIBULA_BOT_TEST_CLIENT_RESULT PASS"
        $secondClientPassed = $secondClientText -match "FIBULA_BOT_TEST_CLIENT_RESULT PASS"
        $clientFailed = $driverText -match "FIBULA_BOT_TEST_CLIENT_RESULT FAIL" -or
            $secondClientText -match "FIBULA_BOT_TEST_CLIENT_RESULT FAIL"

        $clientDamageObserved = $false
        foreach ($clientText in @($driverText, $secondClientText)) {
            if ($clientText -match 'FIBULA_BOT_TEST_CLIENT_RESULT PASS .*?ReplicatedDamage=(\d+)' -and [int]$Matches[1] -gt 0) {
                $clientDamageObserved = $true
                break
            }
        }

        if ($serverPassed -and $driverPassed -and $secondClientPassed -and $clientDamageObserved) {
            Write-Host "PASS: $Mode bot multiplayer smoke test"
            Write-Host "Both clients interacted; bot damage was observed on a client."
            Write-Host "Server and client logs: $logRoot"
            exit 0
        }
        if ($clientFailed) {
            throw "A client could not complete its movement and target actions. Logs: $logRoot"
        }
        if ($server.HasExited -or $driver.HasExited -or $secondClient.HasExited) {
            throw "A test process exited before the assertions completed. Logs: $logRoot"
        }
        Start-Sleep -Seconds 1
    }

    throw "Bot multiplayer smoke test timed out after $TimeoutSeconds seconds. Logs: $logRoot"
}
catch {
    Write-Host "FAIL: $_"
    if (Test-Path $serverLog) {
        Write-Host "--- server.log (last 80 lines) ---"
        Get-Content $serverLog -Tail 80
    }
    if (Test-Path $driverLog) {
        Write-Host "--- driver-client.log (last 60 lines) ---"
        Get-Content $driverLog -Tail 60
    }
    if (Test-Path $secondClientLog) {
        Write-Host "--- second-client.log (last 60 lines) ---"
        Get-Content $secondClientLog -Tail 60
    }
    exit 1
}
finally {
    foreach ($process in $processes) {
        if ($process -and -not $process.HasExited) {
            Stop-Process -Id $process.Id -Force -ErrorAction SilentlyContinue
        }
    }
}
