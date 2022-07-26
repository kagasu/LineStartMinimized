# LineStartMinimized
- Start Minimized.
- Reduce splash window.

# How to use
### Install
1. Move original `dbghelp.dll` 
```
cd C:\Users\%username%\AppData\Local\LINE\bin\current
mkdir C:\Users\%username%\AppData\Local\LINE\bin\current\original_dll
move dbghelp.dll original_dll\dbghelp.dll
```

2. Download modified `dbghelp.dll` from GitHub

3. Copy `dbghelp.dll`
```
copy C:\Users\%username%\Downloads\dbghelp.dll C:\Users\%username%\AppData\Local\LINE\bin\current\dbghelp.dll
```

### Uninstall
1. Move `dbghelp.dll`
```
move /Y C:\Users\%username%\AppData\Local\LINE\bin\current\original_dll\dbghelp.dll C:\Users\%username%\AppData\Local\LINE\bin\current\dbghelp.dll
```

# License
|Name|License|Modified|URL|
|:--|:--|:--|:--|
|Detours|MIT|No|https://github.com/microsoft/Detours|
