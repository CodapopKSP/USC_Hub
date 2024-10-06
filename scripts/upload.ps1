# ----------------
# - BEGIN CONFIG -
# ----------------

$port = "COM5"
$board = "arduino:avr:micro"
$bootloader = "atmega32u4"

$repository = "https://api.github.com/repos/rettoph/USC_HUB_TEST/releases/latest"

# --------------
# - END CONFIG -
# --------------

$arduinoCli = "arduino-cli"

if (Get-Command $arduinoCli -errorAction SilentlyContinue)
{
    Write-Host "Command '$arduinoCli' exists"
}
else {
    Write-Error "Command '$arduinoCli' not found! Ensure you have the Arduino CLI installed to continue.$([Environment]::NewLine)Download at: https://arduino.github.io/arduino-cli/1.0/installation/#latest-release"
    Start-Sleep -s 60
    exit
}

$latest = ConvertFrom-Json (Invoke-WebRequest -Uri $repository)

$binFileName = "USC_HUB_$($latest.tag_name)_$($board.replace(":", "."))_$($bootloader).tar.gz"
$binUrl = ""

foreach($asset in $latest.assets)
{
    if($asset.name -ne $binFileName)
    {
        continue;
    }

    $binUrl = $asset.browser_download_url
    break;
}

if($binUrl -eq "")
{
    Write-Error "No matching asset found: $($binFileName)"
    Start-Sleep -s 60
    exit
}

New-Item -ItemType Directory -Force -Path $PSScriptRoot\.archive
Write-Host "Downloading $($binFileName) at $($binUrl)..."
Invoke-WebRequest $binUrl -OutFile $PSScriptRoot\.archive\$binFileName

Start-Sleep 1

Write-Host "Extracting $($binFileName)..."
New-Item -ItemType Directory -Force -Path $PSScriptRoot\.bin
tar -xvzf $PSScriptRoot\.archive\$binFileName -C $PSScriptRoot\.bin

Start-Sleep 1

Write-Host "Uploading to Arduino on port $($port)..."
arduino-cli upload -p $port -b "$($board):cpu=$($bootloader)" --input-dir "$PSScriptRoot\.bin"