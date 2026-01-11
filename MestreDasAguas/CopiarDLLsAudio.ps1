# Script para copiar DLLs do SFML Audio
Write-Host "==================================" -ForegroundColor Cyan
Write-Host "  COPIAR DLLs SFML AUDIO" -ForegroundColor Cyan
Write-Host "==================================" -ForegroundColor Cyan
Write-Host ""

# Encontrar o executável
Write-Host "?? Procurando teste.exe..." -ForegroundColor Yellow
$exeFiles = Get-ChildItem -Path "C:\Users\Admin\Downloads\teste" -Recurse -Filter "teste.exe" -ErrorAction SilentlyContinue

if ($exeFiles.Count -eq 0) {
    Write-Host "? Nenhum teste.exe encontrado!" -ForegroundColor Red
    Read-Host "Pressione Enter para sair"
    exit
}

$exePath = $exeFiles[0].DirectoryName
Write-Host "? Encontrado: $exePath" -ForegroundColor Green
Write-Host ""

# Verificar DLLs existentes
Write-Host "?? Verificando DLLs necessárias..." -ForegroundColor Yellow
$dllsNecessarias = @("sfml-audio-3.dll", "sfml-system-3.dll", "openal32.dll")
$dllsFaltando = @()

foreach ($dll in $dllsNecessarias) {
    $dllPath = Join-Path $exePath $dll
    if (Test-Path $dllPath) {
        Write-Host "   ? $dll" -ForegroundColor Green
    } else {
        Write-Host "   ? $dll (FALTANDO!)" -ForegroundColor Red
        $dllsFaltando += $dll
    }
}

Write-Host ""

if ($dllsFaltando.Count -eq 0) {
    Write-Host "?? Todas as DLLs necessárias estão presentes!" -ForegroundColor Green
    Write-Host ""
    Write-Host "??  Se a música ainda não toca, o problema pode ser:" -ForegroundColor Yellow
    Write-Host "   1. Arquivo MP3 corrompido ou incompatível" -ForegroundColor White
    Write-Host "   2. Codec MP3 não suportado (tente converter para OGG)" -ForegroundColor White
    Write-Host "   3. Volume do Windows mudo" -ForegroundColor White
} else {
    Write-Host "??  DLLs faltando: $($dllsFaltando.Count)" -ForegroundColor Yellow
    Write-Host ""
    
    # Procurar DLLs no sistema
    Write-Host "?? Procurando DLLs do SFML no sistema..." -ForegroundColor Yellow
    $sfmlPaths = @(
        "C:\SFML-3.0.0\bin",
        "C:\SFML\bin",
        "C:\Program Files\SFML-3.0.0\bin",
        "C:\Program Files (x86)\SFML-3.0.0\bin"
    )
    
    $sfmlFound = $null
    foreach ($path in $sfmlPaths) {
        if (Test-Path $path) {
            $sfmlFound = $path
            Write-Host "   ? SFML encontrado em: $path" -ForegroundColor Green
            break
        }
    }
    
    if ($sfmlFound) {
        Write-Host ""
        $response = Read-Host "Deseja copiar as DLLs automaticamente? (S/N)"
        
        if ($response -eq "S" -or $response -eq "s") {
            foreach ($dll in $dllsFaltando) {
                $origem = Join-Path $sfmlFound $dll
                $destino = Join-Path $exePath $dll
                
                if (Test-Path $origem) {
                    Copy-Item $origem $destino -Force
                    Write-Host "   ? Copiado: $dll" -ForegroundColor Green
                } else {
                    Write-Host "   ??  Não encontrado: $dll" -ForegroundColor Yellow
                }
            }
            Write-Host ""
            Write-Host "?? DLLs copiadas com sucesso!" -ForegroundColor Green
        }
    } else {
        Write-Host ""
        Write-Host "? SFML não encontrado no sistema!" -ForegroundColor Red
        Write-Host ""
        Write-Host "?? SOLUÇÃO:" -ForegroundColor Cyan
        Write-Host "   1. Baixe SFML 3.0.0 de: https://www.sfml-dev.org/download.php" -ForegroundColor White
        Write-Host "   2. Extraia para C:\SFML-3.0.0\" -ForegroundColor White
        Write-Host "   3. Copie manualmente as DLLs de C:\SFML-3.0.0\bin\ para:" -ForegroundColor White
        Write-Host "      $exePath" -ForegroundColor Yellow
        Write-Host ""
        Write-Host "   DLLs necessárias:" -ForegroundColor White
        foreach ($dll in $dllsFaltando) {
            Write-Host "      - $dll" -ForegroundColor Gray
        }
    }
}

Write-Host ""
Write-Host "==================================" -ForegroundColor Cyan
Read-Host "Pressione Enter para sair"
