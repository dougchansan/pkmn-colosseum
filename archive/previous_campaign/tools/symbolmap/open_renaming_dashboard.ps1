param(
    [int]$Port = 8788,
    [string]$HostAddress = "127.0.0.1",
    [switch]$Tailscale,
    [switch]$Fresh,
    [switch]$NoOpen
)

$ErrorActionPreference = "Stop"
$Root = Resolve-Path (Join-Path $PSScriptRoot "..\..")

if ($Tailscale) {
    $HostAddress = "0.0.0.0"
}

function Get-DashboardUrl {
    param([int]$CandidatePort)
    $ClientHost = $HostAddress
    if ($ClientHost -eq "0.0.0.0") {
        $ClientHost = "127.0.0.1"
    }
    return "http://${ClientHost}:$CandidatePort/"
}

function Get-TailscaleIPv4 {
    $Text = (ipconfig | Out-String)
    $Block = [regex]::Match($Text, "(?s)adapter Tailscale:.*?(?=\r?\n\r?\n)")
    if ($Block.Success -and $Block.Value -match "IPv4 Address[^\r\n:]*\s*:\s*(100(?:\.\d{1,3}){3})") {
        return $Matches[1]
    }
    if ($Text -match "IPv4 Address[^\r\n:]*\s*:\s*(100(?:\.\d{1,3}){3})") {
        return $Matches[1]
    }
    return $null
}

function Test-Dashboard {
    param([int]$CandidatePort)
    $HealthUrl = "$(Get-DashboardUrl $CandidatePort)api/health"
    try {
        $Response = Invoke-WebRequest -UseBasicParsing -Uri $HealthUrl -TimeoutSec 1
        return $Response.StatusCode -eq 200
    } catch {
        return $false
    }
}

$CandidatePorts = @($Port)
foreach ($Candidate in 8788..8798) {
    if ($Candidate -notin $CandidatePorts) {
        $CandidatePorts += $Candidate
    }
}

$SelectedPort = $null
foreach ($CandidatePort in $CandidatePorts) {
    if ((-not $Fresh) -and (Test-Dashboard $CandidatePort)) {
        $SelectedPort = $CandidatePort
        break
    }

    if ($Fresh -and (Test-Dashboard $CandidatePort)) {
        continue
    }

    Start-Process `
        -FilePath "python" `
        -ArgumentList @("tools\symbolmap\renaming_dashboard.py", "--host", "$HostAddress", "--port", "$CandidatePort") `
        -WorkingDirectory $Root `
        -WindowStyle Hidden | Out-Null

    $Ready = $false
    for ($i = 0; $i -lt 20; $i++) {
        Start-Sleep -Milliseconds 250
        if (Test-Dashboard $CandidatePort) {
            $Ready = $true
            break
        }
    }
    if ($Ready) {
        $SelectedPort = $CandidatePort
        break
    }
}

if ($null -eq $SelectedPort) {
    throw "Renaming dashboard did not become ready on ports $($CandidatePorts -join ', ')"
}

$Url = Get-DashboardUrl $SelectedPort
if (-not $NoOpen) {
    Start-Process $Url | Out-Null
}

Write-Output "Renaming dashboard: $Url"
if ($HostAddress -eq "0.0.0.0") {
    $TailscaleIp = Get-TailscaleIPv4
    if ($TailscaleIp) {
        Write-Output "Tailscale dashboard: http://${TailscaleIp}:$SelectedPort/"
    }
}
