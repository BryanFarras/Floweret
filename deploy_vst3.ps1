$src = "C:\Users\asus\Documents\Juce Projects\Floweret\build\Floweret_artefacts\Release\VST3\Floweret.vst3"
$dst = "C:\Program Files\Common Files\VST3\Floweret.vst3"

if (Test-Path $src) {
    try {
        Copy-Item -Recurse -Force $src $dst -ErrorAction Stop
        Write-Host "Successfully deployed Floweret.vst3 to $dst" -ForegroundColor Green
    } catch {
        Write-Host "Could not copy because FL Studio (FL64.exe) is currently holding the file open." -ForegroundColor Yellow
        Write-Host "Please close FL Studio and run this script, or close Floweret in FL Studio to update." -ForegroundColor Yellow
    }
}
