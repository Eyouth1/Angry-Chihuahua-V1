; Inno Setup Script for Angry Chihuahua V1 VST3 Plugin
; This script creates a Windows installer for the VST3 plugin

#define MyAppName "Angry Chihuahua V1"
#define MyAppVersion "1.0.0"
#define MyAppPublisher "Electronic Youth"
#define MyAppURL "https://www.electronicyouth.co.uk"

[Setup]
AppId={{LhKSke-AngryChihuahua-VST3}
AppName={#MyAppName}
AppVersion={#MyAppVersion}
AppPublisher={#MyAppPublisher}
AppPublisherURL={#MyAppURL}
AppSupportURL={#MyAppURL}
AppUpdatesURL={#MyAppURL}
DefaultDirName={autopf}\{#MyAppPublisher}\{#MyAppName}
DefaultGroupName={#MyAppName}
AllowNoIcons=yes
LicenseFile=
OutputDir="{#GetEnv('GITHUB_WORKSPACE')}\Builds\VisualStudio2022\x64\Release"
OutputBaseFilename=Angry-Chihuahua-V1-Windows-VST3-Installer
SetupIconFile=
Compression=lz4
SolidCompression=yes
ArchitecturesInstallIn64BitMode=x64
Uninstallable=yes
UninstallDisplayIcon={app}\{#MyAppName}.vst3
CloseApplications=yes
RestartIfNeededByRun=no

[Languages]
Name: "english"; MessagesFile: "compiler:Default.isl"

[Files]
Source: "{#GetEnv('GITHUB_WORKSPACE')}\Builds\VisualStudio2022\x64\Release\VST3\*"; DestDir: "{commoncf}\VST3"; Flags: ignoreversion recursesubdirs createallsubdirs

[Icons]
Name: "{group}\{cm:UninstallProgram,{#MyAppName}}"; Filename: "{uninstallexe}"

[Run]
; No special run configuration needed - plugin is installed to VST3 folder

[Code]
procedure CurStepChanged(CurStep: TSetupStep);
begin
  if CurStep = ssInstall then
    MsgBox('Installing Angry Chihuahua V1 VST3 Plugin...', mbInformation, MB_OK);
  if CurStep = ssPostInstall then
    MsgBox('Angry Chihuahua V1 VST3 Plugin has been successfully installed to your VST3 plugins folder.', mbInformation, MB_OK);
end;
