$fqbns = ("arduino:avr:nano", "atmega328"),("arduino:avr:nano", "atmega328old")
$tag = "v0.0.1"

New-Item -ItemType Directory -Force -Path $PSScriptRoot\.compile\
New-Item -ItemType Directory -Force -Path $PSScriptRoot\..\build\$tag\

foreach ($fqbn in $fqbns)
{
    Write-Host "$($fqbn[0]):cpu=$($fqbn[1])"
    arduino-cli compile --fqbn "$($fqbn[0]):cpu=$($fqbn[1])" -e $PSScriptRoot\.. --clean --output-dir $PSScriptRoot\.compile\
    tar.exe -czf "$PSScriptRoot\..\build\$tag\USC_HUB_$($tag)_$($fqbn[0].Replace(":", "."))_$($fqbn[1]).tar.gz" -C $PSScriptRoot\.compile\ .

    Remove-Item $PSScriptRoot\.compile\* -Recurse -Force
}