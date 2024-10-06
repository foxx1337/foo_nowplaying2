$cfgRel = "Release"
$cfgDeb = "RelWithDebInfo"

function Build-Platform
{
    param (
        [string]$Subdirectory
    )

    msbuild "$Subdirectory\foo_nowplaying2.sln" /p:Configuration="$cfgRel"
    msbuild "$Subdirectory\foo_nowplaying2.sln" /p:Configuration="$cfgDeb"
}

$package = "$PSScriptRoot\package"
Remove-Item -Recurse -Force -ErrorAction Ignore "$package"
$pRel = "$package\release"
New-Item -ItemType Directory -Path "$pRel"
$pDeb = "$package\with-debug"
New-Item -ItemType Directory -Path "$pDeb"

$prefix = "$PSScriptRoot\..\build64"
if (Test-Path -Path "$prefix\foo_nowplaying2.sln")
{
    Build-Platform "$prefix"

    New-Item -ItemType Directory -Path "$pRel\x64"
    Copy-Item "$prefix\src\$cfgRel\foo_nowplaying2.dll" "$pRel\x64"
    New-Item -ItemType Directory -Path "$pDeb\x64"
    Copy-Item "$prefix\src\$cfgDeb\foo_nowplaying2.dll" "$pDeb\x64"
    Copy-Item "$prefix\src\$cfgDeb\foo_nowplaying2.pdb" "$pDeb\x64"
}

$prefix = "$PSScriptRoot\..\build"
if (Test-Path -Path "$prefix\foo_nowplaying2.sln")
{
    Build-Platform "$prefix"

    Copy-Item "$prefix\src\$cfgRel\foo_nowplaying2.dll" "$pRel"
    Copy-Item "$prefix\src\$cfgDeb\foo_nowplaying2.dll" "$pDeb"
    Copy-Item "$prefix\src\$cfgDeb\foo_nowplaying2.pdb" "$pDeb"
}

$prefix = "$PSScriptRoot\..\buildarm64ec"
if (Test-Path -Path "$prefix\foo_nowplaying2.sln")
{
    Build-Platform "$prefix"

    New-Item -ItemType Directory -Path "$pRel\arm64ec"
    Copy-Item "$prefix\src\$cfgRel\foo_nowplaying2.dll" "$pRel\arm64ec"
    New-Item -ItemType Directory -Path "$pDeb\arm64ec"
    Copy-Item "$prefix\src\$cfgDeb\foo_nowplaying2.dll" "$pDeb\arm64ec"
    Copy-Item "$prefix\src\$cfgDeb\foo_nowplaying2.pdb" "$pDeb\arm64ec"
}

$archiveRel = "$PSScriptRoot\foo_nowplaying2.fb2k-component"
$archiveDeb = "$PSScriptRoot\foo_nowplaying2-with-debug.fb2k-component"
Remove-Item -ErrorAction Ignore "$archiveRel"
Remove-Item -ErrorAction Ignore "$archiveDeb"

Compress-Archive -Path "$pRel\*" -DestinationPath "$archiveRel" -CompressionLevel Optimal
Compress-Archive -Path "$pDeb\*" -DestinationPath "$archiveDeb" -CompressionLevel Optimal

Write-Output "$archiveRel"
$hash = (Get-FileHash -Algorithm SHA256 "$archiveRel").Hash
Write-Output "    $hash"
Write-Output "$archiveDeb"
$hash = (Get-FileHash -Algorithm SHA256 "$archiveDeb").Hash
Write-Output "    $hash"
