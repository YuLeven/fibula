param(
    [Parameter(Mandatory = $true)]
    [string]$EngineRoot
)

$ErrorActionPreference = 'Stop'
$repoRoot = (Resolve-Path (Join-Path $PSScriptRoot '..')).Path
$project = Join-Path $repoRoot 'Fibula.uproject'
$buildTool = Join-Path $EngineRoot 'Engine/Build/BatchFiles/Build.bat'
$editorCmd = Join-Path $EngineRoot 'Engine/Binaries/Win64/UnrealEditor-Cmd.exe'
$reportRoot = Join-Path $repoRoot 'Saved/Tests'
$runId = Get-Date -Format 'yyyyMMdd-HHmmss'
$reportPath = Join-Path $reportRoot "Automation-$runId"
$logPath = Join-Path $reportRoot "Automation-$runId.log"

foreach ($path in @($project, $buildTool, $editorCmd)) {
    if (-not (Test-Path -LiteralPath $path -PathType Leaf)) {
        throw "Required Unreal project or tool not found: $path"
    }
}
New-Item -ItemType Directory -Path $reportRoot -Force | Out-Null

Push-Location $repoRoot
try {
    & $buildTool FibulaEditor Win64 Development "-Project=$project" -WaitMutex
    if ($LASTEXITCODE -ne 0) { throw "Unreal Editor target build failed with exit code $LASTEXITCODE" }

    & $editorCmd $project -unattended -nop4 -nosplash -NullRHI -NoSound `
        '-ExecCmds=Automation RunTests Fibula.;Quit' `
        '-TestExit=Automation Test Queue Empty' `
        "-ReportExportPath=$reportPath" "-abslog=$logPath"
    if ($LASTEXITCODE -ne 0) { throw "Unreal Automation failed with exit code $LASTEXITCODE; see $logPath" }

    $reportFile = Join-Path $reportPath 'index.json'
    if (-not (Test-Path -LiteralPath $reportFile -PathType Leaf)) {
        $reportFile = Get-ChildItem -LiteralPath $reportPath -Filter '*.json' -File |
            Select-Object -First 1 -ExpandProperty FullName
    }
    if (-not $reportFile) { throw "Unreal Automation did not export a JSON report: $reportPath" }

    $report = Get-Content -LiteralPath $reportFile -Raw | ConvertFrom-Json
    $passed = $report.succeeded + $report.succeededWithWarnings
    if ($passed -lt 1 -or $report.failed -ne 0 -or $report.notRun -ne 0) {
        throw "Unreal Automation report is not clean (passed=$passed, failed=$($report.failed), notRun=$($report.notRun)); see $reportFile"
    }
    Write-Output "Unreal Automation passed $passed/$($report.tests.Count) tests ($($report.succeeded) clean, $($report.succeededWithWarnings) with warnings). Report: $reportFile"
}
finally {
    Pop-Location
}
