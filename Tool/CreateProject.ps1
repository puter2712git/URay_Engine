$ErrorActionPreference = 'Stop'

$engineRoot = Split-Path -Parent $PSScriptRoot

$editorPath = Join-Path $engineRoot 'Build/Sandbox-Release/URay_Engine.exe'
$engineAssetPath = Join-Path $engineRoot 'Engine'

if (-not (Test-Path -LiteralPath $editorPath -PathType Leaf)) {
    throw "Editor executable not found: $editorPath"
}

if (-not (Test-Path -LiteralPath $engineAssetPath -PathType Container)) {
    throw "Engine asset directory not found: $engineAssetPath"
}

$Name = Read-Host 'Project name'
$Destination = Read-Host 'Parent directory path'

if ([string]::IsNullOrWhiteSpace($Name)) {
    throw 'Please provide a valid project name.'
}

if ($Name -eq '.' -or $Name -eq '..' -or
    $Name.IndexOfAny([System.IO.Path]::GetInvalidFileNameChars()) -ge 0 -or
    $Name.EndsWith('.') -or $Name.EndsWith(' ')) {
    throw 'Please provide a valid project name.'
}

if ($Name -match '^(?i:CON|PRN|AUX|NUL|COM[1-9]|LPT[1-9])(\..*)?$') {
    throw 'Please provide a valid project name.'
}

if ([string]::IsNullOrWhiteSpace($Destination) -or
    -not (Test-Path -LiteralPath $Destination -PathType Container)) {
    throw "The specified path '$Destination' does not exist or is not a valid directory."
}

$destinationPath = (Resolve-Path -LiteralPath $Destination).Path
$projectPath = Join-Path -Path $destinationPath -ChildPath $Name

if (Test-Path -LiteralPath $projectPath) {
    throw "A file or directory already exists at '$projectPath'."
}

$directories = @(
    'Asset/Source'
    'Config'
    'External'
    'Log'
)

$null = New-Item -ItemType Directory -Path $projectPath

foreach ($directory in $directories) {
    $directoryPath = Join-Path -Path $projectPath -ChildPath $directory
    $null = New-Item -ItemType Directory -Path $directoryPath -Force
}

$shortcutPath = Join-Path $projectPath 'Open Editor.lnk'

$shell = New-Object -ComObject WScript.Shell
$shortcut = $shell.CreateShortcut($shortcutPath)

$shortcut.TargetPath = $editorPath
$shortcut.Arguments = "--engine `"$engineAssetPath`" --project `"$projectPath`""
$shortcut.WorkingDirectory = $engineRoot

$shortcut.Save()

Write-Host "Project created: $projectPath"
Write-Host "Editor shortcut created: $shortcutPath"