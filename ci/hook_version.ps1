param(
  $target_version
)

$content = Get-Content .\xmake.lua
$patched = $content -replace "HOOK_VERSION=([0-9]+)", "HOOK_VERSION=$target_version"

Set-Content -Path .\xmake.lua -Value $patched