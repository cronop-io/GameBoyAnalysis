Function Compare-Address($FileA, $FileB)
{
    $A = [System.IO.File]::ReadAllBytes($FileA)
    $B = [System.IO.File]::ReadAllBytes($FileB)

    
    for ($i = 0; $i -lt $A.Count; $i++)
    {
        if ($A[$i] -eq 0 -and $B[$i] -eq 1)
        {
            $Bank = ($i -band 0xff0000) -shr 16
            $Address = $i -band 0xffff
            
            if ($Bank -ne 0x0)
            {
                $Radare = (($Bank -1) -shl 16) -bor $Address
            }
            else 
            {
                $Radare = $Address
            }
            
            [String]::Format("`${0:x}:`${1:x} = 0x{2:x}", $Bank, $Address, $Radare)
        }
    }
}

Function Compare-Coverage($Base, $Compare)
{
    $BaseElements = Read-CoverageFile $Base
    $CompareElements = Read-CoverageFile $Compare

    foreach($Element in $CompareElements.GetEnumerator())
    {
        if (-not $BaseElements.Contains($Element.Key))
        {
            $Bank = ($Element.Key -band 0xff0000) -shr 16
            $Address = $Element.Key -band 0xffff
            
            if ($Bank -ne 0x0)
            {
                $Radare = (($Bank -1) -shl 16) -bor $Address
            }
            else 
            {
                $Radare = $Address
            }
            
            [String]::Format("`${0:x}:`${1:x} = 0x{2:x} [{3:d}]", $Bank, $Address, $Radare, $Element.Value)
        }
    }
}

Function Read-CoverageFile($File)
{
    $FileContent = Get-Content $File
    $Elements = @{}

    foreach($Line in $FileContent)
    {
        $Regex = [Regex]"(0x\w+)\:(\d+)"

        $Match = $Regex.Match($Line)

        if ($Match.Success)
        {
            $Key = $Match.Groups[1].Value.Trim()
            $Value = $Match.Groups[2].Value.Trim()

            $Key = [UInt32]$Key
            $Value = [UInt32]$Value
            $Elements.Add($Key, $Value)
        }
        else
        {
            Write-Error "Bad Line: $Line"
        }
    }
    return $Elements
}