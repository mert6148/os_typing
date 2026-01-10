<#
Minimal Windows hardening script for os_typing deployments.
Run in an elevated PowerShell session (Run as Administrator).
This script is conservative and checks/sets common recommended settings.
#>

[CmdletBinding()]
param()

function Ensure-Firewall {
    Write-Output "Enabling Windows Firewall (all profiles)"
    Set-NetFirewallProfile -Profile Domain,Public,Private -Enabled True
}

function Ensure-Defender {
    Write-Output "Ensuring Windows Defender is enabled (if present)"
    try {
        Set-MpPreference -DisableRealtimeMonitoring $false -ErrorAction Stop
    } catch {
        Write-Warning "Windows Defender not available or needs admin features"
    }
}

function Disable-SMBv1 {
    Write-Output "Disabling SMBv1"
    sc.exe config lanmanworkstation depend= bowser/mrxsmb20/nsi
    Set-SmbServerConfiguration -EnableSMB1Protocol $false -Force
}

function Enforce-TLS12 {
    Write-Output "Ensure TLS 1.2 is enabled and older protocols disabled (best-effort)"
    # Only warn; changing registry keys is intrusive and needs testing in your environment.
    Write-Output "Check TLS settings in registry under HKLM:\SYSTEM\CurrentControlSet\Control\SecurityProviders\SCHANNEL"
}

Write-Output "Starting OS hardening checks..."
Ensure-Firewall
Ensure-Defender
Disable-SMBv1
Enforce-TLS12
Write-Output "Windows minimal hardening completed. Review and adapt to your environment."