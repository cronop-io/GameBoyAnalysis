Function Open-GB($Com)
{
    $global:port = new-Object System.IO.Ports.SerialPort $Com,115200,None,8,one
    $global:port.open()

    for ($k = 0; $k -lt 4; $k++)
    {
        # There are four 0 bytes initially 
        $global:port.ReadByte() | Out-Null
    }
}

Function Read-Address($Address)
{
    $global:port.Write([BitConverter]::GetBytes([UInt32]$Address), 0, 4); 

    for ($k = 0; $k -lt 4; $k++)
    {
        [String]::Format("{0:X02}", $global:port.ReadByte())
    }
}

Function Close-GB()
{
    $global:port.Close()
}

Function Read-Range($Start, $Length)
{
    for ($i = $Start; $i -lt $Start + $Length; $i+=4)
    {
        Write-Host ((Read-Address $i) + " ") -NoNewLine

        if (($i - $Start) % 16 -eq 12)
        {
            Write-Host ""
        }
    }
}

Function Read-Rom($MemoryBankNumber)
{
    $TotalBytes = 0x4000 * $MemoryBankNumber
    $CurrentBytes = 0
    
    $ByteArray = [System.Byte[]]::new($TotalBytes)

    # Read bank 0
    for ($i = 0; $i -lt 0x4000; $i += 4)
    {
        $global:port.Write([BitConverter]::GetBytes([UInt32]$i), 0, 4) | Out-Null

        for ($k = 0; $k -lt 4; $k++)
        {
            $ByteArray[$CurrentBytes] = $global:port.ReadByte()
            $CurrentBytes ++;
        }   
        
        if ($i % 0x200 -eq 0)
        {
            Write-Progress -Activity "Dumping ROM" -PercentComplete (($CurrentBytes/$TotalBytes) * 100)
        }
    }

    # Read all other banks
    for ($Bank = 0; $Bank -lt $MemoryBankNumber -1; $Bank++)
    {
        for ($i = 0x4000; $i -lt 0x8000; $i += 4)
        {
            $Address = ($Bank -shl 16) -bor $i
            $global:port.Write([BitConverter]::GetBytes([UInt32]$Address), 0, 4) | Out-Null

            for ($k = 0; $k -lt 4; $k++)
            {
                $ByteArray[$CurrentBytes] = $global:port.ReadByte()
                $CurrentBytes ++;
            }   

            if ($i % 0x200 -eq 0)
            {
                Write-Progress -Activity "Dumping ROM" -PercentComplete (($CurrentBytes/$TotalBytes) * 100)
            }
        }
    }

    return $ByteArray
}