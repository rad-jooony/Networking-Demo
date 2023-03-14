del /s /q x64\Release\*.*
del /s /q x64\Debug\*.*
del /s /q .vs\*.*
del /s /q NMG\x64\Release\*.*
del /s /q NMG\x64\Debug\*.*
del /s /q NMG\Debug
del /s /q NMG\Release
del /s /q Debug\*.*
del /s /q Release\*.*

rd /s /q x64
rd /s /q .vs
rd /s /q Debug
rd /s /q Release
rd /s /q NMG\Debug
rd /s /q NMG\Release
rd /s /q NMG\x64