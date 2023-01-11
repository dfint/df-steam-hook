param(
  $zip_path,
  $delete_temp
)

if ($zip_path) {
  $zip_path = $zip_path
} else {
  $zip_path = ".\dfint_release\dfint_release.zip"
}

mkdir ".\dfint_release\"
mkdir ".\dfint_release\dfint_data\"
cpi ".\build\windows\x64\release\dfint_launcher.exe" ".\dfint_release\dfint_launcher.exe"
cpi ".\build\windows\x64\release\dfint_hook.dll" ".\dfint_release\dfint_data\dfint_hook.dll"
cpi ".\config\dfint_config.toml" ".\dfint_release\dfint_data\"
cpi ".\config\offsets" ".\dfint_release\dfint_data\" -Recurse
New-Item ".\dfint_release\dfint_data\dfint_dictionary.csv" -type file
Compress-Archive -Path ".\dfint_release\" -DestinationPath $zip_path

if ($delete_temp) {
  rm ".\dfint_release\" -r -force
}
