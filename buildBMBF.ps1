# Builds a .zip file for loading with BMBF
& $PSScriptRoot/build.ps1

if ($?) {
    Compress-Archive -Path "./libs/arm64-v8a/libquestom-playlists.so", "./bmbfmod.json" -DestinationPath "./questom-playlists_v0.1.0.zip" -Update
}
