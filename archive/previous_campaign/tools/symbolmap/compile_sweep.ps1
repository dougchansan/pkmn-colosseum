# Compile each TU in a list with a fixed flag set; print "OK <tu>" / "FAIL <tu>".
# Used to diff before/after a rename: identical flags mean version/env failures
# appear in both runs and cancel; only rename-induced regressions flip.
param([string]$ListFile)
$mwcc = "build\compilers\GC\1.2.5n\mwcceppc.exe"
$flags = "-nodefaults -proc gekko -align powerpc -enum int -fp hard -Cpp_exceptions off -O4,p -inline auto -nosyspath -RTTI off -fp_contract on -str reuse -multibyte -i include -i build/GC6E01/include -DBUILD_VERSION=0 -DVERSION_GC6E01 -DNDEBUG=1".Split(' ')
foreach ($tu in (Get-Content $ListFile | Where-Object { $_.Trim() })) {
  $null = & $mwcc $flags -c $tu.Trim() -o "$env:TEMP\sweep.o" 2>$null
  if ($LASTEXITCODE -eq 0) { "OK   $tu" } else { "FAIL $tu" }
}
