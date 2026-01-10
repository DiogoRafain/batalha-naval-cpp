# Script para configurar a pasta de áudio do jogo Batalha Naval
# Execute este script no PowerShell

Write-Host "==================================" -ForegroundColor Cyan
Write-Host "  CONFIGURADOR DE MÚSICA" -ForegroundColor Cyan
Write-Host "  Batalha Naval - Mestre das Águas" -ForegroundColor Cyan
Write-Host "==================================" -ForegroundColor Cyan
Write-Host ""

# Procurar o executável
Write-Host "?? Procurando teste.exe..." -ForegroundColor Yellow
$exeFiles = Get-ChildItem -Path "C:\Users\Admin\Downloads\teste" -Recurse -Filter "teste.exe" -ErrorAction SilentlyContinue

if ($exeFiles.Count -eq 0) {
    Write-Host "? Nenhum teste.exe encontrado!" -ForegroundColor Red
    Write-Host "   Execute o projeto no Visual Studio primeiro." -ForegroundColor Yellow
    Read-Host "Pressione Enter para sair"
    exit
}

Write-Host "? Encontrado(s) $($exeFiles.Count) executável(is):" -ForegroundColor Green
$exeFiles | ForEach-Object { Write-Host "   ?? $($_.FullName)" -ForegroundColor Gray }
Write-Host ""

# Usar o primeiro (geralmente Debug)
$exePath = $exeFiles[0].DirectoryName
Write-Host "?? Usando: $exePath" -ForegroundColor Cyan
Write-Host ""

# Criar pasta audio
$audioPath = Join-Path $exePath "audio"
Write-Host "?? Criando pasta audio..." -ForegroundColor Yellow

if (Test-Path $audioPath) {
    Write-Host "   ??  Pasta já existe: $audioPath" -ForegroundColor Gray
} else {
    New-Item -Path $audioPath -ItemType Directory -Force | Out-Null
    Write-Host "   ? Pasta criada: $audioPath" -ForegroundColor Green
}

Write-Host ""

# Verificar se já existe música
$musicFiles = Get-ChildItem -Path $audioPath -Filter "musica.*" -ErrorAction SilentlyContinue

if ($musicFiles.Count -gt 0) {
    Write-Host "? Arquivo(s) de música encontrado(s):" -ForegroundColor Green
    $musicFiles | ForEach-Object { 
        $size = [math]::Round($_.Length / 1MB, 2)
        Write-Host "   ?? $($_.Name) ($size MB)" -ForegroundColor Green
    }
} else {
    Write-Host "??  Nenhum arquivo musica.* encontrado!" -ForegroundColor Yellow
    Write-Host ""
    Write-Host "?? AÇÃO NECESSÁRIA:" -ForegroundColor Cyan
    Write-Host "   1. Baixe ou copie seu arquivo de música" -ForegroundColor White
    Write-Host "   2. Renomeie para 'musica.mp3' (ou .ogg ou .wav)" -ForegroundColor White
    Write-Host "   3. Coloque nesta pasta:" -ForegroundColor White
    Write-Host "      $audioPath" -ForegroundColor Yellow
    Write-Host ""
    
    # Oferecer abrir a pasta
    $response = Read-Host "Deseja abrir a pasta agora? (S/N)"
    if ($response -eq "S" -or $response -eq "s") {
        Start-Process explorer.exe -ArgumentList $audioPath
    }
}

Write-Host ""
Write-Host "==================================" -ForegroundColor Cyan
Write-Host "  CONFIGURAÇÃO COMPLETA" -ForegroundColor Cyan
Write-Host "==================================" -ForegroundColor Cyan
Write-Host ""
Write-Host "?? RESUMO:" -ForegroundColor Cyan
Write-Host "   • Executável: $exePath\teste.exe" -ForegroundColor White
Write-Host "   • Pasta áudio: $audioPath" -ForegroundColor White
Write-Host ""
Write-Host "?? PRÓXIMOS PASSOS:" -ForegroundColor Cyan
Write-Host "   1. Certifique-se que existe um arquivo musica.* na pasta audio" -ForegroundColor White
Write-Host "   2. Execute o jogo" -ForegroundColor White
Write-Host "   3. Verifique se o botão de música fica VERDE" -ForegroundColor White
Write-Host ""

Read-Host "Pressione Enter para sair"
