$source = Join-Path $PSScriptRoot "src"
$dest   = Join-Path $PSScriptRoot "..\QFluentExample\libs\src"

# 确保目标根目录存在
if (!(Test-Path -Path $dest)) {
    New-Item -ItemType Directory -Path $dest | Out-Null
}

# 获取所有 .h 文件（递归）
Get-ChildItem -Path $source -Filter *.h -Recurse | ForEach-Object {
    # 计算相对于 $source 的子路径
    $relativePath = $_.FullName.Substring($source.Length + 1)
    
    # 构建目标文件的完整路径
    $targetFile = Join-Path $dest $relativePath
    
    # 创建目标子目录（如果不存在）
    $targetDir = Split-Path -Parent $targetFile
    if (!(Test-Path -Path $targetDir)) {
        New-Item -ItemType Directory -Path $targetDir -Force | Out-Null
    }
    
    # 复制文件
    Copy-Item $_.FullName -Destination $targetFile -Force
}