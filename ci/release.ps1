param(
  $zip_path,
  $delete_temp
)

if ($zip_path) {
  $zip_path = $zip_path
} else {
  $zip_path = ".\dfint_release\dfint_release.zip"
}

echo $zip_path

xmake -y build
mkdir ".\dfint_release\"
mkdir ".\dfint_release\dfint_data\"
mkdir ".\dfint_release\dfint_data\offsets\"
cpi ".\build\windows\x64\release\dfint_launcher.exe" ".\dfint_release\dfint_launcher.exe"
cpi ".\build\windows\x64\release\dfint_hook.dll" ".\dfint_release\dfint_data\dfint_hook.dll"
cpi ".\config\" ".\dfint_release\dfint_data\" -Recurse
Compress-Archive -Path ".\dfint_release\" -DestinationPath $zip_path

if ($delete_temp) {
  rm ".\dfint_release\" -r -force
}
